#include "pit.h"
#include "../../inlineassembly/inlineassembly.h"
#include "../../lfbmemory/lfbmemory.h"
#include "../../scheduler/scheduler.h"

uint64_t pit_tick_counter = 0;
//using for timer
uint64_t freq_copy = 0;

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
    //serial_printf("pit_handler()\n");
    if(!(pit_tick_counter % 10)) {
        scheduler();
    }
}

void pit_sleep(uint32_t milliseconds) {
    uint64_t local_tick_start = pit_tick_counter;
    //The frequency of interruption in milliseconds
    //float work faster
    float tick_in_milliseconds = (float)1000 / freq_copy;
    //Number of ticks to wait for
    uint64_t ticks_to_wait = (float)milliseconds / tick_in_milliseconds;
    uint64_t local_tick_finish = local_tick_start + ticks_to_wait;
    //serial_printf("ticks_to_wait %u\n", ticks_to_wait);
    while (pit_tick_counter < local_tick_finish);
}
