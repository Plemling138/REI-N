#include "rei.h"
#include "ether.h"
#include "ethdrv.h"
#include "lib.h"
#include "h8_69.h"
#include "userprogram.h"

uint32 network_id;

#define MACADDR_SIZE 6 //byte
#define IPADDR_SIZE	 4  //byte
#define PACKET_SIZE  48

#define ETH_HEADER_SIZE 14 //byte

//UDPパケット
struct Packet {
	uint8  eth_srcMAC[MACADDR_SIZE];
	uint8  eth_dstMAC[MACADDR_SIZE];
	uint16 eth_EthernetType;
#define  ETH_PROC_IP  0x0800

	uint8  ip_Ver;
	uint8  ip_ServiceType;
	uint16 ip_TotalLength;
	uint16 ip_ID;
	uint16 ip_FragmentOffset;
	uint8  ip_TTL;
	uint8  ip_Protocol;
#define  IPPROC_PING 1
#define  IPPROC_UDP  17
	uint16 ip_Checksum;
	uint8  ip_src_ip[IPADDR_SIZE];
	uint8  ip_dst_ip[IPADDR_SIZE];

	uint16 udp_SrcPort;
	uint16 udp_DstPort;
	uint16 udp_Length;
	uint16 udp_Checksum;
	uint8  udp_Packet[10];
};

//チェックサム計算が要るヘッダ用のダミー構造体
struct UDP_Header {
	uint16 SrcPort;
	uint16 DstPort;
	uint16 Length;
	uint16 Checksum;
	uint8  Packet[10];
};

struct IP_Header {
	uint8  Ver;
	uint8  ServiceType;
	uint16 TotalLength;
	uint16 ID;
	uint16 FragmentOffset;
	uint8 TTL;
	uint8 Protocol;
	uint16 Checksum;
	uint8  src_ip[IPADDR_SIZE];
	uint8  dst_ip[IPADDR_SIZE];
};

//UDP擬似ヘッダ
struct UDP_Pseudo {
	uint8 srcIP[IPADDR_SIZE];
	uint8 dstIP[IPADDR_SIZE];
	uint8 ZeroPadding;
	uint8 IPProc;
	uint16 Length;
};

//IPパケットのチェックサム計算
static uint16 CalcChecksum(int size, void *buf) {
	int i;
	uint16 val;
	uint32 sum = 0;
	uint16 *p = buf;
	
	for(i=0; i<size; i+=sizeof(uint16)) {
		if(size-i == 1) val = *(unsigned char *)p << 8;
		else val = *p;
		sum += val;
		p++;
	}
	
	while(sum>0xffff) sum = (sum>>16) + (sum & 0xffff); //1の補数和をとる
	return ~sum & 0xffff; //CheckSum
}

//UDPパケットのチェックサム計算
static uint16 CalcChecksum_udp(int size, void *buf) {
	int i;
	uint16 val;
	uint32 sum = 0;
	uint16 *p = buf;
	
	for(i=0; i<size; i+=sizeof(uint16)) {
		if(size-i == 1) val = *(unsigned char *)p << 8;
		else val = *p;
		sum += val;
		p++;
	}
	
	while(sum>0xffff) sum = (sum>>16) + (sum & 0xffff); //1の補数和をとる
	return sum; //CheckSum
}

//UDP送信エラーチェック
static int UDP_SendErrCheck(struct UDP_Header *udp, struct IP_Header *ip) {
	uint16 i;
	uint32 sum = 0;
	uint8  header[12];
	struct UDP_Pseudo *pseudo = 0;
	
	pseudo = (struct UDP_Pseudo *)header;
	for(i=0;i<4;i++) {
		pseudo->srcIP[i] = ip->src_ip[i];
		pseudo->dstIP[i] = ip->dst_ip[i];
	}
	
	pseudo->ZeroPadding = 0;
	pseudo->IPProc = ip->Protocol;
	pseudo->Length = udp->Length;
	
	sum = CalcChecksum_udp(12, header);
	sum += CalcChecksum_udp(14, udp);
	sum = (sum & 0xFFFF) + (sum >> 16);
	sum = (~sum) & 0xFFFF;
	
	if(sum == 0x0000) sum = 0xFFFF;
	return sum;
}

//UDPパケットへのデータ格納
static int udp(struct Packet *udp, struct UDP_Header *copy, struct IP_Header *ip) {
	udp->udp_SrcPort = 1024;
	udp->udp_DstPort = 3000;
	udp->udp_Length = 8+6;
	setpacket(udp->udp_Packet); //送信パケットを生成（usertask.c参照）
	
	copy->SrcPort = 1024;
	copy->DstPort = 3000;
	copy->Length = 8+6;
	memcpy(copy->Packet,udp->udp_Packet,10);
	
	copy->Checksum = 0;
	copy->Checksum = UDP_SendErrCheck(copy,ip);
	
	udp->udp_Checksum = 0;
	udp->udp_Checksum = copy->Checksum;
	
	return 0;
}

//IPパケットの生成
static int ip(struct Packet *ip, struct IP_Header *copy, uint32 srcip, uint32 dstip) {
	int i = 0;
	i++;
	
	ip->ip_Ver = 0x45;
	ip->ip_ServiceType = 0;
	ip->ip_TotalLength = PACKET_SIZE - 14; //イーサネットヘッダの長さを引く
	
	ip->ip_ID = 2;
	
	ip->ip_FragmentOffset = 0x4000;
	ip->ip_TTL = 0x80;
	ip->ip_Protocol = IPPROC_UDP;
	memcpy(ip->ip_src_ip, &srcip, IPADDR_SIZE);
	memcpy(ip->ip_dst_ip, &dstip, IPADDR_SIZE);
	
	copy->Ver = 0x45;
	copy->ServiceType = 0;
	copy->TotalLength = PACKET_SIZE - 14;
	
	copy->ID = 2;
	
	copy->FragmentOffset = 0x4000;
	copy->TTL = 0x80;
	copy->Protocol = IPPROC_UDP;
	memcpy(copy->src_ip, &srcip, IPADDR_SIZE);
	memcpy(copy->dst_ip, &dstip, IPADDR_SIZE);
	copy->Checksum = 0;
	copy->Checksum = CalcChecksum(20,copy);
	
	ip->ip_Checksum = 0;
	ip->ip_Checksum = copy->Checksum;
	
	return 0;
}

//イーサネットパケットの生成
static int ethernet(struct Packet *eth, uint8 srcmac[6], uint8 dstmac[6]) {
	int i = 0;
	for(i=0;i<6;i++) {
		eth->eth_srcMAC[i] = /*(unsigned char)*/srcmac[i];
		eth->eth_dstMAC[i] = /*(unsigned char)*/dstmac[i];
	}
	eth->eth_EthernetType = ETH_PROC_IP;
	return 0;
}

//タスク間通信
static void SendTaskMsg_use(void) {
	char *p;
	p = sys_memalloc(1);
	p[0] = ETHERDRV_CMD_USE;
	sys_send(MLBOX_ID_ETHOUTPUT, 1, p);
}

static void SendTaskMsg_send(int size, char *buf) {
	char *p;
	p = sys_memalloc(size+1);
	p[0] = ETHERDRV_CMD_SEND;
	memcpy(&p[1], buf, size);
	sys_send(MLBOX_ID_ETHOUTPUT, size+1, p);
}

//送信タスク
int send_main(int argc, char *argv[]) {
	int i,j,k,size;
	size = PACKET_SIZE;
	uint8 MACADDR[6];
	uint8 DSTMAC[6];
#define IPADDR 0xc0a80110 //192.168.1.16
#define tIPADDR 0xc0a80111 //192.168.1.17
	uint32 srcip = IPADDR;
	uint32 dstip = tIPADDR;
	
	struct Packet *packet = 0;
	struct UDP_Header *udp_header = 0;
	struct IP_Header *ip_header = 0;
	char *p = 0; //送信パケット格納用

	SendTaskMsg_use();
	puts("[Network ready.]\n");
	//送信先MACアドレス
	DSTMAC[0] = 0x00;
	DSTMAC[1] = 0x00;
	DSTMAC[2] = 0x00;
	DSTMAC[3] = 0x00;
	DSTMAC[4] = 0x00;
	DSTMAC[5] = 0x00;

	//自身のMACアドレス
	uint8 macaddr[6];
	getmacaddr(macaddr);
	puts("[ip_main:NIC MAC-Address:");
	
	for(i=0;i<6;i++) {
		MACADDR[i] = (unsigned char)macaddr[i];
		puthex((unsigned char)MACADDR[i],2);
	}
	puts("]\n");
	
	i = 0;
	
	while(1) {
		if(ether_checkintr(0)) goto end;
		
		*p = 0;
		
		//領域確保
		packet     = sys_memalloc(PACKET_SIZE);
		udp_header = sys_memalloc(8+6);
		ip_header  = sys_memalloc(20);
		
		//パケット格納
		ethernet(packet, DSTMAC, MACADDR);
		ip(packet,ip_header, srcip, dstip);
		udp(packet, udp_header, ip_header);
		p = (char *)packet;
		
		//パケットを送信するときの条件
		if(udp_header->Packet[1] > 0x03 && udp_header->Packet[2] > 50){
			ether_intr_disable(0);
			//パケットをドライバに渡す
			SendTaskMsg_send(PACKET_SIZE,p);
			ether_intr_enable(0);
		}
		
		end:
		//領域開放
		sys_memfree(packet);
		sys_memfree(udp_header);
		sys_memfree(ip_header);
		
		i++;
		if(i==10) {SendTaskMsg_use(); i=0;}
		
		for(j=0;j<2000;j++) for(k=0;k<500;k++);
		//sys_wait();
	}
	return 0;
}

