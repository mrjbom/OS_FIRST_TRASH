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

    //100 Hz = 10 millisecond
    pit_init(100);

    for(uint32_t i = 0; i < 10; ++i) {
        pit_sleep(5000);
        dprintf("%I\n", i);
    }

    dprintf("end of kmain()\n");
}