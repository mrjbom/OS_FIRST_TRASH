#ifndef _mmu_H_
#define _mmu_H_

#include "../../lib/cstdlib.h"
#include "../../lib/stddef.h"
#include "../../lib/stdint.h"
#include "../../lib/string.h"

//page frame allocator

//stack data from bootloader.asm
extern char stack_bottom[];
extern char stack_top[];

//section - what RAM is divided into in memory_map
//frame - what the available section is divided into within physical memory
//4096-byte areas

//information about sections into RAM is determined using the GRUB memory map
typedef struct memory_section_data
{
	//address of the beginning of the memory section
	uint32_t address;
	//section length in bytes
	uint32_t length;
	//section type
	uint8_t type;
} memory_section_data_t;

//information about available sections in which we create frames
typedef struct memory_framing_data
{
	//address of the beginning of the available section for page markup(frames)
	uint32_t address;
	//in bytes (the total size of frames in this sector (the length of the available section within blocks
	//that is, a small portion of memory at the end(that did not fit in the block) is discarded))
	uint32_t length;
	//number of blocks in the available sector
	uint32_t count;
} memory_framing_data_t;

typedef struct memory_page
{
	uint32_t physical_address;
	bool is_busy;
	//if you selected several frames in a row then here we remember how many pages follow to release
	uint32_t next_frames;
	uint32_t owner;
} memory_page_t;

//Contain the addresses of the multiboot_info_t structure in order not to touch them
extern uint32_t multiboot_reserved_start;
extern uint32_t multiboot_reserved_end;

//--------------------------
extern uint8_t memory_section_number;

//RAM sections data
extern memory_section_data_t memory_sections[32];

//number of available sections that we divide into frames
extern uint8_t memory_framing_sections_number;

//available sections that we have divided into frames
extern memory_framing_data_t memory_paging[32];

//all pages
//(4GB / 4096)
extern memory_page_t memory_pages_table[1048576];

//how many frames are in the table(if memory is less than 4GB)
extern uint32_t memory_pages_table_count;

//physical memory
extern bool pm_init_memory_page_allocator();
extern void* pm_malloc(uint32_t nbytes);
extern void* pm_search_npages(uint32_t npages);
extern void pm_free(void* ptr);
extern void* pm_realloc(void* ptr, size_t size);
extern uint32_t pm_getsize(void* ptr);
extern void show_npages_table(uint32_t from, uint32_t to);

//---------------------------
//virtual memory
enum PagingFlags {
	PAGE_PRESENT = 1,
	PAGE_RW = 2,
	PAGE_SUPERVISOR = 4
};

uint32_t* current_directory_table;
uint32_t* kernel_page_directory_table;

extern void* vm_get_physaddr(uint32_t *pd, void* virtualaddr);
extern bool vm_map_page(uint32_t* pd, void* physaddr, void* virtualaddr, unsigned int flags);
extern bool vm_unmap_vpage(uint32_t* pd, void* virtualaddr);
extern bool vm_set_current_page_directory(uint32_t* pd);
extern bool vm_init_paging();
extern void vm_load_page_directory(uint32_t*);
extern void vm_enable_paging();
extern void vm_tlb_flush();
//---------------------------

#endif
