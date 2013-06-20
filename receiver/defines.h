#ifndef _DEFINES_H_INCLUDED_
#define _DEFINES_H_INCLUDED_

#define NULL ((void *)0) // define NULL Pointer
#define SERIAL_DEFAULT_DEVICE 1 //Default Serial Dev
#define SERIAL_SUB_DEVICE 0 //Default Serial Dev

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

typedef uint32 sys_thread_id_t;
typedef int (*sys_func_t)(int argc, char *argv[]);
typedef void (*sys_handler_t)(void);

typedef enum {
  MLBOX_ID_CONSINPUT = 0,
  MLBOX_ID_CONSOUTPUT,
  MLBOX_ID_ETHINPUT,
  MLBOX_ID_ETHOUTPUT,
  MLBOX_ID_ETHSEND,
  MLBOX_ID_SENDCMD,
  MLBOX_ID_USRBOX1,
  MLBOX_ID_USRBOX2,
  MLBOX_ID_USRBOX3,
  MLBOX_ID_NUM
} sys_mlbox_id_t;

#endif
