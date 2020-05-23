/*
 * sfnedit/ui.h
 *
 * Copyright (C) 2019 bzt (bztsrc@gitlab)
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
 * @brief Common user interface definitions and prototypes
 *
 */

#include <stdint.h>

#define THEME_INPUT     0
#define THEME_TABA      1
#define THEME_FG        2
#define THEME_TABU      3
#define THEME_INACT     4
#define THEME_BGLOGO    5
#define THEME_BG        6
#define THEME_INPBG     7
#define THEME_TABBG     8
#define THEME_SAVEACT   9
#define THEME_SAVEINACT 10
#define THEME_BASELINE  11
#define THEME_ONCURVE   12
#define THEME_CONTROL   13
#define THEME_ADVANCE   14
#define THEME_HINTING   15

#define MAIN_W          800
#define MAIN_H          600

#define CURSOR_LOADING  0
#define CURSOR_PTR      1
#define CURSOR_CROSS    2
#define CURSOR_MOVE     3
#define CURSOR_GRAB     4

#define WINTYPE_MAIN    -1U

#define E_NONE          0
#define E_CLOSE         1
#define E_RESIZE        2
#define E_REFRESH       3
#define E_KEY           4
#define E_MOUSEMOVE     5
#define E_BTNPRESS      6
#define E_BTNRELEASE    7

#define K_DOWN          1
#define K_UP            2
#define K_LEFT          3
#define	K_RIGHT         4
#define K_PGDN          5
#define	K_PGUP          6
#define K_DEL           7
#define K_BACKSPC       8
#define K_TAB           9
#define	K_HOME          11
#define	K_END           12
#define K_ENTER         13
#define K_F1            14
#define K_ESC           27

#define MAIN_TAB_PROPS  0
#define MAIN_TAB_RANGES 1
#define MAIN_TAB_GLYPHS 2

#define GLYPH_TAB_EDIT  0
#define GLYPH_TAB_KERN  1

#define MAX_TAB         3

typedef struct {
    int type;
    int px;
    int py;
    int c1x;
    int c1y;
    int c2x;
    int c2y;
} cont_t;

typedef struct {
    int type;
    int frag;
    int idx;
    union {
        struct {
            uint8_t oldpix;
            uint8_t newpix;
        } pixel;
        struct {
            int oldx;
            int oldy;
            int newx;
            int newy;
        } adv;
        cont_t cont;
        struct {
            cont_t *oldc;
            int newidx;
            cont_t *newc;
        } contlist;
        struct {
            int *oldx;
            int *newx;
        } hintlist;
    } data;
} hist_t;


typedef struct {
    void *winid;
    void *surface;
    uint32_t *data;
    uint32_t unicode;
    const char *uniname;
    int v;
    int chr;
    int w;
    int h;
    int p;
    int help;
    int tab;
    int tabpos[MAX_TAB];
    int menu;
    int frag;
    int fscroll;
    int cont, pt;
    int zoom, zx, zy;
    uint32_t *bg;
    int histmin, histmax;
    hist_t *hist;
} ui_win_t;

typedef struct {
    int win;
    int type;
    long x;
    int y;
    int w;
    int h;
} ui_event_t;

extern uint32_t ssfn_fg, ssfn_bg, ssfn_x, ssfn_y;
extern char nchr[], search[], gsearch[], gtmp[], ksearch[];
extern uint32_t tool_icons[];
extern ui_event_t event;
extern ui_win_t *wins;

extern uint32_t theme[];
extern int savingmax;

/* driver specific */
void *ui_createwin(char *title, int w, int h);
void ui_resizewin(ui_win_t *win, int w, int h);
void ui_flushwin(ui_win_t *win, int x, int y, int w, int h);
void ui_destroywin(ui_win_t *win);
void ui_focuswin(ui_win_t *win);
void ui_cursorwin(ui_win_t *win, int cursor);
void ui_init();
void ui_fini();
void ui_getevent();

/* common */
void ui_openwin(int v, uint32_t unicode);
void ui_closewin(int idx);
int ui_getwin(void *winid);
int ui_textwidth(char *s, int len);
void ui_icon(ui_win_t *win, uint32_t *icon, int x, int y, int ih);
void ui_text(ui_win_t *win, char *s, uint32_t a, uint32_t u);
void ui_scale(ui_win_t *win, char *s, int w, int th);
void ui_bool(ui_win_t *win, char *s, int state, int th);
void ui_button(ui_win_t *win, char *s, int w, int b, int th);
int ui_input(ui_win_t *win, char *s, int w, int b, int th);
void ui_progress(ui_win_t *win, int val, int max, int w, int h, int th);
void ui_glyph(ui_win_t *win, int chr, uint32_t unicode, int u, int th);
void ui_bright(ui_win_t *win, int x, int y, int w, int h, int all);
void ui_box(ui_win_t *win, int x, int y, int w, int h, uint32_t c);
void ui_argb(ui_win_t *win, int x, int y, int w, int h, uint32_t c);
void ui_number(ui_win_t *win, int n, int x, int y, uint32_t c);
void ui_tabs(ui_win_t *win, int idx);
void ui_saving(int idx, int scale);
void ui_refreshwin(int idx, int x, int y, int w, int h);
void ui_refreshall();
void ui_updatewin(int v, uint32_t unicode, int diff);
void ui_main(char *fn);
void ui_forcequit(int sig);
void view_help(ui_win_t *win);

/* main window tabs */
void view_props(ui_win_t *win);
int ctrl_props(ui_win_t *win, ui_event_t *evt);
void view_ranges(ui_win_t *win);
int ctrl_ranges(ui_win_t *win, ui_event_t *evt);
void view_glyphs(ui_win_t *win);
int ctrl_glyphs(ui_win_t *win, ui_event_t *evt);

/* glyph window tabs */
void view_edit(ui_win_t *win);
int ctrl_edit(ui_win_t *win, ui_event_t *evt);
void view_kern(ui_win_t *win);
int ctrl_kern(ui_win_t *win, ui_event_t *evt);
void view_color(ui_win_t *win, uint8_t c);
int ctrl_color(ui_win_t *win, ui_event_t *evt, uint8_t *c);
