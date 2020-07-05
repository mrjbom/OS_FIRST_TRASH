#ifndef _TIMER_H_
#define _TIMER_H_

#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../lib/string.h"

extern uint64_t freq_copy;

void pit_init(uint16_t freq);

void pit_handler();

void pit_sleep(uint32_t milliseconds);

#endif
