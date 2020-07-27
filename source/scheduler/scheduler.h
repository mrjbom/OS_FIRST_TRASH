#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/*
Part of the code is borrowed from the @maisvendoo blog, http://phantomexos.blogspot.ru
Thanks.
*/

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"
#include "../lib/glist.h"
#include "../memory/mmu/mmu.h"

extern void task_colored_square(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);

extern bool multi_task;

//scheduler

typedef struct
{
    uint32_t*   page_dir;      //page catalog
    size_t      threads_count; //number of threads in this process
    bool        suspend;       //pause flag
    uint32_t    pid;           //the process ID (PID)
    char        name[256];     //process name
} process_t;

typedef struct
{                                                //field offsets from the beginning of the structure, bytes
    process_t*  process;     //parent process    //+0
    bool        suspend;     //pause flag        //+4
    size_t      stack_size;  //thread stack size //+8
    void*       stack;       //pointer to the memory block under the stack //+12
    uint32_t    esp;         //stack pointer //+16
    uint32_t    entry_point; //point of entry to the thread //+20
    uint32_t    id;          //thread id //+24
    uint32_t    stack_top;   //stack top //+28
} thread_t;

extern void scheduler_init();

extern thread_t* scheduler_thread_create(process_t* proc,   //child process
                                         void* entry_point, //point of entry to the stream
                                         size_t stack_size, //thread stack size
                                         bool kernel,       //kernel thread
                                         bool suspend);     //the thread is paused

//kill current_thread and delete from thread_list
//CALL ONLY FROM CURRENT THREAD
extern void scheduler_thread_exit_current();

//delete some thread from thread_list
extern void scheduler_thread_delete(thread_t* thread);

extern process_t* scheduler_get_current_proc();

extern thread_t* scheduler_get_current_thread();

extern void scheduler_switch();

//asm function
extern void task_switch();

extern void scheduler_thread_show_list();

#endif