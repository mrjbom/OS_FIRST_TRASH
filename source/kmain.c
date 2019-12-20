#include "lfbmemory/lfbmemory.h"
#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "memory/memory.h"
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
    init_memory();

    calclulate_memory();

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
        char str_result[128];
        sprintf(str_result, 
        "\nfirst printf\n"
        "ram_available = %L bytes\n"
        "ram_available = %L mbytes\n", ram_available, ram_available / 1024 / 1024);
        write_serial_str(str_result);

        write_serial_str("\nsecond my output\n");
        itoaUINT64((uint64_t)ram_available, str_result, 10);
        write_serial_str(str_result);
        write_serial_str(" bytes\n");
        itoaUINT64((uint64_t)ram_available / 1024 / 1024, str_result, 10);
        write_serial_str(str_result);
        write_serial_str(" mbytes\n");

        sprintf(str_result,
        "\nthird printf\n"
        "L %L and L %L\n",
        (uint64_t)123, (uint64_t)456);
        write_serial_str(str_result);

        write_serial_str("\nfourth my output\n");
        itoaUINT64((uint64_t)123, str_result, 10);
        write_serial_str(str_result);
        write_serial_str(" and ");
        itoaUINT64((uint64_t)456, str_result, 10);
        write_serial_str(str_result);
        write_serial_str("\n");

        sprintf(str_result,
        "\nfive printf\n"
        "i %i and i %i\n",
        (int32_t)123, (int32_t)456);
        write_serial_str(str_result);

        sprintf(str_result,
        "\nsix printf\n"
        "I %I and I %I\n",
        (uint32_t)123, (uint32_t)456);
        write_serial_str(str_result);
    }

}
