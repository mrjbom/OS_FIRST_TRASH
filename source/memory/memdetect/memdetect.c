#include "memdetect.h"
#include "../mmu/mmu.h"

#define MAX_MEMORY_SIZE 0xFFFFFFFFu

multiboot_memory_map_t* memory_map = NULL;

uint64_t ram_len = NULL;
uint64_t ram_available = NULL;

uint32_t memory_map_addr = NULL;
uint32_t memory_map_len = NULL;

void memory_map_init() {
    memory_map = (multiboot_memory_map_t*)MBI->mmap_addr;
    memory_map_addr = MBI->mmap_addr;
    memory_map_len = MBI->mmap_length;
}

void memory_map_calculate() {
    uint8_t counter = 0;
    while ((void*)memory_map < (void*)(memory_map_addr + memory_map_len))
    {
        //real size, can be more than 4 GB thanks to PAE
        ram_len += memory_map->len;
        //saving data about sections
        //limit memory to 4 GB
        //dprintf("addr = 0x%Z\n", memory_map->addr);
        //dprintf("length = %L\n", memory_map->len);
        if(memory_map->addr + memory_map->len > MAX_MEMORY_SIZE) {
            //if the address is outside of 4 GB, then skip it
            if(memory_map->addr > MAX_MEMORY_SIZE) {
                //watch the next section
                memory_map = (multiboot_memory_map_t*)((uint32_t)memory_map + memory_map->size + sizeof(memory_map->size));
                continue;
            }
            //if the address size is within 4 GB, but the size is not
            //then limit the size so that the partition is within 4 GB
            //(this probably doesn't exist)
            memory_sections[counter].address = (uint32_t)memory_map->addr;
            //recalculating the length so that the section it did not get out for 4 GB
            memory_sections[counter].length = MAX_MEMORY_SIZE - (uint32_t)memory_map->addr;
            memory_sections[counter].type = memory_map->type;
            
        }
        else {
            //within 4 GB
            memory_sections[counter].address = (uint32_t)memory_map->addr;
            memory_sections[counter].length = (uint32_t)memory_map->len;
            memory_sections[counter].type = memory_map->type;
        }
        ++counter;
        //watch the next section
        memory_map = (multiboot_memory_map_t*)((uint32_t)memory_map + memory_map->size + sizeof(memory_map->size));
    }
    memory_section_number = counter;
}
