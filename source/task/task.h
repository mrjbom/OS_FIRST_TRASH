#ifndef __TASK_H__
#define __TASK_H__
 
#include "../lib/stdint.h"
#include "../memory/memmmu/memmmu.h"
 
typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;
 
typedef struct Task {
    Registers regs;
    struct Task *next;
} Task;
 
extern void init_tasking();
extern void create_task(Task*, void(*)(), uint32_t, uint32_t*);
 
extern void yield(); // Switch task frontend
extern void switch_task(Registers *old, Registers *new); // The function which actually switches
 
#endif /* __TASK_H__ */