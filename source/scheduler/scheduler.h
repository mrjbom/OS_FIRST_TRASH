#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"
#include "../lib/list.h"
#include "../memory/mmu/mmu.h"

extern void task_colored_square(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

extern bool multi_task;

//scheduler

typedef struct
{
    list_item_t list_item;     //list item
    uint32_t*   page_dir;      //page catalog
    size_t      threads_count; //number of threads in this process
    bool        suspend;       //pause flag
    uint32_t    pid;           //the process ID (PID)
    char        name[256];     //process name
} process_t;

typedef struct
{                                                //field offsets from the beginning of the structure, bytes
    list_item_t list_item;   //list item         //+0, to field "next": +4
    process_t*  process;     //parent process    //+12
    bool        suspend;     //pause flag        //+16
    size_t      stack_size;  //thread stack size //+20
    void*       stack;       //pointer to the memory block under the stack //+24
    uint32_t    esp;         //stack pointer //+28
    uint32_t    entry_point; //point of entry to the thread //+32
    uint32_t    id;          //thread id //+36
    uint32_t    stack_top;   //stack top //+40
} thread_t;

extern void task_manager_init();

extern thread_t* thread_create(process_t* proc,   //child process
                               void* entry_point, //point of entry to the stream
                               size_t stack_size, //thread stack size
                               bool kernel,       //kernel thread
                               bool suspend);     //the thread is paused

extern process_t* get_current_proc();

//asm function
extern void task_switch();

#endif