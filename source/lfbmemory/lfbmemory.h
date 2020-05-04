#ifndef _LFBMEMORY_H_
#define _LFBMEMORY_H_

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"
#include "../lib/math.h"
#include "../memory/memmmu/memmmu.h"

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
    call ssfn_init_new_context() and save context index -> Configures ctx; Load font; Set up font; Return context index;
           |
           |
           |
    call ssfn_create_cursor() set the index of the cursor and save the cursor
           |
           |
           |
    call ssfn_cache_glyphs() -> Set up font; Render glyphs;
           |
           |
           |
    call tprintf()
*/

//sfn.o start addr from obj
//FreeSans.sfn
extern unsigned char _binary_FSfont_sfn_start;
//FreeSerif.sfn
extern unsigned char _binary_FSifont_sfn_start;

//The text cursor that prints text on the screen
typedef struct ssfn_text_cursor {
    uint32_t x;
    uint32_t y;
    uint32_t context_index;
} __attribute__((packed)) ssfn_text_cursor_t;

//return context_index if success
//return -1 if error
extern int ssfn_init_new_context();

extern void ssfn_free_context(uint32_t context_index);

//return false if error
extern bool ssfn_cache_glyphs(uint32_t context_index, uint32_t font_size);

//void draw_glyph_lfb_mem(ssfn_glyph_t *glyph, int pen_x, int pen_y, uint32_t fgcolor);

//return 0x0 if error
extern ssfn_text_cursor_t* ssfn_create_cursor(uint32_t context_index);

extern void ssfn_free_cursor(ssfn_text_cursor_t* text_cursor);

extern void lfb_draw_ssfn_str(ssfn_text_cursor_t* text_cursor, const char* str, uint32_t fgcolor);

//display printf
//formats
//c - char
//i - int32
//I - uint32
//X - uint32 address
//s - string
extern void tprintf(ssfn_text_cursor_t* text_cursor, uint32_t fgcolor, const char* s2, ...);

#endif
