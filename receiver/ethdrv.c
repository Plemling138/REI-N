#include "defines.h"
#include "rei.h"
#include "interrupt.h"
#include "intr.h"
#include "ether.h"
#include "lib.h"
#include "ethdrv.h"

#define ETH_BUF_SIZE 1800

static void etherdrv_intr(void) {
	int size;
	char *buffer;
	if (ether_checkintr(0)) {
		buffer = svc_memalloc(ETH_BUF_SIZE);
		size = ether_recv(0, buffer);
		if (size > 0) svc_send(MLBOX_ID_ETHINPUT, size, buffer);
		else {
			svc_memfree(buffer);
		}
	}
}

static int etherdrv_init(void) {
	return 0;
}

static int etherdrv_command(int size, char *command) {
	switch(command[0]) {
	case ETHERDRV_CMD_USE:
		ether_init(0);
		ether_intr_enable(0);
		break;
	case ETHERDRV_CMD_SEND:
		ether_send(0, size-1, command+1);
		break;
	default:
		break;
	}
	return 0;
}

int etherdrv_main(int argc, char *argv[]) {
	int size;
	char *p;
	
	etherdrv_init();
	sys_setintr(SOFTVEC_TYPE_ETHINTR, etherdrv_intr);
	
	while(1) {
		sys_recv(MLBOX_ID_ETHOUTPUT, &size, &p);
		etherdrv_command(size, p);
		sys_memfree(p);
	}
	return 0;
}

