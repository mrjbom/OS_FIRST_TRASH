#ifndef _LFBMEMORY_H_
#define _LFBMEMORY_H_

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"
#include "../lib/string.h"
#include "../lib/math.h"
#include "../lib/printf.h"

extern uint32_t* lfb_framebuffer_addr;

extern void lfb_init();

extern void lfb_clear(uint32_t color);

extern void lfb_put_pixel(uint32_t x, uint32_t y, uint32_t color);

extern uint32_t lfb_get_pixel_color(uint32_t x, uint32_t y);

extern void lfb_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

extern void lfb_draw_rectangle(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

extern uint32_t lfb_rgb_to_hex(uint32_t r, uint32_t g, uint32_t b);
//SSFN 2.0
/*
    How do I use SSFN 2.0 for text output?
    call print_ssfn_setup_draw_buf()
           |
           |
           |
           V
    call print_ssfn_init_new_context() and save context index
           |
           |
           |
           V
    call print_ssfn_select_font() USE DEFINES WITH "MY", for example "SSFN_MY_FAMILY_SERIF"
           |
           |
           |
           V
    call print_ssfn_create_cursor() set the index of the cursor and save the cursor
           |
           |
           |
           V
    call print_ssfn_setup_cursor()
           |
           |
           |
           V
    call tprintf()
           |
           |
           |
           V
    call print_ssfn_free_context()
           |
           |
           |
           V
    call pm_free(text_cursor)


    EXAMPLE:
    print_ssfn_setup_draw_buf();
    int context_index = print_ssfn_init_new_context(&_binary_FreeSans_sfn_start);
    if(context_index == -1) {
        serial_printf("print_ssfn_init_new_context() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    if(!print_ssfn_select_font(
        context_index,
        SSFN_MY_FAMILY_SANS, SSFN_MY_STYLE_REGULAR,
        32)) {
            serial_printf("print_ssfn_select_font() error!\n");
            lfb_clear(0xFF0000);
            return;
    }

    ssfn_text_cursor_t* text_cursor = print_ssfn_create_cursor(context_index);
    if(!text_cursor) {
        serial_printf("print_ssfn_create_cursor() error!\n");
        lfb_clear(0xFF0000);
        return;
    }

    print_ssfn_setup_cursor(text_cursor, 0, 30, 0xFF000000);

    lfb_printf(text_cursor, "test1\ntest2");

    print_ssfn_free_context(context_index);
    pm_free(text_cursor);
*/

//from objects
//FreeSans.sfn
extern unsigned char _binary_FreeSans_sfn_start;
//UbuntuBold.sfn
extern unsigned char _binary_UbuntuBold_sfn_start;

//The text cursor that prints text on the screen
typedef struct ssfn_text_cursor {
    uint32_t x;
    uint32_t y;
    uint32_t fgcolor;
    uint32_t context_index;
} __attribute__((packed)) ssfn_text_cursor_t;

//called first of all
extern void print_ssfn_setup_draw_buf();

//return context_index if success
//return -1 if error
extern int print_ssfn_init_new_context(unsigned char* binary_font_start);

extern void print_ssfn_free_context(uint32_t context_index);

#define SSFN_MY_FAMILY_SERIF       0
#define SSFN_MY_FAMILY_SANS        1
#define SSFN_MY_FAMILY_DECOR       2
#define SSFN_MY_FAMILY_MONOSPACE   3
#define SSFN_MY_FAMILY_HAND        4

#define SSFN_MY_STYLE_REGULAR      0
#define SSFN_MY_STYLE_BOLD         1
#define SSFN_MY_STYLE_ITALIC       2

extern bool print_ssfn_select_font(uint32_t context_index, uint8_t font_family, uint8_t font_style, uint32_t font_size);

//void draw_glyph_lfb_mem(ssfn_glyph_t *glyph, int pen_x, int pen_y, uint32_t fgcolor);

//return 0x0 if error
extern ssfn_text_cursor_t* print_ssfn_create_cursor(uint32_t context_index);

extern void print_ssfn_setup_cursor(ssfn_text_cursor_t* text_cursor, uint32_t x, uint32_t y, uint32_t fgcolor);

extern void lfb_draw_ssfn_str(ssfn_text_cursor_t* text_cursor, const char* str);

//display printf
//formats
//c - char
//i - int32
//I - uint32
//l - int64
//L - uint64
//X - uint32 address
//Z - uint64 address
//s - string
extern void lfb_printf(ssfn_text_cursor_t* text_cursor, const char* s2, ...);

#endif
