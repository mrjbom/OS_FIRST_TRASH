#include "scheduler.h"
#include "../lfbmemory/lfbmemory.h"

/*
Part of the code is borrowed from the @maisvendoo blog, http://phantomexos.blogspot.ru
Thanks.
*/

//200, 100, 600, 500
void task_colored_square(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
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

//scheduler

//next process identifier (PID)
uint32_t next_pid = 0;

//next thread identifier
uint32_t next_thread_id = 0;

//process list
GList* process_list = NULL;

//thread list
GList* thread_list = NULL;

//scheduler readiness flag
bool multi_task = false;

//descriptor of the kernel process
process_t* kernel_proc = NULL;
//descriptor of the kernel thread
thread_t*  kernel_thread = NULL;

//descriptor of the current process
process_t* current_proc = NULL;
//descriptor of the current thread
thread_t*  current_thread = NULL;
//descriptor of the next thread
thread_t*  next_thread = NULL;

void show_test_word() {
    serial_printf("Test word!");
}

void scheduler_init() {
    //read current stack pointer
    uint32_t esp = 0;
    __asm__ volatile ("mov %%esp, %0":"=a"(esp));       

    //disabling interrupts and initializing the task lists
    __asm__ volatile ("cli");

    process_list = NULL;
    thread_list = NULL;

    //creating a core process
    //allocate memory for the process descriptor and reset it to zero
    kernel_proc = (process_t*)pm_malloc(sizeof(process_t));

    memset(kernel_proc, 0, sizeof(process_t));

    //initializing the process
    kernel_proc->pid = next_pid++;
    kernel_proc->page_dir = kernel_page_directory_table;
    kernel_proc->threads_count = 1;
    strcpy(kernel_proc->name, "Kernel");
    kernel_proc->suspend = false;
    kernel_proc->kernel = true;

    process_list = g_list_append(process_list, kernel_proc);

    //creating the main core thread
    kernel_thread = (thread_t*)pm_malloc(sizeof(thread_t));

    memset(kernel_thread, 0, sizeof(thread_t));

    kernel_thread->process = kernel_proc;
    kernel_thread->id = next_thread_id++;
    kernel_thread->stack_size = 0x2000;
    kernel_thread->suspend = false;
    kernel_thread->esp = esp;
    kernel_thread->is_inited = true;
    kernel_thread->stack_top = (uint32_t)kernel_stack_top;
   
    thread_list = g_list_append(thread_list, kernel_thread);

    //making the core process and thread current
    current_proc = kernel_proc;
    current_thread = kernel_thread;
   
    //raising the scheduler readiness flag
    multi_task = true;
   
    //enabling interrupts again
    __asm__ volatile ("sti");
}

process_t* scheduler_proc_create(bool kernel,
                                 const char name[256])
{
    process_t* new_proc = (process_t*)pm_malloc(sizeof(process_t));
    if(!new_proc) {
        serial_printf("scheduler_proc_create() failed!\n");
        return NULL;
    }

    memset(new_proc, 0, sizeof(process_t));

    //initializing the process
    new_proc->pid = next_pid++;
    new_proc->page_dir = NULL;
    new_proc->threads_count = 0;
    strcpy(new_proc->name, name);
    new_proc->suspend = false;
    new_proc->kernel = false;

    process_list = g_list_append(process_list, new_proc);
    return new_proc;
}

thread_t* scheduler_thread_create(process_t* proc,   //child process
                        void* entry_point, //point of entry to the stream
                        size_t stack_size, //thread stack size
                        bool suspend)      //the thread is paused
{
    void* stack = NULL;
    uint32_t eflags;
    
    //getting the flag register
    __asm__ volatile ("pushf; pop %0":"=r"(eflags));
    //disabling interrupts
    __asm__ volatile ("cli");

    //creating a new thread descriptor
    thread_t* tmp_thread = (thread_t*)pm_malloc(sizeof(thread_t));
    if(!tmp_thread) {
        serial_printf("thread_create error!\n");
        return NULL;
    }

    //clearing the memory of the descriptor structure
    memset(tmp_thread, 0, sizeof(thread_t));

    //initializing a new thread
    tmp_thread->id = next_thread_id++;
    tmp_thread->process = proc;
    tmp_thread->stack_size = stack_size;
    tmp_thread->suspend = suspend;
    tmp_thread->entry_point = (uint32_t)entry_point;

    //creating a new thread stack
    stack = pm_malloc(stack_size);
    if(!stack) {
        serial_printf("thread_create error!\n");
        return NULL;
    }

    tmp_thread->stack_top = (uint32_t)stack + stack_size;

    for(uint32_t i = 0; i < stack_size / 4; ++i) {
        //fill stack
        ((uint32_t*)stack)[i] = (0xFFFFFFFF - i);
    }
   
    //saving the pointer to the stack memory
    tmp_thread->stack = stack;
    //setting the ESP value
    //in the flow structure - it must point to the value
    //the number of flags that we will put on the stack
    tmp_thread->esp = (uint32_t)stack + stack_size - 12;
    //why 12? look down
    //esp[-1] = (uint32_t)entry_point; and esp[-3] = eflags | (1 << 9);

    //adding a thread to the queue
    thread_list = g_list_append(thread_list, tmp_thread);

    //increasing the number of threads in the parent process
    proc->threads_count++;

    //formed by the stack of tasks

    //creating a pointer to the memory allocated for the stack
    uint32_t* esp = (uint32_t*)(stack + stack_size);

    //put the thread entry point on the stack and
    //flag register value
    //esp[-1] = (uint32_t)entry_point;
    //raising the IF flag
    esp[-3] = eflags | (1 << 9);

    //enabling interrupts
    __asm__ volatile ("sti");

    //return thread pointer
    return tmp_thread;
}

void scheduler_thread_exit_current() {
    /* Отключаем прерывания */
    __asm__ volatile ("cli");

    //serial_printf("scheduler_thread_exit_current\n"
    //              "current_thread id = %u\n", current_thread->id);

    /* Удаляем поток из очереди */
    thread_list = g_list_remove(thread_list, current_thread);
    /* Уменьшаем число потоков в процессе, которому он принадлежит */
    current_thread->process->threads_count--;

    /* Освобождаем память, занятую потоком под стек и собственный описатель */
    pm_free(current_thread->stack);
    pm_free(current_thread);

    /* Грузим в ECX адрес планировщика */
    __asm__ volatile ("mov %0, %%ecx"::"a"(&scheduler_switch));

    /* Включаем прерывания */
    __asm__ volatile ("sti");

    /* Вызываем планировщик */
    __asm__ volatile ("call *%ecx");
}

void scheduler_thread_delete(thread_t* thread) {
    /* Отключаем прерывания */
    __asm__ volatile ("cli");

    //serial_printf("scheduler_thread_exit\n"
    //              "current_thread id = %u\n", current_thread->id);

    /* Удаляем поток из очереди */
    GList* list_copy = thread_list;
    scheduler_thread_show_list();
    thread_list = g_list_remove(thread_list, thread);
    if(list_copy == thread_list) {
        serial_printf("scheduler_thread_delete error!\n");
        serial_printf("thread not found in thread_list\n");
        return;
    }
    /* Уменьшаем число потоков в процессе, которому он принадлежит */
    thread->process->threads_count--;

    /* Освобождаем память, занятую потоком под стек и собственный описатель */
    pm_free(thread->stack);
    pm_free(thread);

    __asm__ volatile ("sti");
}

process_t* scheduler_get_current_proc() {
    return current_proc;
}

thread_t* scheduler_get_current_thread() {
    return current_thread;
}

//sheduler logic
uint32_t counter = 0;

void scheduler_switch() {
    __asm__ volatile ("cli");
    //если других задач нету
    if(!multi_task)
        return;
    if((counter + 1) < g_list_length(thread_list)) {
        ++counter;
    } else {
        counter = 0;
    }
    next_thread = (thread_t*)(g_list_nth_data(thread_list, counter));
    scheduler_thread_show_list();
    serial_printf("\nbefore switch\n");
    serial_printf("current_thread->id = %u, kernel = %u\n", current_thread->id, current_thread->process->kernel);
    serial_printf("next_thread->id = %u, kernel = %u\n", next_thread->id, next_thread->process->kernel);
    if(current_thread->process->kernel == 1 && next_thread->process->kernel == 0 && next_thread->is_inited == 0) {
        serial_printf("going to init user thread\n");
    }
    if(current_thread->process->kernel == 1 && next_thread->process->kernel == 0 && next_thread->is_inited == 1) {
        serial_printf("going to continue user thread\n");
    }
    
    scheduler_low_thread_switch();
    __asm__ volatile ("sti");
}

void scheduler_thread_show_list() {
    serial_printf("\nTHREAD LIST:\n");
    size_t threads_count = g_list_length(thread_list);
	for (uint32_t i = 0; i < threads_count; ++i) {
		serial_printf("thread_list[%u] addr = 0x%x, id = %u\n", i, ((thread_t*)g_list_nth_data(thread_list, i)), ((thread_t*)g_list_nth_data(thread_list, i))->id);
	}
    serial_printf("\n");
}
