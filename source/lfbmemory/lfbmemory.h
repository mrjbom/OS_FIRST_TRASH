#ifndef _VGAMEMORY_H_
#define _VGAMEMORY_H_

#include "../kernel/kernel.h"

extern uint32_t* LFBFRAMEBUFFER_ADDR;

extern void initVGAMemory();

extern void clearVGAMemory(uint32_t color);

extern void writePixelToLFB(uint32_t line, uint32_t column, uint32_t color);

extern void writeSymbolToLFB(uint32_t line, uint32_t column, char symbol);

#endif
