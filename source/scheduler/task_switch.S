.section .bss

//tmp 4 byte value
tmp_var: .space 4, 0


.section .text

.extern current_thread
.extern next_thread

.extern saved_regs
.extern scheduler_thread_exit_current

.global scheduler_low_thread_switch


//eax = current_thread
//ebx = next_thread
//ecx = pointers
//edx = comparisons
scheduler_low_thread_switch:
    push %EBP
    pushf
    mov saved_regs, %ECX
    //save general-purpose registers into stack
    push 4(%ECX)  //push EDI
    push 8(%ECX)  //push ESI
    push 20(%ECX) //push EBX
    push 24(%ECX) //push EDX
    push 32(%ECX) //push EAX
    mov 28(%ECX), %EAX //load saved ECX into EAX
    push %EAX //push ECX

    mov current_thread, %EAX
    mov next_thread, %EBX
    mov %ESP, 16(%EAX)  //save old task ESP
    mov 16(%EBX), %ESP  //load new task ESP


    jmp check_next_thread_kernel_or_userspace


//is this thread owned by a kernel process?
check_next_thread_kernel_or_userspace:
    mov 0(%EBX), %ECX //loading address with process info
    mov 272(%ECX), %DH //loading kernel flag
    mov $1, %DL      //if 1 - kernel thread, if 0 - userspace thread
    cmp %DH, %DL
    je kernel_check_next_thread_init_or_continue
    jne userspace_check_next_thread_init_or_continue


kernel_check_next_thread_init_or_continue:
    mov 32(%EBX), %DH //loading is_inited flag
    mov $0, %DL //if 1 - need continue, if 0 - need init
    cmp %DH, %DL
    je kernel_next_thread_init
    jne kernel_next_thread_continue

kernel_next_thread_init:
    mov 20(%EBX), %ECX //save entry_point
    mov %ECX, tmp_var
    //load saved general-purpose registers from stack
    pop %ECX
    pop %EAX
    pop %EDX
    pop %EBX
    pop %ESI
    pop %EDI
    popf
    pop %EBP
    call *tmp_var
    call scheduler_thread_show_list //test c func call
    call scheduler_thread_show_list //test c func call
    call scheduler_thread_show_list //test c func call
    call scheduler_thread_exit_current


kernel_next_thread_continue:
    pop %ECX
    pop %EAX
    pop %EDX
    pop %EBX
    pop %ESI
    pop %EDI
    popf
    pop %EBP
    ret

userspace_check_next_thread_init_or_continue:
