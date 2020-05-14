#include "inlineassembly/inlineassembly.h"
#include "interruptions/interruptions.h"
#include "devices/cpu/cpu.h"
#include "more/more.h"
#include "memory/memmmu/memmmu.h"
#include "memory/memdetect/memdetect.h"

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

    lfb_draw_rectangle(640, 0, 1279, 1023, 0x000000);

    {
        int ssfn_context_index = ssfn_init_new_context(&_binary_FreeSans_sfn_start);
        if(ssfn_context_index == -1) {
            dprintf("ssfn_init_new_context() error!");
            lfb_clear(0xFF0000);
            return;
        }
        if(!ssfn_select_font(ssfn_context_index,
            SSFN_MY_FAMILY_SANS,
            SSFN_MY_STYLE_REGULAR,
            16)) {
                dprintf("ssfn_cache_glyphs() error!");
                lfb_clear(0xFF0000);
                return;
            }
        ssfn_text_cursor_t* text_cursor = ssfn_create_cursor(ssfn_context_index);
        if(!text_cursor) {
            dprintf("ssfn_create_cursor() error!");
            lfb_clear(0xFF0000);
            return;
        }

        //-----------------------TEST-1-----------------------
        ssfn_setup_cursor(text_cursor, 5, 15, 20, 0xFFFFFF);

        tprintf(text_cursor, "It's test, FreeSans.sfn(first font)\n"\
                             "font size = 16, SANS, REGULAR\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");

        ssfn_setup_cursor(text_cursor, 645, 15, 20, 0xFFFFFF);
        
        tprintf(text_cursor, "It's test, FreeSans.sfn(first font)\n"\
                             "font size = 16, SANS, REGULAR\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");


        if(!ssfn_select_font(ssfn_context_index,
            SSFN_MY_FAMILY_SANS,
            SSFN_MY_STYLE_REGULAR,
            32)) {
                dprintf("ssfn_cache_glyphs() error!");
                lfb_clear(0xFF0000);
                return;
            }

        ssfn_setup_cursor(text_cursor, 5, 150, 30, 0xFFFFFF);

        tprintf(text_cursor, "It's test, FreeSans.sfn(first font)\n"\
                             "font size = 32, SANS, REGULAR\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");

        ssfn_setup_cursor(text_cursor, 645, 150, 30, 0xFFFFFF);

        tprintf(text_cursor, "It's test, FreeSans.sfn(first font)\n"\
                             "font size = 32, SANS, REGULAR\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");

        kfree(text_cursor);
        ssfn_free_context(ssfn_context_index);
    }
    //---------------------------------------------------------------
    {
        int ssfn_context_index = ssfn_init_new_context(&_binary_UbuntuBold_sfn_start);
        if(ssfn_context_index == -1) {
            dprintf("ssfn_init_new_context() error!");
            lfb_clear(0xFF0000);
            return;
        }
        if(!ssfn_select_font(ssfn_context_index,
            SSFN_MY_FAMILY_SANS,
            SSFN_MY_STYLE_BOLD,
            16)) {
                dprintf("ssfn_cache_glyphs() error!");
                lfb_clear(0xFF0000);
                return;
            }
        ssfn_text_cursor_t* text_cursor = ssfn_create_cursor(ssfn_context_index);
        if(!text_cursor) {
            dprintf("ssfn_create_cursor() error!");
            lfb_clear(0xFF0000);
            return;
        }

        //ssfn_setup_cursor(text_cursor, 640, 512, 20, 0xFFFFFF);

        //-----------------------TEST-2-----------------------
        ssfn_setup_cursor(text_cursor, 5, 512, 20, 0xFFFFFF);

        tprintf(text_cursor, "It's test, UbuntuBold.sfn(new font)\n"\
                             "font size = 16, SANS, BOLD\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");

        ssfn_setup_cursor(text_cursor, 645, 512, 20, 0xFFFFFF);
        
        tprintf(text_cursor, "It's test, UbuntuBold.sfn(new font)\n"\
                             "font size = 16, SANS, BOLD\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");


        if(!ssfn_select_font(ssfn_context_index,
            SSFN_MY_FAMILY_SANS,
            SSFN_MY_STYLE_BOLD,
            32)) {
                dprintf("ssfn_cache_glyphs() error!");
                lfb_clear(0xFF0000);
                return;
            }

        ssfn_setup_cursor(text_cursor, 5, 662, 30, 0xFFFFFF);

        tprintf(text_cursor, "It's test, UbuntuBold.sfn(new font)\n"\
                             "font size = 32, SANS, BOLD\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");

        ssfn_setup_cursor(text_cursor, 645, 662, 30, 0xFFFFFF);

        tprintf(text_cursor, "It's test, UbuntuBold.sfn(new font)\n"\
                             "font size = 32, SANS, BOLD\n"\
                             "AaBbCcDd, render with alpha\n"\
                             "1234567890!@#$%^&*()[]/|\\~`\n");


        lfb_clear(0x000000);
        
        for(uint32_t i = 16, j = 0; i < 32; ++i, j += 25) {
            if(!ssfn_select_font(ssfn_context_index,
            SSFN_MY_FAMILY_SANS,
            SSFN_MY_STYLE_BOLD,
            i)) {
                dprintf("ssfn_cache_glyphs() error!");
                lfb_clear(0xFF0000);
                return;
            }
            ssfn_setup_cursor(text_cursor, 5, 10 + j, 20, 0xFFFFFF);
            tprintf(text_cursor, "1234567890, font_size = %I", i);
        }

        kfree(text_cursor);
        ssfn_free_context(ssfn_context_index);
    }

    show_npages_table(3);
    
    dprintf("end of kmain()\n");
}