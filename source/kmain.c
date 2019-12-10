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

    char* str_result;
    itoa(MBI->mem_lower, str_result, 10);
    write_serial_str("mem_lower = ");
    write_serial_str(str_result);
    write_serial('\n');
    itoa(MBI->mem_upper, str_result, 10);
    write_serial_str("mem_upper = ");
    write_serial_str(str_result);
    write_serial('\n');

    if(are_interrupts_enabled()) {
        clear_lfb_mem(0x00FF00);
    }
    else {
        clear_lfb_mem(0xFF0000);
    }
}
