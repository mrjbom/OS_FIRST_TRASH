/*
 * sfnedit/uiwgt.c
 *
 * Copyright (C) 2020 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief User interface widgets
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "ui.h"
#include "ssfn.h"

/**
 * Display a filled box
 */
void ui_box(ui_win_t *win, int x, int y, int w, int h, uint32_t l, uint32_t b, uint32_t d)
{
    int i, j, p = y * win->p + x, p2 = (y + h - 1) * win->p + x, p3;

    if(x < 0 || y < 0 || x >= win->w || y >= win->h || w < 1 || h < 1) return;
    l &= 0xFFFFFF;
    b &= 0xFFFFFF;
    d &= 0xFFFFFF;
    for(i=0; i < w && x + i < win->w; i++) {
        win->data[p + i] = l;
        if(y + h - 1 < win->h) win->data[p2 + i] = d;
    }
    p+=win->p;
    for(j=1, p3=p; j+1 < h && y + j < win->h; j++, p3 += win->p) {
        win->data[p3] = l;
        if(x + w - 1 < win->w) win->data[p3 + w - 1] = d;
    }
    for(j=1; j + 1 < h && y + j < win->h; j++, p += win->p)
        for(i=1; i + 1 < w && x + i < win->w; i++)
            win->data[p + i] = b;

}

/**
 * Blit an icon on window
 */
void ui_icon(ui_win_t *win, int x, int y, int icon, int inactive)
{
    int i, j, m, k, w, h;
    uint8_t *a, *b;

    if(x < 0 || y < 0 || x >= win->w || y >= win->h) return;
    w = win->w > x + 16 ? 16 : win->w - x;
    h = win->h > y + 16 ? 16 : win->h - y;
    for(k = y*win->p + x, m = (icon * 256 + ((16 - h) << 8) + (16 - w)) << 2, j = 0; j < 16 && y + j < win->h;
        j++, k += win->p, m += 16 * 4) {
        for(i = 0; i < w; i++) {
            if(tools[m + (i<<2)+3]) {
                a = (uint8_t*)&win->data[k+i];
                b = (uint8_t*)&tools[m + (i<<2)];
                if(inactive)
                    a[0] = a[1] = a[2] = ((int)(b[0] + b[1] + b[2])*b[3]/6 + (256 - b[3])*a[2]) >> 8;
                else {
                    a[2] = (b[0]*b[3] + (256 - b[3])*a[2]) >> 8;
                    a[1] = (b[1]*b[3] + (256 - b[3])*a[1]) >> 8;
                    a[0] = (b[2]*b[3] + (256 - b[3])*a[0]) >> 8;
                }
            }
        }
    }
}

/**
 * Display text
 */
void ui_text(ui_win_t *win, int x, int y, char *str)
{
    char *s = str;
    if(x < 0 || y < 0) return;
    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.w = win->w;
    ssfn_dst.h = win->h;
    ssfn_dst.x = x;
    ssfn_dst.y = y;
    while(*s && ssfn_dst.x < win->w)
        ssfn_putc(ssfn_utf8(&s));
}
