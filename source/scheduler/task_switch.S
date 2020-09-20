.extern        current_thread   //external links to the current thread
.extern        next_thread      //external links to the next thread
.extern        tss            //and the task status segment
.global        scheduler_low_thread_switch      //making the function global and accessible from the outside
.global        thread_init
.global        thread_continue
.extern        scheduler_thread_exit_current

.set        USER_CS,        0x1B    //Application code selector
.set        USER_SS,        0x23    //Application stack selector
.set        USER_DS,        0x23    //The selector application data

.macro SEND_EOI
    //send EOI
    push   %ax     //save ax
    mov    $0x20, %ax
    outb   %al, $0x20    //send EOI
    pop    %ax
.endm

.macro CURRENT_IS_KERNEL_CHECK
    //is this a kernel thread?
    //if 1 - kernel thread
    mov 0(%eax), %ecx           //loading a structure with information about the process
                                //that the process belongs to in ECX
    
    mov 272(%ecx), %dh         //loading the "kernel" flag in CL
                                //if 1 - kernel thread
                                //if 0 - userspace thread
    mov $1, %dl                //we will compare the flag with the value true
    cmp %dh, %dl
.endm

.macro NEXT_IS_KERNEL_CHECK
    //is this a kernel thread?
    //if 1 - kernel thread
    mov 0(%ebx), %ecx           //loading a structure with information about the process
                                //that the process belongs to in ECX
    
    mov 272(%ecx), %dh         //loading the "kernel" flag in CL
                                //if 1 - kernel thread
                                //if 0 - userspace thread
    mov $1, %dl                //we will compare the flag with the value true
    cmp %dh, %dl
.endm

scheduler_low_thread_switch:
    push    %ebp    //prologue of a C compatible function on call
    pushf           //pushing flags into the stack
    cli             //disabling interrupts

    //saving the stack pointer for the current thread
    mov    current_thread, %eax  //loading EDX with the address of the current task structure
    
    mov    %esp, 16(%eax)        //writing the current ESP to the task structure.
    CURRENT_IS_KERNEL_CHECK
    je     skip_
    mov    useresp_copy_before_int, %ecx
    mov    %ecx, 16(%eax)
    skip_:


    //taking a new thread from the queue
    mov    next_thread, %ebx        //load the ECX address of the structure of the next task

    //switching the stack
    mov    %ebx, current_thread //load next_thread to current_thread
    mov    16(%ebx), %esp       //loading the new task stack pointer to ESP

    //is this a kernel thread?
    NEXT_IS_KERNEL_CHECK
    je check_kernel_init_or_continue
    jne check_userspace_init_or_continue

check_kernel_init_or_continue:
    mov 32(%ebx), %dh            //loading the "is_inited" flag in CL
                                  //if 0 - need to initialize execution(so that return works normally)
                                  //if 1 - continue execution
    mov $0, %dl                  //we will compare the flag with the value false
    cmp %dh, %dl
    je kernel_thread_init
    jne kernel_thread_continue

check_userspace_init_or_continue:
    mov 32(%ebx), %dh            //loading the "is_inited" flag in CL
                                  //if 0 - need to initialize execution(so that return works normally)
                                  //if 1 - continue execution
    mov $0, %dl                  //we will compare the flag with the value false
    cmp %dh, %dl
    je userspace_thread_init
    jne userspace_thread_continue

kernel_thread_init:
    push %ecx
    mov $1, %ecx
    mov %ecx, 32(%ebx)
    pop %ecx


    SEND_EOI
    popf
    sti
    pop    %ebp    //epilogue of a C compatible function on call
    call *20(%ebx)
    call scheduler_thread_exit_current

kernel_thread_continue:
    SEND_EOI
    popf           //returning flags from the stack, implicitly enable interrupts
    sti
    pop    %ebp    //epilogue of a C compatible function on call
    ret

userspace_thread_init:
    push %ecx
    mov $1, %ecx
    mov %ecx, 32(%ebx)
    pop %ecx

    SEND_EOI
    popf
    sti
    pop %ebp
    
    /* Configuring custom data segments by uploading
         they have the appropriate selectors */
    mov     $USER_DS, %ax
    mov     %ax, %ds
    mov     %ax, %es
   
    /* Preparing the current stack for loading EIP, CS, EFLAGS, ESP, and SS */   
    pushl   $USER_SS        /* Push the application stack selector into the stack */
    push    16(%ebx)        /* Push to the stack pointer to the application stack */
    pushf                   /* Pushing flags into the stack  */
    push    $USER_CS        /* Pushing the application code selector into the stack */
    push    20(%ebx)        /* Pushing the entry point to the application code */
                               
    iret                    /* Performing a return from the interrupt in ring 3! */

userspace_thread_continue:
    SEND_EOI
    popf
    sti
    pop %ebp

    /* Configuring custom data segments by uploading
         they have the appropriate selectors */ 
    mov     $USER_DS, %ax
    mov     %ax, %ds
    mov     %ax, %es
   
    /* Preparing the current stack for loading EIP, CS, EFLAGS, ESP, and SS */   
    pushl   $USER_SS        /* Push the application stack selector into the stack */
    push    16(%ebx)        /* Push to the stack pointer to the application stack */
    pushf                   /* Pushing flags into the stack  */
    push    $USER_CS        /* Pushing the application code selector into the stack */
    push    16(%ebx)        /* Pushing the entry point to the application code */

    iret                    /* Performing a return from the interrupt in ring 3! */
