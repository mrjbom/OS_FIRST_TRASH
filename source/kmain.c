#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "devices/cpu/cpu.h"
#include "more/more.h"
#include "memory/memmmu/memmmu.h"
#include "memory/memdetect/memdetect.h"
#include "devices/timer/timer.h"


void kmain(unsigned long magic, multiboot_info_t* mbi) {
    (void)magic;
    init_multiboot_info(mbi);

    lfb_init();
    lfb_clear(0x00FF00);
    //clear_lfb_mem(0xFE01AC);

    lidt(IDT, sizeof(IDT));
    idt_init();
    init_serial();

    detect_cpu();

    init_memory();
    calclulate_memory();
    if(!init_memory_page_allocator()) {
        dprintf("init_memory_page_allocator() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    //100 Hz = 10 millisecond
    init_timer(100);

    ssfn_setup_draw_buf();
    int context_index = ssfn_init_new_context(&_binary_FreeSans_sfn_start);
    if(context_index == -1) {
        dprintf("ssfn_init_new_context() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    if(!ssfn_select_font(
        context_index,
        SSFN_MY_FAMILY_SANS, SSFN_MY_STYLE_REGULAR,
        32)) {
            dprintf("ssfn_select_font() error!\n");
            lfb_clear(0xFF0000);
            return;
    }

    ssfn_text_cursor_t* text_cursor = ssfn_create_cursor(context_index);
    if(!text_cursor) {
        dprintf("ssfn_create_cursor() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    ssfn_setup_cursor(text_cursor, 0, 30, 0xFF000000);

    tprintf(text_cursor, "test1\ntest2\n");

    ssfn_free_context(context_index);
    kfree(text_cursor);

    dprintf("end of kmain()\n");
}