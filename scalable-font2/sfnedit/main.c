/*
 * sfnedit/main.c
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
 * @brief Scalable Screen Font Editor main function
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "lang.h"
#include "ui.h"

char *loc = NULL;
char **lang = NULL;

/*
 * wrapper for Windows SDL support. This partialy came from SDL_windows_main.c
 */
#ifdef __WIN32__
#include <windows.h>

static void UnEscapeQuotes(char *arg)
{
    char *last = NULL;

    while (*arg) {
        if (*arg == '"' && (last != NULL && *last == '\\')) {
            char *c_curr = arg;
            char *c_last = last;

            while (*c_curr) {
                *c_last = *c_curr;
                c_last = c_curr;
                c_curr++;
            }
            *c_last = '\0';
        }
        last = arg;
        arg++;
    }
}

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
    char *bufp;
    char *lastp = NULL;
    int argc, last_argc;

    argc = last_argc = 0;
    for (bufp = cmdline; *bufp;) {
        /* Skip leading whitespace */
        while (SDL_isspace(*bufp)) {
            ++bufp;
        }
        /* Skip over argument */
        if (*bufp == '"') {
            ++bufp;
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            lastp = bufp;
            while (*bufp && (*bufp != '"' || *lastp == '\\')) {
                lastp = bufp;
                ++bufp;
            }
        } else {
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && !SDL_isspace(*bufp)) {
                ++bufp;
            }
        }
        if (*bufp) {
            if (argv) {
                *bufp = '\0';
            }
            ++bufp;
        }

        /* Strip out \ from \" sequences */
        if (argv && last_argc != argc) {
            UnEscapeQuotes(argv[last_argc]);
        }
        last_argc = argc;
    }
    if (argv) {
        argv[argc] = NULL;
    }
    return (argc);
}

int APIENTRY WinMain(__attribute__((unused)) HINSTANCE hInstance, __attribute__((unused)) HINSTANCE hPrevInstance,
    __attribute__((unused)) LPSTR lpCmdLine, __attribute__((unused)) int nCmdShow)
{
    OPENFILENAME  ofn;
    char *cmdline = GetCommandLine();
    int ret, argc = ParseCommandLineA(cmdline, NULL);
    char **argv = (char**)calloc(argc+2, sizeof(char*));
    char fn[1024];
    int lid = 0;

    ParseCommandLineA(cmdline, argv);
    lid = GetUserDefaultLangID(); /* GetUserDefaultUILanguage(); */
    /* see https://docs.microsoft.com/en-us/windows/win32/intl/language-identifier-constants-and-strings */
    switch(lid & 0xFF) {
        case 0x01: loc = "ar"; break;   case 0x02: loc = "bg"; break;
        case 0x03: loc = "ca"; break;   case 0x04: loc = "zh"; break;
        case 0x05: loc = "cs"; break;   case 0x06: loc = "da"; break;
        case 0x07: loc = "de"; break;   case 0x08: loc = "el"; break;
        case 0x0A: loc = "es"; break;   case 0x0B: loc = "fi"; break;
        case 0x0C: loc = "fr"; break;   case 0x0D: loc = "he"; break;
        case 0x0E: loc = "hu"; break;   case 0x0F: loc = "is"; break;
        case 0x10: loc = "it"; break;   case 0x11: loc = "jp"; break;
        case 0x12: loc = "ko"; break;   case 0x13: loc = "nl"; break;
        case 0x14: loc = "no"; break;   case 0x15: loc = "pl"; break;
        case 0x16: loc = "pt"; break;   case 0x17: loc = "rm"; break;
        case 0x18: loc = "ro"; break;   case 0x19: loc = "ru"; break;
        case 0x1A: loc = "hr"; break;   case 0x1B: loc = "sk"; break;
        case 0x1C: loc = "sq"; break;   case 0x1D: loc = "sv"; break;
        case 0x1E: loc = "th"; break;   case 0x1F: loc = "tr"; break;
        case 0x20: loc = "ur"; break;   case 0x21: loc = "id"; break;
        case 0x22: loc = "uk"; break;   case 0x23: loc = "be"; break;
        case 0x24: loc = "sl"; break;   case 0x25: loc = "et"; break;
        case 0x26: loc = "lv"; break;   case 0x27: loc = "lt"; break;
        case 0x29: loc = "fa"; break;   case 0x2A: loc = "vi"; break;
        case 0x2B: loc = "hy"; break;   case 0x2D: loc = "bq"; break;
        case 0x2F: loc = "mk"; break;   case 0x36: loc = "af"; break;
        case 0x37: loc = "ka"; break;   case 0x38: loc = "fo"; break;
        case 0x39: loc = "hi"; break;   case 0x3A: loc = "mt"; break;
        case 0x3C: loc = "gd"; break;   case 0x3E: loc = "ms"; break;
        case 0x3F: loc = "kk"; break;   case 0x40: loc = "ky"; break;
        case 0x45: loc = "bn"; break;   case 0x47: loc = "gu"; break;
        case 0x4D: loc = "as"; break;   case 0x4E: loc = "mr"; break;
        case 0x4F: loc = "sa"; break;   case 0x53: loc = "kh"; break;
        case 0x54: loc = "lo"; break;   case 0x56: loc = "gl"; break;
        case 0x5E: loc = "am"; break;   case 0x62: loc = "fy"; break;
        case 0x68: loc = "ha"; break;   case 0x6D: loc = "ba"; break;
        case 0x6E: loc = "lb"; break;   case 0x6F: loc = "kl"; break;
        case 0x7E: loc = "br"; break;   case 0x92: loc = "ku"; break;
        case 0x09: default: loc = "en"; break;
    }
    SDL_SetMainReady();
    ret = main(argc, argv);
    free(argv);
    exit(ret);
    return ret;
}
#endif

/**
 * Main sfnedit function
 */
int main(int argc, char **argv)
{
    int i;

    if(argc > 2 && argv[1] && argv[1][0] == '-' && argv[1][1] == 'l') {
        loc = argv[2];
        argv += 2;
        argc -= 2;
    }
    if(!loc) loc = getenv("LANG");
    if(!loc) loc = "en";
    for(i = 0; i < NUMLANGS; i++)
        if(!strncmp(loc, dict[i][0], strlen(dict[i][0]))) break;
    if(i >= NUMLANGS) { i = 0; loc = "en"; }

    lang = &dict[i][1];
#ifndef __WIN32__
    signal(SIGQUIT, ui_quit);
    signal(SIGINT, ui_quit);
#endif

    ui_main(argc > 1 && argv[1] ? argv[1] : NULL);
    ui_quit(0);
    return 0;
}
