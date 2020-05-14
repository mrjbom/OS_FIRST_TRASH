#ifndef _LFBMEMORY_H_
#define _LFBMEMORY_H_

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"
#include "../lib/math.h"

extern uint32_t* lfb_framebuffer_addr;

extern void lfb_init();

extern void lfb_clear(uint32_t color);

extern void lfb_put_pixel(uint32_t x, uint32_t y, uint32_t color);

extern uint32_t lfb_get_pixel_color(uint32_t x, uint32_t y);

extern void lfb_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

extern void lfb_draw_rectangle(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

//SSFN
/*
    How do I use ssfn for text output?
    call ssfn_init_new_context() and save context index
           |
           |
           |
    call ssfn_font_select(USE DEFINES WITH "MY", for example "SSFN_MY_FAMILY_SERIF"
           |
           |
           |
    call ssfn_create_cursor() set the index of the cursor and save the cursor
           |
           |
           |
    call ssfn_setup_cursor()
           |
           |
           |
    call tprintf()
           |
           |
           |
    call kfree(text_cursor);
           |
           |
           |
    call ssfn_free_context()


    EXAMPLE:
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
        ssfn_setup_cursor(text_cursor, 5, 15, 20, 0xFFFFFF);

        tprintf(text_cursor, "TEXT\n");
        ssfn_free_cursor(text_cursor);
        kfree(text_cursor);
*/

//sfn.o start addr from obj
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
    uint32_t newline_y_offset;
} __attribute__((packed)) ssfn_text_cursor_t;

//return context_index if success
//return -1 if error
extern int ssfn_init_new_context(unsigned char* binary_font_start);

extern void ssfn_free_context(uint32_t context_index);

//return false if error
#define SSFN_MY_FAMILY_SERIF       0
#define SSFN_MY_FAMILY_SANS        1
#define SSFN_MY_FAMILY_DECOR       2
#define SSFN_MY_FAMILY_MONOSPACE   3
#define SSFN_MY_FAMILY_HAND        4

/* font style flags */
#define SSFN_MY_STYLE_REGULAR      0
#define SSFN_MY_STYLE_BOLD         1
#define SSFN_MY_STYLE_ITALIC       2

extern bool ssfn_select_font(uint32_t context_index, uint8_t font_family, uint8_t font_style, uint32_t font_size);

//void draw_glyph_lfb_mem(ssfn_glyph_t *glyph, int pen_x, int pen_y, uint32_t fgcolor);

//return 0x0 if error
extern ssfn_text_cursor_t* ssfn_create_cursor(uint32_t context_index);

extern void ssfn_setup_cursor(ssfn_text_cursor_t* text_cursor, uint32_t x, uint32_t y, uint32_t newline_y_offset, uint32_t fgcolor);

extern void lfb_draw_ssfn_str(ssfn_text_cursor_t* text_cursor, const char* str);

//display printf
//formats
//c - char
//i - int32
//I - uint32
//X - uint32 address
//s - string
extern void tprintf(ssfn_text_cursor_t* text_cursor, const char* s2, ...);

#endif
