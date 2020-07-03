#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "more/more.h"
#include "memory/memdetect/memdetect.h"
#include "memory/mmu/mmu.h"
#include "devices/cpu/cpu.h"
#include "devices/pit/pit.h"
#include "devices/pcspeaker/pcspeaker.h"
#include "task/task.h"

void kmain(unsigned long magic, multiboot_info_t* mbi) {
    (void)magic;
    multiboot_info_init(mbi);

    lfb_init();
    lfb_clear(0x00FF00);
    //clear_lfb_mem(0xFE01AC);

    lidt(IDT, sizeof(IDT));
    idt_init();
    serial_init();

    detect_cpu();

    memory_map_init();
    memory_map_calculate();

    if(!pm_init_memory_page_allocator()) {
        serial_printf("pm_init_memory_page_allocator() error!\n");
        lfb_clear(0xFF0000);
        return;
    }
    
    if(!vm_init_paging()) {
        serial_printf("vm_init_paging() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    pit_init(100);

    uint32_t* ptr = pm_malloc(100);
    uint32_t flags = 0;
    if(!vm_get_page_flags(current_directory_table, (void*)ptr, &flags)) {
        serial_printf("vm_get_page_flags error!\n");
    }
    serial_printf("0x%x, %u, %i\n", ptr, *ptr, flags);
    serial_printf("set new flags\n");
    //if(!vm_set_page_flags(current_directory_table, ptr, 0)) {
    //    serial_printf("vm_set_page_flags error!\n");
    //}

    vm_map_page(current_directory_table, ptr, ptr, 0);

    *ptr = 10;
    serial_printf("0x%x, %u, %i\n", ptr, *ptr, flags);
    pm_free(ptr);

    serial_printf("end of kmain()\n");

    return;
}