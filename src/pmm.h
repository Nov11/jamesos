#ifndef PMM_H
#define PMM_H

#include "common.h"

#define PAGE_SIZE		0x1000
//#define PMM_STACK_ADDR	0xff000000

#define INDEX_FROM_BIT(a) ((a) / (8 * 4))
#define OFFSET_FROM_BIT(a) ((a) % (8 * 4))

extern char pmm_paging_active;
extern u32int pmm_location;

void init_pmm(u32int, u32int);
u32int pmm_alloc_page();
void pmm_free_page(u32int);

#endif
