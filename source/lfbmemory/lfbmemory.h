#ifndef _VGAMEMORY_H_
#define _VGAMEMORY_H_

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"

extern uint32_t* LFBMEMORY_ADDR;

extern void init_lfb_mem();

extern void clear_lfb_mem(uint32_t color);

extern void write_pixel_lfb_mem(uint32_t x, uint32_t y, uint32_t color);

extern void print_symbol_lfb_mem(uint32_t x, uint32_t y, char symbol);

#endif
