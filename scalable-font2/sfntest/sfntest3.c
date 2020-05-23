/*
 * sfntest3.c
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
 * @brief testing Scalable Screen Font renderer scaling
 *
 */

#include <stdio.h>
#define SSFN_IMPLEMENTATION
#include "../ssfn.h"

#include <SDL.h>

/**
 * Load a font
 */
ssfn_font_t *load_file(char *filename, int *size)
{
    char *fontdata = NULL;
    FILE *f;

    f = fopen(filename, "rb");
    if(!f) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    *size = 0;
    fseek(f, 0, SEEK_END);
    *size = (int)ftell(f);
    fseek(f, 0, SEEK_SET);
    if(!*size) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    fontdata = (char*)malloc(*size);
    if(!fontdata) { fprintf(stderr,"memory allocation error\n"); exit(2); }
    fread(fontdata, *size, 1, f);
    fclose(f);
    return (ssfn_font_t*)fontdata;
}

/**
 * testing the SSFN library (normal renderer)
 */
void do_test(SDL_Surface *screen, char *fontfn, int argc)
{
    char *s, str0[64];
    int i, ret, size;
    ssfn_t ctx;
    ssfn_font_t *font;
    ssfn_buf_t buf;

    /* initialize the normal renderer */
    memset(&ctx, 0, sizeof(ssfn_t));
    buf.ptr = (uint8_t*)screen->pixels;
    buf.p = screen->pitch;
    buf.w = screen->w;
    buf.h = screen->h;
    buf.fg = 0xFF202020;

    /* load and select a font */
    font = load_file(fontfn ? fontfn : "../fonts/FreeSerif.sfn", &size);
    ret = ssfn_load(&ctx, font);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    buf.y = 10;
    for(i=8; buf.y < screen->h - 60; i++) {
        ret = ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR | (argc>2? SSFN_STYLE_NOKERN | SSFN_STYLE_NOCACHE:0), i);
        if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

        sprintf(str0,"%d How much can you read this?",i);
        s = str0;
        buf.x = 5;
        while((ret = ssfn_render(&ctx, &buf, s)) > 0) s += ret;
        if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        buf.y += i;
    }

    /* meaning of... */
    i = 42;
    sprintf(str0,"%d How much can you read this?",i);
    ret = ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR | (argc>2? SSFN_STYLE_NOKERN | SSFN_STYLE_NOCACHE:0), i);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str0;
    buf.x = 5;
    buf.y += 10;
    while((ret = ssfn_render(&ctx, &buf, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    printf("Memory allocated: %d\n", ssfn_mem(&ctx));
    ssfn_free(&ctx);
    free(font);
}

/**
 * Main procedure
 */
int main(int argc __attribute__((unused)), char **argv)
{
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Event event;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)) {
        fprintf(stderr,"SDL error %s\n", SDL_GetError());
        return 2;
    }

    window = SDL_CreateWindow("SSFN scaling test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    screen = SDL_GetWindowSurface(window);
    memset(screen->pixels, 0xF8, screen->pitch*screen->h);

    do_test(screen, argv[1], argc);

    do{ SDL_UpdateWindowSurface(window); SDL_Delay(10); } while(SDL_WaitEvent(&event) && event.type != SDL_QUIT &&
        event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_KEYDOWN);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
