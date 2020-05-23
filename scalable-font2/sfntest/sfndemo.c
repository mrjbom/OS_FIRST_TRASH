/*
 * sfndemo.c
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
 * @brief testing Scalable Screen Font renderer features demo
 *
 */

#include <stdio.h>
#include <stdint.h>
#define SSFN_IMPLEMENTATION
#define SSFN_PROFILING
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "../ssfn.h"
#if HAS_ZLIB
#include <zlib.h>
#endif

#include <SDL.h>

unsigned long int loadtim = 0;

/**
 * Load a file
 */
ssfn_font_t *load_file(char *filename, int *size)
{
    char *fontdata = NULL;
    FILE *f;
#if HAS_ZLIB
    unsigned char hdr[2];
    gzFile g;
#endif
#ifdef SSFN_PROFILING
    struct timeval tv0, tv1, tvd;
    gettimeofday(&tv0, NULL);
#endif

    f = fopen(filename, "rb");
    if(!f) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    *size = 0;
#if HAS_ZLIB
    fread(&hdr, 2, 1, f);
    if(hdr[0]==0x1f && hdr[1]==0x8b) {
        fseek(f, -4L, SEEK_END);
        fread(size, 4, 1, f);
    } else {
        fseek(f, 0, SEEK_END);
        *size = (int)ftell(f);
    }
    fclose(f);
    g = gzopen(filename,"r");
#else
    fseek(f, 0, SEEK_END);
    *size = (int)ftell(f);
    fseek(f, 0, SEEK_SET);
#endif
    if(!*size) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    fontdata = (char*)malloc(*size);
    if(!fontdata) { fprintf(stderr,"memory allocation error\n"); exit(2); }
#if HAS_ZLIB
    gzread(g, fontdata, *size);
    gzclose(g);
#else
    fread(fontdata, *size, 1, f);
    fclose(f);
#endif
#ifdef SSFN_PROFILING
    gettimeofday(&tv1, NULL);
    tvd.tv_sec = tv1.tv_sec - tv0.tv_sec;
    tvd.tv_usec = tv1.tv_usec - tv0.tv_usec;
    if(tvd.tv_usec < 0) { tvd.tv_sec--; tvd.tv_usec += 1000000L; }
    loadtim += tvd.tv_sec * 1000000L + tvd.tv_usec;
#endif
    return (ssfn_font_t*)fontdata;
}

/**
 * testing the SSFN library (normal renderer)
 */
void do_test(SDL_Surface *screen, char *fontfn)
{
    char *s;
    char *title="Scalable Screen Font 2.0 (SSFN2) Feature Demo";
    char *str0="Simple renderer आइऊईऋऌ 丕世丽乇 Многоязычный with GNU unifont (unscaled bitmap fonts only)";
    char *str1="Normal renderer आइऊईऋऌ 丕世丽乇 Многоязычный with GNU unifont (scaled to 1:1)";
    char *str2="Using UNICODE VGA but switching to GNU unifont आइऊईऋऌ for uncovered glyphs automatically";
    char *str3="Scaling bitmap fonts without antialiasing to black and white";
    char *str4="Scaling bitmap fonts with antialiasing and edge smoothing";
    char *str5="Vector based fonts are also supported of course";
    char *str6="%Applying #different @styles to !vector bitmap fonts";
    char *str7="%Applying #different @styles to !bitmap vector fonts";
    char *str8="Rendering vector font without antialiasing";
    char *str9="Rendering vector font with antialiasing";
    char *strA="#;$L Rendering vector font without kerning";
    char *strB="#;$L Rendering vector font with kerning";
    char *strC="FreeSerif #Italic #font and %Bold";
    char *strD="FreeSans #Italic #font and %Bold";
    char *strE="BitStream Vera #Italic and %Bold and !Bold+Italic";
    char *strF="(generated)";
    char *str10="(font provided)";
    char *str11="Oh, almost forgot: the compressed 😀 SSFN format supports bitmap based glyphs, vector based glyphs, and...";
    char *str12="->@-COLORFUL-PIXMAP-GLYPHS-=-ALSO>AVAILABLE-BY:->\n->THE-----------<SSFN>FORMAT-AND>RENDERER-;->=@->";
    char *str13="Vertical, horizontal left-to-right and right-to-left rendering (with flag, no BiDi state machine):";
    char *str14="אבגדהוזחט";
    char strM[128];
    int sintbl[] = { 0, 1, 5, 9, 14, 21, 28, 36, 56, 67, 78, 67, 56, 46, 36, 28, 21, 14, 9, 5, 1 };
    int ret, size, i;
    ssfn_t ctx;

    /* initialize the normal renderer */
    memset(&ctx, 0, sizeof(ssfn_t));

    /* set up destination buffer */
    ssfn_dst.ptr = (uint8_t*)screen->pixels;
    ssfn_dst.p = screen->pitch;
    ssfn_dst.w = screen->w;
    ssfn_dst.h = screen->h;
    ssfn_dst.fg = 0xFF202020;

    /* load and select a font */
    ssfn_src = load_file("../fonts/unifont.sfn.gz", &size);
    ret = ssfn_load(&ctx, ssfn_src);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file(fontfn ? fontfn : "../fonts/FreeSerif.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/FreeSans.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* title */
    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 32);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = title;
    ssfn_dst.x = 55;
    ssfn_dst.y = 32;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* testing simple renderer with unscaled bitmap fonts */
    s = str0;
    ssfn_dst.x = 8;
    ssfn_dst.y = 48;
    ret = SSFN_OK;
    while(*s && ret == SSFN_OK)
        ssfn_putc(ssfn_utf8(&s));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn putc error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* testing normal renderer with unscaled bitmap fonts */
    ret = ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str1;
    ssfn_dst.x = 8;
    ssfn_dst.y = 80;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    ssfn_free(&ctx);

    /* load uvga first, then unifont */
    ret = ssfn_load(&ctx, load_file("../fonts/u_vga16.sfn.gz", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, ssfn_src);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/FreeSerif.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/FreeSans.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/Vera.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/emoji.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* testing switching to other fonts when glyph not found */
    ret = ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str2;
    ssfn_dst.x = 8;
    ssfn_dst.y = 100;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* scaling demos */
    s = str3;
    ssfn_dst.x = 8;
    ssfn_dst.y = 135;
    i = 0;
    do {
        ret = ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE | SSFN_STYLE_NOAA,
            10 + sintbl[(i+1)%(sizeof(sintbl)/sizeof(sintbl[0]))]/2);
        if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        i++;
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    } while(ret);

    s = str4;
    ssfn_dst.x = 8;
    ssfn_dst.y = 170;
    i = 0;
    do {
        ret = ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE,
            10 + sintbl[(i+1)%(sizeof(sintbl)/sizeof(sintbl[0]))]/2);
        if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        i++;
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    } while(ret);

    s = str5;
    ssfn_dst.x = 8;
    ssfn_dst.y = 232;
    i = 0;
    do {
        ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE,
            10 + 2*sintbl[i%(sizeof(sintbl)/sizeof(sintbl[0]))]/2);
        if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        i++;
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    } while(ret);

    /* bitmap style tests */
    s = str6;
    ssfn_dst.x = 8;
    ssfn_dst.y = 270;
    while(*s) {
        if(*s=='@') { s++; ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_UNDERLINE | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s=='!') { s++; ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_STHROUGH | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_MONOSPACE, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    s = str7;
    ssfn_dst.x = 420;
    ssfn_dst.y = 270;
    while(*s) {
        if(*s=='@') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_UNDERLINE | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s=='!') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_STHROUGH | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    /* without and with antialias */
    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOAA | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str8;
    ssfn_dst.x = 8;
    ssfn_dst.y = 290;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str9;
    ssfn_dst.x = 8;
    ssfn_dst.y = 310;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* without and with kerning */
    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOKERN | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = strA;
    ssfn_dst.x = 420;
    ssfn_dst.y = 290;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = strB;
    ssfn_dst.x = 420;
    ssfn_dst.y = 310;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* italic and bold in bigger */
    s = strC;
    ssfn_dst.x = 8;
    ssfn_dst.y = 340;
    ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27);
    while(*s) {
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    s = strD;
    ssfn_dst.x = 370;
    ssfn_dst.y = 340;
    ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27);
    while(*s) {
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    s = strF;
    ssfn_dst.x = 700;
    ssfn_dst.y = 340;
    ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = strE;
    ssfn_dst.x = 10;
    ssfn_dst.y = 365;
    ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans", SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 20);
    while(*s) {
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans", SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 20); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans", SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 20); continue; }
        if(*s=='!') { s++; ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans", SSFN_STYLE_BOLD | SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 20); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans", SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 20); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    ret = ssfn_load(&ctx, load_file("../fonts/FreeSerifI.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/FreeSerifB.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/FreeSansI.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/FreeSansB.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/VeraB.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/VeraI.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/VeraBI.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/stoneage.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    ret = ssfn_load(&ctx, load_file("../fonts/chrome.sfn", &size));
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = strC;
    ssfn_dst.x = 8;
    ssfn_dst.y = 400;
    ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27);
    while(*s) {
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    s = strD;
    ssfn_dst.x = 370;
    ssfn_dst.y = 400;
    ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27);
    while(*s) {
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_ITALIC | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 27); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 27); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    s = str10;
    ssfn_dst.x = 700;
    ssfn_dst.y = 400;
    ssfn_select(&ctx, SSFN_FAMILY_SANS, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = strE;
    ssfn_dst.x = 10;
    ssfn_dst.y = 425;
    ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans", SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 20);
    while(*s) {
        if(*s=='#') { s++; ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans Oblique", SSFN_STYLE_NOCACHE, 20); continue; }
        if(*s=='%') { s++; ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans Bold", SSFN_STYLE_NOCACHE, 20); continue; }
        if(*s=='!') { s++; ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans Bold Oblique", SSFN_STYLE_NOCACHE, 20); continue; }
        if(*s==' ') { ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Bitstream Vera Sans", SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 20); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    /* pixel map fonts features */
    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str11;
    ssfn_dst.x = 8;
    ssfn_dst.y = 460;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str12;
    ssfn_dst.x = 8;
    ssfn_dst.y = 485;
    ret = ssfn_select(&ctx, SSFN_FAMILY_DECOR, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_ABS_SIZE | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
    while(*s) {
        if(*s=='>') { ssfn_select(&ctx, SSFN_FAMILY_DECOR, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_ABS_SIZE | SSFN_STYLE_NOCACHE, 16); }
        if(*s=='<') { ssfn_select(&ctx, SSFN_FAMILY_BYNAME, "Retro Chrome", SSFN_STYLE_REGULAR | SSFN_STYLE_ABS_SIZE | SSFN_STYLE_NOCACHE, 24); ssfn_dst.x -= 164; s++; continue; }
        if(*s=='\n') { s++; ssfn_dst.x = 48; ssfn_dst.y += 28; continue; }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    /* writing directions */
    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str13;
    ssfn_dst.x = 8;
    ssfn_dst.y = 550;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_RTL | SSFN_STYLE_NOCACHE, 28);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = str14;
    ssfn_dst.x = screen->w - 8;
    ssfn_dst.y = 550;
    while((ret = ssfn_render(&ctx, &ssfn_dst, s)) > 0) s += ret;
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    /* free resources */
    sprintf(strM, "Memory allocated: @%d bytes. All of this from a 28k of code in a single ANSI C header, that's something, right?",
        ssfn_mem(&ctx));

    ret = ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16);
    if(ret != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }

    s = strM;
    ssfn_dst.x = 32;
    ssfn_dst.y = 585;
    while(*s) {
        if(*s=='@') { s++; ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_BOLD | SSFN_STYLE_NOCACHE, 16); continue; }
        if(*s=='.') { ssfn_select(&ctx, SSFN_FAMILY_SERIF, NULL, SSFN_STYLE_REGULAR | SSFN_STYLE_NOCACHE, 16); }
        ret = ssfn_render(&ctx, &ssfn_dst, s);
        if(ret < SSFN_OK) { fprintf(stderr, "ssfn render error: err=%d %s\n", ret, ssfn_error(ret)); exit(2); }
        s += ret;
    };

    printf("Memory allocated: %d, sizeof(ssfn_t) = %d\n\n", ssfn_mem(&ctx), (int)sizeof(ssfn_t));
    printf("File load time:   %3ld.%06ld sec\n", loadtim / 1000000L, loadtim % 1000000L);
    printf("Character lookup: %3ld.%06ld sec\n", ctx.lookup / 1000000L, ctx.lookup % 1000000L);
    printf("Rasterization:    %3ld.%06ld sec\n", ctx.raster / 1000000L, ctx.raster % 1000000L);
    printf("Blitting:         %3ld.%06ld sec\n", ctx.blit / 1000000L, ctx.blit % 1000000L);
    printf("Kerning:          %3ld.%06ld sec\n", ctx.kern / 1000000L, ctx.kern % 1000000L);
    ssfn_free(&ctx);
    free(ssfn_src);
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

    window = SDL_CreateWindow("SSFN normal renderer bitmap font test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,800,600,0);
    screen = SDL_GetWindowSurface(window);
    memset(screen->pixels, 0xF8, screen->pitch*screen->h);

    do_test(screen, argv[1]);

    do{ SDL_UpdateWindowSurface(window); SDL_Delay(10); } while(SDL_WaitEvent(&event) && event.type != SDL_QUIT &&
        event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_KEYDOWN);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
