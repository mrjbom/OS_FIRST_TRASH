#include "lfbmemory.h"
#include "../debug/debug.h"

//for SSFN
#include "../memory/memmmu/memmmu.h"
#include "../more/more.h"
#define size_t size_t
#define uint8_t uint8_t
#define SSFN_memcmp memcmp
#define SSFN_memset memset
#define SSFN_realloc krealloc
#define SSFN_free kfree
#define SSFN_IMPLEMENTATION //normal render
#include "../../scalable-font2/ssfn.h"

uint32_t* lfb_framebuffer_addr = NULL;

void lfb_init()
{
    lfb_framebuffer_addr = (uint32_t*)((uint32_t)MBI->framebuffer_addr);
}

void lfb_clear(uint32_t color)
{
    for (uint32_t pixelPos = 0; pixelPos < MBI->framebuffer_width * MBI->framebuffer_height; pixelPos++)
        lfb_framebuffer_addr[pixelPos] = color;
}

void lfb_put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    lfb_framebuffer_addr[y * MBI->framebuffer_pitch / 4 + x] = color;
}

uint32_t lfb_get_pixel_color(uint32_t x, uint32_t y)
{
    return lfb_framebuffer_addr[y * MBI->framebuffer_pitch / 4 + x];
}

void lfb_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color)
{
    int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        lfb_put_pixel(x0, y0, color);
        e2 = 2*err;
        if (e2 >= dy) {
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}

void lfb_draw_rectangle(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color)
{
    for(uint32_t y = y0; y <= y1; ++y) {
        for(uint32_t x = x0; x <= x1; ++x) {
            lfb_put_pixel(x, y, color);
        }
    }
}

//---------------------ssfn 2.0---------------------
#define SSFN_CONTEXTS_MAX_NUMBER 32
ssfn_t* ssfn_contexts[SSFN_CONTEXTS_MAX_NUMBER]; // the renderer contexts
ssfn_buf_t ssfn_draw_buf;
uint32_t ssfn_context_counter = 0;

void ssfn_setup_draw_buf() {
    ssfn_draw_buf.ptr = (uint8_t*)lfb_framebuffer_addr;   /* address of the buffer */
    ssfn_draw_buf.w = (uint16_t)MBI->framebuffer_width;   /* width */
    ssfn_draw_buf.h = (uint16_t)MBI->framebuffer_height;  /* height */
    ssfn_draw_buf.p = (uint16_t)MBI->framebuffer_pitch;   /* bytes per line */
    ssfn_draw_buf.x = ssfn_draw_buf.y = 100;              /* pen position */
    ssfn_draw_buf.fg = 0xFF000000;                        /* foreground color */
}

int ssfn_init_new_context(unsigned char* binary_font_start)
{
    //checking the limit on creating SSFN contexts
    if(!(ssfn_context_counter < (SSFN_CONTEXTS_MAX_NUMBER - 1)))
        return -1;
    
    //the memory allocation for the context
    ssfn_contexts[ssfn_context_counter] = (ssfn_t*)kmalloc(sizeof(ssfn_t));

    //checking memory was allocated successfully
    if(!ssfn_contexts[ssfn_context_counter])
        return -1;

    memset(ssfn_contexts[ssfn_context_counter], 0, sizeof(ssfn_t));

    //trying to load the font
    int errorcode = ssfn_load(ssfn_contexts[ssfn_context_counter], &_binary_FreeSans_sfn_start);

    //if there is an error in loading the font
    if(errorcode != SSFN_OK) {
        dprintf("ssfn_load error: %s\n!", ssfn_error(errorcode));
        kfree(ssfn_contexts[ssfn_context_counter]);
        return -1;
    }
    ssfn_context_counter++;
    return ssfn_context_counter - 1;
}

void ssfn_free_context(uint32_t context_index)
{
    if(ssfn_contexts[context_index]) {
        ssfn_free(ssfn_contexts[context_index]);
        ssfn_context_counter--;
    }
    kfree(ssfn_contexts[ssfn_context_counter]);
}

bool ssfn_select_font(uint32_t context_index, uint8_t font_family, uint8_t font_style, uint32_t font_size)
{
    if(font_size < 8 || font_size > 192) {
        //dprintf("!set_ssfn_render_size() error: invalid size\n");
        return false;
    }

    int errorcode = ssfn_select(
        ssfn_contexts[context_index],
        font_family, NULL,              // family
        font_style, font_size           // style and size
    );

    if(errorcode != SSFN_OK) {
        dprintf("!ssfn_select() error: %s\n!", ssfn_error(errorcode));
        return false;
    }

    return true;
}

ssfn_text_cursor_t* ssfn_create_cursor(uint32_t context_index)
{
    //checking the context is correct
    if(!ssfn_contexts[context_index])
        return 0x0;
    ssfn_text_cursor_t* text_cursor = kmalloc(sizeof(ssfn_text_cursor_t));
    //checking the cursor is correct
    if(!text_cursor)
        return 0x0;
    text_cursor->x = 0;
    text_cursor->y = 0;
    text_cursor->context_index = context_index;
    return text_cursor;
}

void ssfn_setup_cursor(ssfn_text_cursor_t* text_cursor, uint32_t x, uint32_t y, uint32_t fgcolor)
{
    text_cursor->x = x;
    text_cursor->y = y;
    text_cursor->fgcolor = fgcolor;
}

void lfb_draw_ssfn_str(ssfn_text_cursor_t* text_cursor, const char* str)
{
    if(!text_cursor) {
        return;
        dprintf("text_cursor invalid\n");
    }
    ssfn_draw_buf.x = text_cursor->x;
    ssfn_draw_buf.y = text_cursor->y;
    ssfn_draw_buf.fg = text_cursor->fgcolor;

    size_t len = strlen(str);
    int ret = 0;
    int errorcode = 0;
    int start_x = ssfn_draw_buf.x;
    int context_index = text_cursor->context_index;

    for(size_t i = 0; i < len; ++i) {
        if(str[i] == '\n') {
            int w = 0, h = 0, left = 0, top = 0;
            errorcode = ssfn_bbox(ssfn_contexts[context_index], "a", &w, &h, &left, &top);
            if(errorcode != SSFN_OK) {
                dprintf("ssfn_bbox() error: %s\n", ssfn_error(errorcode));
                return;
            }
            ssfn_draw_buf.x = start_x;
            ssfn_draw_buf.y += top;
                //dprintf("\\n h = %i\n", h);
                //dprintf("\\n w = %i\n", w);
                //dprintf("\\n left = %i\n", left);
                //dprintf("\\n top = %i\n", top);
        }
        else {
            ret = ssfn_render(ssfn_contexts[context_index], &ssfn_draw_buf, str + i);
            if(ret < SSFN_OK) {
                dprintf("ssfn_render() error: %s\n", ssfn_error(errorcode));
                return;
            }
        }
    }
}

void tprintf(ssfn_text_cursor_t* text_cursor, const char* s2, ...) {
   char outstr[1024];
   va_list list;
   va_start(list, s2);
   vsprintf(outstr, s2, list);
   lfb_draw_ssfn_str(text_cursor, outstr);
}
