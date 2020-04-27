#include "memmmu.h"

//page frame allocator
uint32_t multiboot_reserved_start;
uint32_t multiboot_reserved_end;

//--------------------------------
//кол-во секций RAM которые мы получили после подсчёта памяти с помощью GRUB
uint8_t memory_section_number = 0;

//секции RAM
memory_section_data_t memory_sections[255];

//кол-во доступных секций которые мы делим на страницы(фреймы)
uint8_t memory_paging_sections_number = 0;

//доступные секции которые мы разбили на страницы(фреймы)
memory_paging_data_t memory_paging[255];

//все страницы
//(4GB / 4096)
memory_page_t memory_pages_table[1048575];

//сколько страниц(фреймов) есть в таблице(если память меньше 4GB)
uint32_t memory_pages_table_count = 0;

//bool init_memory_page_allocator();
//void* kmalloc(uint32_t nbytes);
//void* search_npages(uint32_t number);
//void kfree(void* ptr);

void show_page_table(uint32_t to_n);

bool init_memory_page_allocator() {
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
			memory_paging[memory_paging_sections_number].address = memory_sections[section_index].address;
			memory_paging[memory_paging_sections_number].count = memory_sections[section_index].length / 4096;
			memory_paging[memory_paging_sections_number].length = 4096 * memory_paging[memory_paging_sections_number].count;

			//dprintf("Avaiable section address: 0x%X\n", memory_paging[memory_paging_sections_number].address);
			//dprintf("Avaiable section count: %I\n", memory_paging[memory_paging_sections_number].count);
			//dprintf("Avaiable section length: %I bytes(%I kilobytes)\n\n", memory_paging[memory_paging_sections_number].length, memory_paging[memory_paging_sections_number].length / 1024);

			uint32_t unavailable_page_counter = 0;
			//делим текущую доступную секцию на страницы(фреймы)
			for (uint32_t page_index = 0; page_index < memory_paging[memory_paging_sections_number].count; ++page_index) {
				//проверяем не заденем ли мы ядро и его память, также резервируем nullptr
				if (
				       (
					       (memory_paging[memory_paging_sections_number].address + (0x1000 * page_index) + 0x1000 < (uint32_t)startkernel || memory_paging[memory_paging_sections_number].address + (0x1000 * page_index) > (uint32_t)endkernel)
				   	   ) && 
					   (
						   (memory_paging[memory_paging_sections_number].address + (0x1000 * page_index) + 0x1000 < (uint32_t)multiboot_reserved_start || memory_paging[memory_paging_sections_number].address + (0x1000 * page_index) > (uint32_t)multiboot_reserved_end)
					   ) && 
					   		memory_paging[memory_paging_sections_number].address + (0x1000 * page_index) > 0x0
					) {
					if(page_index + memory_pages_table_count < 1048575)
						memory_pages_table[page_index + memory_pages_table_count].physical_address = memory_paging[memory_paging_sections_number].address + (0x1000 * page_index);
				}
				else {
					++unavailable_page_counter;
					--memory_pages_table_count;
				}
			}
			memory_pages_table_count += memory_paging[memory_paging_sections_number].count;
			memory_paging[memory_paging_sections_number].count -= unavailable_page_counter;
			++memory_paging_sections_number;
		}
	}
	/*
	for (uint32_t i = 0; i < memory_pages_table_count; ++i) {
		printf("Physical page address: 0x%x is_busy: %u\n", memory_pages_table[i].physical_address, memory_pages_table[i].is_busy);
	}
	*/
    return true;
}

void* kmalloc(uint32_t nbytes) {
	//dprintf("\nkmalloc try allocate %I bytes\n", nbytes);
	void* first_page_address = search_npages(nbytes / 4096 + 1);
	if (first_page_address != 0x0)
		return first_page_address;
	else
		return 0x0;
}

void* search_npages(uint32_t n) {
	//dprintf("search_npages try search %I pages\n", n);
	//счётчик идущих друг за другом страниц(если надо выделить больше 1 страницы(фрейма))
	uint32_t placed_one_after_the_other = 0;
	void* first_page_address = 0x0;
	for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
		if (n == 1) {
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
			if (n == placed_one_after_the_other) {
				for (uint32_t i = page_index - (placed_one_after_the_other - 1); i <= page_index; ++i) {
					memory_pages_table[i].is_busy = 1;
				}
				memory_pages_table[page_index - (placed_one_after_the_other - 1)].next_pages = placed_one_after_the_other - 1;
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

void kfree(void* ptr) {
	//dprintf("\nkfree try free 0x%X\n", (uint32_t)address);
	for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
		if ((void*)memory_pages_table[page_index].physical_address == ptr) {
			if (memory_pages_table[page_index].next_pages == 0) {
				memory_pages_table[page_index].is_busy = 0;
				return;
			}
			else {
				for (uint32_t i = page_index; i <= page_index + memory_pages_table[page_index].next_pages; ++i) {
					memory_pages_table[i].is_busy = 0;
				}
				memory_pages_table[page_index].next_pages = 0;
			}
			return;
		}
	}
}

void* krealloc(void* ptr, size_t size) {
	void* newptr = 0;
	size_t ptr_size = ksizeof(ptr);
	if(size == 0) {
		kfree(ptr);
		return 0x0;
	}
	//          == size in bytes(blocks * block_size)
	if(ptr_size == (((size / 4096) + 1) * 4096)) {
		return ptr;
	}
	else
	{
		//newptr = kmalloc(size);
		/*
		if(!newptr) {
			return 0x0;
		}
		*/
		//if you need more memory
		if(ptr_size < (((size / 4096) + 1) * 4096)) {
			newptr = kmalloc(size);
			if(!newptr) {
				return 0x0;
			}
			memcpy(newptr, ptr, ptr_size);
			return newptr;
		}
		//if you need less memory
		else if(ptr_size > (((size / 4096) + 1) * 4096)) {
			for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
				if((void*)memory_pages_table[page_index].physical_address == ptr) {
					if(((size / 4096) + 1) < (memory_pages_table[page_index].next_pages + 1)) {
						for(uint32_t j = ((size / 4096) + 1); j < (memory_pages_table[page_index].next_pages + 1); ++j) {
							memory_pages_table[page_index + j].is_busy = 0;
						}
						memory_pages_table[page_index].next_pages -= ((size / 4096) + 1);
						memcpy(newptr, ptr, size);
						return ptr;
					}
					else {
						//dprintf("\n!!!realloc error!!!\n");
						//dprintf("((size / 4096) + 1) = %I\n", ((size / 4096) + 1));
						//dprintf("(memory_pages_table[page_index].next_pages + 1) = %I\n", (memory_pages_table[page_index].next_pages + 1));
						return 0x0;
					}
					
				}
			}
			//memcpy(newptr, ptr, size);
			return 0x0;
		}
		kfree(ptr);
		return newptr;
	}
}

//return size as (4096 * number of blocks in the pointer)
uint32_t ksizeof(void* ptr) {
	for (uint32_t page_index = 0; page_index < memory_pages_table_count; ++page_index) {
		if((void*)memory_pages_table[page_index].physical_address == ptr) {
			return (memory_pages_table[page_index].next_pages + 1) * 4096;
		}
	}
	return 0;
}
//-------------------------------------

void show_npages_table(uint32_t to_n) {
	dprintf("\nPAGES_TABLE:\n");
	for (uint32_t i = 0; i < to_n; ++i) {
		dprintf("%I: Physical address: 0x%X is_busy: %I next_pages: %I\n", i, memory_pages_table[i].physical_address, memory_pages_table[i].is_busy, memory_pages_table[i].next_pages);
	}
}
