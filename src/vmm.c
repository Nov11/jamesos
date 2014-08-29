#include "vmm.h"
#include "isr_handler.h"

page_directory_t* current_directory;
//extern char pmm_paging_active;

void page_fault(registers_t regs)
{
	u32int fault_addr;
	asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

	prtf("page fault: %x\n", fault_addr);
	prtf("error code: %x\n", regs.err_code);
	PANIC("!!");
}

void switch_page_directory(page_directory_t* dir)
{
	current_directory = dir;
	asm volatile("mov %0, %%cr3"::"r"(dir));
}

void enable_paging()
{
	u32int cr0;
	asm volatile("mov %%cr0, %0":"=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0"::"r"(cr0));
}

void map(u32int va, u32int pa, u32int flags)
{
	/*if(pmm_paging_active == 0){
		PANIC("map with paging actived");
	}
	*/
	u32int* pd = (u32int*)PAGE_DIR_VR;
	if(pd[PAGE_DIR_IND(va)] == 0){//prtf("yes! dir:%x\n", PAGE_DIR_IND(va));
		u32int tmp = pmm_alloc_page();
		pd[PAGE_DIR_IND(va)] = tmp | PAGE_PRESENT | PAGE_WRITE;
		u32int* table_first_ptr = (u32int*)(PAGE_TBL_VR + (PAGE_DIR_IND(va) << 12));//prtf("right! ptr:%x 0xffe80000%x\n", table_first_ptr, 0xffe80000);
		memset(table_first_ptr, 0, PAGE_SIZE);
	}
	//prtf("oo\n");
	u32int* pt = (u32int*)(PAGE_TBL_VR + (PAGE_DIR_IND(va) << 12));
	pt[PAGE_TBL_IND(va)] = (pa & PAGE_MASK) | flags;
}

void unmap(u32int va)
{
	u32int ind = va >> 12 & 0xfffff;
	u32int* pt = (u32int*)PAGE_TBL_VR;
	pt[ind] = 0;
	asm volatile("invlpg (%0)" : : "a"(va));
}

char get_mapping(u32int va, u32int* pa)
{
	u32int* pd = (u32int*)PAGE_DIR_VR;
	
	if(pd[PAGE_DIR_IND(va)] == 0){
		return 0;
	}
	
	if(pa == NULL){
		return 1;
	}
	u32int* pt = (u32int*)PAGE_TBL_VR;
	*pa = pt[va >> 12 & 0xfffff] & PAGE_MASK;
	return 1;
}
	
void init_vmm()
{
	int i;
	
	reg_handler(page_fault, 14);
	
	page_directory_t* pd = (page_directory_t*)pmm_alloc_page();
	memset(pd, 0, PAGE_SIZE);
	
	pd[0] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;
	u32int* pt = (u32int*)(pd[0] & PAGE_MASK);
	if(pmm_location > 0x1000 * 0x400){
		PANIC("pmm > 4M!!!!!");
	}prtf("pmm_location %x\n", pmm_location);
	for(i = 0; i < 1024/*(pmm_location >> 12)*/; i++){
		pt[i] = (i << 12) & 0x003ff000;
		pt[i] |= PAGE_PRESENT;
		pt[i] |= PAGE_WRITE;
	}
	
	//0xffbff000 PAGE_DIR_VIRTUAL_ADDR
	//dir:1111 1111 10 :1022 table: 11 1111 1111: 1023
	//no that much point in doing this with the later one exists
	//but screw some virtual addr by this 
	//pd[1022] = pmm_alloc_page() | PAGE_WRITE | PAGE_PRESENT;
	//pt = (u32int*)(pd[1022] & PAGE_MASK);
	//memset(pt, 0, PAGE_SIZE);
	//pt[1023] = (u32int)pd | PAGE_PRESENT | PAGE_WRITE;
	//self-refer trick
	//wanna change both tables ? this is adequate
	pd[1023] = (u32int)pd | PAGE_PRESENT | PAGE_WRITE;
	
	
	//map PMM_STACK_ADDR 0xff000000 it hasn't exist in page dir
	/*u32int* tmp = (u32int*)pmm_alloc_page();
	memset(tmp, 0, PAGE_SIZE);
	pd[PAGE_DIR_IND(PMM_STACK_ADDR)] = (u32int)tmp | PAGE_PRESENT | PAGE_WRITE;
	pmm_paging_active = 1;
	*/
	
	switch_page_directory(pd);
	enable_paging();//while(1);
}
