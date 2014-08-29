#include "pmm.h"

u32int pmm_stack_loc = PMM_STACK_ADDR;
u32int pmm_stack_max = PMM_STACK_ADDR;
u32int pmm_location;

char pmm_paging_active;

void init_pmm(u32int start)
{
	pmm_location = (start + 0x1000) & PAGE_MASK;
}
/*
	what the stack is like :
------------PMM_STACK_ADDR	<- contents
--							<- contents
--							<- contents
--  <-------pmm_stack_loc   <- no contents
--							<- unused
.
.
.
maybe pmm_max_loc is here indicates the 
largest position mapped or say can be used
*/


u32int pmm_alloc_page()
{
	if(pmm_paging_active == 0){
		u32int ret = pmm_location;
		pmm_location += 0x1000;
		return ret;
	}
	
	if(pmm_stack_loc == PMM_STACK_ADDR){
		PANIC("pmm_alloc_page");
	}
	
	pmm_stack_loc -= sizeof(u32int);
	u32int* stack = (u32int*)pmm_stack_loc;
	return *stack;
}


//without paging, memory alloc by pmm_alloc_page
//is in use until os quit,so
//they won't need to be freed
void pmm_free_page(u32int p)
{
	if(pmm_paging_active == 0){
		PANIC("no free until enabled paging");
	}
	
	if(p < pmm_location){
		return;
	}
	
	if(pmm_stack_max <= pmm_stack_loc){
		map(pmm_stack_max, p, PAGE_PRESENT | PAGE_WRITE);
		pmm_stack_max += 4096;
		//prt("not likely be here");
		return;
	}
	
	u32int* stack = (u32int*)pmm_stack_loc;
	*stack = p;
	pmm_stack_loc += sizeof(u32int);	
}
