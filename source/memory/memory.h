#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"

#include "../more/more.h"

extern multiboot_memory_map_t* memory_map;

//in bytes
extern uint64_t ram_len;
extern uint64_t ram_available;

extern uint32_t memory_map_addr;
extern uint32_t memory_map_len;

extern void init_memory();

extern void calclulate_memory();

#endif
