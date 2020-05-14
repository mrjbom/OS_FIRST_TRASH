#include "task.h"
#include "../memory/memmmu/memmmu.h"
 
static Task *runningTask;
static Task mainTask;
static Task otherTask;
 
void otherMain() {
    dprintf("Multitasking work!\n");
    yield();
}
 
void init_tasking() {
    // Get EFLAGS and CR3
    __asm__ volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    __asm__ volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
 
    create_task(&otherTask, otherMain, mainTask.regs.eflags, (uint32_t*)mainTask.regs.cr3);
    mainTask.next = &otherTask;
    otherTask.next = &mainTask;
 
    runningTask = &mainTask;
}
 
void create_task(Task *task, void (*kmain)(), uint32_t flags, uint32_t *pagedir) {
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = flags;
    task->regs.eip = (uint32_t)kmain;
    task->regs.cr3 = (uint32_t)pagedir;
    task->regs.esp = (uint32_t)search_npages(1) + 0x1000; // Not implemented here
    task->next = 0;
}
 
void yield() {
    Task *last = runningTask;
    runningTask = runningTask->next;
    switch_task(&last->regs, &runningTask->regs);
}