#include "rei.h"
#include "ether.h"
#include "ethdrv.h"
#include "lib.h"
#include "h8_69.h"
#include "userprogram.h"

uint32 network_id;
uint8  MACADDR[6];

//自身のIPアドレス
#define IPADDR 0xc0a80111 //192.168.1.17

static uint32 ipaddr = IPADDR;

#define MACADDR_SIZE 6 //byte
#define IPADDR_SIZE	 4  //byte

#define ETH_HEADER_SIZE 14 //byte

//イーサネットヘッダ
struct Ethernet_header {
	uint8 dstMAC[MACADDR_SIZE];
	uint8 srcMAC[MACADDR_SIZE];
	uint16 EthernetType;
#define ETH_PROC_IP  0x0800
#define ETH_PROC_ARP 0x0806
};

//ARPパケット
struct ARP_msg {
	uint16 HardwareType;
#define ARP_HARDWARETYPE 1
	uint16 ProtocolType;
	uint8  HardwareLength;
	uint8  ProtocolLength;
	uint16 Operation;
#define ARP_REQUEST 1
#define ARP_REPLY   2
	uint8  src_macaddr[MACADDR_SIZE];
	uint8  src_ipaddr[IPADDR_SIZE];
	uint8  dst_macaddr[MACADDR_SIZE];
	uint8  dst_ipaddr[IPADDR_SIZE];
};

//IPヘッダ
struct IP_Header {
	uint8  Ver;
	uint8  ServiceType;
	uint16 TotalLength;
	uint16 ID;
	uint16 FragmentOffset;
	uint8 TTL;
	uint8 Protocol;
#define IPPROC_PING 1
#define IPPROC_UDP  17
	uint16 Checksum;
	uint8  src_ip[IPADDR_SIZE];
	uint8  dst_ip[IPADDR_SIZE];
};

//ICMPパケット
struct ICMP_msg {
	uint8  Type;
#define PING_REQUEST 8
#define PING_REPLY   0
	uint8  Code;
	uint16 Checksum;
};

//UDPヘッダ
struct UDP_Header {
	uint16 SrcPort;
	uint16 DstPort;
	uint16 Length;
	uint16 Checksum;
	uint8  Packet[10];
};

//UDP擬似ヘッダ
struct UDP_Pseudo {
	uint8 srcIP[IPADDR_SIZE];
	uint8 dstIP[IPADDR_SIZE];
	uint8 ZeroPadding;
	uint8 IPProc;
	uint16 Length;
};

//パケット構造体の初期化
int InitPacket(void) {
	int i;
	struct Ethernet_header *eth = 0;
	struct ARP_msg *arp = 0;
	struct IP_Header *ip = 0;
	struct ICMP_msg *icmp = 0;
	struct UDP_Header *udp = 0;
	
	for(i=0;i<6;i++) {
		eth->dstMAC[i] = 0;
		eth->srcMAC[i] = 0;
	}
	eth->EthernetType = 0;
	
	arp->HardwareType = 0;
	arp->ProtocolType = 0;
	arp->HardwareLength = 0;
	arp->ProtocolLength = 0;
	arp->Operation = 0;
	for(i=0;i<6;i++) {
		arp->src_macaddr[i] = 0;
		arp->dst_macaddr[i] = 0;
	}
	for(i=0;i<4;i++) {
		arp->src_ipaddr[i] = 0;
		arp->dst_ipaddr[i] = 0;
	}
	
	ip->Ver = 0;
	ip->ServiceType = 0;
	ip->TotalLength = 0;
	ip->ID = 0;
	ip->FragmentOffset = 0;
	ip->TTL = 0;
	ip->Protocol = 0;
	ip->Checksum = 0;
	for(i=0;i<4;i++) {
		ip->src_ip[i] = 0;
		ip->dst_ip[i] = 0;
	}	
	
	icmp->Type = 0;
	icmp->Code = 0;
	icmp->Checksum = 0;
	
	udp->SrcPort = 0;
	udp->DstPort = 0;
	udp->Length = 0;
	udp->Checksum = 0;
	for(i=0;i<10;i++) udp->Packet[i] = 0;
	return 0;
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
	while(sum>0xffff) sum = ((sum>>16) & 0xffff) + (sum & 0xffff); //1の補数和をとる
	return sum; //CheckSum
}

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
	
	while(sum>0xffff) sum = ((sum>>16) & 0xffff) + (sum & 0xffff); //1の補数和をとる
	return (~sum) & 0xffff; //CheckSum
}

//UDPパケットのエラーチェック
static int UDP_RecvErrCheck(struct UDP_Header *udp, struct IP_Header *ip) {
	uint16 i;
	uint32 sum;
	uint8  header[12];
	struct UDP_Pseudo *pseudo = 0;
	
	if(udp->Checksum != 0x0000) {
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
		
		if(sum != 0x0000) {
			puthex(sum, 4);
			puts("x");
			return 1;
		}
		else puts("o");
	}
	return 0;
}

//UDPパケットを取り出して処理する
static int udp(int size, struct UDP_Header *udp, struct IP_Header *ip) {
	if(udp->DstPort != 3000) return 0;
	recvpacket(udp->Packet); //受信データの解析（usertask.c参照）
	if(UDP_RecvErrCheck(udp,ip) != 0) return 0;
	
	return 0;
}

//PING応答
static int ping(int size, struct ICMP_msg *ping) {
	int ret = 0;
	
	switch(ping->Type) {
	case PING_REQUEST:
		ping->Type = PING_REPLY;
		ping->Checksum = 0;
		ping->Checksum = CalcChecksum(size, ping);
		ret = size;
		break;
	default:
		break;
	}
	
	return ret;
}

//IPパケット解析
static int ip(int size, struct IP_Header *ip) {
	int ret = 0, hdrlen, nextsize;
	void *nextip;
	
	if(((ip->Ver >> 4) & 0xf) != 4) return 0;
	if(memcmp(ip->dst_ip, &ipaddr, IPADDR_SIZE)) return 0;
	
	hdrlen = (ip->Ver & 0xf) << 2;
	
	if (size > ip->TotalLength) size = ip->TotalLength;
	
	nextsize = size - hdrlen;
	nextip = (char *)ip+hdrlen;
	
	switch(ip->Protocol) {
	case IPPROC_PING:
		ret = ping(nextsize, nextip);
		break;
	case IPPROC_UDP:
		ret = udp(nextsize, nextip,ip);
	default:
		break;
	}
	
	if(ret > 0) {
		memcpy(ip->dst_ip, ip->src_ip, IPADDR_SIZE);
		memcpy(ip->src_ip, &ipaddr, IPADDR_SIZE);
		ip->Checksum = 0;
		ip->Checksum = CalcChecksum(hdrlen, ip);
		ret += hdrlen;
	}
	return ret;
}

//ARP応答
static int arp(int size, struct ARP_msg *arp) {
	int ret = 0;
	if(arp->HardwareType != ARP_HARDWARETYPE) return 0;
	if(arp->ProtocolType != ETH_PROC_IP) return 0;
	
	switch(arp->Operation) {
	case ARP_REQUEST:
		if(memcmp(arp->dst_ipaddr, &ipaddr, IPADDR_SIZE)) break;
		memcpy(arp->dst_macaddr, arp->src_macaddr, MACADDR_SIZE);
		memcpy(arp->dst_ipaddr, arp->src_ipaddr, IPADDR_SIZE);
		memcpy(arp->src_macaddr, MACADDR, MACADDR_SIZE);
		memcpy(arp->src_ipaddr, &ipaddr, IPADDR_SIZE);
		arp->Operation = ARP_REPLY;
		ret = size;
	default:
		break;
	}
	
	return ret;
}

//イーサネットヘッダの解析
static int ethernet(int size, struct Ethernet_header *eth) {
	int ret = 0, nextsize;
	void *nextip;
	
	if(!(eth->dstMAC[0] & 1) && memcmp(eth->dstMAC, MACADDR, MACADDR_SIZE)) return 0;
	
	nextsize = size - ETH_HEADER_SIZE;
	nextip = (char *)eth + ETH_HEADER_SIZE;
	
	switch(eth->EthernetType) {
	case ETH_PROC_ARP:
		ret = arp(nextsize, nextip);
		break;
	case ETH_PROC_IP:
		ret = ip(nextsize, nextip);
		break;
	default:
		break;
	}
	
	if (ret > 0) {
		memcpy(eth->dstMAC, eth->srcMAC, MACADDR_SIZE);
		memcpy(eth->srcMAC, MACADDR, MACADDR_SIZE);
		ret += ETH_HEADER_SIZE;
	}
	
	return ret;
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

//受信タスク
int recv_main(int argc, char *argv[]) {
	char *p;
	int size,i;	

	SendTaskMsg_use();
	puts("[Network ready.]\n");

	uint8 macaddr[6];
	getmacaddr(macaddr);
	puts("[recv_main:NIC MAC-Address:");
	
	for(i=0;i<6;i++) {
		MACADDR[i] = (unsigned char)macaddr[i];
		puthex((unsigned char)MACADDR[i],2);
	}
	puts("]\n");
	i = 0;

	while(1) {
		DI;
		*p = 0;
		InitPacket();
		i++;
		
		if(i == 10) {SendTaskMsg_use(); i=0;}
		sys_recv(MLBOX_ID_ETHINPUT, &size, &p);
		size = ethernet(size, (struct Ethernet_header *)p);
		if(size>0) {
			SendTaskMsg_send(size, p);
		}
		sys_memfree(p);
		
		EI;
	}
	
	return 0;
}

