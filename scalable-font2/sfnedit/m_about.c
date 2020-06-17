/*
 * sfnedit/m_about.c
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
 * @brief Main window About
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"

extern ssfn_t logofnt;

void view_about()
{
    int i;
    char *s = "Scalable Screen Font Editor";
    char url[] = "https://gitlab.com/bztsrc/scalable-font2";
    char *copyright[] = {
        "Copyright (C) 2020 bzt (bztsrc@gitlab)", "",
        "Permission is hereby granted, free of charge, to any person",
        "obtaining a copy of this software and associated documentation",
        "files (the \"Software\"), to deal in the Software without",
        "restriction, including without limitation the rights to use, copy,",
        "modify, merge, publish, distribute, sublicense, and/or sell copies",
        "of the Software, and to permit persons to whom the Software is",
        "furnished to do so, subject to the following conditions:","",
        "The above copyright notice and this permission notice shall be",
        "included in all copies or substantial portions of the Software.",
        NULL
    };
    ui_win_t *win = &wins[0];

    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.w = win->w;
    ssfn_dst.h = win->h;
    ssfn_dst.x = (win->w - 580) / 2;
    ssfn_dst.y = 160;
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.bg = 0;
    while((i = ssfn_render(&logofnt, &ssfn_dst, s)) > 0) s += i;
    ssfn_dst.fg = 0xFF000000;
    ui_text(win, (win->w - 320) / 2 + 1, 181, url);
    ssfn_dst.fg = theme[THEME_FG];
    ui_text(win, (win->w - 320) / 2, 180, url);
    ssfn_dst.y = win->h - 16 * 16;
    if(ssfn_dst.y < 192) ssfn_dst.y = 192;
    for(i = 0; copyright[i] && (int)ssfn_dst.y < win->h - 18; i++) {
        ssfn_dst.y += 16;
        ui_text(win, 8, ssfn_dst.y, copyright[i]);
    }
}
