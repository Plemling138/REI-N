#include "defines.h"
#include "rei.h"
#include "consdrv.h"
#include "lib.h"

static void send_use(int index) {
  char *p;
  p = sys_memalloc(3);
  p[0] = '0';
  p[1] = CONSDRV_CMD_USE;
  p[2] = '0' + index;
  sys_send(MLBOX_ID_CONSOUTPUT, 3, p);
}

static void send_write(char *str) {
  char *p;
  int len;
  len = strlen(str);
  p = sys_memalloc(len+2);
  p[0] = '0';
  p[1] = CONSDRV_CMD_WRITE;
  memcpy(&p[2], str, len);
  sys_send(MLBOX_ID_CONSOUTPUT, len+2, p);
}

int command_main(int argc, char *argv[]) {
  char *p;
  int size;

  send_use(SERIAL_DEFAULT_DEVICE);

  while(1) {
    send_write("REI > ");

    sys_recv(MLBOX_ID_CONSINPUT, &size, &p);
    p[size] = '\0';

    if(!strncmp(p, "echo", 4)) {
      send_write(p+4);
      send_write("\n");
    }
    else if(!strncmp(p, "", 1)) send_write("");
    else send_write("[Undefined command!!]\n");

    sys_memfree(p);
  }

  return 0;
}
