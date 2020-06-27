#include "mmu.h"
#include "../memdetect/memdetect.h"
#include "../mmu/mmu.h"
#include "../../debug/debug.h"

//physical memory
uint32_t multiboot_reserved_start;
uint32_t multiboot_reserved_end;

//--------------------------------
//кол-во секций RAM которые мы получили после подсчёта памяти с помощью GRUB
uint8_t memory_section_number = 0;

//секции RAM
memory_section_data_t memory_sections[32];

//кол-во доступных секций которые мы делим на страницы(фреймы)
uint8_t memory_framing_sections_number = 0;

//доступные секции которые мы разбили на страницы(фреймы)
memory_framing_data_t memory_framing[32];

//все страницы
//(4GB / 4096)
memory_page_t memory_pages_table[1048576];

//сколько страниц(фреймов) есть в таблице(если память меньше 4GB)
uint32_t memory_pages_table_count = 0;

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
bool pm_init_memory_page_allocator() {
    if ((MBI->flags & (1<<6)) == false) {
        dprintf("Error: memory map not detected!\n");
        return false;
    }
    multiboot_reserved_start = (uint32_t)MBI;
    multiboot_reserved_end = (uint32_t)(MBI + sizeof(multiboot_info_t));

	//dprintf("\nstartkernel: 0x%X\nendkernel: 0x%X\n", startkernel, endkernel);
	for (uint8_t section_index = 0; section_index < memory_section_number; ++section_index) {
		if (memory_sections[section_index].type == MULTIBOOT_MEMORY_AVAILABLE) {
			//сохраняем данные доступного сектора
			//вычесляем кол-во страниц(фреймов) которых можно в нём создать
			//вычесляем суммарный размер страниц(фреймов) в этом секторе

			//выравниваем адрес
			if (memory_sections[section_index].address % 4096) {
				uint32_t old_addr = memory_sections[section_index].address;
        		memory_sections[section_index].address += 4096 - (memory_sections[section_index].address % 4096);
				//перерасчёт длинны т.к. мы уменьшили область увеличив адрес
				memory_sections[section_index].length = memory_sections[section_index].length - (memory_sections[section_index].address - old_addr);
			}
			else {
				memory_framing[memory_framing_sections_number].address = memory_sections[section_index].address;
				memory_framing[memory_framing_sections_number].count = memory_sections[section_index].length / 4096;
			}
			memory_framing[memory_framing_sections_number].count = memory_sections[section_index].length / 4096;
			memory_framing[memory_framing_sections_number].length = 4096 * memory_framing[memory_framing_sections_number].count;

			//dprintf("Avaiable section address: 0x%X - 0x%X\n", memory_framing[memory_framing_sections_number].address, memory_framing[memory_framing_sections_number].address + memory_framing[memory_framing_sections_number].length);
			//dprintf("Avaiable section count: %I\n", memory_framing[memory_framing_sections_number].count);
			//dprintf("Avaiable section length: %I bytes(%I kilobytes)\n\n", memory_framing[memory_framing_sections_number].length, memory_framing[memory_framing_sections_number].length / 1024);

			uint32_t unavailable_page_counter = 0;
			//делим текущую доступную секцию на страницы(фреймы)
			for (uint32_t page_index = 0; page_index < memory_framing[memory_framing_sections_number].count; ++page_index) {
				//проверяем не заденем ли мы ядро и multiboot информацию, также резервируем память выше ниже 1 МБ
				if (
				       (
					       (memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) + 0x1000 < (uint32_t)startkernel || memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) > (uint32_t)endkernel)
				   	   ) && 
					   (
						   (memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) + 0x1000 < (uint32_t)multiboot_reserved_start || memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) > (uint32_t)multiboot_reserved_end)
					   ) && 
					   		//Memory below 1 MB may be used by devices
					   		memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) > 0x100000
					) {
					if(page_index + memory_pages_table_count < 1048575) {
						memory_pages_table[page_index + memory_pages_table_count].physical_address = memory_framing[memory_framing_sections_number].address + (0x1000 * page_index);
					}
				}
				else {
					++unavailable_page_counter;
					--memory_pages_table_count;
				}
			}
			memory_pages_table_count += memory_framing[memory_framing_sections_number].count;
			memory_framing[memory_framing_sections_number].count -= unavailable_page_counter;
			++memory_framing_sections_number;
		}
	}
	/*
	for (uint32_t i = 0; i < memory_pages_table_count; ++i) {
		printf("Physical page address: 0x%x is_busy: %u\n", memory_pages_table[i].physical_address, memory_pages_table[i].is_busy);
	}
	*/
	ram_available = memory_pages_table_count * 4096;
    return true;
}

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
void* pm_malloc(uint32_t nbytes) {
	//dprintf("kmalloc()\n");
	//dprintf("\nkmalloc try allocate %I bytes\n", nbytes);
	void* first_page_address = 0x0;
	if(!(nbytes % 4096)) {
		first_page_address = pm_search_npages(nbytes / 4096);
	}
	else {
		first_page_address = pm_search_npages((nbytes / 4096) + 1);
	}
	if (first_page_address) {
		//dprintf("kmalloc return 0x%X\n", first_page_address);
		//memset(first_page_address, 0, (nbytes / (4096 + 1)) + 1);
		return first_page_address;
	}
	return 0x0;
}

/*
 * pm_search_npages
 * Allocates physical memory frames.
 * 
 * @Number of pages to be allocated.
 * 
 * Returns a pointer to the first frame of the allocated memory area if successful.
 * Returns 0x0 in case of an error.
 */
void* pm_search_npages(uint32_t npages) {
	//dprintf("search_npages try search %I pages\n", n);
	//счётчик идущих друг за другом страниц(если надо выделить больше 1 страницы(фрейма))
	uint32_t placed_one_after_the_other = 0;
	void* first_page_address = 0x0;
	for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
		if (npages == 1) {
			if (memory_pages_table[page_index].is_busy == 0) {
				memory_pages_table[page_index].is_busy = 1;
				return (void*)memory_pages_table[page_index].physical_address;
			}
		}
		else {
			if (memory_pages_table[page_index].is_busy == 0 && first_page_address == 0x0) {
				first_page_address = (void*)memory_pages_table[page_index].physical_address;
				++placed_one_after_the_other;
				continue;
			}
			if (memory_pages_table[page_index].is_busy == 0 && first_page_address != 0x0) {
				++placed_one_after_the_other;
			}
			if (npages == placed_one_after_the_other) {
				for (uint32_t i = page_index - (placed_one_after_the_other - 1); i <= page_index; ++i) {
					memory_pages_table[i].is_busy = 1;
				}
				memory_pages_table[page_index - (placed_one_after_the_other - 1)].next_frames = placed_one_after_the_other - 1;
				return first_page_address;
			}
			if (memory_pages_table[page_index].is_busy == 1 && first_page_address != 0x0) {
				first_page_address = 0x0;
				placed_one_after_the_other = 0;
			}
		}
	}
	return 0x0;
}

/*
 * pm_free
 * Frees up memory allocated using pm_malloc.
 * 
 * @Pointer to the first byte of the allocated memory area.
 * 
 * No return value.
 */
void pm_free(void* ptr) {
	if(!ptr) {
		return;
	}
	//dprintf("kfree()\n");
	//dprintf("\nkfree try free 0x%X\n", (uint32_t)ptr);
	for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
		if ((void*)memory_pages_table[page_index].physical_address == ptr) {
			if (memory_pages_table[page_index].next_frames == 0) {
				memory_pages_table[page_index].is_busy = 0;
				return;
			}
			else {
				for (uint32_t i = page_index; i <= page_index + memory_pages_table[page_index].next_frames; ++i) {
					memory_pages_table[i].is_busy = 0;
				}
				memory_pages_table[page_index].next_frames = 0;
			}
			return;
		}
	}
}

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
void* pm_realloc(void* ptr, size_t size) {
	void* newptr = 0;
	size_t ptr_size = pm_getsize(ptr);
	if(size == 0) {
		pm_free(ptr);
		//dprintf("pm_realloc return 0x%X\n", 0x0);
		return 0x0;
	}
	//          == size in bytes(blocks * block_size)
	if(ptr_size == (((size / 4096) + 1) * 4096)) {
		//dprintf("pm_realloc return 0x%X\n", ptr);
		return ptr;
	}
	else
	{
		//newptr = kmalloc(size);
		//
		//if(!newptr) {
		//	return 0x0;
		//}
		//
		//if you need more memory
		if(ptr_size < (((size / 4096) + 1) * 4096)) {
			newptr = pm_malloc(size);
			if(!newptr) {
				return 0x0;
			}
			memcpy(newptr, ptr, ptr_size);
			pm_free(ptr);
			//dprintf("pm_realloc return 0x%X\n", newptr);
			return newptr;
		}
		//if you need less memory
		else if(ptr_size > (((size / 4096) + 1) * 4096)) {
			for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
				if((void*)memory_pages_table[page_index].physical_address == ptr) {
					if(((size / 4096) + 1) < (memory_pages_table[page_index].next_frames + 1)) {
						for(uint32_t j = ((size / 4096) + 1); j < (memory_pages_table[page_index].next_frames + 1); ++j) {
							memory_pages_table[page_index + j].is_busy = 0;
						}
						memory_pages_table[page_index].next_frames -= ((size / 4096) + 1);
						//memcpy(newptr, ptr, size);
						//dprintf("pm_realloc return 0x%X\n", ptr);
						return ptr;
					}
					else {
						//dprintf("\n!!!pm_realloc error!!!\n");
						//dprintf("((size / 4096) + 1) = %I\n", ((size / 4096) + 1));
						//dprintf("(memory_pages_table[page_index].next_frames + 1) = %I\n", (memory_pages_table[page_index].next_frames + 1));
						//dprintf("pm_realloc return 0x%X\n", 0x0);
						return 0x0;
					}
					
				}
			}
			//memcpy(newptr, ptr, size);
			//dprintf("pm_realloc return 0x%X\n", 0x0);
			return 0x0;
		}
		pm_free(ptr);
		//dprintf("pm_realloc return 0x%X\n", newptr);
		return newptr;
	}
}

/*
 * pm_getsize
 * Calculates the size of the memory area in frames.
 * 
 * @Pointer to the first byte of the allocated memory area.
 * 
 * Returns the size in bytes of the memory area allocated using pm_malloc.
 * Returns 0 if the size cannot be calculated.
 */
uint32_t pm_getsize(void* ptr) {
	//dprintf("ksizeof()\n");
	for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
		if((void*)memory_pages_table[page_index].physical_address == ptr && memory_pages_table[page_index].is_busy) {
			return (memory_pages_table[page_index].next_frames + 1) * 4096;
		}
	}
	return 0;
}

void show_npages_table(uint32_t from, uint32_t to) {
	//dprintf("\nPAGES_TABLE:\n");
	for (uint32_t i = from; i <= to; ++i) {
		if(i == memory_pages_table_count - 1)
			return;
		dprintf("%I: Physical address: 0x%X is_busy: %I next_frames: %I\n", i, memory_pages_table[i].physical_address, memory_pages_table[i].is_busy, memory_pages_table[i].next_frames);
	}
}

//-------------------------
//virtual memory

/*
 * NAME
 * WRITE THIS SOMETHICalculates the size of the memory area in frames.
 * 
 * @PARAM DESCRIPTION to the first byte of the allocated memory area.
 * 
 * RETURN DESCR the size in bytes of the memory area allocated using pm_malloc.
 * RETURN DESCR 0 if the size cannot be calculated.
 */
void* vm_get_physaddr(uint32_t *pd, void *virtualaddr)
{
	uint32_t pdindex = (uint32_t)virtualaddr >> 22;
	uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

	// Check if the PD entry is present
	if (pd[pdindex]) {
		uint32_t* pt = (uint32_t*)(pd[pdindex] & ~0xFFF);
		// Check if the PT entry is present
		if (pt[ptindex]) {
			return (void*)((pt[ptindex] & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
		}
	}
	return NULL;
}

bool vm_map_page(uint32_t* pd, void* physaddr, void* virtualaddr, unsigned int flags)
{
	//dprintf("flags = %I\n", flags);
	if ((uint32_t)pd % 4096 || !pd) //check addr
		return false;
	// Make sure that both addresses are page-aligned.
	if((uint32_t)physaddr % 4096 || (uint32_t)virtualaddr % 4096)
		return false;
	
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.

	//If the PD entry is not present, create it
	if(!pd[pdindex]) {
		pd[pdindex] = (uint32_t)pm_malloc(sizeof(uint32_t) * 1024);
		if ((uint32_t)pd[pdindex] % 4096) {
			//dprintf("pd[pdindex] address is not aligned!\n");
			pm_free((void*)pd[pdindex]);
			return false;
		}
		memset((void*)pd[pdindex], 0, sizeof(uint32_t) * 1024);
		//Copy the User\Supervisor flag + make the page table Read/Write
		//uint16_t pdir_flags = (flags & PAGE_SUPERVISOR) | PAGE_RW;
		//pd[pdindex] = ((uint32_t)pd[pdindex]) | (pdir_flags & 0xFFF) | PAGE_PRESENT;
		pd[pdindex] = ((uint32_t)pd[pdindex]) | flags;
	}
	
	uint32_t *pt = (uint32_t*)(pd[pdindex] & ~0xFFF);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?

	// If the PT entry is not present, map it
	//if (!pt[ptindex])
	{
		//pt[ptindex] = ((uint32_t)physaddr) | (flags & 0xFFF) | PAGE_PRESENT;
		pt[ptindex] = ((uint32_t)physaddr) | flags;
		//dprintf("0x%X\n", pt[ptindex]);
		// Now you need to flush the entry in the TLB
    	// or you might not notice the change.

		//Flush the entry in the TLB
		vm_tlb_flush();
		return true;
	}
	return false;
}

bool vm_unmap_vpage(uint32_t* pd, void* virtualaddr)
{
	if ((uint32_t)pd % 4096 && !pd)
		return false;
	// Make sure that addresses are page-aligned.
	if((uint32_t)virtualaddr % 4096)
		return false;

	uint32_t pdindex = (uint32_t)virtualaddr >> 22;
	uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

	// If the PD entry is not present
	if (!pd[pdindex]) {
		return false;
	}

	uint32_t* pt = (uint32_t*)(pd[pdindex] & ~0xFFF);

	// If the PT entry is present, unmap it
	if (pt[ptindex]) {
		pt[ptindex] = (uint32_t)NULL;
		// Flush the entry in the TLB
		vm_tlb_flush();
		return true;
	}
	//not exist
	return false;
}

bool vm_set_current_page_directory(uint32_t* pd) {
	if((uint32_t)pd % 4096 && !pd) { //check addr
		return false;
	}
	current_directory_table = pd;
	return true;
}

bool vm_init_paging() {
	kernel_page_directory_table = (uint32_t*)pm_malloc(sizeof(uint32_t) * 1024);
	if ((uint32_t)kernel_page_directory_table % 4096 && !kernel_page_directory_table) { //check addr
		dprintf("kernel_page_directory_table address is not aligned!\n");
		pm_free(kernel_page_directory_table);
		return false;
	}
	memset(kernel_page_directory_table, 0, sizeof(uint32_t) * 1024);

	for(uint32_t i = 0; i < 1024; ++i) {
		//dprintf("----%I----\n", i);
		for (uint32_t j = 0; j < 1024; ++j) {
			uint32_t* physaddr = (uint32_t*)((j + (i * 1024)) * 0x1000);
			//dprintf("%I physaddr = 0x%X(%I)\n", j, physaddr, physaddr);
			//dprintf("kernel_page_directory_table[%I] = %I\n", i, kernel_page_directory_table[i]);
			if(!vm_map_page(kernel_page_directory_table, physaddr, physaddr, PAGE_PRESENT | PAGE_SUPERVISOR | PAGE_RW)) {
				dprintf("error map_page()!\n");
				pm_free(kernel_page_directory_table);
				return false;
			}
		}
		//dprintf("--------\n", i);
	}
	vm_set_current_page_directory(kernel_page_directory_table);

	vm_load_page_directory(current_directory_table);
	vm_enable_paging();
	return true;
}
