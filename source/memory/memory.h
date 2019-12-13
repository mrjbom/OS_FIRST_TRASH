#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "../kernel/kernel.h"
#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"

extern multiboot_memory_map_t* memory_map;

//in bytes
extern uint32_t ram_len;
extern uint32_t ram_available;

extern uint32_t memory_map_addr;
extern uint32_t memory_map_len;

extern void calclulate_memory();

#endif
