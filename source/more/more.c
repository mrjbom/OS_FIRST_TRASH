#include "more.h"
#include "../devices/cpu/cpu.h"
#include "../inlineassembly/inlineassembly.h"
#include "../memory/memdetect/memdetect.h"
#include "../memory/mmu/mmu.h"

multiboot_info_t* MBI;

void init_multiboot_info(multiboot_info_t* mbi_temp) {
    MBI = mbi_temp;
}


void show_base_info(ssfn_text_cursor_t* text_cursor)
{
    //check interruptions
    if(are_interrupts_enabled()) {
        dprintf("are_interrupts_enabled() ok\n");
    }
    else {
        dprintf("ERROR! are_interrupts_enabled() error\n");
    }

    //show processor info
    if(cpu_info.intel_or_amd == 1) {
        dprintf("This is Intel processor\n");
        dprintf("CPU Family: %s\n", cpu_info.intel_family_str);
        dprintf("CPU Model: %s\n", cpu_info.intel_model_str);
        //dprintf("CPU Type: %s\n", cpu_info.intel_type_str);
        dprintf("CPU Brand: %s\n", cpu_info.intel_brand_str);
    }
    else if(cpu_info.intel_or_amd == 2) {
        dprintf("This is AMD processor\n");
        dprintf("CPU Family: %s\n", cpu_info.amd_family_str);
        dprintf("CPU Model: %s\n", cpu_info.amd_model_str);
        dprintf("CPU Brand: %s\n", cpu_info.amd_brand_str);
    }
    else if(cpu_info.intel_or_amd == 3) {
        dprintf("This is unknown processor\n");
    }

    dprintf("RAM Total: %I MBytes\n", (uint32_t)(ram_len / 1024 / 1024) + 1);
    dprintf("RAM Available: %I MBytes\n", (uint32_t)(ram_available / 1024 / 1024) + 1);
    /*
    dprintf("RAM sectors info:\n");
    for(uint32_t i = 0; i < memory_section_number; ++i)
    {
        if(memory_sections[i].type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            dprintf("Available RAM addr: 0x%X\n", memory_sections[i].address);
            dprintf("    Length: %I bytes(%I kilobytes)\n", memory_sections[i].length, memory_sections[i].length / 1024);
        }
        else
        {
            dprintf("Unavailable RAM addr: 0x%X\n", memory_sections[i].address);
        }
    }
    */
    dprintf("kernel start: 0x%X end: 0x%X size: %I bytes(%I MBytes)\n", (uint32_t)startkernel, (uint32_t)endkernel, (uint32_t)endkernel - (uint32_t)startkernel, (uint32_t)(endkernel - startkernel) / 1024 / 1024);
    dprintf("multiboot info start: 0x%X end: 0x%X\n", (uint32_t)multiboot_reserved_start, (uint32_t)multiboot_reserved_end);
    
    // --------------------------------------------- //
    if(text_cursor)
    {
        //check interruptions
        if(are_interrupts_enabled()) {
            tprintf(text_cursor, "are_interrupts_enabled() ok\n");
        }
        else {
            tprintf(text_cursor, "ERROR! are_interrupts_enabled() error\n");
        }

        //show processor info
        if(cpu_info.intel_or_amd == 1) {
            tprintf(text_cursor, "This is Intel processor\n");
            tprintf(text_cursor, "CPU Family: %s\n", cpu_info.intel_family_str);
            tprintf(text_cursor, "CPU Model: %s\n", cpu_info.intel_model_str);
            //tprintf(text_cursor, "CPU Type: %s\n", cpu_info.intel_type_str);
            tprintf(text_cursor, "CPU Brand: %s\n", cpu_info.intel_brand_str);
        }
        else if(cpu_info.intel_or_amd == 2) {
            tprintf(text_cursor, "This is AMD processor\n");
            tprintf(text_cursor, "CPU Family: %s\n", cpu_info.amd_family_str);
            tprintf(text_cursor, "CPU Model: %s\n", cpu_info.amd_model_str);
            tprintf(text_cursor, "CPU Brand: %s\n", cpu_info.amd_brand_str);
        }
        else if(cpu_info.intel_or_amd == 3) {
            tprintf(text_cursor, "This is unknown processor\n");
        }

        tprintf(text_cursor, "RAM Total: %I MBytes\n", (uint32_t)(ram_len / 1024 / 1024) + 1);
        tprintf(text_cursor, "RAM Available: %I MBytes\n", (uint32_t)(ram_available / 1024 / 1024) + 1);
        /*
        tprintf(text_cursor, "RAM sectors info:\n");
        for(uint32_t i = 0; i < memory_section_number; ++i)
        {
            if(memory_sections[i].type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                tprintf(text_cursor, "Available RAM addr: 0x%X\n", memory_sections[i].address);
                tprintf(text_cursor, "    Length: %I bytes(%I kilobytes)\n", memory_sections[i].length, memory_sections[i].length / 1024);
            }
            else
            {
                tprintf(text_cursor, "Unavailable RAM addr: 0x%X\n", memory_sections[i].address);
            }
        }
        */
        tprintf(text_cursor, "kernel start: 0x%X end: 0x%X size: %I bytes(%I MBytes)\n", (uint32_t)startkernel, (uint32_t)endkernel, (uint32_t)endkernel - (uint32_t)startkernel, (uint32_t)(endkernel - startkernel) / 1024 / 1024);
        tprintf(text_cursor, "multiboot info start: 0x%X end: 0x%X\n", (uint32_t)multiboot_reserved_start, (uint32_t)multiboot_reserved_end);
    }
}
