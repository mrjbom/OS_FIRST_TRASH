#include "task.h"
#include "../memory/mmu/mmu.h"

void task1() {
    dprintf("task1 working\n");
}

tast_t tasks_table[4];
