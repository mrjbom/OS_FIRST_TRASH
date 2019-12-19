#ifndef _TEXTMODEMEMORY_H_
#define _TEXTMODEMEMORY_H_

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"

#define BASE_TEXT_MODE_ADDRESS 0xB8000

enum base_textmode_colors
{
    TEXT_MODE_BLACK = 0x0,
    TEXT_MODE_BLUE = 0x1,
    TEXT_MODE_GREEN = 0x2,
    TEXT_MODE_CYAN = 0x3,
    TEXT_MODE_RED = 0x04,
    TEXT_MODE_MAGENTA = 0x05,
    TEXT_MODE_BROWN = 0x06,
    TEXT_MODE_LIGHT_GRAY = 0x07,
    TEXT_MODE_DARK_GRAY = 0x08,
    TEXT_MODE_LIGHT_BLUE = 0x09,
    TEXT_MODE_LIGHT_GREEN = 0x0A,
    TEXT_MODE_LIGHT_CYAN = 0x0B,
    TEXT_MODE_LIGHT_RED = 0x0C,
    TEXT_MODE_LIGHT_MAGENTA = 0x0D,
    TEXT_MODE_LIGNT_BROWN = 0x0E,
    TEXT_MODE_WHITE = 0x0F
};

extern void clear_base_text_mem(uint32_t color);

extern void write_word_base_text_mem(short line, short column, const char* word, uint32_t color);

#endif
