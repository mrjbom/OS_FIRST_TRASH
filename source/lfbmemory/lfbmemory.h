#ifndef _VGAMEMORY_H_
#define _VGAMEMORY_H_

#include "../kernel/kernel.h"
#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"

extern uint32_t* LFBFRAMEBUFFER_ADDR;

extern void init_lfb_mem();

extern void clear_lfb_mem(uint32_t color);

extern void write_pixel_lfb_mem(uint32_t line, uint32_t column, uint32_t color);

extern void print_symbol_lfb_mem(uint32_t line, uint32_t column, char symbol);

#endif
