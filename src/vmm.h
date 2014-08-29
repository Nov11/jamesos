#ifndef VMM_H
#define VMM_H

#include "common.h"
#include "isr_handler.h"
#include "pmm.h"

#define PAGE_PRESENT	0x1
#define PAGE_WRITE		0x2
#define PAGE_USER		0x4
#define PAGE_MASK		0xfffff000
#define PAGE_SIZE		0x1000

//0xffbff000 dir:1111 1111 10 :1022 table: 11 1111 1111: 1023
#define PAGE_DIR_VIRTUAL_ADDR	0xffbff000 
//0xffc00000 dir:1111 1111 11 : 1023 table:0 
#define PAGE_TABLE_VIRTUAL_ADDR	0xffc00000
#define PAGE_DIR_VR				0xfffff000
#define PAGE_TBL_VR				0xffc00000


#define PAGE_DIR_IND(x)	(((u32int)(x) >> 22) & 0x3ff)
#define PAGE_TBL_IND(x)	(((u32int)(x) >> 12) & 0x3ff)
	
typedef u32int page_directory_t;

void init_vmm();

void switch_page_directory(page_directory_t*);

void map(u32int va, u32int pa, u32int);

void unmap(u32int va);

char get_mapping(u32int va, u32int* pa);

void page_fault(registers_t regs);
#endif
