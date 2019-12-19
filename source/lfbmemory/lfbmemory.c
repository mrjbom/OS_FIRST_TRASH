#include "../lfbmemory/lfbmemory.h"
#include "../more/more.h"

uint32_t* LFBMEMORY_ADDR = NULL;

void init_lfb_mem() {
    LFBMEMORY_ADDR = (uint32_t*)((uint32_t)MBI->framebuffer_addr);
}

void clear_lfb_mem(uint32_t color) {
    for (uint32_t pixelPos = 0; pixelPos < MBI->framebuffer_width * MBI->framebuffer_height; pixelPos++)
        LFBMEMORY_ADDR[pixelPos] = color;
}

void write_pixel_lfb_mem(uint32_t line, uint32_t column, uint32_t color) {
    uint32_t pixelPos = line * MBI->framebuffer_width + column;
    LFBMEMORY_ADDR[pixelPos] = color;
}

void print_symbol_lfb_mem(uint32_t line, uint32_t column, char symbol) {
    
}
