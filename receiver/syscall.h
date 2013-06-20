#ifndef _REI_SYSCALL_H_INCLUDED_
#define _REI_SYSCALL_H_INCLUDED_

#include "defines.h"
#include "interrupt.h"

typedef enum { //Define syscall-num
  REI_SYSCALL_TYPE_RUN = 0,
  REI_SYSCALL_TYPE_EXIT,
  REI_SYSCALL_TYPE_WAIT,
  REI_SYSCALL_TYPE_SLEEP,
  REI_SYSCALL_TYPE_WAKEUP,
  REI_SYSCALL_TYPE_GETID,
  REI_SYSCALL_TYPE_CHGPRI,
  REI_SYSCALL_TYPE_MEMALLOC,
  REI_SYSCALL_TYPE_MEMFREE,
  REI_SYSCALL_TYPE_SEND,
  REI_SYSCALL_TYPE_RECV,
  REI_SYSCALL_TYPE_SETINTR,
} sys_syscall_type_t;

typedef struct { //Define param-area when systemcall called
  union {
    struct {
      sys_func_t func;
      char *name;
      int priority;
      int stacksize;
      int argc;
      char **argv;
      sys_thread_id_t ret;
    } run;

    struct {
      int dummy;
    }exit;

    struct {
      int ret;
    } wait;

    struct {
      int ret;
    } sleep;

    struct {
      sys_thread_id_t id;
      int ret;
    } wakeup;

    struct {
      sys_thread_id_t ret;
    } getid;

    struct {
      int priority;
      int ret;
    } chgpri;

    struct {
      int size;
      void *ret;
    } memalloc;

    struct {
      char *p;
      int ret;
    } memfree;

    struct {
      sys_mlbox_id_t id;
      int size;
      char *p;
      int ret;
    } send;

    struct {
      sys_mlbox_id_t id;
      int *sizep;
      char **pp;
      sys_thread_id_t ret;
    } recv;

    struct {
      softvec_type_t type;
      sys_handler_t handler;
      int ret;
    } setintr;

  } un;
} sys_syscall_param_t;

#endif
