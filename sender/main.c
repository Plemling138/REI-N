#include "defines.h"
#include "rei.h"
#include "lib.h"
#include "interrupt.h"
#include "dram.h"
#include "serial.h"
#include "lcd.h"

//Initialize function
static int init(void){
  extern int erodata, data_start, edata, bss_start, ebss; //Defined symbol by linker script
  
  dram_init();
  memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
  memset(&bss_start, 0, (long)&ebss - (long)&bss_start);
  
  softvec_init(); //Initialize software interrupt vector
  
  serial_init(SERIAL_DEFAULT_DEVICE);
  serial_init(SERIAL_SUB_DEVICE);
  
	InitializeLCD();
	ClearLCD();
  
  return 0;
}

//Start SystemTask and UserThread
static int start_threads(int argc, char *argv[]) {
  sys_run(consdrv_main,	 	"consdrv",		 1, 0x200, 0, NULL);
  sys_run(etherdrv_main, 	"etherdrv",		 2, 0x200, 0, NULL);
  
  sys_run(command_main,		"command",		 3, 0x200, 0, NULL);
	sys_run(recv_main,			"recv",				 4, 0x200, 0, NULL);
  sys_run(send_main,			"send",				 5, 0x200, 0, NULL);
  

  sys_chgpri(15);
  EI;
  while(1) asm volatile ("sleep");

  return 0;
}

int main(void) {
  DI;
  init();

  puts("\n\n\n\nRealtime Embedded Infrastructure with Network Kernel\n\n\n");

  sys_start(start_threads, "idle", 0, 0x100, 0, NULL);

  return 0;
}
