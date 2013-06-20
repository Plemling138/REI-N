#ifndef _REI_MEMORY_H_INCLUDED_
#define _REI_MEMORY_H_INCLUDED_

int inmem_init(void);
void *inmem_alloc(int size);
void inmem_free(void *mem);

#endif
