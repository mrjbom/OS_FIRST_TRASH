#include "lfbmemory/lfbmemory.h"
#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "memory/memdetect/memdetect.h"
#include "memory/memmmu/memmmu.h"
#include "debug/debug.h"
#include "devices/cpu/cpu.h"
#include "more/more.h"
#include "lib/cstdlib.h"

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
    if(!init_memory_page_allocator())
    {
        dprintf("!!!MMAP ERROR!!!\n");
        return;
    }

    show_base_info();
    clear_lfb_mem(0x00FF00);
    //---------------------

    //test
    show_npages_table(25);
    uint32_t* arr1 = (uint32_t*)kmalloc(sizeof(uint32_t) * 10);
    for(uint32_t i = 0; i < 10; ++i)
    {
        arr1[i] = i;
    }
    uint32_t* arr2 = (uint32_t*)kmalloc(sizeof(uint32_t) * 10);
    for(uint32_t i = 0; i < 10; ++i)
    {
        arr2[i] = i * 2;
    }
    show_npages_table(25);

    for(uint32_t i = 0; i < 10; ++i)
    {
        dprintf("arr1[%I] = %I\n", i, arr1[i]);
    }

    memcpy(arr1, arr2, sizeof(uint32_t) * 10);
    dprintf("\n");

    for(uint32_t i = 0; i < 10; ++i)
    {
        dprintf("arr1[%I] = %I\n", i, arr1[i]);
    }

    kfree(arr1);
    kfree(arr2);
    //---------
    show_npages_table(25);

    void* ptr = kmalloc(27000);

    show_npages_table(25);

    kfree(ptr);

    show_npages_table(25);

    dprintf("end of kmain()\n");
}