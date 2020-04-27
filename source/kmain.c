#include "more/more.h"

void kmain(unsigned long magic, multiboot_info_t* mbi) {
    (void)magic;
    init_multiboot_info(mbi);

    init_lfb_mem();
    //clear_lfb_mem(0xFE01AC);

    lidt(IDT, sizeof(IDT));
    idt_init();
    init_serial();

    detect_cpu();

    init_memory();
    calclulate_memory();
    if(!init_memory_page_allocator()) {
        dprintf("!!!MMAP ERROR!!!\n");
        return;
    }

    show_base_info();
    clear_lfb_mem(0x00FF00);

    init_tasking();

    //test_func();

    dprintf("end of kmain()\n");
}