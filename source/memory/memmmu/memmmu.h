#ifndef _MEMMMU_H_
#define _MEMMMU_H_

#include "../../lib/cstdlib.h"
#include "../../lib/stddef.h"
#include "../../lib/stdint.h"
#include "../../lib/string.h"

#include "../memdetect/memdetect.h"

#include "../../debug/debug.h"

//page frame allocator

//секция - то, на что делится оперативная память в memory_map
//страница(фрейм) - то, на что делится доступная секция

//информация о секциях в RAM определяемых с помощью GRUB memory map
typedef struct memory_section_data
{
	//адрес начала секции памяти
	uint32_t address;
	//длина секции в байтах
	uint32_t length;
	//тип секции
	uint8_t type;
} memory_section_data_t;

//информация о доступных секциях в котором мы создаём страницы(фреймы)
typedef struct memory_paging_data
{
	//адрес начала доступной секции для разметки на страницы(фреймы)
	uint32_t address;
	//in bytes(суммарный размер страниц в этом секторе (длина доступной секции в рамках блоков, то есть небольшая часть памяти в конце(которая не вместилась в блок) отбрасывается))
	uint32_t length;
	//кол-во блоков в доступном секторе
	uint32_t count;
} memory_paging_data_t;

typedef struct memory_page
{
	uint32_t physical_address;
	bool is_busy;
	//если выделяли несколько страниц подряд то тут запоминаем сколько страниц идёт следом для освобождения
	uint32_t next_pages;
	uint32_t owner;
} memory_page_t;

//Contain the addresses of the multiboot_info_t structure in order not to touch them
extern uint32_t multiboot_reserved_start;
extern uint32_t multiboot_reserved_end;

//--------------------------
extern uint8_t memory_section_number;

//секции RAM
extern memory_section_data_t memory_sections[255];

//кол-во доступных секций которые мы делим на страницы(фреймы)
extern uint8_t memory_paging_sections_number;

//доступные секции которые мы разбили на страницы(фреймы)
extern memory_paging_data_t memory_paging[255];

//все страницы
//(4GB / 4096)
extern memory_page_t memory_pages_table[1048575];

//сколько страниц(фреймов) есть в таблице(если память меньше 4GB)
extern uint32_t memory_pages_table_count;

extern bool init_memory_page_allocator();
extern void* kmalloc(uint32_t nbytes);
extern void* search_npages(uint32_t n);
extern void kfree(void* ptr);
extern void* krealloc(void* ptr, size_t size);
extern uint32_t ksizeof(void* ptr);

extern void show_npages_table(uint32_t to_n);
//---------------------------

#endif
