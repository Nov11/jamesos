#ifndef PMM_H
#define PMM_H

#include "common.h"
#include "vmm.h"

#define PMM_STACK_ADDR	0xff000000

extern char pmm_paging_active;



void init_pmm(u32int);
u32int pmm_alloc_page();
void pmm_free_page(u32int);

#endif
