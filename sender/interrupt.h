#ifndef _INTERRUPT_H_INCLUDED_
#define _INTERRUPT_H_INCLUDED_

//defined linker script
extern char softvec;
#define SOFTVEC_ADDR (&softvec)

typedef short softvec_type_t; //Define type of Software-Inteerupt vector

typedef void (*softvec_handler_t)(softvec_type_t type, unsigned long sp); //Define interrupt-handler

#define SOFTVECS ((softvec_handler_t *)SOFTVEC_ADDR)

#define EI asm volatile ("andc.b #0x3f,ccr") //Enable Interrputor
#define DI asm volatile ("orc.b #0xc0,ccr") //Disable Interruptor

int softvec_init(void);

int softvec_setintr(softvec_type_t type, softvec_handler_t handler);

/*Common Interrupt handler*/
void interrupt(softvec_type_t type, unsigned long sp);

#endif
