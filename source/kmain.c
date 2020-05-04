#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "devices/cpu/cpu.h"
#include "more/more.h"

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
    if(!init_memory_page_allocator()) {
        dprintf("init_memory_page_allocator() ERROR!\n");
        lfb_clear(0xFF0000);
        return;
    }

    show_npages_table(1);

    dprintf("end of kmain()\n");
}