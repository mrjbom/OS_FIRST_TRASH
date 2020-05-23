/*
 * libsfn/libsfn.h
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
 * @brief SSFN Utility functions
 *
 */

#include <stdint.h>
#include "ssfn.h"

#define iswhitespace(x) ((x)==0x20||(x)==0xA0)

enum {
    PBAR_NONE = 0,
    PBAR_MEASURE,
    PBAR_OUTLINE,
    PBAR_GETKERN,
    PBAR_QUANT,
    PBAR_BITMAP,
    PBAR_TALLPIX,
    PBAR_WIDEPIX,
    PBAR_GENFRAG,
    PBAR_COMPFRAG,
    PBAR_SERFRAG,
    PBAR_WRTCHARS,
    PBAR_WRTFILE
};
typedef void (*sfnprogressbar_t)(int step, int numstep, int curr, int total, int msg);

typedef struct {
    unsigned char type;
    unsigned char px;
    unsigned char py;
    unsigned char c1x;
    unsigned char c1y;
    unsigned char c2x;
    unsigned char c2y;
} sfncont_t;

typedef struct {
    unsigned char type;
    int w, h;
    int pos;
    int len;
    int cnt;
    int idx;
    unsigned char *data;
} sfnfrag_t;

typedef struct {
    unsigned char type;
    unsigned char color;
    int len, miny, minx;
    unsigned char *data;
} sfnlayer_t;

typedef struct {
    int n;
    char x;
    char y;
} sfnkern_t;

typedef struct {
    int first;
    int last;
    int idx;
} sfnkgrp_t;

typedef struct {
    int idx;
    int pos;
    int len;
    unsigned char *data;
} sfnkpos_t;

typedef struct {
    unsigned char width;
    unsigned char height;
    unsigned char ovl_x;
    unsigned char adv_x;
    unsigned char adv_y;
    unsigned char numlayer;
    sfnlayer_t *layers;
    int numkern, kgrp;
    sfnkern_t *kern;
    unsigned char hintv[32], hinth[32];
    int numfrag;
    int *frags;
} sfnglyph_t;

typedef struct {
    unsigned char family;
    unsigned char style;
    unsigned char width;
    unsigned char height;
    unsigned char baseline;
    unsigned char underline;
    char *filename;
    char *name;
    char *familyname;
    char *subname;
    char *revision;
    char *manufacturer;
    char *license;
    sfnglyph_t glyphs[0x110000];
    int numcpal;
    unsigned char cpal[1024];
    char *ligatures[SSFN_LIG_LAST-SSFN_LIG_FIRST+1];
    int numfrags;
    sfnfrag_t *frags;
    int numkpos;
    sfnkpos_t *kpos;
    int numskip;
    int *skip;
    long int total;
} sfnctx_t;
extern sfnctx_t ctx;
extern sfnprogressbar_t pbar;

/*** arguments ***/
extern int rs, re, replace, hinting, adv, relul, rasterize, dump, origwh, unicode, lastuni, quiet, dorounderr;

/*** unicode.c ***/
#include "unicode.h"

/*** util.c ***/
#include "util.h"

/*** sfn.c ***/
void sfn_init(sfnprogressbar_t pb);
void sfn_setfamilytype(int t);
void sfn_setstr(char **s, char *n, int len);
void sfn_chardel(int unicode);
int sfn_charadd(int unicode, int w, int h, int ax, int ay, int ox);
sfnlayer_t *sfn_layeradd(int unicode, int t, int x, int y, int w, int h, int c, unsigned char *data);
int sfn_contadd(sfnlayer_t *lyr, int t, int px, int py, int c1x, int c1y, int c2x, int c2y);
int sfn_kernadd(int unicode, int next, int x, int y);
void sfn_hintadd(int unicode);
unsigned char sfn_cpaladd(int r, int g, int b, int a);
void sfn_skipadd(int unicode);
int sfn_load(char *filename, int dump);
int sfn_save(char *filename, int ascii, int compress);
void sfn_sanitize();
void sfn_rasterize(int size);
void sfn_coverage();
void sfn_free();

/*** potrace.c ***/
void sfn_vectorize();
