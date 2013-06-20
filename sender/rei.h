#ifndef _REI_H_INCLUDED_
#define _REI_H_INCLUDED_

#include "defines.h"
#include "interrupt.h"
#include "syscall.h"

/*SystemCall*/
sys_thread_id_t sys_run(sys_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]);
void sys_exit(void);
int sys_wait(void);
int sys_sleep(void);
int sys_wakeup(sys_thread_id_t id);
sys_thread_id_t sys_getid(void);
int sys_chgpri(int priority);
void *sys_memalloc(int size);
int sys_memfree(void *p);
int sys_send(sys_mlbox_id_t id, int size, char *p);
sys_thread_id_t sys_recv(sys_mlbox_id_t id, int *sizep, char **pp);
int sys_setintr(softvec_type_t type, sys_handler_t handler);

//ServiceCall
int svc_wakeup(sys_thread_id_t id);
void *svc_memalloc(int size);
int svc_memfree(void *p);
int svc_send(sys_mlbox_id_t id, int size, char *p);


/*Library function*/
void sys_start(sys_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]); //Start first thread
void sys_sysdown(void);
void sys_syscall(sys_syscall_type_t type, sys_syscall_param_t *param); //Run SystemCall
void svc_svccall(sys_syscall_type_t type, sys_syscall_param_t *param); //Run ServiceCall

/*SystemTasks*/
int consdrv_main(int argc, char *argv[]);
int etherdrv_main(int argc, char *argv[]);

/*UserTasks*/
int command_main(int argc, char *argv[]);
int send_main(int argc, char *argv[]);
int recv_main(int argc, char *argv[]);

#endif
