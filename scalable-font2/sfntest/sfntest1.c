/*
 * sfntest1.c
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
 * @brief testing Scalable Screen Font simple renderer
 *
 */

#include <stdio.h>
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "../ssfn.h"
#if HAS_ZLIB
#include <zlib.h>
#endif

#include <SDL.h>

/**
 * Load a font
 */
ssfn_font_t *load_font(char *filename)
{
    char *fontdata = NULL;
    long int size;
    FILE *f;
#if HAS_ZLIB
    unsigned char hdr[2];
    gzFile g;
#endif

    f = fopen(filename, "rb");
    if(!f) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    size = 0;
#if HAS_ZLIB
    fread(&hdr, 2, 1, f);
    if(hdr[0]==0x1f && hdr[1]==0x8b) {
        fseek(f, -4L, SEEK_END);
        fread(&size, 4, 1, f);
    } else {
        fseek(f, 0, SEEK_END);
        size = ftell(f);
    }
    fclose(f);
    g = gzopen(filename,"r");
#else
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
#endif
    if(!size) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    fontdata = malloc(size);
    if(!fontdata) { fprintf(stderr,"memory allocation error\n"); exit(2); }
#if HAS_ZLIB
    gzread(g, fontdata, size);
    gzclose(g);
#else
    fread(fontdata, size, 1, f);
    fclose(f);
#endif

    return (ssfn_font_t*)fontdata;
}

/**
 * testing the SSFN library (simple bitmap renderer)
 */
void do_test(SDL_Surface *screen)
{
    char *s, *str[] = {
        "Simple renderer with UNICODE VGA!", "Üdvözlet!", "¡Bienvenido!", "Здравствуйте!", "Καλως ηρθες!", "متعدد اللغات",
        NULL };
    char *str0 = "Should appear on the left";
    char *str1 = "Should not appear on the left";
    char *str2 = "Többnyelvű 多种语言 丕世丽乇 Многоязычный with GNU unifont";
    char *str3 = "01234567890123456789012345678901234567890";
    int i;

    /* initialize the simple renderer */
    ssfn_src = load_font("../fonts/u_vga16.sfn.gz");
    ssfn_dst.ptr = (uint8_t*)screen->pixels;
    ssfn_dst.p = screen->pitch;
    ssfn_dst.fg = 0xFFFFFF;
    ssfn_dst.bg = 0;

    for(i = 0; i < screen->pitch / 4 * screen->h; i += 14)
        ((uint32_t*)(screen->pixels))[i] = 0xFF202020;

    printf("Testing simple renderer with fixed size bitmap font\n");

    /* display strings */
    for(i=0; str[i]; i++) {
        s = str[i];
        ssfn_dst.x = 20;
        ssfn_dst.y = 16*(i+1);
        while(*s)
            ssfn_putc(ssfn_utf8(&s));
    }

    printf("Testing simple renderer screen edge cutting\n");

    /* check screen edges */
    for(i=0; i<8; i++) {
        ssfn_dst.bg = i & 1 ? 0x444444 : 0;
        ssfn_dst.y = 16*(i+3)+128;
        ssfn_dst.x = 440+2*i;
        s = str0;
        while(*s)
            ssfn_putc(ssfn_utf8(&s));
    }
    ssfn_dst.w = screen->w;
    for(i=0; i<8; i++) {
        ssfn_dst.bg = i & 1 ? 0x444444 : 0;
        ssfn_dst.y = 16*(i+3);
        ssfn_dst.x = 440+2*i;
        s = str1;
        while(*s)
            ssfn_putc(ssfn_utf8(&s));
    }

    printf("Testing simple renderer with varying sized bitmap font\n");

    /* load another font */
    free(ssfn_src);
    ssfn_src = load_font("../fonts/unifont.sfn.gz");

    /* display strings */
    s = str2;
    ssfn_dst.x = 16;
    ssfn_dst.y = 320;
    while(*s)
        ssfn_putc(ssfn_utf8(&s));

    s = str3;
    ssfn_dst.x = 16;
    ssfn_dst.y = 336;
    while(*s)
        ssfn_putc(ssfn_utf8(&s));

    printf("Memory allocated: %d\n", 0);
    free(ssfn_src);
}

/**
 * Main procedure
 */
int main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Event event;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)) {
        fprintf(stderr,"SDL error %s\n", SDL_GetError());
        return 2;
    }

    window = SDL_CreateWindow("SSFN simple renderer test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    screen = SDL_GetWindowSurface(window);

    do_test(screen);

    do{ SDL_UpdateWindowSurface(window); SDL_Delay(10); } while(SDL_WaitEvent(&event) && event.type != SDL_QUIT &&
        event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_KEYDOWN);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
