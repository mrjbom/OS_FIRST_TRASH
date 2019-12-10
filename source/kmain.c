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

    uint64_t ram_len;

    while((uint32_t)memory_map < MBI->mmap_addr + MBI->mmap_length) {
	    memory_map = (multiboot_memory_map_t*)((unsigned int)memory_map + memory_map->size + sizeof(memory_map->size));
        ram_len += memory_map->len;
    }
    char str_result[16];
    itoaINT32(MBI->mem_lower, str_result, 10);
    write_serial_str("mem_lower = ");
    write_serial_str(str_result);
    write_serial('\n');
    itoaINT32(MBI->mem_upper, str_result, 10);
    write_serial_str("mem_upper = ");
    write_serial_str(str_result);
    write_serial('\n');
    itoaINT32(ram_len, str_result, 10);
    write_serial_str("ram_len = ");
    write_serial_str(str_result);
    write_serial('\n');
}
