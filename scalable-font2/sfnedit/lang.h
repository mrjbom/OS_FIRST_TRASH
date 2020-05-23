/*
 * sfnedit/lang.h
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
 * @brief Multilanguage definitions
 *
 */

enum {
    LOADING = 0,
    ERR_MEM,
    ERR_LOAD,
    ERR_BADFILE,
    ERR_COLLECTION,
    ERR_SAVE,
    ERR_UNSAVED,
    ERR_COLOR,
    ERR_DISPLAY,
    HELP,
    MMENU_PROPS,
    MMENU_RANGES,
    MMENU_GLYPHS,
    GMENU_EDIT,
    GMENU_KERN,
    PROP_FAMILY,
    PROP_STYLE,
    PROP_GRID,
    PROP_BASELINE,
    PROP_ULINE,
    PROP_NAME,
    PROP_FAMNAME,
    PROP_SUBFAMNAME,
    PROP_REVISION,
    PROP_MANUFACT,
    PROP_LICENSE,
    PROP_NEWCHAR,
    PROP_SAVESFN,
    PROP_SAVEASC,
    PROP_NOQUIT,
    FAM_SERIF,
    FAM_SANS,
    FAM_DECOR,
    FAM_MONO,
    FAM_HAND,
    STYL_BOLD,
    STYL_ITALIC,
    RECALIBRATE,
    EMPTYTOO,
    COVERAGE,
    RANGENAME,
    WHOLERANGE,
    NOGLYPH,
    UNDEFINED,
    LOCAL0,
    LOCAL1,
    LOCAL2,
    LOCAL3,
    LOCAL4,
    LOCAL5,
    LOCAL6,
    AREYOUSURE,
    YES,
    NO,

    /* order matters */
    HELP_MAINWIN,
    HELP_GLYPHWIN,
    HELP_PROPS,
    HELP_RANGES,
    HELP_GLYPHS,
    HELP_EDIT,
    HELP_KERN,
    HELP_ADV,
    HELP_HINT,
    HELP_MODIFY,
    /* must be the last */
    NUMTEXTS
};

#define NUMLANGS         2

extern char *dict[NUMLANGS][NUMTEXTS + 1], **lang;
void lang_init();

