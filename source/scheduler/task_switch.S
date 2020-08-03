.section .text
//DONT USE
.global switch_task_k
switch_task_k:
    pusha
    pushf
    mov %cr3, %eax #Push CR3
    push %eax
    mov 44(%esp), %eax #The first argument, where to save
    mov %ebx, 4(%eax)
    mov %ecx, 8(%eax)
    mov %edx, 12(%eax)
    mov %esi, 16(%eax)
    mov %edi, 20(%eax)
    mov 36(%esp), %ebx #EAX
    mov 40(%esp), %ecx #IP
    mov 20(%esp), %edx #ESP
    add $4, %edx #Remove the return value ;)
    mov 16(%esp), %esi #EBP
    mov 4(%esp), %edi #EFLAGS
    mov %ebx, (%eax)
    mov %edx, 24(%eax)
    mov %esi, 28(%eax)
    mov %ecx, 32(%eax)
    mov %edi, 36(%eax)
    pop %ebx #CR3
    mov %ebx, 40(%eax)
    push %ebx #Goodbye again ;)
    mov 48(%esp), %eax #Now it is the new object
    mov 4(%eax), %ebx #EBX
    mov 8(%eax), %ecx #ECX
    mov 12(%eax), %edx #EDX
    mov 16(%eax), %esi #ESI
    mov 20(%eax), %edi #EDI
    mov 28(%eax), %ebp #EBP
    push %eax
    mov 36(%eax), %eax #EFLAGS
    push %eax
    popf
    pop %eax
    mov 24(%eax), %esp #ESP
    push %eax
    mov 40(%eax), %eax #CR3
    mov %eax, %cr3
    pop %eax
    push %eax
    mov 32(%eax), %eax #EIP
    xchg (%esp), %eax #We do not have any more registers to use as tmp storage
    mov (%eax), %eax #EAX
    ret #This ends all!


.extern        current_thread   //external links to the current thread
.extern        next_thread      //external links to the next thread
//.extern        tss            //and the task status segment
.global        task_switch      //making the function global and accessible from the outside
.global        thread_init
.global        thread_continue
.extern        scheduler_thread_exit_current

task_switch:
    push    %ebp    //prologue of a C compatible function on call
    pushf           //pushing flags into the stack
    cli             //disabling interrupts

    //saving the stack pointer for the current thread
    mov    current_thread, %edx  //loading EDX with the address of the current task structure
    mov    %esp, 16(%edx)        //writing the current ESP to the task structure.

    //taking a new thread from the queue
    mov    next_thread, %ecx        //load the ECX address of the structure of the next task
    //mov    %ecx, current_thread //modifying the pointer to the current thread

    //switching the stack
    //mov    current_thread, %edx //loading EDX with a pointer to a new task
    mov    %ecx, current_thread //load next_thread to current_thread
    mov    16(%ecx), %esp       //loading the new task stack pointer to ESP

    //modifying the top of the core stack in TSS
    //mov    28(%edx), %eax      //read the top of the stack of flow structure
    //mov    $tss, %edx        //we ship EDX with the TSS address
    //mov    %eax, 4(%edx)     //writing the stack vertex in the tss.esp0 field

    push %eax
    mov $0, %eax
    cmp 32(%ecx), %eax
    pop %eax
    je thread_init
endOfInitCheck:
    push   %ax     //save ax
    mov    $0x20, %ax
    outb   %al, $0x20    //send EOI
    pop    %ax
    popf           //returning flags from the stack, implicitly enable interrupts
    pop    %ebp    //epilogue of a C compatible function on call
    ret

thread_init:
    push %eax
    mov $1, %eax
    mov %eax, 32(%ecx)
    pop %eax

    //send EOI
    push   %ax     //save ax
    mov    $0x20, %ax
    outb   %al, $0x20    //send EOI
    pop    %ax
    sti

    call *20(%ecx)
    jmp endOfProg

endOfProg:
    call scheduler_thread_exit_current
