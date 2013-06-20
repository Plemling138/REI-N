#include "defines.h"
#include "lib.h"
#include "ether.h"
#include "nic.h"
#include "h8_69.h"

static void udelay(int usec)
{
  volatile int i;
  for (i = 0; i < 20; i++) /* 20MHz */
    ;
}

static void mdelay(int msec)
{
  volatile int i;
  for (i = 0; i < msec; i++) {
    udelay(1000);
  }
}

static int read_data(int addr, int size, char *buf)
{
  int i;

  NIC_CR    = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  NIC_RBCR0 = size & 0xff;
  NIC_RBCR1 = (size >> 8) & 0xff;
  NIC_RSAR0 = addr & 0xff;
  NIC_RSAR1 = (addr >> 8) & 0xff;
  NIC_CR    = NIC_CR_P0 | NIC_CR_RD_READ | NIC_CR_STA;
  for (i = 0; i < size; i++) {
    buf[i] = NIC_RDMAP;
  }
  while ((NIC_ISR & NIC_ISR_RDC) == 0)
    ;

  return 0;
}

static int write_data(int addr, int size, char *buf)
{
  int i;
        
  NIC_CR    = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  NIC_RBCR0 = size & 0xff;
  NIC_RBCR1 = (size >> 8) & 0xff;
  NIC_RSAR0 = addr & 0xff;
  NIC_RSAR1 = (addr >> 8) & 0xff;
  NIC_CR    = NIC_CR_P0 | NIC_CR_RD_WRITE | NIC_CR_STA;
  for (i = 0; i < size; i++) {
    NIC_RDMAP = buf[i];
  }
  while ((NIC_ISR & NIC_ISR_RDC) == 0)
    ;
}

static int port_init()
{
#if 0 /* DRAMで設定しているので不要 */
  P1DDR = 0x1f; /* A0 - A4 */
  P3DDR = ...; /* モード５では D8-D15 の設定は不要 */
  P6DDR = ...; /* モード５では HWR,RD の設定は不要 */
#endif

#if 0
  P8DDR = 0xe8; /* CS1 */
#else
  P8DDR = 0xec; /* CS1(ether) and CS2(DRAM) */
#endif

#if 0
  P9DDR = ...; /* IERの設定によるので設定不要 */
#else
  ISCR = 0x00; /* lowレベルで割り込み */
  IPRA = 0x00;
  IER  = 0x20; /* IRQ5割り込み有効化 */
#endif
  return 0;
}

int getmacaddr(unsigned char macaddr[6]) {
	unsigned char t2[12];
	int i;
	
  read_data(0, 12, t2);
  for (i = 0; i < 6; i++) {
    macaddr[i] = t2[i * 2];
  }
  
  return 0;
}

int ether_init(int index)
{
  unsigned char t1;
  unsigned char macaddr[6];

  port_init();
        
  t1 = NIC_RP;
  NIC_RP = t1;

  mdelay(10);

  NIC_CR     = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STP; //0x21
  NIC_DCR    = NIC_DCR_F1 | NIC_DCR_LS | NIC_DCR_BOS;    //0x4A
  NIC_RBCR0  = 0x00;
  NIC_RBCR1  = 0x00;
  NIC_RCR    = NIC_RCR_MON;  //0x20
  NIC_TCR    = NIC_TCR_ILB;	 //0x02
  NIC_TPSR   = NIC_TP_START; //0x40
  NIC_PSTART = NIC_RP_START; //0x46
  NIC_BNRY   = NIC_RP_START; //0x46
  NIC_PSTOP  = NIC_RP_STOP;  //0x60
  NIC_IMR    = 0x00;
  NIC_ISR    = 0xff;

  getmacaddr(macaddr);

  NIC_CR   = NIC_CR_P1 | NIC_CR_RD_ABORT | NIC_CR_STP;
  NIC_PAR0 = macaddr[0];
  NIC_PAR1 = macaddr[1];
  NIC_PAR2 = macaddr[2];
  NIC_PAR3 = macaddr[3];
  NIC_PAR4 = macaddr[4];
  NIC_PAR5 = macaddr[5];
  NIC_CURR = NIC_RP_START + 1;
  NIC_MAR0 = 0x00;
  NIC_MAR1 = 0x00;
  NIC_MAR2 = 0x00;
  NIC_MAR3 = 0x00;
  NIC_MAR4 = 0x00;
  NIC_MAR5 = 0x00;
  NIC_MAR6 = 0x00;
  NIC_MAR7 = 0x00;

#if 0
  NIC_CR   = NIC_CR_P3 | NIC_CR_RD_ABORT | NIC_CR_STP;
  *RTL8019_9346CR  = 0xc0;
  *RTL8019_CONFIG1 = 0x80 | 0x40;
  *RTL8019_CONFIG2 = 0x00;
  *RTL8019_9346CR  = 0x00;
#endif

  NIC_CR   = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STP;
  NIC_RCR  = NIC_RCR_AM | NIC_RCR_AB | NIC_RCR_PRO; 
  NIC_CR   = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  NIC_TCR  = NIC_TCR_NORMAL;
  NIC_IMR  = 0x00;

  return 0;
}

void ether_intr_enable(int index)
{
  NIC_CR  = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  NIC_IMR = 0x01;
}

void ether_intr_disable(int index)
{
  NIC_CR  = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STP;
  NIC_IMR = 0x00;
}

int ether_checkintr(int index)
{
  unsigned char status;
  NIC_CR = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  status = NIC_ISR;
  return (status & 0x01) ? 1 : 0;
}

static int clear_irq(int index)
{
  if (ether_checkintr(index)) {
    ISR = 0x00;
    NIC_CR    = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
    NIC_ISR   = 0xff;
  }
  return 0;
}

int ether_recv(int index, char *buf)
{
  unsigned char start, curr;
  unsigned char header[4];
  int size;

  NIC_CR = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  start = NIC_BNRY + 1;

  NIC_CR = NIC_CR_P1 | NIC_CR_RD_ABORT | NIC_CR_STA;
  curr = NIC_CURR;

  if (curr < start)
    curr += NIC_RP_STOP - NIC_RP_START;
  if (start == curr)
    return 0;
  if (start == NIC_RP_STOP)
    start = NIC_RP_START;

  read_data(start * 256, 4, header);

  size = ((int)header[3] << 8) + header[2] - 4;
  read_data((start * 256) + 4, size, buf);

  NIC_CR = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  if (header[1] == NIC_RP_START)
    header[1] = NIC_RP_STOP;
  NIC_BNRY = header[1] - 1;

  clear_irq(index);
  return size;
}

int ether_send(int index, int size, char *buf)
{
  write_data(NIC_TP_START * 256, size, buf);
        
  if(size < 60)
    size = 60;

  NIC_CR    = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_STA;
  NIC_TBCR0 = size & 0xff;
  NIC_TBCR1 = (size >> 8) & 0xff;
  NIC_TPSR  = NIC_TP_START;
  NIC_CR    = NIC_CR_P0 | NIC_CR_RD_ABORT | NIC_CR_TXP | NIC_CR_STA;
  while ((NIC_CR & NIC_CR_TXP) != 0)
    ;

  return 0;
}
