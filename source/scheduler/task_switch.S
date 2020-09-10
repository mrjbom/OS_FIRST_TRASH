.section .text

.extern        current_thread                 //external link to the current thread
.extern        next_thread                    //external link to the next thread
.extern        tss                            //and the task status segment
.extern        scheduler_thread_exit_current  //external link to the exit from kernel thread
.global        scheduler_low_thread_switch    //making the function global and accessible from the outside

.macro SEND_EOI
    mov $0x20, %al //load EOI command
    outb %al, $0x20 //send EOI command
.endm

scheduler_low_thread_switch:
    push %ebp   //prologue of a C compatible function on call
    pushf       //save EFLAGS to stack
    pusha       //save general-purpose registers to the stack
    cli

    //saving the stack pointer for the current thread
    mov current_thread, %eax    //loading EDX with the address of the current task structure
    mov %esp, 16(%eax)          //writing the current ESP to the task structure.

    mov next_thread, %ebx       //loading EDX with the address of the next task structure

    //is this a kernel thread?
    //if 1 - kernel thread
    mov 0(%ebx), %ecx           //loading a structure with information about the process
                                //that the process belongs to in ECX
    
    mov 272(%ecx), %cl         //loading the "kernel" flag in CL
                                //if 1 - kernel thread
                                //if 0 - userspace thread
    mov $1, %dl                //we will compare the flag with the value true
    cmp %cl, %dl
    je check_kernel_init_or_continue
    jne run_userspace_thread

check_kernel_init_or_continue:
    mov 32(%ebx), %cl            //loading the "is_inited" flag in CL
                                  //if 0 - need to initialize execution(so that return works normally)
                                  //if 1 - continue execution
    mov $0, %dl                  //we will compare the flag with the value false
    cmp %cl, %dl
    je init_kernel_thread
    jne continue_kernel_thread

init_kernel_thread:
    //set "is_inited" flag, now flag = true
    movl $1, 32(%ebx)
    SEND_EOI
    pop %ebp                  //epilogue of a C compatible function on call
    popf
    popa
    sti
    //mov 32(%ebx), %esp        //load new stack
    call *20(%ebx)
    jmp end_kernel_thread

continue_kernel_thread:
    SEND_EOI
    pop %ebp                  //epilogue of a C compatible function on call
    popf
    popa
    sti
    mov 32(%ebx), %esp        //load new stack
    ret                       //go to new task

run_userspace_thread:
    nop
    nop
    nop
    

end_kernel_thread:
    call scheduler_thread_exit_current
    ret
