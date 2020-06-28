#ifndef _TIMER_H_
#define _TIMER_H_

#include "../../lib/cstdlib.h"
#include "../../lib/stddef.h"
#include "../../lib/stdint.h"
#include "../../lib/string.h"

void pit_init(uint16_t freq);

void pit_handler();

void pit_sleep(uint32_t milliseconds);

#endif
