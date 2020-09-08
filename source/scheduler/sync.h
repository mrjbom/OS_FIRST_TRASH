#ifndef _SYNC_H_
#define _SYNC_H_

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"
#include "../lib/glist.h"
#include "../memory/mmu/mmu.h"

/*
typedef struct semaphore
{
    uint32_t max_value;
    uint32_t current_value;
} semaphore_t;

//semaphore
semaphore_t* semaphore_create(uint32_t max_value);

void semaphore_lock(semaphore_t* semaphore);

void semaphore_unlock(semaphore_t* semaphore);

void semaphore_delete(semaphore_t* semaphore);
*/

#endif
