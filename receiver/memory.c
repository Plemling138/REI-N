#include "defines.h"
#include "rei.h"
#include "lib.h"
#include "memory.h"

typedef struct _inmem_block {
  struct _inmem_block *next;
  int size;
} inmem_block;

typedef struct _inmem_pool {
  int size;
  int num;
  inmem_block *free;
} inmem_pool;

static inmem_pool pool[] = {
  {16, 32, NULL}, {32, 32, NULL}, {64, 32, NULL}, {2048, 16, NULL},
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

static int inmem_init_pool(inmem_pool *p) {
  int i;
  inmem_block *mp;
  inmem_block **mpp;
  extern char freearea;
  static char *area = &freearea;

  mp = (inmem_block *)area;

  mpp = &p->free;
  for(i = 0; i < p->num; i++) {
    *mpp = mp;
    memset(mp, 0, sizeof(*mp));
    mp->size = p->size;
    mpp = &(mp->next);
    mp = (inmem_block *)((char *)mp + p->size);
    area += p->size;
  }

  return 0;
}

int inmem_init(void) {
  int i;
  for(i = 0; i < MEMORY_AREA_NUM; i++) 
    inmem_init_pool(&pool[i]);
  return 0;
}

void *inmem_alloc(int size) {
  int i;
  inmem_block *mp;
  inmem_pool *p;

  for(i = 0; i < MEMORY_AREA_NUM; i++) {
    p = &pool[i];
    if(size <= p->size - sizeof(inmem_block)) {
      if(p->free == NULL) {
	sys_sysdown();
	return NULL;
      }
      mp = p->free;
      p->free = p->free->next;
      mp->next = NULL;

      return mp+1;
    }
  }

  sys_sysdown();
  return NULL;
}

void inmem_free(void *mem) {
  int i;
  inmem_block *mp;
  inmem_pool *p;

  mp = ((inmem_block *)mem - 1);

  for(i = 0; i < MEMORY_AREA_NUM; i++) {
    p = &pool[i];
    if(mp->size == p->size) {
      mp->next = p->free;
      p->free = mp;
      return;
    }
  }

  sys_sysdown();
}
