.section .text

.extern        current_thread                 //external links to the current thread
.extern        next_thread                    //external links to the next thread
.extern        tss                            //and the task status segment
.global        scheduler_low_thread_switch    //making the function global and accessible from the outside
.extern        user_mode_switch
.extern        scheduler_thread_exit_current

.macro SEND_EOI_POP_EBP

    //send EOI
    push   %ax     //save ax
    mov    $0x20, %ax
    outb   %al, $0x20    //send EOI
    pop    %ax
    popf           //returning flags from the stack, implicitly enable interrupts
    pop    %ebp    //epilogue of a C compatible function on call
    sti

.endm

scheduler_low_thread_switch:
    push    %ebp    //prologue of a C compatible function on call
    pushf           //pushing flags into the stack
    cli             //disabling interrupts

    //saving the stack pointer for the current thread
    mov    current_thread, %edx  //loading EDX with the address of the current task structure
    mov    %esp, 16(%edx)        //writing the current ESP to the task structure.

    //taking a new thread from the queue
    mov    next_thread, %ecx        //load the ECX address of the structure of the next task

    //switching the stack
    mov    %ecx, current_thread //load next_thread to current_thread
    mov    16(%ecx), %esp       //loading the new task stack pointer to ESP

    //modifying the top of the core stack in TSS
    mov    36(%edx), %eax      //read the top of the stack of flow structure
    mov    $tss, %edx        //we ship EDX with the TSS address
    mov    %eax, 4(%edx)     //writing the stack vertex in the tss.esp0 field

    //check is_inited
    push %eax
    mov $0, %eax
    cmp 32(%ecx), %eax
    pop %eax
    je thread_init
    jmp thread_continue

thread_init:
    //set is inited - true
    push %eax
    mov $1, %eax
    mov %eax, 32(%ecx)
    pop %eax

    //check kernel or userspace
    //if 0 - userspace
    push %eax
    push %ebx
    //parent proccess info to eax
    mov 0(%ecx), %eax
    //kernel or userspace flag to eax
    mov 272(%eax), %eax
    mov $0, %ebx
    cmp %eax, %ebx
    pop %ebx
    pop %eax
    je thread_userspace_init

    //kernel thread init
    SEND_EOI_POP_EBP
    call *20(%ecx)
    jmp end_thread

thread_continue:
    //if inited
    //check kernel or userspace
    //if 0 - userspace
    push %eax
    push %ebx
    //parent proccess info to eax
    mov 0(%ecx), %eax
    //kernel or userspace flag to eax
    mov 272(%eax), %eax
    mov $0, %ebx
    cmp %eax, %ebx
    pop %ebx
    pop %eax
    je thread_userspace_continue
    SEND_EOI_POP_EBP
    ret

thread_userspace_init:
    //push entry_point into parameters
    SEND_EOI_POP_EBP
    push 20(%ecx)
    call user_mode_switch

thread_userspace_continue:
    //push esp to params
    SEND_EOI_POP_EBP
    push 16(%ecx)
    call user_mode_switch

end_thread:
    call scheduler_thread_exit_current
    ret
