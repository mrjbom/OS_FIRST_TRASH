#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "devices/cpu/cpu.h"
#include "more/more.h"
#include "memory/mmu/mmu.h"
#include "memory/memdetect/memdetect.h"
#include "devices/timer/timer.h"
#include "task/task.h"

void kmain(unsigned long magic, multiboot_info_t* mbi) {
    (void)magic;
    init_multiboot_info(mbi);

    lfb_init();
    lfb_clear(0x00FF00);
    //clear_lfb_mem(0xFE01AC);

    lidt(IDT, sizeof(IDT));
    idt_init();
    init_serial();

    detect_cpu();

    init_memory();
    calclulate_memory();

    if(!pm_init_memory_page_allocator()) {
        dprintf("pm_init_memory_page_allocator() error!\n");
        lfb_clear(0xFF0000);
        return;
    }
    
    if(!vm_init_paging()) {
        dprintf("vm_init_paging() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    uint32_t* ptr = (uint32_t*)pm_malloc(4096);
    uint32_t* ptr2 = (uint32_t*)pm_malloc(4096);
    dprintf("ptr 0x%X = %I\n", ptr, *ptr = 123);
    dprintf("ptr2 0x%X = %I\n", ptr2, *ptr2 = 456);

    if(!vm_map_page(current_directory_table, ptr, ptr2, PAGE_PRESENT)
        ) lfb_clear(0xFF0000);

    dprintf("map_page ptr2 to ptr\n");

    dprintf("ptr 0x%X = %I\n", ptr, *ptr);
    dprintf("ptr2 0x%X = %I\n", ptr2, *ptr2);

    dprintf("map_page ptr2 to ptr2 and write 456 to ptr2\n");

    if(!vm_map_page(current_directory_table, ptr2, ptr2, PAGE_PRESENT)
        ) lfb_clear(0xFF0000);

    *ptr2 = 456;

    dprintf("ptr 0x%X = %I\n", ptr, *ptr);
    dprintf("ptr2 0x%X = %I\n", ptr2, *ptr2);

    //100 Hz = 10 millisecond
    init_timer(100);

    dprintf("end of kmain()\n");
}