/*
 * libsfn/vector.c
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
 * @brief File functions for vector fonts using FreeType2
 *
 */

#include <stdlib.h>
#include <string.h>

#ifdef HAS_FT

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_IMAGE_H
#include FT_BBOX_H
#include FT_SFNT_NAMES_H
#include "libsfn.h"
#include "vector.h"

#define FT_SIZE 1024

FT_Library ft = NULL;
FT_Face face;
int min_y, max_y, min_x, max_x, max_b, max_s, max_w, max_h, unicode;
sfnlayer_t *currlayer;
FT_BBox *bbox;

typedef struct {
    int unicode;
    FT_BBox bbox;
} ftchr_t;
ftchr_t *ftchars;

/**
 * Transform and scale coordinates
 */
int sx(int i)
{
    return (i - bbox->xMin) * 254 / max_s;
}

int sy(int i)
{
    return (max_b - i) * 254 / max_s;
}

/*** contour command callbacks for FT2 ***/

int move(const FT_Vector *to, void *ptr)
{
    (void)ptr;
    currlayer = sfn_layeradd(unicode, SSFN_FRAG_CONTOUR, 0, 0, 0, 0, 0xFE, NULL);
    sfn_contadd(currlayer, SSFN_CONTOUR_MOVE, sx(to->x), sy(to->y), 0,0, 0,0);
    return 0;
}

int line(const FT_Vector *to, void *ptr)
{
    (void)ptr;
    sfn_contadd(currlayer, SSFN_CONTOUR_LINE, sx(to->x), sy(to->y), 0,0, 0,0);
    return 0;
}

int quad(const FT_Vector *control, const FT_Vector *to, void *ptr)
{
    (void)ptr;
    sfn_contadd(currlayer, SSFN_CONTOUR_QUAD, sx(to->x), sy(to->y), sx(control->x), sy(control->y), 0,0);
    return 0;
}

int cubic(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *ptr)
{
    (void)ptr;
    sfn_contadd(currlayer, SSFN_CONTOUR_CUBIC, sx(to->x), sy(to->y), sx(control1->x), sy(control1->y),
        sx(control2->x), sy(control2->y));
    return 0;
}

FT_Outline_Funcs funcs = { move, line, quad, cubic, 0, 0 };

/**
 * Parse a FT2 vector font
 */
void ttf()
{
    FT_UInt agi;
    FT_ULong cp;
    FT_Vector v;
    FT_Error error;
    int i, j, n, x, y, numchars = 0;
    long int avg_w = 0, avg_h = 0;
    ftchr_t *c;

    /* unbeliveable, FT2 lib does not return the number of characters in the font... */
    for(cp = FT_Get_First_Char(face, &agi); agi; cp = FT_Get_Next_Char(face, cp, &agi)) numchars++;

    c = ftchars = (ftchr_t*)malloc((numchars + 1) * sizeof(ftchr_t));
    if(!ftchars) { fprintf(stderr,"memory allocation error\n"); return; }
    memset(ftchars, 0, (numchars + 1) * sizeof(ftchr_t));

    /* ...and it returns incorrect bounding box. Never trust FT2's scaling. */
    min_x = min_y = face->units_per_EM*2; max_x = max_y = max_b = max_w = max_h = -face->units_per_EM*2;
    for(cp = FT_Get_First_Char(face, &agi), i = 0; i <= numchars && agi; i++) {
        if(pbar) (*pbar)(1, 3, i, numchars, PBAR_MEASURE);
        for(j = 0; j < ctx.numskip && ctx.skip[j] != (int)cp; j++);
        if(j >= ctx.numskip) {
            c->unicode = (int)cp;
            c->bbox.xMin = c->bbox.xMax = c->bbox.yMin = c->bbox.yMax = 0;
            if(FT_Load_Glyph(face, FT_Get_Char_Index(face, cp), FT_LOAD_NO_SCALE) ||
                (i && !face->glyph->glyph_index)) continue;
            if(!origwh) {
                FT_Outline_Get_BBox(&face->glyph->outline, &c->bbox);
            } else {
                if(!face->glyph->metrics.vertBearingX && !face->glyph->metrics.vertBearingY) {
                    c->bbox.xMin = face->glyph->metrics.horiBearingX;
                    c->bbox.xMax = face->glyph->metrics.horiBearingX + face->glyph->metrics.width;
                    c->bbox.yMin = face->glyph->metrics.horiBearingY - face->glyph->metrics.height;
                    c->bbox.yMax = face->glyph->metrics.horiBearingY;
                } else {
                    c->bbox.xMin = face->glyph->metrics.vertBearingX;
                    c->bbox.xMax = face->glyph->metrics.vertBearingX + face->glyph->metrics.width;
                    c->bbox.yMin = face->glyph->metrics.vertBearingY - face->glyph->metrics.height;
                    c->bbox.yMax = face->glyph->metrics.vertBearingY;
                }
            }
            if(face->glyph->metrics.width > max_w) max_w = face->glyph->metrics.width;
            if(face->glyph->metrics.height > max_h) max_h = face->glyph->metrics.height;
            if(c->bbox.yMax > max_b) max_b = c->bbox.yMax;
            if(c->bbox.xMin < min_x) min_x = c->bbox.xMin;
            if(c->bbox.xMax > max_x) max_x = c->bbox.xMax;
            if(c->bbox.yMin < min_y) min_y = c->bbox.yMin;
            if(c->bbox.yMax > max_y) max_y = c->bbox.yMax;
            avg_w += face->glyph->metrics.width;
            avg_h += face->glyph->metrics.height;
/*
if(cp==0x21) {
    printf("\n%ld UUUUU Bounding box (%ld, %ld), (%ld, %ld)\n", cp,c->bbox.xMin, c->bbox.yMin, c->bbox.xMax, c->bbox.yMax);
    printf("metrics w %ld h %ld horiAdv %ld vertAdv %ld hbear %ld %ld vbear %ld %ld\n", face->glyph->metrics.width, face->glyph->metrics.height,
        face->glyph->metrics.horiAdvance, face->glyph->metrics.vertAdvance, face->glyph->metrics.horiBearingX, face->glyph->metrics.horiBearingY,
        face->glyph->metrics.vertBearingX, face->glyph->metrics.vertBearingY);
}
*/
            if(!quiet && (c->bbox.xMin != face->glyph->metrics.horiBearingX ||
                c->bbox.yMin != face->glyph->metrics.horiBearingY - face->glyph->metrics.height ||
                c->bbox.xMax != c->bbox.xMin + face->glyph->metrics.width ||
                c->bbox.yMax != c->bbox.yMin + face->glyph->metrics.height)) {
                    fprintf(stderr, "\rInconsistent font: U+%06lX", cp);
                    if(c->bbox.xMin != face->glyph->metrics.horiBearingX)
                        fprintf(stderr, ", xMin %ld != hBX %ld", c->bbox.xMin, face->glyph->metrics.horiBearingX);
                    if(c->bbox.yMin != face->glyph->metrics.horiBearingY - face->glyph->metrics.height)
                        fprintf(stderr, ", yMin %ld != hBY-h %ld", c->bbox.yMin,
                            face->glyph->metrics.horiBearingY - face->glyph->metrics.height);
                    if(c->bbox.xMax != c->bbox.xMin + face->glyph->metrics.width)
                        fprintf(stderr, ", xMax %ld != xMin+w %ld", c->bbox.xMax,
                            c->bbox.xMin + face->glyph->metrics.width);
                    if(c->bbox.yMax != c->bbox.yMin + face->glyph->metrics.height)
                        fprintf(stderr, ", yMax %ld != yMin+h %ld", c->bbox.yMax,
                            c->bbox.yMin + face->glyph->metrics.height);
                    fprintf(stderr, "    \n");
            }
            c++;
        }
        cp = FT_Get_Next_Char(face, cp, &agi);
    }
    avg_w /= (long int)numchars;
    avg_h /= (long int)numchars;
    /* due to optionally skipped characters */
    numchars = (int)((unsigned long int)c - (unsigned long int)ftchars) / sizeof(ftchr_t);
    if(min_x >= max_x || min_y >= max_y) min_x = max_x = min_y = max_y = max_w = max_h = max_b = 0;

    if(face->ascender != max_b) {
        fprintf(stderr, "\rInconsistent font: ascender %d != max(yMax) %d    \n", face->ascender, max_b);
    }
    printf("\r  Numchars: %d, Bounding box: (%d, %d), (%d, %d) dx %d dy %d, w: %d, h: %d, baseline: %d\n", numchars,
        min_x, min_y, max_x, max_y, max_x - min_x, max_y - min_y, max_w, max_h, max_b);
/*
    if(!origwh) {
        if(max_x - min_x > max_w) max_w = max_x - min_x;
        if(max_y - min_y > max_h) max_h = max_y - min_y;
    }
*/
    max_s = max_w > max_h ? max_w : max_h;
    if(max_s > 0) {
        printf("  Scaling to %d x %d, average: %ld x %ld\n", max_s, max_s, avg_w, avg_h);

        /* get font style */
        if(face->style_flags & FT_STYLE_FLAG_BOLD) ctx.style |= SSFN_STYLE_BOLD;
        if(face->style_flags & FT_STYLE_FLAG_ITALIC) ctx.style |= SSFN_STYLE_ITALIC;

        /* baseline and underline */
        ctx.baseline = max_b * 254 / max_s;
        ctx.underline = (max_b - face->underline_position) * 254 / max_s;

        /* iterate on characters and contour outlines */
        lastuni = -1;
        for(i = 0; i < numchars; i++) {
            unicode = ftchars[i].unicode;
            if(pbar) (*pbar)(2, 3, i, numchars, PBAR_OUTLINE);
            if(FT_Load_Glyph(face, FT_Get_Char_Index(face, unicode), FT_LOAD_NO_SCALE) ||
                (i && !face->glyph->glyph_index)) continue;
            bbox = &ftchars[i].bbox;
            if(face->glyph->metrics.horiAdvance /*!face->glyph->metrics.vertBearingX && !face->glyph->metrics.vertBearingY*/) {
                x = face->glyph->metrics.horiAdvance - bbox->xMin; y =0;
                if(x < 0) x = 0;
            } else {
                x = 0; y = face->glyph->metrics.vertAdvance - bbox->yMin;
                if(y < 0) y = 0;
            }
            if(face->glyph->metrics.width >= 2*avg_w+avg_w/2 || face->glyph->metrics.height >= 2*avg_h+avg_h/2) {
                fprintf(stderr, "\rIrregular dimensions in font: U+%06X", unicode);
                if(face->glyph->metrics.width >= 2*avg_w+avg_w/2) {
                    fprintf(stderr, ", width: %ld ", face->glyph->metrics.width);
                    if(face->glyph->metrics.width >= 3*avg_w)
                        fprintf(stderr, "(%ld times the average, can't be right!!!)", face->glyph->metrics.width / avg_w);
                }
                if(face->glyph->metrics.height >= 2*avg_h+avg_h/2) {
                    fprintf(stderr, ", height: %ld ", face->glyph->metrics.height);
                    if(face->glyph->metrics.height >= 3*avg_h)
                        fprintf(stderr, "(%ld times the average, can't be right!!!)", face->glyph->metrics.height / avg_h);
                }
                fprintf(stderr, "\n");
            }
            /* don't trust FT2's width and height, we'll recalculate them from the coordinates */
            x = x * 255 / max_s; if(face->glyph->metrics.horiAdvance) x += 4;
            y = y * 255 / max_s; if(!face->glyph->metrics.horiAdvance) y += 4;
            if(sfn_charadd(unicode, 0, 0, x, y, bbox->xMin < 0 ? -bbox->xMin * 255 / max_s : 0)) {
                error = FT_Outline_Decompose(&face->glyph->outline, &funcs, NULL);
                if(error && !quiet) { fprintf(stderr, "Freetype2 decompose error %x\n", error); }
            }
        }

        /* get kerning information */
        if(face->face_flags & FT_FACE_FLAG_KERNING) {
            for(i = n = 0; i < numchars; i++) {
                if(pbar) (*pbar)(3, 3, i, numchars, PBAR_GETKERN);
                for(j = 0; j < numchars; j++) {
                    if(ftchars[i].unicode<33 || ftchars[j].unicode<33) continue;
                    v.x = v.y = 0;
                    FT_Get_Kerning(face, ftchars[i].unicode, ftchars[j].unicode, FT_KERNING_UNSCALED, &v);
                    if(v.x) { v.x = v.x * 254 / max_s; v.y = 0; }
                    else { v.x = 0; v.y = v.y * 254 / max_s; }
                    if(v.x >= -1 && v.x <= 1 && v.y >= -1 && v.x <= 1) continue;
                    sfn_kernadd(ftchars[i].unicode, ftchars[j].unicode, v.x, v.y);
                    n++;
                }
            }
            printf("\r\x1b[K  Kerning %d pairs\n", n);
        } else
            printf("\r\x1b[K  No kerning information\n");
    }
    free(ftchars);
}

/**
 * Parse PCF (or any other bitmap font that FT2 supports)
 */
void pcf()
{
    int i, j, k, l, n, a = 0, w = 8, h = 16, nc = 0, numchars = 0;
    unsigned char *glyph, *bitmap;
    FT_UInt agi;
    FT_ULong cp;
/*    FT_PropertyRec aprop; */

    /* unbeliveable, FT2 lib does not return the number of characters in the font... */
    for(cp = FT_Get_First_Char(face, &agi); agi; cp = FT_Get_Next_Char(face, cp, &agi)) numchars++;

    for(i = 0; i < face->num_fixed_sizes; i++)
        if(face->available_sizes[i].height > h) {
            h = face->available_sizes[i].height;
            w = face->available_sizes[i].width;
        }
    printf("\r  Numchars: %d, num_fixed_sizes: %d, selected: %d x %d\n", numchars, face->num_fixed_sizes, w, h);
    FT_Set_Pixel_Sizes(face, w, h);
    n = 2*h*h;
    bitmap = (unsigned char*)malloc(n);
    if(!bitmap) { fprintf(stderr,"memory allocation error\n"); return; }

    cp = FT_Get_First_Char(face, &agi);
    while(agi) {
        if(pbar) (*pbar)(0, 1, ++nc, numchars, PBAR_BITMAP);
        for(j = 0; j < ctx.numskip && ctx.skip[j] != (int)cp; j++);
        if(j < ctx.numskip) continue;
        if(cp < (FT_ULong)rs || cp > (FT_ULong)re || FT_Load_Glyph(face, FT_Get_Char_Index(face, cp), FT_LOAD_NO_SCALE) ||
            (cp && !face->glyph->glyph_index)) continue;
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
        if(face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_MONO) continue;
        memset(bitmap, 0, n);
        glyph = face->glyph->bitmap.buffer;
        w = ((int)(face->glyph->bitmap.width+7) & ~7);
        for(i = k = 0; i < (int)face->glyph->bitmap.rows; i++) {
            for(j = 0; j < (int)(face->glyph->bitmap.width+7)/8; j++)
                for(l=0x80;l && k < n;l>>=1)
                    bitmap[k++] = (glyph[j] & l) ? 0xFE : 0xFF;
            glyph += face->glyph->bitmap.pitch;
        }
        a += face->glyph->bitmap_top;
        if(sfn_charadd(cp, w, face->glyph->bitmap.rows, face->glyph->advance.x >> 6, face->glyph->advance.y >> 6,
            face->glyph->bitmap_left))
                sfn_layeradd(cp, SSFN_FRAG_BITMAP, 0, 0, w, face->glyph->bitmap.rows, 0xFE, bitmap);
        cp = FT_Get_Next_Char(face, cp, &agi);
    }

    /* I can see that PCF has manufacturer, family, slant etc. strings too, but I have no clue how to get them with FT2 */
/*    if(!FT_Get_BDF_Property(face, "", &aprop)) { } */

    ctx.baseline = a/numchars;
    ctx.underline = ctx.baseline + (h - ctx.baseline) / 2;

    free(bitmap);
}

/**
 * Check if FT2 can read the font
 */
int ft2_read(char *filename)
{
    if(!ft && FT_Init_FreeType(&ft)) { fprintf(stderr, "Unable to initialize FreeType2\n"); return 0; }
    face = NULL;
    return (!FT_New_Face(ft, filename, 0, &face) && face) ? 1 : 0;
}

/**
 * Parse a FT2 font
 */
void ft2_parse()
{
    FT_SfntName name;
    if(!ft || !face) return;

    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    printf("\r  Name '%s' num_glyphs: %ld, units_per_EM: %d, ascender: %d, underline: %d, %s\n",
        face->family_name, face->num_glyphs, face->units_per_EM, face->ascender, face->underline_position,
        face->face_flags & FT_FACE_FLAG_SCALABLE ? "vector" : "bitmap");

    if(face->face_flags & FT_FACE_FLAG_SCALABLE) ttf();
    else pcf();

    /* unique font name */
    if(!ctx.name) {
        if(!FT_Get_Sfnt_Name(face, 3, &name)) sfn_setstr(&ctx.name, (char*)name.string, name.string_len); else
        if(!FT_Get_Sfnt_Name(face, 20, &name)) sfn_setstr(&ctx.name, (char*)name.string, name.string_len);
    }
    /* fallback */
    if(!ctx.name) sfn_setstr(&ctx.name, face->family_name, 0);

    /* family name */
    if(!ctx.familyname && !FT_Get_Sfnt_Name(face, 1, &name)) sfn_setstr(&ctx.familyname, (char*)name.string, name.string_len);

    /* subfamily name */
    if(!ctx.subname && !FT_Get_Sfnt_Name(face, 2, &name)) sfn_setstr(&ctx.subname, (char*)name.string, name.string_len);

    /* version / revision */
    if(!ctx.revision && !FT_Get_Sfnt_Name(face, 5, &name)) sfn_setstr(&ctx.revision, (char*)name.string, name.string_len);

    /* manufacturer */
    if(!ctx.manufacturer) {
        if(!FT_Get_Sfnt_Name(face, 8, &name)) sfn_setstr(&ctx.manufacturer, (char*)name.string, name.string_len); else
        if(!FT_Get_Sfnt_Name(face, 9, &name)) sfn_setstr(&ctx.manufacturer, (char*)name.string, name.string_len);
    }

    /* copyright */
    if(!ctx.license) {
        if(!FT_Get_Sfnt_Name(face, 0, &name)) sfn_setstr(&ctx.license, (char*)name.string, name.string_len); else
        if(!FT_Get_Sfnt_Name(face, 7, &name)) sfn_setstr(&ctx.license, (char*)name.string, name.string_len);
    }

    FT_Done_Face(face); face = NULL;
    FT_Done_FreeType(ft); ft = NULL;
}

#endif
