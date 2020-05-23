/*
 * sfnedit/ui_sdl.c
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
 * @brief User interface driver for SDL
 *
 */

#include <SDL.h>
#include <string.h>
#include "lang.h"
#include "util.h"
#include "ui.h"

extern unsigned int wm_icon[];
SDL_Surface *icons = NULL;
SDL_Cursor *cursors[3];
int btnflags = 0, keyflags = 0, mx = 0, my = 0, ti = 0;

/**
 * Create a window
 */
void *ui_createwin(char *title, int w, int h)
{
    SDL_Window *window;

    if(!(window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_RESIZABLE)))
        return NULL;

    SDL_SetWindowIcon(window, icons);
    return (void *)window;
}

/**
 * Resize a window
 */
void ui_resizewin(ui_win_t *win, int w, int h)
{
    int i;

    win->surface = SDL_GetWindowSurface((SDL_Window *)win->winid);
    win->data = (uint32_t*)((SDL_Surface*)win->surface)->pixels;
    win->p = ((SDL_Surface*)win->surface)->pitch/4;
    win->w = w;
    win->h = h;
    for(i = 0; i < h * win->p; i++) win->data[i] = theme[THEME_BG];
}

/**
 * Display modified window
 */
void ui_flushwin(ui_win_t *win, int x __attribute__((unused)), int y __attribute__((unused)),
    int w __attribute__((unused)), int h __attribute__((unused)))
{
    SDL_UpdateWindowSurface((SDL_Window*)win->winid);
}

/**
 * Destroy a window
 */
void ui_destroywin(ui_win_t *win)
{
    SDL_DestroyWindow((SDL_Window*)win->winid);
}

/**
 * Focus a window
 */
void ui_focuswin(ui_win_t *win)
{
    SDL_UpdateWindowSurface((SDL_Window*)win->winid);
    SDL_ShowWindow((SDL_Window*)win->winid);
    SDL_RaiseWindow((SDL_Window*)win->winid);
}

/**
 * Change cursor of window
 */
void ui_cursorwin(ui_win_t *win __attribute__((unused)), int cursor)
{
    SDL_SetCursor(cursors[cursor]);
}

/**
 * Initialize SDL driver
 */
void ui_init()
{

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)) error("sdl", ERR_DISPLAY);

    icons = SDL_CreateRGBSurfaceFrom((Uint32 *)&wm_icon[2], wm_icon[0], wm_icon[1], 32, wm_icon[0]*4,
        0xFF0000, 0xFF00, 0xFF, 0xFF000000);
    cursors[CURSOR_LOADING] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
    cursors[CURSOR_PTR]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    cursors[CURSOR_CROSS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    cursors[CURSOR_MOVE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    cursors[CURSOR_GRAB] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    SDL_StartTextInput();
}

/**
 * Finish SDL driver
 */
void ui_fini()
{
    int i;

    SDL_StopTextInput();
    for(i=0;i<5;i++) SDL_FreeCursor(cursors[i]);
    SDL_FreeSurface(icons);
    SDL_VideoQuit();
    SDL_Quit();
}

/**
 * Convert an SDL event
 */
void ui_getevent()
{
    SDL_Event e;
    if(SDL_WaitEvent(&e)) {
        switch(e.type) {
            case SDL_QUIT: event.type = E_CLOSE; event.win = 0; break;
            case SDL_WINDOWEVENT:
                event.win = ui_getwin(SDL_GetWindowFromID(e.window.windowID));
                switch(e.window.event) {
                    case SDL_WINDOWEVENT_CLOSE: event.type = E_CLOSE; break;
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        event.type = E_RESIZE;
                        event.x = event.y = 0;
                        event.w = e.window.data1;
                        event.h = e.window.data2;
                    break;
                    default:
                        event.type = E_REFRESH;
                        event.x = event.y = 0;
                        event.w = e.window.data1;
                        event.h = e.window.data2;
                    break;
                }
            break;
            case SDL_TEXTINPUT:
                event.x = 0;
                memcpy(&event.x, e.text.text, strlen(e.text.text)+1);
                if(event.x) {
                    event.win = ui_getwin(SDL_GetWindowFromID(e.key.windowID));
                    event.h = keyflags;
                    event.type = E_KEY;
                    ti++;
                }
            break;
            case SDL_KEYUP:
                if(ti) { ti--; break; }
                event.x = e.key.keysym.sym;
                event.win = ui_getwin(SDL_GetWindowFromID(e.key.windowID));
                switch(e.key.keysym.sym) {
                    case SDLK_F1: event.x = K_F1; break;
                    case SDLK_ESCAPE: event.x = K_ESC; break;
                    case SDLK_DELETE: event.x = K_DEL; break;
                    case SDLK_BACKSPACE: event.x = K_BACKSPC; break;
                    case SDLK_TAB: event.x = K_TAB; break;
                    case SDLK_UP: event.x = K_UP; break;
                    case SDLK_DOWN: event.x = K_DOWN; break;
                    case SDLK_LEFT: event.x = K_LEFT; break;
                    case SDLK_RIGHT: event.x = K_RIGHT; break;
                    case SDLK_HOME: event.x = K_HOME; break;
                    case SDLK_END: event.x = K_END; break;
                    case SDLK_PAGEUP: event.x = K_PGUP; break;
                    case SDLK_PAGEDOWN: event.x = K_PGDN; break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT: event.x = 0; keyflags &= ~1; break;
                    case SDLK_LCTRL:
                    case SDLK_RCTRL: event.x = 0; keyflags &= ~2; break;
                    case SDLK_LGUI:
                    case SDLK_RGUI:
                    case SDLK_LALT:
                    case SDLK_RALT: event.x = 0; keyflags &= ~4; break;
                }
                event.h = keyflags;
                if(event.x) event.type = E_KEY;
            break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT: keyflags |= 1; break;
                    case SDLK_LCTRL:
                    case SDLK_RCTRL: keyflags |= 2; break;
                    case SDLK_LGUI:
                    case SDLK_RGUI:
                    case SDLK_LALT:
                    case SDLK_RALT: keyflags |= 4; break;
                }
            break;
            case SDL_MOUSEMOTION:
                event.type = E_MOUSEMOVE;
                event.win = ui_getwin(SDL_GetWindowFromID(e.motion.windowID));
                event.x = mx = e.motion.x;
                event.y = my = e.motion.y;
                event.w = btnflags;
                event.h = keyflags;
            break;
            case SDL_MOUSEBUTTONDOWN:
                event.type = E_BTNPRESS;
                event.win = ui_getwin(SDL_GetWindowFromID(e.button.windowID));
                event.x = mx = e.button.x;
                event.y = my = e.button.y;
                btnflags |= (1<<(e.button.button-1));
                event.w = btnflags;
                event.h = keyflags;
            break;
            case SDL_MOUSEBUTTONUP:
                event.type = E_BTNRELEASE;
                event.win = ui_getwin(SDL_GetWindowFromID(e.button.windowID));
                event.x = mx = e.button.x;
                event.y = my = e.button.y;
                btnflags &= ~(1<<(e.button.button-1));
                event.w = btnflags;
                event.h = keyflags;
            break;
            case SDL_MOUSEWHEEL:
                event.type = E_BTNPRESS;
                event.win = ui_getwin(SDL_GetWindowFromID(e.button.windowID));
                event.x = mx;
                event.y = my;
                event.w = btnflags | (1 << (e.wheel.y > 0? 3 : (e.wheel.y < 0? 4 : (e.wheel.x > 0? 5 : 6))));
                event.h = keyflags;
            break;
        }
    }
}
