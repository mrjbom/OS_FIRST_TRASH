/*
 * sfnconv/main.c
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
 * @brief SSFN converter main function
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "libsfn.h"
#include "ssfn.h"
#if HAS_ZLIB
# include <zlib.h>
#endif

int zip = 1, ascii = 0, dump = 0, quiet = 0, lastpercent = 100;

/**
 * Load a (compressed) file
 */
ssfn_font_t *load_file(char *infile, int *size)
{
    ssfn_font_t *data = NULL;
    long int origsize = 0;
    FILE *f;
#if HAS_ZLIB
    unsigned char hdr[2];
    gzFile g;
#endif

    f = fopen(infile,"rb");
    if(!f) { fprintf(stderr,"unable to load '%s'\n", infile); exit(3); }
#if HAS_ZLIB
    fread(&hdr, 2, 1, f);
    if(hdr[0]==0x1f && hdr[1]==0x8b) {
        fseek(f, -4L, SEEK_END);
        fread(&origsize, 4, 1, f);
    } else {
        fseek(f, 0, SEEK_END);
        origsize = ftell(f);
    }
    fclose(f);
    g = gzopen(infile,"r");
#else
    fseek(f, 0, SEEK_END);
    origsize = ftell(f);
    fseek(f, 0, SEEK_SET);
#endif
    data = (ssfn_font_t*)malloc(origsize);
    if(!data) { fprintf(stderr,"memory allocation error\n"); exit(2); }
#if HAS_ZLIB
    gzread(g, data, origsize);
    gzclose(g);
#else
    fread(data, origsize, 1, f);
    fclose(f);
#endif
    *size = origsize;
    return data;
}

/**
 * Save a (compressed) file
 */
void save_file(char *outfile, ssfn_font_t *font)
{
    FILE *f;
#if HAS_ZLIB
    gzFile g;
#endif

#if HAS_ZLIB
    if(zip) {
        g = gzopen(outfile, "wb");
        if(!g) { fprintf(stderr, "unable to write '%s'\n", outfile); exit(4); }
        gzwrite(g, font, font->size);
        gzclose(g);
    } else
#endif
    {
        f = fopen(outfile, "wb");
        if(!f) { fprintf(stderr, "unable to write '%s'\n", outfile); exit(4); }
        fwrite(font, font->size, 1, f);
        fclose(f);
    }
}

/**
 * Usage instructions
 */
void usage()
{
    printf("Scalable Screen Font 2.0 by bzt Copyright (C) 2020 MIT license\n"
           " https://gitlab.com/bztsrc/scalable-font2\n\n"
           "./sfnconv [-c|-e|-d|-dd|-dd...|-D] [-C] "
#if HAS_ZLIB
            "[-U] "
#endif
           "[-A] [-R] [-B <size>|-V] [-g]\n   [-b <p>] [-u <+p>] [-a <+p>] [-o] [-q] [-S <U+xxx>] [-E] [-t [b][i]<0..4>]");
    printf("\n   [-n <name>] [-f <family>] [-s <subfamily>] [-v <ver>] [-m <manufacturer>] "
           "\n   [-l <license>] [-r <from> <to>] <in> [ [-r <from> <to>] <in> ...] <out>\n\n"
           " -c:  create font collection\n"
           " -e:  extract font collection\n"
           " -d:  dump font (-d = header, -dd = string table, -ddd = fragments etc.)\n"
           " -D:  dump all tables in the font\n"
           " -C:  UNICODE range coverage report\n"
#if HAS_ZLIB
           " -U:  save uncompressed, non-gzipped output\n"
#endif
           " -A:  output SSFN ASCII\n"
           " -R:  replace characters from new files\n");
    printf(" -B:  rasterize vector fonts to bitmaps\n"
#if HAS_POTRACE
           " -V:  vectorize bitmap fonts to scalable fonts\n"
#endif
           " -g:  save grid information for hinting\n"
           " -b:  horizontal baseline in pixels (1-255)\n"
           " -u:  underline position in pixels (relative to baseline)\n"
           " -a:  add a constant to advance (1-255, some fonts need it, others don't)\n"
           " -o:  use original width and height instead of calculated one\n"
           " -q:  quiet, don't report font errors\n"
           " -S:  skip a UNICODE code point, this flag can be repeated\n");
    printf(" -E:  don't care about rounding errors\n"
           " -t:  set type b=bold,i=italic,u,U,0=Serif,1/s=Sans,2/d=Decor,3/m=Mono,4/h=Hand\n"
           " -n:  set font unique name\n"
           " -f:  set font family (like FreeSerif, Vera, Helvetica)\n"
           " -s:  set subfamily (like Regular, Medium, Bold, Oblique, Thin, etc.)\n"
           " -v:  set font version / revision (like creation date for example)\n"
           " -m:  set manufacturer (creator, designer, foundry)\n"
           " -l:  set license (like MIT, GPL or URL to the license)\n");
    printf(" -r:  code point range, this flag can be repeated before each input\n"
           " in:  input font(s) SSFN,ASC,"
#if HAS_FT
           "PST1,TTF,OTF,WinFNT,PCF,"
#endif
           "PSF2,BDF,hex,TGA,PNG*\n"
           " out: output SSFN/ASC filename"
#if HAS_ZLIB
           "**"
#endif
           "\n\n*  - input files can be gzip compressed, like .psfu.gz, .bdf.gz or .hex.gz\n"
#if HAS_ZLIB
           "** - output file will be gzip compressed by default (use -U to avoid)\n"
#endif
           "\n"
        );
    exit(1);
}

/**
 * Progressbar hook
 */
void progressbar(int step, int numstep, int curr, int total, int msg)
{
    int i, n;
    char *str[] = { "", "Measuring BBox", "Querying outlines", "Querying all kerning combinations", "Quantizing image",
        "Reading bitmap", "Reading tall pixel map", "Reading wide pixel map", "Generating fragments", "Compressing fragments",
        "Serializing fragments", "Writing character map", "Writing file" };

    n = (long int)(curr + 1) * 100L / (long int)(total + 1);
    if(n == lastpercent) return;
    lastpercent = n;
    printf("\r\x1b[K");
    if(numstep) printf("%d / %d ", step, numstep);
    printf("[");
    for(i = 0; i < 20; i++)
        printf(i < n/5 ? "#" : " ");
    printf("] %3d%% %s  ", n, str[msg]);
    fflush(stdout);
}

/**
 * Main procedure
 */
int main(int argc, char **argv)
{
    int i, j, in = 0;
    char *outfile = NULL, *c;
    int size = 0, total = 8;
    ssfn_font_t *font, *end;
    unsigned char *out = NULL;

    /* parse flags and arguments */
    if(argc<3) usage();
    /* collection management */
    if(argv[1][0] == '-' && argv[1][1] == 'c') {
        /* create collection */
        if(argc<5) usage();
        i = 2;
#if HAS_ZLIB
        for(; i<argc && argv[i][0] == '-'; i++)
            if(argv[i][0] == '-' && argv[i][1] == 'U') zip = 0;
#endif
        for(; i + 1 < argc; i++) {
            font = load_file(argv[i], &size);
            if(memcmp(font->magic, SSFN_MAGIC, 4)) {
                fprintf(stderr, "not an SSFN font '%s'\n", argv[i]);
                return 1;
            }
            out = (unsigned char *)realloc(out, total+size);
            memcpy(out + total, font, font->size);
            total += size;
            size = 0;
            free(font);
        }
        if(out) {
            memcpy(out, SSFN_COLLECTION, 4);
            memcpy(out + 4, &total, 4);
            save_file(argv[i], (ssfn_font_t*)out);
        }
        return 0;
    }
    if(argv[1][0] == '-' && argv[1][1] == 'e') {
        /* extract collection */
        i = 2; j = 0;
#if HAS_ZLIB
        for(; i<argc && argv[i][0] == '-'; i++)
            if(argv[i][0] == '-' && argv[i][1] == 'U') zip = 0;
#endif
        font = load_file(argv[i], &size);
        if(memcmp(font->magic, SSFN_COLLECTION, 4)) {
            fprintf(stderr, "not an SSFN font collection '%s'\n", argv[i]);
            return 1;
        }
        end = (ssfn_font_t*)((uint8_t*)font + font->size);
        for(i++, font = (ssfn_font_t*)((uint8_t*)font + 8); font < end; font = (ssfn_font_t*)((uint8_t*)font + font->size)) {
            if(argc < 3) {
                if(!j) { j = 1; printf("-t\t-n\n"); }
                printf("%s%s%s%s%d\t%s\n", SSFN_TYPE_STYLE(font->type) & SSFN_STYLE_BOLD ? "b":"",
                    SSFN_TYPE_STYLE(font->type) & SSFN_STYLE_ITALIC ? "i":"",
                    SSFN_TYPE_STYLE(font->type) & SSFN_STYLE_USRDEF1 ? "u":"",
                    SSFN_TYPE_STYLE(font->type) & SSFN_STYLE_USRDEF2 ? "U":"",
                    SSFN_TYPE_FAMILY(font->type),
                    (char*)font + sizeof(ssfn_font_t));
            } else
                save_file(argv[i++], font);
        }
        return 0;
    }
    /* convert fonts */
    sfn_init(progressbar);
    for(i=1;argv[i];i++){
        if(argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'n': if(++i>=argc) usage(); sfn_setstr(&ctx.name, argv[i], 0); continue;
                case 'f': if(++i>=argc) usage(); sfn_setstr(&ctx.familyname, argv[i], 0); continue;
                case 's': if(++i>=argc) usage(); sfn_setstr(&ctx.subname, argv[i], 0); continue;
                case 'v': if(++i>=argc) usage(); sfn_setstr(&ctx.revision, argv[i], 0); continue;
                case 'm': if(++i>=argc) usage(); sfn_setstr(&ctx.manufacturer, argv[i], 0); continue;
                case 'l': if(++i>=argc) usage(); sfn_setstr(&ctx.license, argv[i], 0); continue;
                case 'b': if(++i>=argc) usage(); ctx.baseline = atoi(argv[i]); continue;
                case 'a': if(++i>=argc) usage(); adv = atoi(argv[i]); continue;
                case 'u': if(++i>=argc) usage(); relul = atoi(argv[i]); continue;
                case 'B': if(++i>=argc) usage(); rasterize = atoi(argv[i]); continue;
                case 'S': if(++i>=argc) usage(); sfn_skipadd(getnum(argv[i])); continue;
                case 't':
                    if(++i>=argc) usage();
                    for(j = 0, c = argv[i]; *c; c++) {
                        switch(*c) {
                            case 'b': ctx.style |= SSFN_STYLE_BOLD; break;
                            case 'i': ctx.style |= SSFN_STYLE_ITALIC; break;
                            case 'u': ctx.style |= SSFN_STYLE_USRDEF1; break;
                            case 'U': ctx.style |= SSFN_STYLE_USRDEF2; break;
                            case '1': case 's': j = 1; break;
                            case '2': case 'd': j = 2; break;
                            case '3': case 'm': j = 3; break;
                            case '4': case 'h': j = 4; break;
                        }
                    }
                    sfn_setfamilytype(j);
                continue;
                case 'r':
                    if(++i>=argc) usage();
                    if((argv[i][0] >= '0' && argv[i][0] <= '9') || (argv[i][0]=='U' && argv[i][1]=='+') || argv[i][0]=='\'') {
                        if(i+1>=argc) usage();
                        rs = getnum(argv[i++]); re = getnum(argv[i]);
                    } else {
                        for(rs=re=j=0;j<UNICODE_NUMBLOCKS;j++)
                            if(!unicmp(argv[i], ublocks[j].name)) {
                                rs = ublocks[j].start; re = ublocks[j].end;
                                break;
                            }
                        if(!re) {
                            fprintf(stderr, "unable to get range '%s', did you mean:\n", argv[i]);
                            for(j=0;j<UNICODE_NUMBLOCKS;j++)
                                if(tolower(argv[i][0]) == tolower(ublocks[j].name[0]) &&
                                    tolower(argv[i][1]) == tolower(ublocks[j].name[1])) {
                                    fprintf(stderr, " %s\n", ublocks[j].name);
                                    re++;
                                }
                            if(!re)
                                for(j=0;j<UNICODE_NUMBLOCKS;j++)
                                    if(tolower(argv[i][0]) == tolower(ublocks[j].name[0])) {
                                        fprintf(stderr, " %s\n", ublocks[j].name);
                                        re++;
                                    }
                            if(!re)
                                fprintf(stderr, "no matching blocks found\n");
                            return 1;
                        }
                    }
                    if(rs > 0x10FFFF || re > 0x10FFFF || rs > re) {
                        fprintf(stderr, "unable to get range '%s' '%s'\n", argv[i], argv[i]+1);
                        return 1;
                    }
                    continue;
                default:
                    for(j=1;argv[i][j];j++) {
                        switch(argv[i][j]) {
                            case 'g': hinting = 1; break;
                            case 'U': zip = 0; break;
                            case 'V': rasterize = -1; break;
                            case 'R': replace = 1; break;
                            case 'A': ascii = 1; break;
                            case 'o': origwh = 1; break;
                            case 'q': quiet = 1; break;
                            case 'E': dorounderr = 1; break;
                            case 'd': dump++; break;
                            case 'D': dump = 99; break;
                            case 'C': dump = -1; break;
                            default: fprintf(stderr, "unknown flag '%c'\n", argv[i][j]); return 1;
                        }
                    }
                break;
            }
        } else {
            if(dump) sfn_load(argv[i], dump);
            else {
                if(argv[i+1]) {
                    if(!argv[i]) usage();
                    if(sfn_load(argv[i], dump))
                        printf("\r\x1b[K");
                    else {
                        if(!quiet) fprintf(stderr, "sfnconv: unable to open '%s'\n", argv[i]);
                        return 1;
                    }
                    rs = 0; re = 0x10FFFF; in++;
                } else outfile = argv[i];
            }
        }
    }
    /* save output font */
    if(!dump) {
        if(!in || !outfile) usage();
        sfn_sanitize();
        if(rasterize) {
            if(rasterize == -1) {
                sfn_vectorize();
            } else {
                sfn_rasterize(rasterize);
            }
        }
        printf("\r\x1b[KSaving '%s' (%s%s%s)\n", outfile, ascii ? "ASCII" : "bin",
            zip ? ", compress" : "", hinting ? ", hinting" : "");
        i = sfn_save(outfile, ascii, zip);
        if(!i)
            printf("\r\x1b[KError saving!\n\n");
        else {
            printf("\r\x1b[KDone.");
            if(ctx.total > 0 && i > 1)
                printf(" Compressed to %ld.%ld%% (%ld bytes)", (long int)i*100/ctx.total, ((long int)i*10000/ctx.total)%100,
                    i - ctx.total);
            printf("\n\n");
        }
    } else
    if(dump == -1) {
        printf("\r\x1b[K\n");
        sfn_coverage();
    }

    /* free resources */
    sfn_free();
    uniname_free();
    return 0;
}
