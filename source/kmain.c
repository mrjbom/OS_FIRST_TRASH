#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "devices/cpu/cpu.h"
#include "more/more.h"
#include "memory/mmu/mmu.h"
#include "memory/memdetect/memdetect.h"
#include "devices/pit/pit.h"
#include "task/task.h"

void kmain(unsigned long magic, multiboot_info_t* mbi) {
    (void)magic;
    init_multiboot_info(mbi);

    lfb_init();
    lfb_clear(0x00FF00);
    //clear_lfb_mem(0xFE01AC);

    lidt(IDT, sizeof(IDT));
    idt_init();
    serial_init();

    detect_cpu();

    memory_map_init();
    memory_map_calculate();

    if(!pm_init_memory_page_allocator()) {
        serial_printf("pm_init_memory_page_allocator() error!\n");
        lfb_clear(0xFF0000);
        return;
    }
    
    if(!vm_init_paging()) {
        serial_printf("vm_init_paging() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    pit_init(100);

    serial_printf("end of kmain()\n");
}