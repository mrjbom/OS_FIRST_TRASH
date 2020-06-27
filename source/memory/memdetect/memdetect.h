#ifndef _MEMDETECT_H_
#define _MEMDETECT_H_

#include "../../lib/cstdlib.h"
#include "../../lib/stddef.h"
#include "../../lib/stdint.h"
#include "../../lib/string.h"

#include "../../more/more.h"

extern multiboot_memory_map_t* memory_map;

//in bytes
extern uint64_t ram_len;
extern uint64_t ram_available;

extern uint32_t memory_map_addr;
extern uint32_t memory_map_len;

//array containing addresses of the beginning of RAM sections
extern uint32_t memory_sections_addreses[6];
//array containing the length of a sections of RAM memory
extern uint32_t memory_sections_lengths[6];
//array containing the type of RAM memory sections
extern uint32_t memory_sections_types[6];

extern char startkernel[];
extern char endkernel[];

extern void memory_map_init();

extern void memory_map_calculate();

#endif