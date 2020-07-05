#include "scheduler.h"
#include "../lfbmemory/lfbmemory.h"

//200, 100, 600, 500
void task_colored_square(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
    for(uint32_t l = 0; l < 100; ++l) {
        uint32_t r, g, b;
        uint32_t hex;
        r = 255; g = 0; b = 0;
        /*
        255 0 0
            |
            V
        255 0 255
        */
        for(uint32_t i = b; i <= 255; ++i) {
            hex = lfb_rgb_to_hex(r, g, i);
            //for(uint32_t t = 0; t < 6000000; ++t) {};
            lfb_draw_rectangle(x0, y0, x1, y1, hex);
        }
        /*
        255 0 255
            |
            V
        0   0 255
        */
        for(uint32_t i = r; i <= 0; --i) {
            hex = lfb_rgb_to_hex(i, g, b);
            lfb_draw_rectangle(x0, y0, x1, y1, hex);
        }
        /*
        0   0  255
            |
            V
        0  255 255
        */
        for(uint32_t i = g; i <= 255; ++i) {
            hex = lfb_rgb_to_hex(r, i, b);
            lfb_draw_rectangle(x0, y0, x1, y1, hex);
        }
        /*
        0  255 255
            |
            V
        0  255 0
        */
        for(uint32_t i = b; i <= 0; --i) {
            hex = lfb_rgb_to_hex(r, g, i);
            lfb_draw_rectangle(x0, y0, x1, y1, hex);
        }
        /*
        0   255 0
            |
            V
        255 255 0
        */
        for(uint32_t i = r; i <= 255; ++i) {
            hex = lfb_rgb_to_hex(i, g, b);
            lfb_draw_rectangle(x0, y0, x1, y1, hex);
        }
        /*
        255 255 0
            |
            V
        255 0   0
        */
        for(uint32_t i = g; i <= 0; --i) {
            hex = lfb_rgb_to_hex(r, i, b);
            lfb_draw_rectangle(x0, y0, x1, y1, hex);
        }
    }
}

//scheduler

//next process identifier (PID)
uint32_t next_pid = 0;

//next thread identifier
uint32_t next_thread_id = 0;

//process list
list_t process_list;

//thread list
list_t thread_list;

//scheduler readiness flag
bool multi_task = false;

//descriptor of the kernel process
process_t* kernel_proc = 0;
//descriptor of the kernel thread
thread_t*  kernel_thread = 0;

//descriptor of the  current process
process_t* current_proc;
//descriptor of the  current thread
thread_t*  current_thread;

void task_manager_init()
{
    //read current stack pointer
    uint32_t esp = 0;
    __asm__ volatile ("mov %%esp, %0":"=a"(esp));       

    //disabling interrupts and initializing the task lists
    __asm__ volatile ("cli");

    list_init(&process_list);
    list_init(&thread_list);

    //creating a core process
    //allocate memory for the process descriptor and reset it to zero
    kernel_proc = (process_t*)pm_malloc(sizeof(process_t));

    memset(kernel_proc, 0, sizeof(process_t));

    //initializing the process
    kernel_proc->pid = next_pid++;
    kernel_proc->page_dir = kernel_page_directory_table;
    kernel_proc->list_item.list = NULL;
    kernel_proc->threads_count = 1;
    strcpy(kernel_proc->name, "Kernel");
    kernel_proc->suspend = false;

    list_add(&process_list, &kernel_proc->list_item);

    //creating the main core thread
    kernel_thread = (thread_t*)pm_malloc(sizeof(thread_t));

    memset(kernel_thread, 0, sizeof(thread_t));

    kernel_thread->process = kernel_proc;
    kernel_thread->list_item.list = NULL;
    kernel_thread->id = next_thread_id++;
    kernel_thread->stack_size = 0x2000;
    kernel_thread->suspend = false;
    kernel_thread->esp = esp;
   
    list_add(&thread_list, &kernel_thread->list_item);

    //making the core process and thread current
    current_proc = kernel_proc;
    current_thread = kernel_thread;
   
    //raising the scheduler readiness flag
    multi_task = true;
   
    //enabling interrupts again
    __asm__ volatile ("sti");
}

void switch_task()
{
    //if the scheduler is ready to work
    if (multi_task)
    {
       //push the flags to the stack and disable interrupts
       __asm__ volatile ("pushf; cli");

       //storing the stack pointer in the current task structure
       __asm__ volatile ("mov %%esp, %0":"=a"(current_thread->esp));

       //taking a new issue from the queue
       current_thread = (thread_t*)current_thread->list_item.next;
      
       //setting a new page directory (for processes)
       __asm__ volatile ("mov %0, %%cr3"::"a"(current_proc->page_dir));
       //switching to the new task stack
       __asm__ volatile ("mov %0, %%esp"::"a"(current_thread->esp));

       //pushing flags from the stack, thus implicitly allowing interrupts
       __asm__ volatile ("popf");
       __asm__ volatile ("sti");
    }
}

thread_t* thread_create(process_t* proc,   /* Родительский процесс */
                        void* entry_point, /* Точка входа в поток */
                        size_t stack_size, /* Размер стека потока */
                        bool kernel,       /* Поток ядра */
                        bool suspend)      /* Поток приостановлен */
{
    void* stack = NULL;
    uint32_t eflags;
    
    //getting the flag register
    __asm__ volatile ("pushf; pop %0":"=r"(eflags));
    //disabling interrupts
    __asm__ volatile ("cli");

    //creating a new stream descriptor
    thread_t* tmp_thread = (thread_t*)pm_malloc(sizeof(thread_t));

    //clearing the memory of the descriptor structure
    memset(tmp_thread, 0, sizeof(thread_t));

    //initializing a new thread
    tmp_thread->id = next_thread_id++;
    tmp_thread->list_item.list = NULL;
    tmp_thread->process = proc;
    tmp_thread->stack_size = stack_size;
    tmp_thread->suspend = suspend;/* */
    tmp_thread->entry_point = (uint32_t)entry_point;

    //creating a new thread stack
    stack = pm_malloc(stack_size);
   
    //saving the pointer to the stack memory
    tmp_thread->stack = stack;
    //setting the ESP value
    //in the flow structure - it must point to the value
    //the number of flags that we will put on the stack
    tmp_thread->esp = (uint32_t)stack + stack_size - 12;

    //adding a thread to the queue
    list_add(&thread_list, &tmp_thread->list_item);

    //increasing the number of threads in the parent process
    proc->threads_count++;

    //formed by the stack of tasks

    //creating a pointer to the memory allocated for the stack
    uint32_t* esp = (uint32_t*)(stack + stack_size);

    //put the stream entry point on the stack and
    //flag register value
    esp[-1] = (uint32_t)entry_point;
    //raising the IF flag
    esp[-3] = eflags | (1 << 9);

    //enabling interrupts
    __asm__ volatile ("sti");

    //возвращаем указатель на структуру потока
    return tmp_thread;
}

process_t* get_current_proc()
{
    return current_proc;
}
