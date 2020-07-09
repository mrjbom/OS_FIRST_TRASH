#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "more/more.h"
#include "memory/memdetect/memdetect.h"
#include "memory/mmu/mmu.h"
#include "devices/cpu/cpu.h"
#include "devices/pit/pit.h"
#include "devices/pcspeaker/pcspeaker.h"
#include "scheduler/scheduler.h"

void task01()
{
    serial_printf("I'm thread #1\n");
    return;
}

void task02()
{
    serial_printf("I'm thread #2\n");
    return;
}

//pointers to thread structures
thread_t* thread01;
thread_t* thread02;

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
    
    /*
    if(!vm_init_paging()) {
        serial_printf("vm_init_paging() error!\n");
        lfb_clear(0xFF0000);
        return;
    }
    */
    pit_init(100);

    //page_fault error test
    //uint32_t* ptr = (uint32_t*)pm_malloc(4096);
    //vm_set_page_flags(current_directory_table, ptr, 0);
    //*ptr = 10;

    //tnitializing the scheduler
    task_manager_init();
 
    //getting a pointer to the current process
    process_t* proc = get_current_proc();
 
    //creating two threads
    thread01 = thread_create(proc,
               &task01,
               0x2000,
               true,
               false);

    thread02 = thread_create(proc,
               &task02,
               0x2000,
               true,
               false);

    serial_printf("end of kmain()\n");
    return;
}