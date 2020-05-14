#include "lfbmemory.h"

//for SSFN
#include "../memory/memmmu/memmmu.h"
#include "../more/more.h"
#define size_t size_t
#define SSFN_memcmp memcmp
#define SSFN_memset memset
#define SSFN_realloc krealloc
#define SSFN_free kfree
#include "../lib/ssfn/ssfn.h"

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

//ssfn
#define SSFN_CONTEXTS_MAX_NUMBER 32
#define SSFN_GLYPHS_MAX_NUMBER 127
ssfn_t* ssfn_contexts[SSFN_CONTEXTS_MAX_NUMBER]; /* the renderer contexts */
ssfn_glyph_t* ssfn_glyphs_caches[SSFN_CONTEXTS_MAX_NUMBER][SSFN_GLYPHS_MAX_NUMBER]; /* cached glyphs for each individual context */
uint32_t ssfn_context_counter = 0;

int ssfn_init_new_context(unsigned char* binary_font_start)
{
    if(!(ssfn_context_counter < (SSFN_CONTEXTS_MAX_NUMBER - 1)))
        return -1;
    ssfn_contexts[ssfn_context_counter] = (ssfn_t*)kmalloc(sizeof(ssfn_t));
    memset(ssfn_contexts[ssfn_context_counter], 0, sizeof(ssfn_t));
    if(ssfn_load(ssfn_contexts[ssfn_context_counter], (ssfn_font_t*)binary_font_start) != SSFN_OK) {
        dprintf("ssfn_load() error: %s\n!", ssfn_error(ssfn_lasterr(ssfn_contexts[ssfn_context_counter])));
        ssfn_context_counter--;
        return -1;
    }
    ssfn_context_counter++;
    return ssfn_context_counter - 1;
}

void ssfn_free_context(uint32_t context_index)
{
    for(uint32_t i = 0; i < SSFN_GLYPHS_MAX_NUMBER; ++i) {
        kfree(ssfn_glyphs_caches[context_index][i]);
    }
    if(ssfn_contexts[context_index]) {
        ssfn_free(ssfn_contexts[context_index]);
        ssfn_context_counter--;
    }
}

extern bool ssfn_select_font(uint32_t context_index, uint8_t font_family, uint8_t font_style, uint32_t font_size)
{
    if(font_size < 8 || font_size > 255) {
        //dprintf("!set_ssfn_render_size() error: invalid size\n");
        return false;
    }

    int errorcode = ssfn_select(
        ssfn_contexts[context_index],
        font_family, NULL,              /* family */
        font_style, font_size,          /* style and size */
        SSFN_MODE_ALPHA                 /* rendering mode */
    );

    if(errorcode != SSFN_OK) {
        //dprintf("!ssfn_select() error: %s\n!", ssfn_error(errorcode));
        return false;
    }

    //characters below or above are not rendered
    for(uint32_t i = 32; i < SSFN_GLYPHS_MAX_NUMBER; ++i) {
        //if we previously cached glyphs
        if(ssfn_glyphs_caches[context_index][i] != NULL) {
            kfree(ssfn_glyphs_caches[context_index][i]);
        }
        ssfn_glyphs_caches[context_index][i] = ssfn_render(ssfn_contexts[context_index], (uint8_t)(char)i);
        if(ssfn_glyphs_caches[context_index][i] == NULL) {
            //dprintf("ssfn_render error!\n");
            dprintf("error with symbol number '%I'\n", i);
            //return false;
        }
    }
    return true;
}

ssfn_text_cursor_t* ssfn_create_cursor(uint32_t context_index)
{
    ssfn_text_cursor_t* text_cursor = kmalloc(sizeof(ssfn_text_cursor_t));
    if(!text_cursor && !ssfn_contexts[context_index])
        return 0x0;
    text_cursor->x = 0;
    text_cursor->y = 0;
    text_cursor->context_index = context_index;
    return text_cursor;
}

void ssfn_setup_cursor(ssfn_text_cursor_t* text_cursor, uint32_t x, uint32_t y, uint32_t newline_y_offset, uint32_t fgcolor)
{
    text_cursor->x = x;
    text_cursor->y = y;
    text_cursor->newline_y_offset = newline_y_offset;
    text_cursor->fgcolor = fgcolor;
}

void lfb_draw_ssfn_glyph(ssfn_glyph_t *glyph, int pen_x, int pen_y, uint32_t fgcolor)
{
    int x, y, i, m;

    //dprintf("glyph height: %i\n", glyph->h);
    //dprintf("glyph width: %i\n", glyph->w);
    //dprintf("gryph baseline: %i\n", glyph->baseline);

    /* align glyph properly, we may have received a vertical letter */
    if(glyph->adv_y)
        pen_x -= glyph->baseline;
    else
        pen_y -= glyph->baseline;

    switch(glyph->mode) {
        case SSFN_MODE_OUTLINE:
            if(glyph->pitch>1) {
                x = glyph->data[0]; y = glyph->data[1];
                for(i = 0; i < glyph->pitch; i += 2) {
                    /* end of a contour? */
                    if(glyph->data[i] == 255 && glyph->data[i+1] == 255) i += 2;
                    /* no, connect this point to the previous one. You should provide your own line() */
                    else lfb_draw_line(pen_x + x, pen_y + y, pen_x + glyph->data[i], pen_y + glyph->data[i+1], fgcolor);
                    x = glyph->data[i]; y = glyph->data[i+1];
                }
            }
        break;

        case SSFN_MODE_BITMAP:
            for(y = 0; y < glyph->h; y++)
                for(x = 0, i = 0, m = 1; x < glyph->w; x++, m <<= 1) {
                    if(m > 0x80) { m = 1; i++; }
                    if((glyph->data[y * glyph->pitch + i] & m)) {
                        lfb_put_pixel(x + pen_x, y + pen_y, fgcolor);
                    }
                    else {
                        //put_pixel_lfb_mem(x + pen_x, y + pen_y, /*background color*/);
                    }
                }
        break;

        case SSFN_MODE_ALPHA:
            for(y = 0; y < glyph->h; y++)
                for(x = 0; x < glyph->w; x++) {
                    //put_pixel_lfb_mem(x + pen_x, y + pen_y, (uint32_t)((glyph->data[y * glyph->pitch + x] << 24) | fgcolor));
                    uint32_t bgcolor = lfb_get_pixel_color(x + pen_x, y + pen_y);
                    uint32_t alpha = (glyph->data[y * glyph->pitch + x]);
                    lfb_put_pixel(x + pen_x, y + pen_y, (bgcolor * (0xff - alpha) + fgcolor * alpha) / 0xff);
                }
        break;

        case SSFN_MODE_CMAP:
            for(y = 0; y < glyph->h; y++)
                for(x = 0; x < glyph->w; x++)
                    lfb_framebuffer_addr[(pen_y + y - glyph->baseline) * MBI->framebuffer_pitch / 4 + (pen_x + x)] = SSFN_CMAP_TO_ARGB(glyph->data[y * glyph->pitch + x], glyph->cmap, fgcolor);
        break;
    }
}

void lfb_draw_ssfn_str(ssfn_text_cursor_t* text_cursor, const char* str)
{
    if(!text_cursor) {
        return;
    }
    uint32_t context_index = text_cursor->context_index;
    uint32_t fgcolor = text_cursor->fgcolor;
    uint32_t start_x = text_cursor->x;
    for(uint32_t i = 0; i < strlen(str); ++i) {
        //dprintf("glyph addr = 0x%X\n", glyph);
        //dprintf("glyph number %I h = %I, w = %I\nadv_x = %I, adv_y = %I\n\n", (uint8_t)str[i], ssfn_glyphs_caches[context_index][(uint8_t)str[i]]->h, ssfn_glyphs_caches[context_index][(uint8_t)str[i]]->w, ssfn_glyphs_caches[context_index][(uint8_t)str[i]]->adv_x, ssfn_glyphs_caches[context_index][(uint8_t)str[i]]->adv_y);
        lfb_draw_ssfn_glyph(ssfn_glyphs_caches[context_index][(uint8_t)str[i]], text_cursor->x, text_cursor->y, fgcolor);
        //not working(problem in ssfn glyph height)
        //text_cursor->y += ssfn_glyphs_caches[context_index][(uint8_t)str[i]]->adv_y;
        if(str[i] != '\n') {
            text_cursor->x += ssfn_glyphs_caches[context_index][(uint8_t)str[i]]->adv_x;
        }
        else
        {
            text_cursor->x = start_x;
            text_cursor->y += text_cursor->newline_y_offset;
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
