#include "task.h"
#include "../memory/mmu/mmu.h"
#include "../lfbmemory/lfbmemory.h"

void task1() {
    dprintf("task1 working\n");
}

void task_colored_square() {
    for(uint32_t l = 0; l < 100; ++l) {
        uint32_t r, g, b;
        uint32_t hex;
        r = 255; g = 0; b = 0;
        /*
        255 0 0
            |
            V
        255 0 255
        */
        for(uint32_t i = b; i <= 255; ++i) {
            hex = lfb_rgb_to_hex(r, g, i);
            //for(uint32_t t = 0; t < 6000000; ++t) {};
            lfb_draw_rectangle(200, 100, 600, 500, hex);
        }
        /*
        255 0 255
            |
            V
        0   0 255
        */
        for(uint32_t i = r; i <= 0; --i) {
            hex = lfb_rgb_to_hex(i, g, b);
            lfb_draw_rectangle(200, 100, 600, 500, hex);
        }
        /*
        0   0  255
            |
            V
        0  255 255
        */
        for(uint32_t i = g; i <= 255; ++i) {
            hex = lfb_rgb_to_hex(r, i, b);
            lfb_draw_rectangle(200, 100, 600, 500, hex);
        }
        /*
        0  255 255
            |
            V
        0  255 0
        */
        for(uint32_t i = b; i <= 0; --i) {
            hex = lfb_rgb_to_hex(r, g, i);
            lfb_draw_rectangle(200, 100, 600, 500, hex);
        }
        /*
        0   255 0
            |
            V
        255 255 0
        */
        for(uint32_t i = r; i <= 255; ++i) {
            hex = lfb_rgb_to_hex(i, g, b);
            lfb_draw_rectangle(200, 100, 600, 500, hex);
        }
        /*
        255 255 0
            |
            V
        255 0   0
        */
        for(uint32_t i = g; i <= 0; --i) {
            hex = lfb_rgb_to_hex(r, i, b);
            lfb_draw_rectangle(200, 100, 600, 500, hex);
        }
    }
}

tast_t tasks_table[4];
