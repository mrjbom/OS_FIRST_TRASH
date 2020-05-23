#ifndef _TIMER_H_
#define _TIMER_H_

#include "../../lib/cstdlib.h"
#include "../../lib/stddef.h"
#include "../../lib/stdint.h"
#include "../../lib/string.h"

void init_timer(uint16_t freq);

void timer_handler();

#endif
