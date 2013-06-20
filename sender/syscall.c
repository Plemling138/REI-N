#include "syscall.h"
#include "rei.h"
#include "defines.h"
#include "interrupt.h"

sys_thread_id_t sys_run(sys_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]) {
  sys_syscall_param_t param;
  param.un.run.func = func;
  param.un.run.name = name;
  param.un.run.priority = priority;
  param.un.run.stacksize = stacksize;
  param.un.run.argc = argc;
  param.un.run.argv = argv;
  sys_syscall(REI_SYSCALL_TYPE_RUN, &param);
  return param.un.run.ret;
}

void sys_exit(void) {
  sys_syscall(REI_SYSCALL_TYPE_EXIT, NULL);
}

int sys_wait(void) {
  sys_syscall_param_t param;
  sys_syscall(REI_SYSCALL_TYPE_WAIT, &param);
  return param.un.wait.ret;
}

int sys_sleep(void) {
  sys_syscall_param_t param;
  sys_syscall(REI_SYSCALL_TYPE_SLEEP, &param);
  return param.un.sleep.ret;
}

int sys_wakeup(sys_thread_id_t id) {
  sys_syscall_param_t param;
  param.un.wakeup.id = id;
  sys_syscall(REI_SYSCALL_TYPE_WAKEUP, &param);
  return param.un.wakeup.ret;
}

sys_thread_id_t sys_getid(void) {
  sys_syscall_param_t param;
  sys_syscall(REI_SYSCALL_TYPE_GETID, &param);
  return param.un.getid.ret;
}

int sys_chgpri(int priority) {
  sys_syscall_param_t param;
  param.un.chgpri.priority = priority;
  sys_syscall(REI_SYSCALL_TYPE_CHGPRI, &param);
  return param.un.chgpri.ret;
}

void *sys_memalloc(int size) {
  sys_syscall_param_t param;
  param.un.memalloc.size = size;
  sys_syscall(REI_SYSCALL_TYPE_MEMALLOC, &param);
  return param.un.memalloc.ret;
}

int sys_memfree(void *p) {
  sys_syscall_param_t param;
  param.un.memfree.p = p;
  sys_syscall(REI_SYSCALL_TYPE_MEMFREE, &param);
  return param.un.memfree.ret;
}

int sys_send(sys_mlbox_id_t id, int size, char *p) {
  sys_syscall_param_t param;
  param.un.send.id = id;
  param.un.send.size = size;
  param.un.send.p = p;
  sys_syscall(REI_SYSCALL_TYPE_SEND, &param);
  return param.un.send.ret;
}

sys_thread_id_t sys_recv(sys_mlbox_id_t id, int *sizep, char **pp) {
  sys_syscall_param_t param;
  param.un.recv.id = id;
  param.un.recv.sizep = sizep;
  param.un.recv.pp = pp;
  sys_syscall(REI_SYSCALL_TYPE_RECV, &param);
  return param.un.recv.ret;
}

int sys_setintr(softvec_type_t type, sys_handler_t handler) {
  sys_syscall_param_t param;
  param.un.setintr.type = type;
  param.un.setintr.handler = handler;
  sys_syscall(REI_SYSCALL_TYPE_SETINTR, &param);
  return param.un.setintr.ret;
}

int svc_wakeup(sys_thread_id_t id) {
  sys_syscall_param_t param;
  param.un.wakeup.id = id;
  svc_svccall(REI_SYSCALL_TYPE_WAKEUP, &param);
  return param.un.wakeup.ret;
}

void *svc_memalloc(int size) {
  sys_syscall_param_t param;
  param.un.memalloc.size = size;
  svc_svccall(REI_SYSCALL_TYPE_MEMALLOC, &param);
  return param.un.memalloc.ret;
}

int svc_memfree(void *p) {
  sys_syscall_param_t param;
  param.un.memfree.p = p;
  svc_svccall(REI_SYSCALL_TYPE_MEMFREE, &param);
  return param.un.memfree.ret;
}

int svc_send(sys_mlbox_id_t id, int size, char *p) {
  sys_syscall_param_t param;
  param.un.send.id = id;
  param.un.send.size = size;
  param.un.send.p = p;
  svc_svccall(REI_SYSCALL_TYPE_SEND, &param);
  return param.un.send.ret;
}
