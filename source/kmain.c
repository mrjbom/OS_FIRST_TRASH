#include "kernel/kernel.h"
#include "lfbmemory/lfbmemory.h"
#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "debug/debug.h"
#include "cstdfunctions/cstdfunctions.h"

void kmain(unsigned long magic, multiboot_info_t* mbi)
{
    (void)magic;
    init_multiboot_info(mbi);

    init_lfb_mem();
    //clear_lfb_mem(0xFE01AC);

    lidt(IDT, sizeof(IDT));
    idt_init();
    init_serial();

    if(are_interrupts_enabled()) {
        clear_lfb_mem(0x00FF00);
    }
    else {
        clear_lfb_mem(0xFF0000);
        return;
    }

    multiboot_memory_map_t* memory_map = (multiboot_memory_map_t*)MBI->mmap_addr;

    //in bytes
    uint32_t ram_len;
    uint32_t ram_available;

    int memory_map_addr = mbi->mmap_addr;
	int memory_map_len = mbi->mmap_length;

    while (memory_map < (void*)(memory_map_addr + memory_map_len))
    {
        ram_len += memory_map->len;
        if(memory_map->type == 1)
            ram_available += memory_map->len;
        memory_map = (multiboot_memory_map_t*)((unsigned int)memory_map + memory_map->size + sizeof(memory_map->size));
    }


    {
        char str_result[16];
        itoaINT32(ram_available, str_result, 10);
        write_serial_str("ram_available = ");
        write_serial_str(str_result);
        write_serial('\n');
    }
}
