#include "more.h"

multiboot_info_t* MBI;

void init_multiboot_info(multiboot_info_t* mbi_temp) {
    MBI = mbi_temp;
}

void show_base_info()
{
    //check interruptions
    if(are_interrupts_enabled()) {
        dprintf("In kmain(): are_interrupts_enabled() ok\n");
    }
    else {
        dprintf("ERROR! are_interrupts_enabled() error\n");
    }

    //show processor info
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
    dprintf("RAM sectors info:\n");
    for(uint32_t i = 0; i < 6; ++i)
    {
        if(memory_sections[i].type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            dprintf("Avaiable RAM addr: 0x%X\n", memory_sections[i].address);
            dprintf("    Length: %I bytes(%I kilobytes)\n", memory_sections[i].length, memory_sections[i].length / 1024);
        }
        else
        {
            dprintf("Unavaiable RAM addr: 0x%X\n", memory_sections[i].address);
        }
    }
    dprintf("Kernel start: 0x%X end: 0x%X size: %I bytes(%I MBytes)\n", (uint32_t)startkernel, (uint32_t)endkernel, (uint32_t)endkernel - (uint32_t)startkernel, (uint32_t)(endkernel - startkernel) / 1024 / 1024);
    dprintf("multiboot info start: 0x%X end: 0x%X\n", (uint32_t)multiboot_reserved_start, (uint32_t)multiboot_reserved_end);
}
