/*
 * sfnedit/ui.c
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
 * @brief Common user interface routines
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "stb_png.h"
#include "lang.h"
#include "ui.h"
#include "icon.h"
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_COMMON
#include "libsfn.h"

char verstr[8];

uint8_t *icon32, *tools, *numbers, *bga;

uint32_t theme[] = { 0xFF454545, 0xFFBEBEBE, 0xFF545454, 0xFF3C3C3C };
/*
    0xFFF0F0F0, 0xFFF4F4F4, 0xFFBEBEBE, 0xFF808080, 0xFF5C5C5C, 0xFF4C4C4C, 0xFF454545, 0xFF383838, 0xFF303030,
    0xFFFF0000, 0xFF800000, 0xFF004040, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00FFFF };
*/
int gw = 36+16+512, gh = 24+8+512, gotevt = 0, quiet = 0, lastpercent = 100, mainloop = 1, modified = 0;

int numwin = 0;
ui_win_t *wins = NULL;
ui_event_t event;

void *winid = NULL;
int winidx = 0;

ssfn_t logofnt;

/**
 * report an error and exit
 */
void ui_error(char *subsystem, int fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "sfnedit: %s: ", subsystem);
    vfprintf(stderr, lang[fmt], args);
    fprintf(stderr, "\n");
    exit(fmt + 1);
}

/**
 * Update window title
 */
void ui_updatetitle(int idx)
{
    char title[278], *fn = NULL;
    if(ctx.filename)
        fn = strrchr(ctx.filename,
#ifdef __WIN32__
            '\\'
#else
            '/'
#endif
        );
    if(!fn) fn = ctx.filename; else fn++;
    if(!idx) {
        memcpy(title, "sfnedit", 8);
        if(fn) { memcpy(title + 7, " - ", 3); strncpy(title + 10, fn, 256); }
    } else {
        sprintf(title, "sfnedit - U+%06X - ", wins[idx].unicode);
        if(fn) strncpy(title + 21, fn, 256);
    }
    ui_titlewin(&wins[idx], title);
}

int ui_maxfield(int idx)
{
    if(!idx) {
        switch(wins[idx].tool) {
            case MAIN_TOOL_LOAD: return 4;
            case MAIN_TOOL_SAVE: return 4;
            case MAIN_TOOL_PROPS: return 4;
            case MAIN_TOOL_RANGES: return 4;
            case MAIN_TOOL_GLYPHS: return 4;
            case MAIN_TOOL_DOSAVE: return 4;
        }
    } else {
        switch(wins[idx].tool) {
            case GLYPH_TOOL_COORD: return 7;
            case GLYPH_TOOL_LAYER: return 7;
            case GLYPH_TOOL_KERN: return 7;
            case GLYPH_TOOL_COLOR: return 7;
        }
    }
    return 3;
}

/**
 * Open a window
 */
void ui_openwin(uint32_t unicode)
{
    int w, h, i, j=0;

    for(i=0; i < numwin; i++) {
        if(wins[i].winid) {
            if(wins[i].unicode == unicode) { ui_focuswin(&wins[i]); return; }
        } else {
            if(!j) j = i;
        }
    }
    if(!j) {
        if(!numwin) {
            w = MAIN_W; h = MAIN_H; unicode = WINTYPE_MAIN;
        } else {
            w = gw; h = gh;
        }
        j = numwin++;
        wins = (ui_win_t*)realloc(wins, numwin*sizeof(ui_win_t));
        if(!wins) ui_error("openwin", ERR_MEM);
    }
    memset(&wins[j], 0, sizeof(ui_win_t));
    wins[j].unicode = unicode;
    wins[j].uniname = uninames[uniname(unicode)].name;
    wins[j].winid = ui_createwin(w, h);
    wins[j].field = wins[j].seltool = -1;
    wins[j].tool = 0;
    wins[j].zoom = 512;
    ui_updatetitle(j);
    ui_resizewin(&wins[j], w, h);
    ui_focuswin(&wins[j]);
    ui_refreshwin(j, 0, 0, w, h);
}

/**
 * Close a window
 */
void ui_closewin(int idx)
{
    int i;

    ui_cursorwin(&wins[idx], CURSOR_PTR);
    ui_cursorwin(&wins[0], CURSOR_PTR);
    if(idx < 0 || idx >= numwin || !wins[idx].winid) return;
/*
    if(wins[idx].bg) free(wins[idx].bg);
    wins[idx].bg = NULL;
    if(wins[idx].hist) {
        hist_cleanup(&wins[idx], 0);
        free(wins[idx].hist);
    }
    wins[idx].hist = NULL;
*/
    if(!idx) {
        for(i=1; i < numwin; i++)
            if(wins[i].winid)
                ui_closewin(i);
        numwin = 1;
        if(modified && wins[0].tool != MAIN_TOOL_DOSAVE) {
            wins[0].tool = MAIN_TOOL_DOSAVE;
            ui_resizewin(&wins[0], wins[0].w, wins[0].h);
            ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
        } else
            mainloop = 0;
        return;
    }
    ui_destroywin(&wins[idx]);
    wins[idx].winid = NULL;
    wins[idx].surface = NULL;
    wins[idx].data = NULL;
    wins[idx].unicode = -1;
    wins[idx].histmin = wins[idx].histmax = 0;
    while(numwin && !wins[numwin-1].winid) numwin--;
}

/**
 * Get common window id (idx) for driver specific window id
 */
int ui_getwin(void *wid)
{
    int i;

    if(wid == winid) return winidx;
    for(i=0; i < numwin; i++)
        if(wins[i].winid == wid) {
            winid = wid;
            winidx = i;
            return i;
        }
    return -1;
}

/**
 * Load resources
 */
static void ui_loadrc()
{
    unsigned int w, h, f;
    uint8_t c, *ptr;
    stbi__context s;
    stbi__result_info ri;

    sprintf(verstr, "v%d.%d.%d", SSFN_VERSION >> 8, (SSFN_VERSION >> 4) & 0xF, SSFN_VERSION & 0xF);

    /* load icons */
    s.read_from_callbacks = 0;

    s.img_buffer = s.img_buffer_original = icon32_png;
    s.img_buffer_end = s.img_buffer_original_end = icon32_png + sizeof(icon32_png);
    ri.bits_per_channel = 8;
    icon32 = (uint8_t*)stbi__png_load(&s, (int*)&w, (int*)&h, (int*)&f, 0, &ri);

    s.img_buffer = s.img_buffer_original = icons_png;
    s.img_buffer_end = s.img_buffer_original_end = icons_png + sizeof(icons_png);
    ri.bits_per_channel = 8;
    tools = (uint8_t*)stbi__png_load(&s, (int*)&w, (int*)&h, (int*)&f, 0, &ri);

    s.img_buffer = s.img_buffer_original = numbers_png;
    s.img_buffer_end = s.img_buffer_original_end = numbers_png + sizeof(numbers_png);
    ri.bits_per_channel = 8;
    numbers = (uint8_t*)stbi__png_load(&s, (int*)&w, (int*)&h, (int*)&f, 0, &ri);

    s.img_buffer = s.img_buffer_original = bga_png;
    s.img_buffer_end = s.img_buffer_original_end = bga_png + sizeof(bga_png);
    ri.bits_per_channel = 8;
    bga = (uint8_t*)stbi__png_load(&s, (int*)&w, (int*)&h, (int*)&f, 0, &ri);

    /* uncompress ui font */
    ptr = unifont_gz + 3;
    c = *ptr++; ptr += 6;
    if(c & 4) { w = *ptr++; w += (*ptr++ << 8); ptr += w; }
    if(c & 8) { while(*ptr++ != 0); }
    if(c & 16) { while(*ptr++ != 0); }
    f = sizeof(unifont_gz) - ((uint64_t)ptr - (uint64_t)unifont_gz);
    w = 0;
    ssfn_src = (ssfn_font_t*)stbi_zlib_decode_malloc_guesssize_headerflag((const char*)ptr, f, 4096, (int*)&w, 0);

    memset(&logofnt, 0, sizeof(ssfn_t));
    ssfn_load(&logofnt, &logo_sfn);
    ssfn_select(&logofnt, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR, 48);
}

/**
 * Quit handler (via Ctrl-C signal or normally)
 */
void ui_quit(int sig __attribute__((unused)))
{
    ui_fini();
    ssfn_free(&logofnt);
    sfn_free();
    uniname_free();
    free(icon32);
    free(tools);
    free(numbers);
    free(bga);
    free(ssfn_src);
    if(sig) exit(1);
}

/**
 * Progress bar hook
 */
void ui_pb(int step, int numstep, int curr, int total, int msg)
{
    int i, n;
    char s[16];

    n = (long int)(curr + 1) * 100L / (long int)(total + 1);
    if(n == lastpercent) return;
    lastpercent = n;
    ui_box(&wins[0], 0, wins[0].h - 18, wins[0].w, 18, theme[THEME_DARK], theme[THEME_DARK], theme[THEME_DARK]);
    i = 18;
    if(numstep > 0) {
        ui_box(&wins[0], 0, wins[0].h - 18, wins[0].w * step / numstep, 4, theme[THEME_LIGHT], theme[THEME_LIGHT],
            theme[THEME_LIGHT]);
        i = 14;
        sprintf(s, "[ %d / %d ] %3d%%", step, numstep, n);
    } else
        sprintf(s, "%3d%%", n);
    ui_box(&wins[0], 0, wins[0].h - i, (int)(long)wins[0].w * (long)(curr + 1) / (long)(total + 1), i,
        theme[THEME_LIGHT], theme[THEME_LIGHT], theme[THEME_LIGHT]);
    ui_text(&wins[0], 0, wins[0].h - 18, s);
    ui_text(&wins[0], ssfn_dst.x + 8, wins[0].h - 18, !msg ? "" : lang[msg - 1 + STAT_MEASURE]);
    ui_flushwin(&wins[0], 0, wins[0].h - 18, wins[0].w, 18);
}

/**
 * Redraw part of a window
 */
void ui_refreshwin(int idx, int wx, int wy, int ww, int wh)
{
    ui_win_t *win = &wins[idx];
    int i, j, k, m, p, q;
    uint8_t *b = (uint8_t*)&theme[THEME_BG], *c = (uint8_t*)&theme[THEME_LIGHT];

    if(idx < 0 || idx >= numwin || win->w < 8 || win->h < 8) return;
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.bg = 0;
    if(!idx) {
        p = win->w > 256 ? 256 : win->w;
        q = win->h > 256 ? 256 : win->h;
        for(k = (win->h-q+1)*win->p - p, m = ((256 - q) << 8) + (256 - p), j = 0; j < q; j++, k += win->p, m += 256)
            for(i = 0; i < p; i++)
                if(bga[m+i]) {
                    ((uint8_t*)&win->data[k+i])[0] = (c[0]*bga[m+i] + (256 - bga[m+i])*b[0])>>8;
                    ((uint8_t*)&win->data[k+i])[1] = (c[1]*bga[m+i] + (256 - bga[m+i])*b[1])>>8;
                    ((uint8_t*)&win->data[k+i])[2] = (c[2]*bga[m+i] + (256 - bga[m+i])*b[2])>>8;
                }
    }
    view_toolbox(idx);
    ssfn_dst.bg = theme[THEME_BG];
    if(!idx) {
        switch(wins[idx].tool) {
            case -1:
            case MAIN_TOOL_ABOUT: view_about(); break;
            case MAIN_TOOL_LOAD: view_fileops(0); break;
            case MAIN_TOOL_SAVE: view_fileops(1); break;
            case MAIN_TOOL_PROPS: view_props(); break;
            case MAIN_TOOL_RANGES: view_ranges(); break;
            case MAIN_TOOL_GLYPHS: view_glyphs(); break;
            case MAIN_TOOL_DOSAVE: view_dosave(); break;
        }
    } else {
        switch(wins[idx].tool) {
            case GLYPH_TOOL_COORD: view_coords(idx); break;
            case GLYPH_TOOL_LAYER: view_layers(idx); break;
            case GLYPH_TOOL_KERN: view_kern(idx); break;
            case GLYPH_TOOL_COLOR: view_color(idx); break;
        }
    }
    ui_box(win, 0, win->h - 18, win->w, 18, theme[THEME_DARK], theme[THEME_DARK], theme[THEME_DARK]);
    ui_flushwin(win, wx, wy, ww, wh);
}

/**
 * Refresh all windows
 */
void ui_refreshall()
{
    int i;

    for(i=0; i < numwin; i++)
        if(wins[i].winid) {
/*
            if(wins[i].bg) free(wins[i].bg);
            wins[i].bg = NULL;
            if(wins[i].hist) {
                hist_cleanup(&wins[i], 0);
                free(wins[i].hist);
            }
*/
            ui_resizewin(&wins[i], wins[i].w, wins[i].h);
            ui_refreshwin(i, 0, 0, wins[i].w, wins[i].h);
        }
}

/**
 * Main user interface event handler
 */
void ui_main(char *fn)
{
    int i;
    char *status;

    /* load resources */
    ui_loadrc();
    /* driver specific init */
    ui_init();
    /* if specified a file on command line, load it */
    ui_openwin(WINTYPE_MAIN);
    sfn_init(ui_pb);
    ui_cursorwin(&wins[0], CURSOR_LOADING);
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
    wins[0].tool = -1;
    if(fn && sfn_load(fn, 0)) {
        wins[0].tool = MAIN_TOOL_PROPS;
        ui_updatetitle(0);
        ui_resizewin(&wins[0], wins[0].w, wins[0].h);
        ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
    } else
        wins[0].tool = MAIN_TOOL_ABOUT;
    ui_cursorwin(&wins[0], CURSOR_PTR);

    /* main event loop */
    while(mainloop) {
        ui_getevent();
        if(event.win < 0) continue;
        switch(event.type) {
            case E_CLOSE: ui_closewin(event.win); break;
            case E_RESIZE:
/*
                if(wins[event.win].bg) free(wins[event.win].bg);
                wins[event.win].bg = NULL;
*/
                ui_resizewin(&wins[event.win], event.w, event.h);
                ui_refreshwin(event.win, 0, 0, event.w, event.h);
                if(event.win) { gw = event.w; gh = event.h; }
            break;
            case E_REFRESH: ui_refreshwin(event.win, event.x, event.y, event.w, event.h); break;
            case E_MOUSEMOVE:
                status = NULL;
                if(event.y < 23) {
                    i = (event.x - 1) / 24;
                    if(i < (!event.win ? 6 : 3))
                        status = lang[(!event.win ? MTOOL_ABOUT : GTOOL_MEASURES) + i];
                }
                ui_box(&wins[event.win], 0, wins[event.win].h - 18, wins[event.win].w, 18,
                    theme[THEME_DARK], theme[THEME_DARK], theme[THEME_DARK]);
                if(status) ui_text(&wins[event.win], 0, wins[event.win].h - 18, status);
                ui_cursorwin(&wins[event.win], status ? CURSOR_GRAB : CURSOR_PTR);
                ui_flushwin(&wins[event.win], 0, wins[event.win].h - 18, wins[event.win].w, 18);
            break;
            case E_KEY:
                switch(event.x) {
                    case ' ': ui_openwin(0x20); break;
                    case K_ESC: ui_closewin(event.win); break;
                    case K_TAB:
                        if(event.h & 1) {
                            if(wins[event.win].field == -1)
                                wins[event.win].field = ui_maxfield(event.win);
                            else
                                wins[event.win].field--;
                        } else {
                            if(wins[event.win].field == ui_maxfield(event.win))
                                wins[event.win].field = -1;
                            else
                                wins[event.win].field++;
                        }
                        ui_refreshwin(event.win, 0, 0, wins[event.win].w, wins[event.win].h);
                    break;
                    case K_ENTER:
                        if(wins[event.win].field > -1 && wins[event.win].field < (!event.win ? 6 : 3)) {
                            if(!event.win && wins[event.win].field > 1 && wins[event.win].field < 6 && !ctx.filename)
                                wins[event.win].field = 1;
                            wins[event.win].tool = wins[event.win].field;
                            wins[event.win].field = wins[event.win].seltool = -1;
                            ui_resizewin(&wins[event.win], wins[event.win].w, wins[event.win].h);
                            ui_refreshwin(event.win, 0, 0, wins[event.win].w, wins[event.win].h);
                        }
                    break;
                }
            break;
            case E_BTNPRESS:
                if(event.y < 23) {
                    i = (event.x - 1) / 24;
                    if(i < (!event.win ? 6 : 3)) {
                        wins[event.win].seltool = i;
                        ui_refreshwin(event.win, 0, 0, wins[event.win].w, wins[event.win].h);
                    }
                }
            break;
            case E_BTNRELEASE:
                if(event.y < 23) {
                    i = (event.x - 1) / 24;
                    if(i < (!event.win ? 6 : 3) && i == wins[event.win].seltool) {
                        wins[event.win].tool = !event.win && i > 1 && i < 6 && !ctx.filename ? 1 : i;
                        wins[event.win].field = -1;
                        ui_resizewin(&wins[event.win], wins[event.win].w, wins[event.win].h);
                    }
                }
                wins[event.win].seltool = -1;
                ui_refreshwin(event.win, 0, 0, wins[event.win].w, wins[event.win].h);
            break;
        }
    } /* while mainloop */
}
