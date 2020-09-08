/*---------------------------------------------------------------------
/
/        Switching to the 3rd ring of protection
/
/--------------------------------------------------------------------*/
.set        USER_CS,        0x1B    /* Application code selector */ 
.set        USER_SS,        0x23    /* Application stack selector */
.set        USER_DS,        0x23    /* The selector application data */

.global user_mode_switch

user_mode_switch:
    mov     4(%esp), %edx           /* We put the address of the entry point in EDX  */
   
    /* Configuring custom data segments by uploading
         they have the appropriate selectors */
    mov     $USER_DS, %ax
    mov     %ax, %ds
    mov     %ax, %es
   
    /* Preparing the current stack for loading EIP, CS, EFLAGS, ESP, and SS */   
    mov     8(%esp), %eax   /* Read the pointer to the new stack from the parameter in EAX */
    pushl   $USER_SS        /* Push the application stack selector into the stack */
    pushl   %eax            /* Push to the stack pointer to the application stack */
    pushf                   /* Pushing flags into the stack  */
    push    $USER_CS        /* Pushing the application code selector into the stack */
    push    %edx            /* Pushing the entry point to the application code */
                               
    iret                    /* Performing a return from the interrupt in ring 3! */
