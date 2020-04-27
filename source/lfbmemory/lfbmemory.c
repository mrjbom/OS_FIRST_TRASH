#include "lfbmemory.h"
#include "../lib/math.h"

//for SSFN
#define size_t size_t
#define SSFN_memcmp memcmp
#define SSFN_memset memset
#define SSFN_realloc krealloc
#define SSFN_free kfree
#include "../lib/ssfn/ssfn.h"

uint32_t* lfb_framebuffer_addr = NULL;

void init_lfb_mem() {
    lfb_framebuffer_addr = (uint32_t*)((uint32_t)MBI->framebuffer_addr);
}

void clear_lfb_mem(uint32_t color) {
    for (uint32_t pixelPos = 0; pixelPos < MBI->framebuffer_width * MBI->framebuffer_height; pixelPos++)
        lfb_framebuffer_addr[pixelPos] = color;
}

void put_pixel_lfb_mem(uint32_t x, uint32_t y, uint32_t color) {
    lfb_framebuffer_addr[y * MBI->framebuffer_pitch / 4 + x] = color;
}

void draw_line_lfb_mem(int x0, int y0, int x1, int y1, uint32_t color)
{
    int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;

    for (;;) {
        put_pixel_lfb_mem(x0, y0, color);
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

void draw_glyph_lfb_mem(ssfn_glyph_t *glyph, int pen_x, int pen_y, uint32_t fgcolor)
{
    int x, y, i, m;

    dprintf("glyph height: %i\n", glyph->h);
    dprintf("glyph width: %i\n", glyph->w);
    dprintf("gryph baseline: %i\n", glyph->baseline);

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
                    else draw_line_lfb_mem(pen_x + x, pen_y + y, pen_x + glyph->data[i], pen_y + glyph->data[i+1], fgcolor);
                    x = glyph->data[i]; y = glyph->data[i+1];
                }
            }
        break;

        case SSFN_MODE_BITMAP:
            for(y = 0; y < glyph->h; y++)
                for(x = 0, i = 0, m = 1; x < glyph->w; x++, m <<= 1) {
                    if(m > 0x80) { m = 1; i++; }
                    //SDL_PIXEL = (glyph->data[y * glyph->pitch + i] & m) ? 0xFF000000 | fgcolor : 0;
                    lfb_framebuffer_addr[(pen_y + y - glyph->baseline) * MBI->framebuffer_pitch / 4 + (pen_x + x)] = ((glyph->data[y * glyph->pitch + i] & m) ? 0xFF000000 | fgcolor : 0);
                }
        break;

        case SSFN_MODE_ALPHA:
            for(y = 0; y < glyph->h; y++)
                for(x = 0; x < glyph->w; x++)
                    lfb_framebuffer_addr[(pen_y + y - glyph->baseline) * MBI->framebuffer_pitch / 4 + (pen_x + x)] = (uint32_t)((glyph->data[y * glyph->pitch + x] << 24) | fgcolor);
        break;

        case SSFN_MODE_CMAP:
            for(y = 0; y < glyph->h; y++)
                for(x = 0; x < glyph->w; x++)
                    lfb_framebuffer_addr[(pen_y + y - glyph->baseline) * MBI->framebuffer_pitch / 4 + (pen_x + x)] = SSFN_CMAP_TO_ARGB(glyph->data[y * glyph->pitch + x], glyph->cmap, fgcolor);
        break;
    }
}


void test_func()
{
    ssfn_t ctx;                                         /* the renderer context */
    ssfn_glyph_t *glyph;                                /* the returned rasterized bitmap */

    memset(&ctx, 0, sizeof(ssfn_t));

    //FreeSans.sfn
    ssfn_load(&ctx, (ssfn_font_t*)&_binary_FSfont_sfn_start);

    ssfn_select(&ctx,
        SSFN_FAMILY_SANS, NULL,                             /* family */
        SSFN_STYLE_REGULAR /*| SSFN_STYLE_UNDERLINE*/, 64,  /* style and size */
        SSFN_MODE_BITMAP                                    /* rendering mode */
    );

    //A
    //glyph = ssfn_render(&ctx, 'A');
    //⋈
    //glyph = ssfn_render(&ctx, 0x22c8);

    //render gryph
    int x = 400, y = 300;
    glyph = ssfn_render(&ctx, 'A');
    draw_glyph_lfb_mem(glyph, x, y, 0xFFFFFF);
    x += glyph->adv_x;
    y += glyph->adv_y;
    glyph = ssfn_render(&ctx, 'a');
    draw_glyph_lfb_mem(glyph, x, y, 0xFFFFFF);
    x += glyph->adv_x;
    y += glyph->adv_y;
    glyph = ssfn_render(&ctx, 'B');
    draw_glyph_lfb_mem(glyph, x, y, 0xFFFFFF);
    x += glyph->adv_x;
    y += glyph->adv_y;
    glyph = ssfn_render(&ctx, 'b');
    draw_glyph_lfb_mem(glyph, x, y, 0xFFFFFF);

    /*
    put_pixel_lfb_mem(399, 301, 0xFE01AC);
    put_pixel_lfb_mem(400, 301, 0xFE01AC);
    put_pixel_lfb_mem(401, 301, 0xFE01AC);
    put_pixel_lfb_mem(399, 300, 0xFE01AC);
    put_pixel_lfb_mem(400, 300, 0xFE01AC);
    put_pixel_lfb_mem(401, 300, 0xFE01AC);
    put_pixel_lfb_mem(399, 299, 0xFE01AC);
    put_pixel_lfb_mem(400, 299, 0xFE01AC);
    put_pixel_lfb_mem(401, 299, 0xFE01AC);
    */
    
    put_pixel_lfb_mem(400, 300, 0xFE01AC);

    /* free resources */
    kfree(glyph);                                        /* no special treatment for freeing glyphs */
    ssfn_free(&ctx);                                     /* free the renderer context's internal buffers */
}

