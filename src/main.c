#include "monitor.h"
#include "idt.h"
#include "gdt.h"
#include "timer.h"
#include "multiboot.h"
#include "pmm.h"
#include "vmm.h"

u16int kss;
u32int kesp;
extern u32int end;
//写个struct mulitboot 省着老有警告 用的时候再改
//grub标准里有这个http://gnu.april.org/software/grub/manual/multiboot/multiboot.html
//struct multiboot{};
int kmain(struct multiboot_info* mboot_ptr)//name is mentioned in boot.s
{
	init_gdt();
	init_idt();
	monitor_write("qhello!!@#$%^&*()[]+= bcdef:wworld! 1234");
	monitor_write("\n");
	monitor_write_hex(256);
	monitor_write("finished");
	monitor_write_dec(256);
	monitor_write("done ss");
	monitor_write_hex(kss);
	monitor_write("done esp");
	monitor_write_hex(kesp);
	
	monitor_put('\n');
	//init_gdt();
	//init_idt();
	asm volatile("int $0x3");
	asm volatile("int $0x4");
	prtf("aa bb %x %u %s 11\t \nbb\n", 10, 10, "str");	
	//asm volatile("sti");
    	//init_timer(500);
	//monitor_write_hex((u32int)&end);
	prtf("1\tend is at addr :%x\n", (u32int)&end);

	/*旧的paging实现
	init_paging();
	prtf("paging enabled!\n");
	u32int* ptr = (u32int*)0xa0000000;
	*ptr = 1;
	*/
	//新的paging 
	//换了个管理物理内存的方法 这个没啥大区别
	//分割物理内存管理 虚拟内存管理
	//显式映射虚拟地址
	//pmm里搞的都是物理地址 函数返回的也是物理地址
	init_pmm (end);
	init_vmm ();
	prtf("mboot_ptr : %x\n", mboot_ptr);//大概0x2d000 没到640k呢
	prtf("mem_upper %x\n", mboot_ptr->mem_upper);
	u32int phys = align(end);
	pmm_free_page(phys);
	for(; phys < 1 << 24; phys += 0x100){
		pmm_free_page(phys);
	}
	u32int* ptr = (u32int*)0xa0000000;
	*ptr = 1;
	return 0xdeadbeef;
}
