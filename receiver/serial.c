#include "defines.h"
#include "serial.h"

#define SERIAL_SCI_NUM 3

//Define SCI
#define H8_3069F_SCI0 ((volatile struct h8_3069f_sci *)0xFFFFB0)
#define H8_3069F_SCI1 ((volatile struct h8_3069f_sci *)0xFFFFB8)
#define H8_3069F_SCI2 ((volatile struct h8_3069f_sci *)0xFFFFC0)

//Define SCI Resisters
struct h8_3069f_sci {
  volatile uint8 smr;
  volatile uint8 brr;
  volatile uint8 scr;
  volatile uint8 tdr;
  volatile uint8 ssr;
  volatile uint8 rdr;
  volatile uint8 scmr;
};

//Define SMR Bit 
#define H8_3069F_SCI_SMR_CKS_PER1  (0<<0)
#define H8_3069F_SCI_SMR_CKS_PER4  (1<<0)
#define H8_3069F_SCI_SMR_CKS_PER16 (2<<0)
#define H8_3069F_SCI_SMR_CKS_PER64 (3<<0)
#define H8_3069F_SCI_SMR_MP        (1<<2)
#define H8_3069F_SCI_SMR_STOP      (1<<3)
#define H8_3069F_SCI_SMR_OE        (1<<4)
#define H8_3069F_SCI_SMR_PE        (1<<5)
#define H8_3069F_SCI_SMR_CHR       (1<<6)
#define H8_3069F_SCI_SMR_CA        (1<<7)

#define H8_3069F_SCI_SCR_CKE0      (1<<0)
#define H8_3069F_SCI_SCR_CKE1      (1<<1)
#define H8_3069F_SCI_SCR_TEIE      (1<<2)
#define H8_3069F_SCI_SCR_MPIE      (1<<3)
#define H8_3069F_SCI_SCR_RE        (1<<4) //Receive OK
#define H8_3069F_SCI_SCR_TE        (1<<5) //Send OK
#define H8_3069F_SCI_SCR_RIE       (1<<6) //Enable Receive Intrupt
#define H8_3069F_SCI_SCR_TIE       (1<<7) //Enable Transmit Intrupt

//Define SSR Bit
#define H8_3069F_SCI_SSR_MPBT      (1<<0)
#define H8_3069F_SCI_SSR_MPB       (1<<1)
#define H8_3069F_SCI_SSR_TEND      (1<<2)
#define H8_3069F_SCI_SSR_PER       (1<<3)
#define H8_3069F_SCI_SSR_FERERS    (1<<4)
#define H8_3069F_SCI_SSR_ORER      (1<<5)
#define H8_3069F_SCI_SSR_RDRF      (1<<6) //End Receive
#define H8_3069F_SCI_SSR_TDRE      (1<<7) //End Send

static struct {
  volatile struct h8_3069f_sci *sci;
}
  regs[SERIAL_SCI_NUM] = {
    {H8_3069F_SCI0},
    {H8_3069F_SCI1},
    {H8_3069F_SCI2},
  };

//Initialize Dev
int serial_init(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  sci -> tdr = 0;
  sci -> smr = 0;
  sci -> brr = 32;
  sci -> scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE;
  sci -> ssr = 0;

  return 0;
}

int serial_init_96(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  sci -> tdr = 0;
  sci -> smr = 0;
  sci -> brr = 64;
  sci -> scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE;
  sci -> ssr = 0;

  return 0;
}

//Whether enable Sending or not?
int serial_is_send_enable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  return (sci->ssr & H8_3069F_SCI_SSR_TDRE);
}

//send 1 char
int serial_send_byte(int index, unsigned char c){
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  //wait for send enable
  while(!serial_is_send_enable(index));
  sci->tdr = c;
  sci->ssr &= ~H8_3069F_SCI_SSR_TDRE;

  return 0;
}

//Whether enable Receiving or not?
int serial_is_recv_enable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  return (sci -> ssr & H8_3069F_SCI_SSR_RDRF);
}

//recv 1 char
unsigned char serial_recv_byte(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  unsigned char c;

  //wait for recv enable
  while(!serial_is_recv_enable(index));
  c = sci -> rdr;
  sci -> ssr &= ~H8_3069F_SCI_SSR_RDRF; //complete receiving

  return c;
}

//whether serial interruptor is send enable
int serial_intr_is_send_enable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  return (sci->scr & H8_3069F_SCI_SCR_TIE) ? 1 : 0;
}

//Send-interruptor enable
void serial_intr_send_enable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  sci->scr |= H8_3069F_SCI_SCR_TIE;
}

//Send-interruptor disable
void serial_intr_send_disable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  sci->scr &= ~H8_3069F_SCI_SCR_TIE;
}

//Whether serial interruptor is recv enable
int serial_intr_is_recv_enable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  return (sci->scr & H8_3069F_SCI_SCR_RIE) ? 1 : 0;
}

//Recv-interruptor enable
void serial_intr_recv_enable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  sci->scr |= H8_3069F_SCI_SCR_RIE;
}

//Recv-interruptor disable
void serial_intr_recv_disable(int index){
  volatile struct h8_3069f_sci *sci = regs[index].sci;
  sci->scr &= ~H8_3069F_SCI_SCR_RIE;
}
