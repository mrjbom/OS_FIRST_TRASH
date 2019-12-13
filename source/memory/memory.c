#include "memory.h"
#include "../kernel/kernel.h"
#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"

multiboot_memory_map_t* memory_map = (multiboot_memory_map_t*)MBI->mmap_addr;

uint32_t ram_len;
uint32_t ram_available;

uint32_t memory_map_addr = MBI->mmap_addr;
uint32_t memory_map_len = MBI->mmap_length;

void calclulate_memory() {
    while ((void*)memory_map < (void*)(memory_map_addr + memory_map_len))
    {
        ram_len += memory_map->len;
        if(memory_map->type == 1)
            ram_available += memory_map->len;
        memory_map = (multiboot_memory_map_t*)((unsigned int)memory_map + memory_map->size + sizeof(memory_map->size));
    }
}
