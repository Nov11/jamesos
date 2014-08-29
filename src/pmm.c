#include "pmm.h"

//u32int pmm_stack_loc = PMM_STACK_ADDR;
//u32int pmm_stack_max = PMM_STACK_ADDR;
u32int pmm_location;
//char pmm_paging_active;

u32int* frames;
u32int nframes;

void set_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

void clear_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

u32int test_frame(u32int frame_addr)
{
	u32int frame = frame_addr / 0x1000;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	return frames[idx] & (0x1 << off);
}

u32int first_frame()
{
	u32int i, j;
	
	for(i = 0; i < INDEX_FROM_BIT(nframes); i++){
		if(frames[i] != 0xffffffff){
			for(j = 0; j < 32; j++){
				u32int mask = 0x1 << j;
				if(!(frames[i] & mask)){
					return i * 32 + j;
				}
			}
		}
	}
	return 0xffffffff;//应该加个参数 否则无法分辨是成功分配了还是满了
}

u32int alloc_frame()
{
	u32int idx = first_frame();
	if(idx == 0xffffffff){
		//full先这么着，换页的事儿后面再写
		PANIC("full");
	}
	u32int ret = idx * 0x1000;
	set_frame(ret);
	return ret;
}

void free_frame(u32int p)
{
	clear_frame(p);
}




void init_pmm(u32int start, u32int end)
{	
	//32MB最多(25) 那就是8k个页(13 + 12) 256个u32int(8 + 5 + 12)
	//一个页面足够存了
	pmm_location = align(start);
	frames = (u32int*)pmm_location;
	memset(frames, 0, 0x1000);
	pmm_location += 0x1000;
	u32int iter;
	for(iter = 0; iter < pmm_location; iter += 0x100){
		set_frame(iter);
		nframes++;
	}
	for(iter = pmm_location; iter < end; iter += 0x100){
		free_frame(iter);
		nframes++;
	}
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
	/*if(pmm_paging_active == 0){
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
	*/
	u32int ret = alloc_frame();
	return ret;
}


//without paging, memory alloc by pmm_alloc_page
//is in use until os quit,so
//they won't need to be freed
void pmm_free_page(u32int p)
{
	/*if(pmm_paging_active == 0){
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
	*/
	free_frame(p);
}
