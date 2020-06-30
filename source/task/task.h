#ifndef __TASK_H__
#define __TASK_H__

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"

typedef struct registers {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} registers_t;
 
typedef struct task {
    uint8_t pid;
    registers_t regs;
} tast_t;

extern tast_t tasks_table[4];

#endif /* __TASK_H__ */