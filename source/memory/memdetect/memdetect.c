#include "memdetect.h"
#include "../mmu/mmu.h"

#define MAX_MEMORY_SIZE 0xFFFFFFFFu

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
    uint8_t counter = 0;
    while ((void*)memory_map < (void*)(memory_map_addr + memory_map_len))
    {
        //реальный размер, может быть больше 4 GB благодаря PAE
        ram_len += memory_map->len;
        //сохраняем данные о секциях
        //ограничиваем всё 4 ГБ
        //dprintf("addr = 0x%Z\n", memory_map->addr);
        //dprintf("length = %L\n", memory_map->len);
        if(memory_map->addr + memory_map->len > MAX_MEMORY_SIZE) {
            //если адресс за пределами 4 ГБ
            if(memory_map->addr > MAX_MEMORY_SIZE) {
                //смотрим следующую секцию
                memory_map = (multiboot_memory_map_t*)((uint32_t)memory_map + memory_map->size + sizeof(memory_map->size));
                continue;
            }
            //если размер адресс в пределах 4 ГБ, но размер нет
            //то ограничиваем размер чтобы секция была в пределах 4 ГБ
            //(вероятно такой случай не существует)
            memory_sections[counter].address = (uint32_t)memory_map->addr;
            //пересчитываем длину чтобы секция она не вылазила за 4 ГБ
            memory_sections[counter].length = MAX_MEMORY_SIZE - (uint32_t)memory_map->addr;
            memory_sections[counter].type = memory_map->type;
            
        }
        else {
        //в пределах 4 ГБ
            memory_sections[counter].address = (uint32_t)memory_map->addr;
            memory_sections[counter].length = (uint32_t)memory_map->len;
            memory_sections[counter].type = memory_map->type;
        }
        ++counter;
        //смотрим следующую секцию
        memory_map = (multiboot_memory_map_t*)((uint32_t)memory_map + memory_map->size + sizeof(memory_map->size));
    }
    memory_section_number = counter;
}
