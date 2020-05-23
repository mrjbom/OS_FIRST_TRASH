/*
 * libsfn/bitmap.c
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
 * @brief File functions for bitmap and pixmap fonts
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAS_QUANT
#include <libimagequant.h>
#endif
#define STBI_IMPLEMENTATION
#include "stb_png.h"
#include <ssfn.h>
#include "libsfn.h"
#include "bitmap.h"

typedef struct {
    unsigned int magic;
    unsigned int version;
    unsigned int headersize;
    unsigned int flags;
    unsigned int numglyph;
    unsigned int bytesperglyph;
    unsigned int height;
    unsigned int width;
    unsigned char glyphs;
} __attribute__((packed)) psf_t;

/****************************** file format parsers ******************************/

/**
 * Parse PSF2 font (binary)
 */
void psf(unsigned char *ptr, int size)
{
    psf_t *psf = (psf_t*)ptr;
    uint16_t *utbl = NULL;
    uint32_t c, g=0, unicode, nc = 0, numchars = psf->numglyph;
    unsigned char *s, *e, *glyph, *bitmap;
    int i, j, k;

    s=(unsigned char*)(ptr + psf->headersize + psf->numglyph*psf->bytesperglyph);
    e=ptr + size;
    /* try to fix bad fonts */
    if(s > e) {
        for(s = e; s + 1 > ptr + psf->headersize && (s[-1] || s[0] == 0xFF); s--);
        psf->numglyph = ((int)(s - ptr) - psf->headersize) / psf->bytesperglyph;
    }
    if(s < e) {
        numchars = 0;
        utbl = (uint16_t*)malloc(0x110000*sizeof(uint16_t));
        if(!utbl) { fprintf(stderr,"memory allocation error\n"); return; }
        memset(utbl, 0, 0x110000*sizeof(uint16_t));
        while(s<e && g<psf->numglyph) {
            c = (uint16_t)((uint8_t)s[0]);
            if(c == 0xFF) { g++; } else {
                if((c & 128) != 0) {
                    if((c & 32) == 0 ) { c=((s[0] & 0x1F)<<6)+(s[1] & 0x3F); s++; } else
                    if((c & 16) == 0 ) { c=((((s[0] & 0xF)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F); s+=2; } else
                    if((c & 8) == 0 ) { c=((((((s[0] & 0x7)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F))<<6)+(s[3] & 0x3F); s+=3;}
                    else c=0;
                }
                if(c<0x110000) {
                    utbl[c] = g;
                    numchars++;
                }
            }
            s++;
        }
    }
    if((psf->flags >> 24) && !ctx.baseline) ctx.baseline = (psf->flags >> 24);
    bitmap = (unsigned char*)malloc(psf->width * psf->height);
    if(!bitmap) { fprintf(stderr,"memory allocation error\n"); return; }
    for(unicode=rs;unicode<=(uint32_t)re;unicode++) {
        g = utbl? utbl[unicode] : unicode;
        if((!g && unicode && !iswhitespace(unicode)) || g >= psf->numglyph) continue;
        glyph = ptr + psf->headersize + g*psf->bytesperglyph;
        memset(bitmap, 0, psf->width * psf->height);
        for(i=k=0;i<(int)psf->bytesperglyph;i++) {
            for(j=0x80;j;j>>=1) bitmap[k++] = (glyph[i] & j) ? 0xFE : 0xFF;
        }
        if(sfn_charadd(unicode, psf->width, psf->height, 0, 0, 0))
            sfn_layeradd(unicode, SSFN_FRAG_BITMAP, 0, 0, psf->width, psf->height, 0xFE, bitmap);
        if(pbar) (*pbar)(0, 0, ++nc, numchars, PBAR_BITMAP);
    }
    free(bitmap);
    free(utbl);
}

/**
 * Parse GNU unifont hex format (text)
 */
void hex(char *ptr, int size)
{
    uint32_t i, j, c, unicode, nc = 0, numchars;
    int w;
    char *end = ptr + size;
    unsigned char bitmap[256];

    numchars = 0;
    for(numchars=0;ptr < end && *ptr;ptr++) if(*ptr=='\n') numchars++;
    ptr = end - size;
    while(ptr < end && *ptr) {
        unicode = gethex(ptr, 6);
        while(*ptr && *ptr!=':') ptr++;
        if(!*ptr) return;
        ptr++;
        while(*ptr && (*ptr==' '||*ptr=='\t')) ptr++;
        if(!*ptr) return;
        memset(bitmap, 0, 256);
        for(i = 0;i<256 && *ptr && *ptr!='\n' && *ptr!='\r';ptr += 2) {
            c = gethex(ptr, 2);
            for(j=0x80;j;j>>=1) bitmap[i++] = (c & j) ? 0xFE : 0xFF;
        }
        while(*ptr && *ptr!='\n' && *ptr!='\r') ptr++;
        while(*ptr && (*ptr=='\n' || *ptr=='\r')) ptr++;
        w = i > 128 ? 16 : 8;
        if(sfn_charadd(unicode, w, 16, 0, 0, 0))
            sfn_layeradd(unicode, SSFN_FRAG_BITMAP, 0, 0, w, 16, 0xFE, bitmap);
        if(pbar) (*pbar)(0, 0, ++nc, numchars, PBAR_BITMAP);
    }
}

/**
 * Parse X11 BDF font format (text)
 */
void bdf(char *ptr, int size)
{
    uint32_t c;
    int w, h, i, j, unicode, nc = 0, numchars = 0, defchar = 0;
    char *end = ptr + size, *face, *name = NULL, *style = NULL, *manu = NULL;
    unsigned char *bitmap = NULL;

    for(face = ptr; face < end && *face; face++)
        if(!memcmp(face, "ENCODING ", 9)) numchars++;
    face = NULL;

    while(ptr < end && *ptr) {
        if(!memcmp(ptr, "FACE_NAME ", 10) && !ctx.name && !name) {
            ptr += 10; if(*ptr=='\"') ptr++; face = ptr;
        }
        if(!memcmp(ptr, "FONT_VERSION ", 13) && !ctx.revision) {
            ptr += 13; if(*ptr=='\"') ptr++;
            sfn_setstr(&ctx.revision, ptr, 0);
        }
        if(!memcmp(ptr, "ADD_STYLE_NAME ", 15) && !ctx.name && !style) {
            ptr += 15; if(*ptr=='\"') ptr++; style = ptr;
        }
        if(!memcmp(ptr, "FOUNDRY ", 8) && !ctx.manufacturer) {
            ptr += 8; if(*ptr=='\"') ptr++; manu = ptr;
        }
        if(!memcmp(ptr, "HOMEPAGE ", 9) && !ctx.manufacturer && !manu) {
            ptr += 9; if(*ptr=='\"') ptr++; manu = ptr;
        }
        if(!memcmp(ptr, "FAMILY_NAME ", 12) && !ctx.familyname) {
            ptr += 12; if(*ptr=='\"') ptr++;
            sfn_setstr(&ctx.familyname, ptr, 0);
        }
        if(!memcmp(ptr, "WEIGHT_NAME ", 12) && !ctx.subname) {
            ptr += 12; if(*ptr=='\"') ptr++;
            sfn_setstr(&ctx.subname, ptr, 0);
        }
        if(!memcmp(ptr, "COPYRIGHT ", 10) && !ctx.license) {
            ptr += 10; if(*ptr=='\"') ptr++;
            sfn_setstr(&ctx.license, ptr, 0);
        }
        if(!memcmp(ptr, "FONT_ASCENT ", 12)) { ptr += 12; if(!ctx.baseline) ctx.baseline = atoi(ptr); }
        if(!memcmp(ptr, "UNDERLINE_POSITION ", 19)) { ptr += 19; relul = atoi(ptr); }
        if(!memcmp(ptr, "DEFAULT_CHAR ", 13)) { ptr += 13; defchar = atoi(ptr); break; }
        while(*ptr && *ptr!='\n') ptr++;
        while(*ptr=='\n') ptr++;
    }
    if(!ctx.name) {
        if(!face) face = ctx.familyname;
        if(face && style && style[0]) {
            for(i = 0; face[i] && face[i] != '\"' && face[i] != '\r' && face[i] != '\n'; i++);
            for(j = 0; style[j] && style[i] != '\"' && style[i] != '\r' && style[i] != '\n'; j++);
            name = malloc(i + j + 2);
            if(!name) { fprintf(stderr,"memory allocation error\n"); return; }
            memcpy(name, face, i);
            name[i] = ' ';
            memcpy(name + i + 1, style, j);
            name[i + j + 1] = 0;
            sfn_setstr(&ctx.name, name, 0);
            free(name);
        } else
            sfn_setstr(&ctx.name, ctx.familyname, 0);
    }
    if(!ctx.manufacturer && manu)
        sfn_setstr(&ctx.manufacturer, manu, 0);

    while(ptr < end && *ptr) {
        if(!memcmp(ptr, "ENCODING ", 9)) { ptr += 9; unicode = atoi(ptr); }
        if(!memcmp(ptr, "BBX ", 4)) {
            ptr += 4; w = atoi(ptr);
            while(*ptr && *ptr!=' ') ptr++;
            ptr++; h = atoi(ptr);
        }
        if(!memcmp(ptr, "BITMAP", 6)) {
            ptr += 6; while(*ptr && *ptr!='\n') ptr++;
            ptr++;
            bitmap = realloc(bitmap, w * h);
            if(!bitmap) { fprintf(stderr,"memory allocation error\n"); return; }
            for(i = 0;i < w * h && *ptr; ptr += 2) {
                while(*ptr=='\n' || *ptr=='\r') ptr++;
                c = gethex(ptr, 2);
                for(j=0x80;j;j>>=1) bitmap[i++] = c & j ? 0xFE : 0xFF;
            }
            while(i < w * h) bitmap[i++] = 0;
            if(unicode == defchar) {
                sfn_chardel(0);
                unicode = 0;
            }
            if(sfn_charadd(unicode, w, h, 0, 0, 0))
                sfn_layeradd(unicode, SSFN_FRAG_BITMAP, 0, 0, w, h, 0xFE, bitmap);
            if(pbar) (*pbar)(0, 0, ++nc, numchars, PBAR_BITMAP);
        }
        while(*ptr && *ptr!='\n') ptr++;
        while(*ptr=='\n') ptr++;
    }
    if(bitmap) free(bitmap);
}

/**
 * Parse a pixel map in memory
 */
void pixmap_parse(unsigned char *data, int w, int h)
{
    unsigned char *data2;
    int i, j, k, x, y, m, o, unicode;

    if(rs==0 && re==0x10FFFF)
        re = h > w ? h / w : w / h;
    if(h > w ) {
        m = h / (re - rs + 1);
        if(m < 8 || w < 8) { fprintf(stderr, "unable to determine glyph size\n"); return; }
        for(unicode=rs, i=0; unicode<=re; unicode++, i += w*m) {
            for(y=k=0;y<m;y++)
                for(j=w-1;j>k;j--)
                    if(data[i+y*w+j] < 0xFE) k=j;
            if(sfn_charadd(unicode, w, m, k+1, 0, 0))
                sfn_layeradd(unicode, SSFN_FRAG_PIXMAP, 0, 0, w, m, 0xFE, data + i);
            if(pbar) (*pbar)(0, 0, unicode, re - rs, PBAR_TALLPIX);
        }
    } else {
        m = w / (re - rs + 1);
        if(m < 8 || h < 8) { fprintf(stderr, "unable to determine glyph size\n"); return; }
        data2 = (unsigned char*)malloc(m*h);
        if(!data2) { fprintf(stderr,"memory allocation error\n"); return; }
        for(unicode=rs; unicode<=re; unicode++) {
            for(y=o=k=0;y<h;y++) {
                i = y*w + (unicode-rs)*m;
                for(x=0;x<m;x++) {
                    if(data[i] < 0xFE && k < x) k = x;
                    data2[o++] = data[i++];
                }
            }
            if(sfn_charadd(unicode, m, h, k+1, 0, 0))
                sfn_layeradd(unicode, SSFN_FRAG_PIXMAP, 0, 0, m, h, 0xFE, data2);
            if(pbar) (*pbar)(0, 0, unicode, re - rs, PBAR_WIDEPIX);
        }
        free(data2);
    }
}

/**
 * Parse PNG format for pixel fonts (binary)
 */
void png(unsigned char *ptr, int size)
{
    unsigned char *data, *data2;
    unsigned int i, w, h, f;
    stbi__context s;
    stbi__result_info ri;
#ifdef HAS_QUANT
    liq_attr *handle = NULL;
    liq_image *input_image = NULL;
    liq_result *quantization_result;
    const liq_palette *liqpalette;
    unsigned char pal[256];
#endif

    s.read_from_callbacks = 0;
    s.img_buffer = s.img_buffer_original = ptr;
    s.img_buffer_end = s.img_buffer_original_end = ptr+size;
    w = h = size = 0;
    ri.bits_per_channel = 8;
    data = (uint8_t*)stbi__png_load(&s, (int*)&w, (int*)&h, (int*)&f, 0, &ri);
    if(!data || (f != STBI_rgb_alpha && f != STBI_rgb) || w < 1 || h < 1) { fprintf(stderr,"Unsupported PNG format\n"); return; }
    data2 = (unsigned char*)malloc(w * h);
    if(!data2) { fprintf(stderr,"memory allocation error\n"); return; }
#ifdef HAS_QUANT
    handle = liq_attr_create();
    liq_set_max_colors(handle, 254);
    if(f == STBI_rgb_alpha)
        input_image = liq_image_create_rgba(handle, data, w, h, 0);
    if(pbar) (*pbar)(0, 0, 0, 1, PBAR_QUANT);
    if (f == STBI_rgb_alpha && liq_image_quantize(input_image, handle, &quantization_result) == LIQ_OK) {
        liq_set_dithering_level(quantization_result, 1.0);
        liqpalette = liq_get_palette(quantization_result);
        liq_write_remapped_image(quantization_result, input_image, data, w * h);
        for(i = 0; i < liqpalette->count && i < 254; i++) {
            pal[i] = sfn_cpaladd(liqpalette->entries[i].r, liqpalette->entries[i].g,
                liqpalette->entries[i].b, liqpalette->entries[i].a);
        }
        for(i = 0; i < w * h; i++)
            data2[i] = pal[data[i]];
        liq_result_destroy(quantization_result);
    } else
#endif
    {
        if(f == STBI_rgb_alpha)
            for(i = 0; i < w * h; i++) {
                if(pbar) (*pbar)(0, 0, i, w * h, PBAR_QUANT);
                data2[i] = sfn_cpaladd(data[i*4+2], data[i*4+1], data[i*4], data[i*4+3]);
            }
        else
            for(i = 0; i < w * h; i++) {
                if(pbar) (*pbar)(0, 0, i, w * h, PBAR_QUANT);
                data2[i] = sfn_cpaladd(data[i*3+2], data[i*3+1], data[i*3], 255);
            }
    }
#ifdef HAS_QUANT
    if(input_image) liq_image_destroy(input_image);
    liq_attr_destroy(handle);
#endif
    free(data);
    pixmap_parse(data2, w, h);
    free(data2);
}

/**
 * Parse TGA format for pixel fonts (binary)
 */
void tga(unsigned char *ptr, int size)
{
    unsigned char *data;
    int i, j, k, x, y, w, h, o, m;

    o = (ptr[11] << 8) + ptr[10];
    w = (ptr[13] << 8) + ptr[12];
    h = (ptr[15] << 8) + ptr[14];
    if(w<1 || h<1) {
tgaerr: fprintf(stderr,"unsupported TGA file format\n");
        return;
    }
    m = ((ptr[1]? (ptr[7]>>3)*ptr[5] : 0) + 18);
    data = (unsigned char*)malloc(w*h);
    if(!data) { fprintf(stderr,"memory allocation error\n"); return; }
    switch(ptr[2]) {
        case 1:
            if(ptr[6]!=0 || ptr[4]!=0 || ptr[3]!=0 || (ptr[7]!=24 && ptr[7]!=32)) goto tgaerr;
            for(y=i=0; y<h; y++) {
                k = ((!o?h-y-1:y)*w);
                for(x=0; x<w; x++) {
                    j = ptr[m + k++]*(ptr[7]>>3) + 18;
                    data[i++] = sfn_cpaladd(ptr[j+2], ptr[j+1], ptr[j], ptr[7]==32?ptr[j+3]:0xFF);
                }
            }
        break;

        case 2:
            if(ptr[5]!=0 || ptr[6]!=0 || ptr[1]!=0 || (ptr[16]!=24 && ptr[16]!=32)) goto tgaerr;
            for(y=i=0; y<h; y++) {
                j = ((!o?h-y-1:y)*w*(ptr[16]>>3));
                for(x=0; x<w; x++) {
                    data[i++] = sfn_cpaladd(ptr[j+2], ptr[j+1], ptr[j], ptr[16]==32?ptr[j+3]:0xFF);
                    j += ptr[16]>>3;
                }
            }
        break;

        case 9:
            if(ptr[6]!=0 || ptr[4]!=0 || ptr[3]!=0 || (ptr[7]!=24 && ptr[7]!=32)) goto tgaerr;
            y = i = 0;
            for(x=0; x<w*h && m<size;) {
                k = ptr[m++];
                if(k > 127) {
                    k -= 127; x += k;
                    j = ptr[m++]*(ptr[7]>>3) + 18;
                    while(k--) {
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[i++] = sfn_cpaladd(ptr[j+2], ptr[j+1], ptr[j], ptr[7]==32?ptr[j+3]:0xFF);
                    }
                } else {
                    k++; x += k;
                    while(k--) {
                        j = ptr[m++]*(ptr[7]>>3) + 18;
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[i++] = sfn_cpaladd(ptr[j+2], ptr[j+1], ptr[j], ptr[7]==32?ptr[j+3]:0xFF);
                    }
                }
            }
        break;

        case 10:
            if(ptr[5]!=0 || ptr[6]!=0 || ptr[1]!=0 || (ptr[16]!=24 && ptr[16]!=32)) goto tgaerr;
            y = i = 0;
            for(x=0; x<w*h && m<size;) {
                k = ptr[m++];
                if(k > 127) {
                    k -= 127; x += k;
                    while(k--) {
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[i++] = sfn_cpaladd(ptr[m+2], ptr[m+1], ptr[m], ptr[16]==32?ptr[m+3]:0xFF);
                    }
                    m += ptr[16]>>3;
                } else {
                    k++; x += k;
                    while(k--) {
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[i++] = sfn_cpaladd(ptr[m+2], ptr[m+1], ptr[m], ptr[16]==32?ptr[m+3]:0xFF);
                        m += ptr[16]>>3;
                    }
                }
            }
        break;
    }
    pixmap_parse(data, w, h);
    free(data);
}

