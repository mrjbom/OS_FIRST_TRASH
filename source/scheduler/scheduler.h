#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"
#include "../lib/list.h"
#include "../memory/mmu/mmu.h"

void task_colored_square(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

//scheduler

typedef struct
{
    list_item_t list_item;     //list item
    uint32_t*   page_dir;      //page catalog
    size_t      threads_count; //number of threads in this process
    bool        suspend;       //pause flag
    uint32_t    pid;           //the process ID (PID)
    char        name[256];     //process name
} __attribute__((packed)) process_t;

typedef struct
{
    list_item_t list_item;   //list item
    process_t*  process;     //parent process
    bool        suspend;     //pause flag
    size_t      stack_size;  //thread stack size
    void*       stack;       //pointer to the memory block under the stack
    uint32_t    esp;         //stack pointer
    uint32_t    entry_point; //point of entry to the thread
    uint32_t    id;          //thread id

} __attribute__((packed)) thread_t;

extern void task_manager_init();

extern thread_t* thread_create(process_t* proc,   /* Родительский процесс */
                        void* entry_point, /* Точка входа в поток */
                        size_t stack_size, /* Размер стека потока */
                        bool kernel,       /* Поток ядра */
                        bool suspend);      /* Поток приостановлен */

extern process_t* get_current_proc();

extern void switch_task();

#endif