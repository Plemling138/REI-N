#include "defines.h"

extern void start(void);
extern void intr_softerr(void);
extern void intr_syscall(void);
extern void intr_serintr(void);
extern void intr_timintr(void);
extern void intr_ethintr(void);

/*割込みvectorの設定*/

void (*vectors[])(void) = {
  start,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
  intr_syscall,intr_softerr,intr_softerr,intr_softerr,
  NULL,NULL,NULL,NULL,NULL,intr_ethintr,NULL,NULL,
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
  intr_timintr,intr_timintr,intr_timintr,intr_timintr,
  intr_timintr,intr_timintr,intr_timintr,intr_timintr,
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
  intr_serintr,intr_serintr,intr_serintr,intr_serintr,
  intr_serintr,intr_serintr,intr_serintr,intr_serintr,
  intr_serintr,intr_serintr,intr_serintr,intr_serintr,
};

