#include "memory.h"

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
    while ((void*)memory_map < (void*)(memory_map_addr + memory_map_len))
    {
        ram_len += memory_map->len;
        if(memory_map->type == 1)
            ram_available += memory_map->len;
        memory_map = (multiboot_memory_map_t*)((unsigned int)memory_map + memory_map->size + sizeof(memory_map->size));
    }
}

//page frame allocator
uint32_t multiboot_reserved_start;
uint32_t multiboot_reserved_end;

uint32_t next_free_frame;

bool memory_page_allocator_init() {
    if ((MBI->flags & (1<<6)) == false)
    {
        dprintf("Error: memory map not detected!\n");
        return false;
    }
    multiboot_reserved_start = (uint32_t)MBI;
    multiboot_reserved_end = (uint32_t)(MBI + sizeof(multiboot_info_t));
    next_free_frame = 1;
    return true;
}

uint32_t mmap_read(uint32_t request, uint8_t mode) {
    // We're reserving frame number 0 for errors, so skip all requests for 0
    if (request == 0) return 0;

    // If the user specifies an invalid mode, also skip the request
    if (mode != MMAP_GET_NUM && mode != MMAP_GET_ADDR) return 0;

    // Increment through each entry in the multiboot memory map
    uint32_t* cur_mmap_addr = (uint32_t*)MBI->mmap_addr;
    uint32_t* mmap_end_addr = cur_mmap_addr + MBI->mmap_length;
    uint32_t cur_num = 0;
    while (cur_mmap_addr < mmap_end_addr) {
        // Get a pointer to the current entry
        multiboot_memory_map_t *current_entry = (multiboot_memory_map_t *)cur_mmap_addr;

        // Now let's split this entry into page sized chunks and increment our
        // internal frame number counter
        uint64_t i;
        uint64_t current_entry_end = current_entry->addr + current_entry->len;
        for (i=current_entry->addr; i + PAGE_SIZE < current_entry_end; i += PAGE_SIZE) {
            if (mode == MMAP_GET_NUM && request >= i && request <= i + PAGE_SIZE) {
                // If we're looking for a frame number from an address and we found it
                // return the frame number
                return cur_num + 1;
            }

            // If the requested chunk is in reserved space, skip it
            if (current_entry->type == MULTIBOOT_MEMORY_RESERVED) {
                if (mode == MMAP_GET_ADDR && cur_num == request) {
                    // The address of a chunk in reserved space was requested
                    // Increment the request until it is no longer reserved
                    ++request;
                }
                // Skip to the next chunk until it's no longer reserved
                ++cur_num;
                continue;
            } else if (mode == MMAP_GET_ADDR && cur_num == request) {
                // If we're looking for a frame starting address and we found it
                // return the starting address
                return i;
            }
            ++cur_num;
        }

        // Increment by the size of the current entry to get to the next one
        cur_mmap_addr += current_entry->size + sizeof(uint32_t*);
    }
    // If no results are found, return 0
    return 0;
}

uint32_t allocate_frame() {
    // Get the address for the next free frame
    uint32_t cur_addr = mmap_read(next_free_frame, MMAP_GET_ADDR);

    // Verify that the frame is not in the multiboot reserved area
    // If it is, increment the next free frame number and recursively call back.
    if (cur_addr >= multiboot_reserved_start && cur_addr <= multiboot_reserved_end) {
        ++next_free_frame;
        return allocate_frame();
    }

    // Call mmap_read again to get the frame number for our address
    uint32_t cur_num = mmap_read(cur_addr, MMAP_GET_NUM);

    // Update next_free_frame to the next unallocated frame number
    next_free_frame = cur_num + 1;

    // Finally, return the newly allocated frame num
    return cur_num;
}
