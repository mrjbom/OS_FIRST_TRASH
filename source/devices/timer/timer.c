#include "timer.h"
#include "../../inlineassembly/inlineassembly.h"
#include "../../lfbmemory/lfbmemory.h"

uint32_t tick = 0;

void init_timer(uint16_t freq) {

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    /* Send the command */
    outb(0x43, 0x36); /* Command port */
    outb(0x40, low);
    outb(0x40, high);
    tick = 0;
}

void timer_handler() {
    ++tick;
    if(!(tick % 100)) { //1 second
    }
}
