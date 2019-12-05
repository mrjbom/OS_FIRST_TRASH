#include "kernel/kernel.h"
#include "lfbmemory/lfbmemory.h"
#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "debug/debug.h"

void kmain(unsigned long magic, multiboot_info_t* mbi)
{
    (void)magic;
    initMultibootInfo(mbi);
    initLFBMemory();
    clearLFBMemory(0xFE01AC);
    writePixelToLFB(MBI->framebuffer_height - 1, MBI->framebuffer_height - 1, 0x00FFFFFF);

    idt_init();
    
    if(are_interrupts_enabled())
    {
        //green color - OK
        clearLFBMemory(0x00FF00);
    }
    else
    {
        //red color - ERROR
        clearLFBMemory(0xFF0000);
    }
}
