#ifndef _TEXTMODEMEMORY_H_
#define _TEXTMODEMEMORY_H_

#include "../kernel/kernel.h"

extern void clearBaseTextBuffer(uint32_t color);

extern void writeToBaseTextBuffer(short line, short column, const char* word, uint32_t color);

#endif
