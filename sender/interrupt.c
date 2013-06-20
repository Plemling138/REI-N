#include "defines.h"
#include "intr.h"
#include "interrupt.h"

//Initialize software interrupt vector
int softvec_init(void){
  int type;
  for(type=0;type<SOFTVEC_TYPE_NUM;type++)
    softvec_setintr(type, NULL);
  return 0;
}

//Define Software interrput vector
int softvec_setintr(softvec_type_t type, softvec_handler_t handler){
  SOFTVECS[type] = handler;
  return 0;
}

//Common interrupt handler
//Switch handler by software interrupt vector

void interrupt(softvec_type_t type, unsigned long sp){
  softvec_handler_t handler = SOFTVECS[type];
  if(handler) handler(type, sp);
}
