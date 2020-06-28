#include "pit.h"
#include "../../inlineassembly/inlineassembly.h"
#include "../../lfbmemory/lfbmemory.h"

uint32_t pit_tick_counter = 0;
//using for timer
uint32_t freq_copy = 0;

void pit_init(uint16_t freq) {
    //freq: >= 19 && <= 65535
    //Get the PIT value: hardware clock at 1193180 Hz
    uint16_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    //Send the command
    outb(0x43, 0x36); //Command port
    outb(0x40, low);
    outb(0x40, high);
    freq_copy = freq;
    pit_tick_counter = 0;
}

void pit_handler() {
    ++pit_tick_counter;
    if(!(pit_tick_counter % 100)) { //1 second
        
    }
}

void pit_sleep(uint32_t milliseconds) {
    uint32_t local_tick_start = pit_tick_counter;
    //The frequency of interruption in milliseconds
    //float work faster
    float tick_in_milliseconds = (double)1000 / freq_copy;
    //Number of ticks to wait for
    uint32_t ticks_to_wait = (float)milliseconds / tick_in_milliseconds;
    uint32_t local_tick_finish = local_tick_start + ticks_to_wait;
    serial_printf("ticks_to_wait %I\n", ticks_to_wait);
    while (pit_tick_counter < local_tick_finish);
}
