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
extern memory_section_data_t memory_sections[6];

//number of available sections that we divide into frames
extern uint8_t memory_framing_sections_number;

//available sections that we have divided into frames
extern memory_framing_data_t memory_paging[6];

//all frames
//(4GB / 4096)
extern memory_page_t memory_frames_table[1048576];

//how many frames are in the table(if memory is less than 4GB)
extern uint32_t memory_frames_table_size;

//physical memory

/*
 * pm_init_memory_page_allocator
 * Collects information about sections of RAM and saves it.
 * Divides the section into frames and enters it in a single table.
 * Protects the kernel data.
 * 
 * @No parameters
 * 
 * Returns false if:
 * Multiboot memory map not found
 * Returns true if all OK.
 */
extern bool pm_init_memory_page_allocator();

/*
 * pm_malloc
 * Allocates physical memory.
 * Guarantees that no less than the value specified in the first parameter will be selected.
 * 
 * @Number of bytes to be allocated.
 * 
 * Returns a pointer to the first byte of the allocated memory area if successful.
 * Returns 0x0 in case of an error.
 */
extern void* pm_malloc(uint32_t nbytes);

/*
 * pm_search_nfranes
 * Allocates physical memory frames.
 * 
 * @Number of frames to be allocated.
 * 
 * Returns a pointer to the first frame of the allocated memory area if successful.
 * Returns 0x0 in case of an error.
 */
extern void* pm_search_nframes(uint32_t nframes);

/*
 * pm_free
 * Frees up memory allocated using pm_malloc.
 * 
 * @Pointer to the first byte of the allocated memory area.
 * 
 * No return value.
 */
extern void pm_free(void* ptr);

/*
 * pm_realloc
 * Typical realloc
 * 
 * @Pointer to the first byte of the allocated memory area.
 * @Required size in bytes.
 * 
 * Returns pointer to a reallocated memory block
 * that can either be the same as the ptrmem argument or refer to a new location.
 * Returns 0x0 if the function failed to allocate the required memory block,
 * the memory block that the ptr argument points to remains unchanged.
 */
extern void* pm_realloc(void* ptr, size_t size);

/*
 * pm_getsize
 * Calculates the size of the memory area in frames.
 * 
 * @Pointer to the first byte of the allocated memory area.
 * 
 * Returns the size in bytes of the memory area allocated using pm_malloc.
 * Returns 0 if the size cannot be calculated.
 */
extern uint32_t pm_getsize(void* ptr);

extern void pm_show_nframes_table(uint32_t from, uint32_t to);

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

//assembler functions
extern void vm_load_page_directory(uint32_t*);
extern void vm_enable_paging();
extern void vm_tlb_flush();
//---------------------------

#endif
