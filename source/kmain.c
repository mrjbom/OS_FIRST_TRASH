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
        dprintf("In kmain(): are_interrupts_enabled() ok\n");
        clear_lfb_mem(0x00FF00);
    }
    else {
        dprintf("In kmain(): are_interrupts_enabled() error\n");
        clear_lfb_mem(0xFF0000);
        dprintf("In kmain(): return from kmain()\n");
        return;
    }

    detect_cpu();
    if(cpu_info.intel_or_amd == 1)
    {
        dprintf("This is Intel processor\n");
        dprintf("CPU Family: %s\n", cpu_info.intel_family_str);
        dprintf("CPU Model: %s\n", cpu_info.intel_model_str);
        dprintf("CPU Type: %s\n", cpu_info.intel_type_str);
        dprintf("CPU Brand: %s\n", cpu_info.intel_brand_str);
    }
    else if(cpu_info.intel_or_amd == 2)
    {
        dprintf("This is AMD processor\n");
        dprintf("CPU Family: %s\n", cpu_info.amd_family_str);
        dprintf("CPU Model: %s\n", cpu_info.amd_model_str);
        dprintf("CPU Brand: %s\n", cpu_info.amd_brand_str);
    }
    else if(cpu_info.intel_or_amd == 3)
    {
        dprintf("This is unknown processor\n");
    }

    dprintf("RAM Total: %I MBytes\n", (uint32_t)(ram_len / 1024 / 1024) + 1);

    memory_page_allocator_init();

    uint32_t new_frame = allocate_frame();
    uint32_t new_frame_addr = mmap_read(new_frame, MMAP_GET_ADDR);
    dprintf("New frame allocated at: 0x%X\n", new_frame_addr);

    new_frame = allocate_frame();
    new_frame_addr = mmap_read(new_frame, MMAP_GET_ADDR);
    dprintf("New frame allocated at: 0x%X\n", new_frame_addr);

    new_frame = allocate_frame();
    new_frame_addr = mmap_read(new_frame, MMAP_GET_ADDR);
    dprintf("New frame allocated at: 0x%X\n", new_frame_addr);
}
