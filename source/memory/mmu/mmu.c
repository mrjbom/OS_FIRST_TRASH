#include "mmu.h"
#include "../memdetect/memdetect.h"
#include "../mmu/mmu.h"
#include "../../debug/debug.h"

//physical memory
uint32_t multiboot_reserved_start;
uint32_t multiboot_reserved_end;

//--------------------------------
//number of RAM sections that we received after counting memory using GRUB
uint8_t memory_section_number = 0;

//RAM sections from GRUB memory_map
memory_section_data_t memory_sections[6];

//number of available sections that we divide into frames
uint8_t memory_framing_sections_number = 0;

//доступные секции которые мы разбили на фреймы
memory_framing_data_t memory_framing[6];

//all frames
//(4GB / 4096)
memory_page_t memory_frames_table[1048576];

//how many frames are in the table
uint32_t memory_frames_table_size = 0;

bool pm_init_memory_page_allocator() {
    if ((MBI->flags & (1<<6)) == false) {
        serial_printf("memory map not detected!\n");
        return false;
    }
    multiboot_reserved_start = (uint32_t)MBI;
    multiboot_reserved_end = (uint32_t)(MBI + sizeof(multiboot_info_t));

	//serial_printf("\nstartkernel: 0x%x\nendkernel: 0x%x\n", startkernel, endkernel);
	for (uint8_t section_index = 0; section_index < memory_section_number; ++section_index) {
		if (memory_sections[section_index].type == MULTIBOOT_MEMORY_AVAILABLE) {
			//saving data for the available section
			//calculating the number of frames that can be created in it
			//calculating the total size of frames in this section

			// align the address
			if (memory_sections[section_index].address % 4096) {
				uint32_t old_addr = memory_sections[section_index].address;
        		memory_sections[section_index].address += 4096 - (memory_sections[section_index].address % 4096);
				//recalculate the length because we have reduced the area by increasing the address
				memory_sections[section_index].length = memory_sections[section_index].length - (memory_sections[section_index].address - old_addr);
			}
			else {
				memory_framing[memory_framing_sections_number].address = memory_sections[section_index].address;
				memory_framing[memory_framing_sections_number].count = memory_sections[section_index].length / 4096;
			}
			memory_framing[memory_framing_sections_number].count = memory_sections[section_index].length / 4096;
			memory_framing[memory_framing_sections_number].length = 4096 * memory_framing[memory_framing_sections_number].count;

			//serial_printf("Avaiable section address: 0x%x - 0x%x\n", memory_framing[memory_framing_sections_number].address, memory_framing[memory_framing_sections_number].address + memory_framing[memory_framing_sections_number].length);
			//serial_printf("Avaiable section count: %u\n", memory_framing[memory_framing_sections_number].count);
			//serial_printf("Avaiable section length: %u bytes(%u kilobytes)\n\n", memory_framing[memory_framing_sections_number].length, memory_framing[memory_framing_sections_number].length / 1024);

			uint32_t unavailable_page_counter = 0;
			//dividing the current available section into frames
			for (uint32_t page_index = 0; page_index < memory_framing[memory_framing_sections_number].count; ++page_index) {
				//check whether we will not touch the core and multiboot information, and reserve memory above or below 1 MB
				if (
				       (
					       (memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) + 0x1000 < (uint32_t)startkernel || memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) > (uint32_t)endkernel)
				   	   ) && 
					   (
						   (memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) + 0x1000 < (uint32_t)multiboot_reserved_start || memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) > (uint32_t)multiboot_reserved_end)
					   ) && 
					   		//memory below 1 MB may be used by devices
					   		memory_framing[memory_framing_sections_number].address + (0x1000 * page_index) > 0x100000
					) {
					if(page_index + memory_frames_table_size < 1048575) {
						memory_frames_table[page_index + memory_frames_table_size].physical_address = memory_framing[memory_framing_sections_number].address + (0x1000 * page_index);
					}
				}
				else {
					++unavailable_page_counter;
					--memory_frames_table_size;
				}
			}
			memory_frames_table_size += memory_framing[memory_framing_sections_number].count;
			memory_framing[memory_framing_sections_number].count -= unavailable_page_counter;
			++memory_framing_sections_number;
		}
	}
	/*
	for (uint32_t i = 0; i < memory_frames_table_size; ++i) {
		printf("Physical page address: 0x%x is_busy: %u\n", memory_frames_table[i].physical_address, memory_frames_table[i].is_busy);
	}
	*/
	ram_available = memory_frames_table_size * 4096;
    return true;
}

void* pm_malloc(uint32_t nbytes) {
	//serial_printf("kmalloc()\n");
	//serial_printf("\nkmalloc try allocate %u bytes\n", nbytes);
	void* first_page_address = 0x0;
	if(!(nbytes % 4096)) {
		first_page_address = pm_search_nframes(nbytes / 4096);
	}
	else {
		first_page_address = pm_search_nframes((nbytes / 4096) + 1);
	}
	if (first_page_address) {
		//serial_printf("kmalloc return 0x%x\n", first_page_address);
		//memset(first_page_address, 0, (nbytes / (4096 + 1)) + 1);
		return first_page_address;
	}
	return 0x0;
}

void* pm_search_nframes(uint32_t nframes) {
	//serial_printf("search_npages try search %I pages\n", n);
	//счётчик идущих друг за другом страниц(если надо выделить больше 1 страницы(фрейма))
	uint32_t placed_one_after_the_other = 0;
	void* first_page_address = 0x0;
	for (uint32_t page_index = 0; page_index < memory_frames_table_size; ++page_index) {
		if (nframes == 1) {
			if (memory_frames_table[page_index].is_busy == 0) {
				memory_frames_table[page_index].is_busy = 1;
				return (void*)memory_frames_table[page_index].physical_address;
			}
		}
		else {
			if (memory_frames_table[page_index].is_busy == 0 && first_page_address == 0x0) {
				first_page_address = (void*)memory_frames_table[page_index].physical_address;
				++placed_one_after_the_other;
				continue;
			}
			if (memory_frames_table[page_index].is_busy == 0 && first_page_address != 0x0) {
				++placed_one_after_the_other;
			}
			if (nframes == placed_one_after_the_other) {
				for (uint32_t i = page_index - (placed_one_after_the_other - 1); i <= page_index; ++i) {
					memory_frames_table[i].is_busy = 1;
				}
				memory_frames_table[page_index - (placed_one_after_the_other - 1)].next_frames = placed_one_after_the_other - 1;
				return first_page_address;
			}
			if (memory_frames_table[page_index].is_busy == 1 && first_page_address != 0x0) {
				first_page_address = 0x0;
				placed_one_after_the_other = 0;
			}
		}
	}
	return 0x0;
}

void pm_free(void* ptr) {
	if(!ptr) {
		return;
	}
	//serial_printf("kfree()\n");
	//serial_printf("\nkfree try free 0x%x\n", (uint32_t)ptr);
	for (uint32_t page_index = 0; page_index < memory_frames_table_size; ++page_index) {
		if ((void*)memory_frames_table[page_index].physical_address == ptr) {
			if (memory_frames_table[page_index].next_frames == 0) {
				memory_frames_table[page_index].is_busy = 0;
				return;
			}
			else {
				for (uint32_t i = page_index; i <= page_index + memory_frames_table[page_index].next_frames; ++i) {
					memory_frames_table[i].is_busy = 0;
				}
				memory_frames_table[page_index].next_frames = 0;
			}
			return;
		}
	}
}

void* pm_realloc(void* ptr, size_t size) {
	void* newptr = 0;
	size_t ptr_size = pm_getsize(ptr);
	if(size == 0) {
		pm_free(ptr);
		//serial_printf("pm_realloc return 0x%x\n", 0x0);
		return 0x0;
	}
	//          == size in bytes(blocks * block_size)
	if(ptr_size == (((size / 4096) + 1) * 4096)) {
		//serial_printf("pm_realloc return 0x%x\n", ptr);
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
			//serial_printf("pm_realloc return 0x%x\n", newptr);
			return newptr;
		}
		//if you need less memory
		else if(ptr_size > (((size / 4096) + 1) * 4096)) {
			for (uint32_t page_index = 0; page_index < memory_frames_table_size; ++page_index) {
				if((void*)memory_frames_table[page_index].physical_address == ptr) {
					if(((size / 4096) + 1) < (memory_frames_table[page_index].next_frames + 1)) {
						for(uint32_t j = ((size / 4096) + 1); j < (memory_frames_table[page_index].next_frames + 1); ++j) {
							memory_frames_table[page_index + j].is_busy = 0;
						}
						memory_frames_table[page_index].next_frames -= ((size / 4096) + 1);
						//memcpy(newptr, ptr, size);
						//serial_printf("pm_realloc return 0x%x\n", ptr);
						return ptr;
					}
					else {
						//serial_printf("pm_realloc error\n");
						//serial_printf("((size / 4096) + 1) = %u\n", ((size / 4096) + 1));
						//serial_printf("(memory_frames_table[page_index].next_frames + 1) = %u\n", (memory_frames_table[page_index].next_frames + 1));
						//serial_printf("pm_realloc return 0x%x\n", 0x0);
						return 0x0;
					}
					
				}
			}
			//memcpy(newptr, ptr, size);
			//serial_printf("pm_realloc return 0x%x\n", 0x0);
			return NULL;
		}
		pm_free(ptr);
		//serial_printf("pm_realloc return 0x%x\n", newptr);
		return newptr;
	}
}

uint32_t pm_getsize(void* ptr) {
	//serial_printf("ksizeof()\n");
	for (uint32_t page_index = 0; page_index < memory_frames_table_size; ++page_index) {
		if((void*)memory_frames_table[page_index].physical_address == ptr && memory_frames_table[page_index].is_busy) {
			return (memory_frames_table[page_index].next_frames + 1) * 4096;
		}
	}
	return 0;
}

void pm_show_nframes_table(uint32_t from, uint32_t to) {
	//serial_printf("\nPAGES_TABLE:\n");
	for (uint32_t i = from; i <= to; ++i) {
		if(i == memory_frames_table_size - 1)
			return;
		serial_printf("%u: Physical address: 0x%x is_busy: %i next_frames: %u\n", i, memory_frames_table[i].physical_address, memory_frames_table[i].is_busy, memory_frames_table[i].next_frames);
	}
}

//-------------------------
//virtual memory

void* vm_get_physaddr(uint32_t *pd, void *virtualaddr)
{
	//check addresses
	if (!addr32_is_page_aligned(pd) || !pd)
		return NULL;
	if (!addr32_is_page_aligned(virtualaddr) || !virtualaddr)
		return NULL;

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

bool vm_map_page(uint32_t* pd, void* physaddr, void* virtualaddr, uint32_t flags)
{
	//serial_printf("flags = %u\n", flags);
	if (!addr32_is_page_aligned(pd) || !pd) //check addr
		return false;
	// Make sure that both addresses are page-aligned.
	if(!addr32_is_page_aligned(physaddr) || !addr32_is_page_aligned(virtualaddr))
		return false;
	
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.

	//If the PD entry is not present, create it
	if(!pd[pdindex]) {
		pd[pdindex] = (uint32_t)pm_malloc(sizeof(uint32_t) * 1024);
		if (!addr32_is_page_aligned(pd[pdindex]) || !pd[pdindex]) {
			//serial_printf("pd[pdindex] address is not aligned!\n");
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
		//serial_printf("0x%x\n", pt[ptindex]);
		// Now you need to flush the entry in the TLB
    	// or you might not notice the change.

		//Flush the entry in the TLB
		vm_tlb_flush();
		return true;
	}
	//return false;
}

bool vm_unmap_vpage(uint32_t* pd, void* virtualaddr)
{
	if (!addr32_is_page_aligned(pd) || !pd)
		return false;
	if (!addr32_is_page_aligned(virtualaddr))
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
	if(!addr32_is_page_aligned(pd) && !pd) { //check addr
		return false;
	}
	current_directory_table = pd;
	return true;
}

bool vm_init_paging() {
	kernel_page_directory_table = (uint32_t*)pm_malloc(sizeof(uint32_t) * 1024);
	if (!addr32_is_page_aligned(kernel_page_directory_table) || !kernel_page_directory_table) { //check addr
		serial_printf("kernel_page_directory_table address is not aligned!\n");
		pm_free(kernel_page_directory_table);
		return false;
	}
	memset(kernel_page_directory_table, 0, sizeof(uint32_t) * 1024);

	for(uint32_t i = 0; i < 1024; ++i) {
		//serial_printf("----%u----\n", i);
		for (uint32_t j = 0; j < 1024; ++j) {
			uint32_t* physaddr = (uint32_t*)((j + (i * 1024)) * 0x1000);
			//serial_printf("%u physaddr = 0x%x(%u)\n", j, physaddr, physaddr);
			//serial_printf("kernel_page_directory_table[%u] = %u\n", i, kernel_page_directory_table[i]);
			if(!vm_map_page(kernel_page_directory_table, physaddr, physaddr, PAGE_PRESENT | PAGE_RW)) {
				serial_printf("error map_page()!\n");
				pm_free(kernel_page_directory_table);
				return false;
			}
		}
		//serial_printf("--------\n");
	}
	vm_set_current_page_directory(kernel_page_directory_table);

	vm_load_page_directory(current_directory_table);
	vm_enable_paging();
	return true;
}

bool vm_set_page_flags(uint32_t* pd, void* virtualaddr, uint32_t flags) {
	if(!addr32_is_page_aligned(pd) || !pd)
		return false;
	if(!addr32_is_page_aligned(virtualaddr))
		return false;

	uint32_t pdindex = (uint32_t)virtualaddr >> 22;
	uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

	// If the PD entry is not present
	if (!pd[pdindex]) {
		return false;
	}

	uint32_t* pt = (uint32_t*)(pd[pdindex] & ~0xFFF);

	// If the PT entry is present, set flags
	if (pt[ptindex]) {
		pt[ptindex] = ((uint32_t)vm_get_physaddr(pd, virtualaddr)) | flags;
		if(pd == current_directory_table) {
			vm_tlb_flush();
		}
		return true;
	}
	//not exist
	return false;
}

bool vm_get_page_flags(uint32_t* pd, void* virtualaddr, uint32_t* flags) {
	if(!addr32_is_page_aligned(pd) || !pd)
		return false;
	if(!addr32_is_page_aligned(virtualaddr))
		return false;

	uint32_t pdindex = (uint32_t)virtualaddr >> 22;
	uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

	// If the PD entry is not present
	if (!pd[pdindex]) {
		return false;
	}

	uint32_t* pt = (uint32_t*)(pd[pdindex] & ~0xFFF);

	// If the PT entry is present, get flags
	if (pt[ptindex]) {
		*flags = 0;
		//PAGE_PRESENT
		set_n_bit(*flags, 0, get_n_bit(pt[ptindex], 0));
		//PAGE_RW
		set_n_bit(*flags, 1, get_n_bit(pt[ptindex], 1));
		//PAGE_USERSPACE_AVAILABLE
		set_n_bit(*flags, 2, get_n_bit(pt[ptindex], 2));
		
		//serial_printf(
		//	"PAGE_PRESENT = %u\n"
		//	"PAGE_RW = %u\n"
		//	"PAGE_USERSPACE_AVAILABLE = %u\n",
		//	((pt[ptindex] >> 0) & 1),
		//	((pt[ptindex] >> 1) & 1),
		//	((pt[ptindex] >> 2) & 1)
		//	);
		return true;
	}
	//not exist
	return false;
}
