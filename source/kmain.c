#include "kernel/kernel.h"
//stdlib
#include "lib/cstdlib.h"
#include "lib/stddef.h"
#include "lib/stdint.h"

#include "lfbmemory/lfbmemory.h"
#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "debug/debug.h"
#include "devices/cpu/cpu.h"

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
        write_serial_str("In kmain(): are_interrupts_enabled() ok\n");
        clear_lfb_mem(0x00FF00);
    }
    else {
        write_serial_str("In kmain(): are_interrupts_enabled() error\n");
        clear_lfb_mem(0xFF0000);
        write_serial_str("In kmain(): return from kmain()\n");
        return;
    }

    {
        char str_result[16];
        //Bytes to Megabytes
        //itoaINT32(ram_available / 1024 / 1024, str_result, 10);
        write_serial_str("ram_available = ");
        write_serial_str(str_result);
        write_serial_str(" MBytes\n");
    }

    detect_cpu();
}
