#include "memdetect.h"
#include "../memmmu/memmmu.h"

multiboot_memory_map_t* memory_map = NULL;

uint64_t ram_len = NULL;
uint64_t ram_available = NULL;

uint32_t memory_map_addr = NULL;
uint32_t memory_map_len = NULL;

void init_memory() {
    memory_map = (multiboot_memory_map_t*)MBI->mmap_addr;
    memory_map_addr = MBI->mmap_addr;
    memory_map_len = MBI->mmap_length;
}

void calclulate_memory() {
    uint32_t counter = 0;
    while ((void*)memory_map < (void*)(memory_map_addr + memory_map_len))
    {
        ram_len += memory_map->len;
        
        //сохраняем данные о секциях
        memory_sections[counter].address = (uint32_t)memory_map->addr;
        memory_sections[counter].length = (uint32_t)memory_map->len;
        memory_sections[counter].type = memory_map->type;
        if(memory_map->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            ram_available += memory_map->len;
            //dprintf("Avaiable RAM addr: 0x%X\n", (uint32_t)memory_map->addr);
            //dprintf("Length: %I bytes(%I kilobytes)\n", (uint32_t)memory_map->len, memory_map->len / 1024);
        }
        else
        {
            //dprintf("Unavaiable RAM addr: 0x%X\n", (uint32_t)memory_map->addr);
        }
        memory_map = (multiboot_memory_map_t*)((uint32_t)memory_map + memory_map->size + sizeof(memory_map->size));
        ++counter;
    }
    memory_section_number = counter;
}
