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

//page frame allocator

#define MMAP_GET_NUM 0
#define MMAP_GET_ADDR 1
#define PAGE_SIZE 4096

//Contain the addresses of the multiboot_info_t structure in order not to touch them
extern uint32_t multiboot_reserved_start;
extern uint32_t multiboot_reserved_end;
//number of the next free frame
extern uint32_t next_free_frame;

extern bool memory_page_allocator_init();

/*
* A function to iterate through the multiboot memory map.
* If `mode` is set to MMAP_GET_NUM, it will return the frame number for the
* frame at address `request`.
* If `mode` is set to MMAP_GET_ADDR, it will return the starting address for
* the frame number `request`.
*
* What does this function?
* 1. Read each entry in the memory map
* 2. Split each entry into frame-sized chunks (4096 bytes)
* 3. Iterate through each chunk until a valid address is found for the given frame number or a frame number is found for the given address.
*/
extern uint32_t mmap_read(uint32_t request, uint8_t mode);

//Allocate the next free frame and return it's frame number
extern uint32_t allocate_frame();

#endif
