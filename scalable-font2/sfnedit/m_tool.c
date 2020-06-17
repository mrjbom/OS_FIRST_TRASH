/*
 * sfnedit/m_tool.c
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
 * @brief Toolbox widget
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"

void view_toolbox(int idx)
{
    int i;
    char un[256];
    ui_win_t *win = &wins[idx];

    if(!idx)
        ui_text(win, wins[idx].w - 7*8, 4, verstr);

    for(i = 0; i < (idx ? 3 : 6); i++)
        if(wins[idx].field == i)
            ui_box(win, 1 + i * 24, 1, 22, 22, theme[THEME_FG], theme[THEME_LIGHT], theme[THEME_FG]);
        else if(wins[idx].seltool == i)
            ui_box(win, 1 + i * 24, 1, 22, 22, theme[THEME_DARK], theme[THEME_BG], theme[THEME_LIGHT]);
        else
            ui_box(win, 1 + i * 24, 1, 22, 22, theme[THEME_LIGHT], theme[THEME_BG], theme[THEME_DARK]);

    if(!idx) {
        for(i = 0; i < 6; i++)
            ui_icon(win, 4 + i * 24, 4, ICON_ABOUT + i, wins[idx].tool == -1 || (i > 1 && !ctx.filename) ? 1 : 0);
    } else {
        for(i = 0; i < 3; i++)
            ui_icon(win, 4 + i * 24, 4, ICON_MEASURES + i, 0);
        sprintf(un, "a%s", utf8(wins[idx].unicode));
        if(wins[idx].field == 3)
            ui_box(win, 6 + 3 * 24, 3, 64, 18, theme[THEME_FG], theme[THEME_DARK], theme[THEME_FG]);
        else
            ui_box(win, 6 + 3 * 24, 3, 64, 18, theme[THEME_DARK], theme[THEME_DARK], theme[THEME_DARK]);
        ui_text(win, 8 + 3 * 24, 4, un);
        sprintf(un,"U+%06x %s", wins[idx].unicode, wins[idx].uniname);
        ui_text(win, 76 + 3 * 24, 3, un);
    }
}
