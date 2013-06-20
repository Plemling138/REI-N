#include "defines.h"
#include "rei.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "memory.h"
#include "lib.h"

#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15
#define PRIORITY_NUM 16

/*Thread Context*/
typedef struct _sys_context {
  uint32 sp; //StackPointer
} sys_context;

/*Task Control Block(TCB)*/
typedef struct _sys_thread {
  struct _sys_thread *next; //Use connect to Ready-cue
  char name[THREAD_NAME_SIZE + 1]; //Thread name
  int priority;
  char *stack;
  uint32 flags;
#define SYS_THREAD_FLAG_READY (1<<0)

  struct { //Pass to thread_init()
    sys_func_t func;
    int argc;
    char **argv;
  }init;

  struct {
    sys_syscall_type_t type;
    sys_syscall_param_t *param;
  } syscall;

  sys_context context;
} sys_thread;

//MailBuffer
typedef struct _sys_mlbuf {
  struct _sys_mlbuf *next;
  sys_thread *sender;
  struct {
    int size;
    char *p;
  } param;
} sys_mlbuf;

//MailBox
typedef struct _sys_mlbox {
  sys_thread *receiver;
  sys_mlbuf *head;
  sys_mlbuf *tail;
  long dummy[1];
} sys_mlbox;

/*Threads' ReadyQue*/
static struct {
  sys_thread *head;
  sys_thread *tail;
} readyque[PRIORITY_NUM];

static sys_thread *current; //Current stack
static sys_thread threads[THREAD_NUM]; //Task Control Block
static sys_handler_t handlers[SOFTVEC_TYPE_NUM];
static sys_mlbox mlboxes[MLBOX_ID_NUM];

void dispatch(sys_context *context);

//Extract CurrentThread from ReadyQue
static int getcurrent(void) {
  if (current == NULL) {
    return -1;
  }
  if(!(current->flags & SYS_THREAD_FLAG_READY)) return 1;

  //extract from top
  readyque[current->priority].head = current->next;
  if (readyque[current->priority].head == NULL) readyque[current->priority].tail = NULL;
  current->flags &= ~SYS_THREAD_FLAG_READY;
  current->next = NULL;

  return 0;
}

//Connect CurrentThread for ReadyQue
static int putcurrent(void) {
  if (current == NULL) return -1;
  if (current->flags & SYS_THREAD_FLAG_READY)  return 1;

  //Connect tail of ReadyQue
  if(readyque[current->priority].tail) {
    readyque[current->priority].tail->next = current;
  }
  else readyque[current->priority].head = current;

  readyque[current->priority].tail = current;
  current->flags |= SYS_THREAD_FLAG_READY;

  return 0;
}

//End thread
static void thread_end(void) {
  sys_exit();
}

//Startup thread
static void thread_init(sys_thread *thp){
  //Call threads' main func
  thp->init.func(thp->init.argc, thp->init.argv);
  thread_end(); //return main, end thread
}

//Start thread
static sys_thread_id_t thread_run(sys_func_t func, char *name, int priority, int stacksize, int argc, char **argv) {
  int i;
  sys_thread *thp;
  uint32 *sp;
  extern char userstack;
  static char *thread_stack = &userstack;

  //Search blank TCB
  for(i=0;i<THREAD_NUM;i++) {
    thp = &threads[i];
    if(!thp->init.func) break; //found
    if(i == THREAD_NUM) return -1; //not found
  }

    memset(thp,0,sizeof(*thp));

    //Settings of TCB
    strcpy(thp->name, name);
    thp->next = NULL;
    thp->priority = priority;
    thp->flags = 0;
    thp->init.func = func;
    thp->init.argc = argc;
    thp->init.argv = argv;

    //Get StackArea
    memset(thread_stack, 0, stacksize);
    thread_stack += stacksize;

    thp->stack = thread_stack;

    //Initialize stack
    sp = (uint32 *)thp->stack;
    *(--sp) = (uint32)thread_end;

    //Setting ProgramCounter
    *(--sp) = (uint32)thread_init | ((uint32)(priority ? 0 : 0xc0) << 24);

    *(--sp) = 0; //ER6
    *(--sp) = 0; //ER5
    *(--sp) = 0; //ER4
    *(--sp) = 0; //ER3
    *(--sp) = 0; //ER2
    *(--sp) = 0; //ER1

    //Pass to Startup thread:first value
    *(--sp) = (uint32)thp;

    //Set Thread-Context
    thp->context.sp = (uint32)sp;

    //Return new-thread to ReadyQue
    putcurrent();

    //Connect ReadyQue to new-thread
    current = thp;
    putcurrent();

    return (sys_thread_id_t)current;
}

//End thread
static int thread_exit(void) {
  puts("[");
  puts(current->name);
  puts(" end.]\n");
  memset(current, 0, sizeof(*current)); //Clear TCB
  return 0;
}

//cancel Thread-runnning
static int thread_wait(void) {
  putcurrent();
  return 0;
}

//thread sleep
static int thread_sleep(void) {
  return 0;
}

//thread wakeup
static int thread_wakeup(sys_thread_id_t id) {
  putcurrent();

  current = (sys_thread *)id;
  putcurrent();

  return 0;
}

//Get threadID
static sys_thread_id_t thread_getid(void) {
  putcurrent();
  return (sys_thread_id_t)current;
}

//change thread priority
static int thread_chgpri(int priority) {
  int old = current->priority;
  if(priority >= 0) current->priority = priority;
  putcurrent();
  return old;
}

static void *thread_memalloc(int size) {
  putcurrent();
  return inmem_alloc(size);
}

static int thread_memfree(char *p) {
  inmem_free(p);
  putcurrent();
  return 0;
}

static void sendml(sys_mlbox *mboxp, sys_thread *thp, int size, char *p) {
  sys_mlbuf *mp;

  mp = (sys_mlbuf *)inmem_alloc(sizeof(*mp));
  if(mp == NULL) sys_sysdown();

  mp->next = NULL;
  mp->sender = thp;
  mp->param.size = size;
  mp->param.p = p;

  if(mboxp->tail) mboxp->tail->next = mp;
  else mboxp->head = mp;

  mboxp->tail = mp;
}

static void recvml(sys_mlbox *mboxp) {
  sys_mlbuf *mp;
  sys_syscall_param_t *p;

  mp = mboxp->head;
  mboxp->head = mp->next;
  if(mboxp->head == NULL) mboxp->tail = NULL;
  mp->next = NULL;

  p = mboxp->receiver->syscall.param;
  p->un.recv.ret = (sys_thread_id_t)mp->sender;
  if(p->un.recv.sizep)
    *(p->un.recv.sizep) = mp->param.size;
  if(p->un.recv.pp)
    *(p->un.recv.pp) = mp->param.p;

  mboxp->receiver = NULL;

  inmem_free(mp);
}

static int thread_send(sys_mlbox_id_t id, int size, char *p) {
  sys_mlbox *mboxp = &mlboxes[id];

  putcurrent();
  sendml(mboxp, current, size, p);

  if(mboxp->receiver) {
    current = mboxp->receiver;
    recvml(mboxp);
    putcurrent();
  }

  return size;
}

static sys_thread_id_t thread_recv(sys_mlbox_id_t id, int *sizep, char **pp) {
  sys_mlbox *mboxp = &mlboxes[id];

  if(mboxp->receiver) sys_sysdown();

  mboxp->receiver = current;

  if(mboxp->head == NULL) return -1;

  recvml(mboxp);
  putcurrent();

  return current->syscall.param->un.recv.ret;
}

//Entry interrupt handler
static int thread_setintr(softvec_type_t type, sys_handler_t handler) {
  static void thread_intr(softvec_type_t type, unsigned long sp);

  softvec_setintr(type, thread_intr);

  handlers[type] = handler;
  putcurrent();

  return 0;
}

//Call SystemCall
static void call_functions(sys_syscall_type_t type, sys_syscall_param_t *p) {
  switch(type){
  case REI_SYSCALL_TYPE_RUN: //sys_run()
    p->un.run.ret = thread_run(p->un.run.func, p->un.run.name, p->un.run.priority, p->un.run.stacksize, p->un.run.argc, p->un.run.argv);
    break;
  case REI_SYSCALL_TYPE_EXIT: //sys_exit()
    //Don't write return value
    thread_exit();
    break;
  case REI_SYSCALL_TYPE_WAIT:
    p->un.wait.ret = thread_wait();
    break;
  case REI_SYSCALL_TYPE_SLEEP:
    p->un.sleep.ret = thread_sleep();
    break;
  case REI_SYSCALL_TYPE_WAKEUP:
    p->un.wakeup.ret = thread_wakeup(p->un.wakeup.id);
    break;
  case REI_SYSCALL_TYPE_GETID:
    p->un.getid.ret = thread_getid();
    break;
  case REI_SYSCALL_TYPE_CHGPRI:
    p->un.chgpri.ret = thread_chgpri(p->un.chgpri.priority);
    break;
  case REI_SYSCALL_TYPE_MEMALLOC:
    p->un.memalloc.ret = thread_memalloc(p->un.memalloc.size);
    break;
  case REI_SYSCALL_TYPE_MEMFREE:
    p->un.memfree.ret = thread_memfree(p->un.memfree.p);
    break;
  case REI_SYSCALL_TYPE_SEND:
    p->un.send.ret = thread_send(p->un.send.id, p->un.send.size, p->un.send.p);
    break;
  case REI_SYSCALL_TYPE_RECV:
    p->un.recv.ret = thread_recv(p->un.recv.id, p->un.recv.sizep, p->un.recv.pp);
    break;
  case REI_SYSCALL_TYPE_SETINTR:
    p->un.setintr.ret = thread_setintr(p->un.setintr.type, p->un.setintr.handler);
    break;
  default:
    break;
  }
}

//Task SystemCall
static void syscall_proc(sys_syscall_type_t type, sys_syscall_param_t *p){
  getcurrent();
  call_functions(type, p);
}

static void svccall_proc(sys_syscall_type_t type, sys_syscall_param_t *p) {
  current = NULL;
  call_functions(type, p);
}

//Threads' scheduling
static void schedule(void) {
  int i;

  for(i=0;i<PRIORITY_NUM; i++) {
    if(readyque[i].head) break;
  }
  if(i == PRIORITY_NUM) //Que is not found
    sys_sysdown();

  current = readyque[i].head;
}

static void syscall_intr(void) {
  syscall_proc(current->syscall.type, current->syscall.param);
}

static void softerr_intr(void) {
  puts("[");
  puts(current->name);
  puts(" is down!]\n");
  getcurrent();
  thread_exit();
}

//Entry Interrupt task
static void thread_intr(softvec_type_t type, unsigned long sp) {
  current->context.sp = sp;

  if(handlers[type]) handlers[type]();

  schedule();

  dispatch(&current->context);
}

//start first thread
void sys_start(sys_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]) {
  inmem_init();

  current = NULL;
  memset(readyque, 0, sizeof(readyque));
  memset(threads,  0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));
  memset(mlboxes,  0, sizeof(mlboxes));

  //Entry interrupt handler
  thread_setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
  thread_setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

  current = (sys_thread *)thread_run(func, name, priority, stacksize, argc, argv);

  dispatch(&current->context);

}

void sys_sysdown(void) {
	extern void start(void);

  puts("[System Error!]\n");
  start();
}

//Library-func for call syscall
void sys_syscall(sys_syscall_type_t type, sys_syscall_param_t *param) {
  current->syscall.type = type;
  current->syscall.param = param;
  asm volatile ("trapa #0"); //Trap interrput
}

void svc_svccall(sys_syscall_type_t type, sys_syscall_param_t *param) {
  svccall_proc(type, param);
}
