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

//symbol print
extern void scale_symbol(uint32_t n_lines_map, uint32_t n_columns_map, bool map[][n_columns_map], uint32_t x_pos, uint32_t y_pos, uint32_t text_color, uint32_t back_color, uint32_t scale);
extern void print_symbol_lfb_mem(uint32_t x, uint32_t y, char symbol, uint32_t text_color, uint32_t back_color, uint32_t scale);

#endif
