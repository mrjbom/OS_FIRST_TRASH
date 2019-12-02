#include "kernel/kernel.h"
#include "lfbmemory/lfbmemory.h"
#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"

void kmain(unsigned long magic, multiboot_info_t* mbi)
{
    (void)magic;
    initMultibootInfo(mbi);
    initVGAMemory();
    clearVGAMemory(0xFE01AC);
    writePixelToLFB(MBI->framebuffer_height - 1, MBI->framebuffer_height - 1, 0x00FFFFFF);

    idt_init();

    if(are_interrupts_enabled())
    {
        clearVGAMemory(0x00FF00);
    }
    else
    {
        clearVGAMemory(0xFF0000);
    }
    
}
