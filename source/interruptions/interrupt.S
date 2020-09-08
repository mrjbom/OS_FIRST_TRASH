.section .text

/*-------------------------------------------------
// Макрос для обработчика без возврата кода ошибки
//-----------------------------------------------*/
.macro ISR_NOERRCODE isr_num

.global    isr\isr_num

isr\isr_num:
    cli                     /* Запрет всех прерываний */
    push    $0              /* Проталкиваем 0 в стек */
                            /* этот ноль - фиктивный код ошибки */
    push    $\isr_num       /* Сохраняем в стек номер прерывания */
    jmp    isr_common_stub  /* Передаем управление обработчику */
   
.endm
   
/*-----------------------------------------------------------
// Макрос для обработчика прерывания с возвратом кода ошибки
//---------------------------------------------------------*/
.macro ISR_ERRCODE isr_num

.global    isr\isr_num

isr\isr_num:
    cli                     /* Запрет всех прерываний */
    push    $\isr_num       /* Номер прерывания  - в стек */
    jmp    isr_common_stub  /* Переходим к обработчику */
   
.endm

/*-------------------------------------------------
//    Обработчики на первые 32 прерывания
//-----------------------------------------------*/
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14

// not used
ISR_ERRCODE 15

ISR_NOERRCODE 16
ISR_ERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20

// not used
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29

ISR_ERRCODE 30

// not used
ISR_NOERRCODE 31

ISR_NOERRCODE 32
ISR_NOERRCODE 33
ISR_NOERRCODE 34
ISR_NOERRCODE 35
ISR_NOERRCODE 36
ISR_NOERRCODE 37
ISR_NOERRCODE 38
ISR_NOERRCODE 39
ISR_NOERRCODE 40
ISR_NOERRCODE 41
ISR_NOERRCODE 42
ISR_NOERRCODE 43
ISR_NOERRCODE 44
ISR_NOERRCODE 45
ISR_NOERRCODE 46
ISR_NOERRCODE 47

/*-------------------------------------------------
// Common parts of the interrupt handler
//-----------------------------------------------*/
.extern isr_handler

isr_common_stub:
      pusha                /* Pushing all registers into the stack */
                           /* General purpose */
   
      mov    %ds, %ax      /* Saving the data segment selector in the stack */ 
      push   %eax          
     
      mov    $0x10, %ax    /* Loading the core data segment */
      mov    %ax, %ds
      mov    %ax, %es
      mov    %ax, %fs
      mov    %ax, %gs
     
      call   isr_handler   /* Calling handler */
     
      pop    %eax          /* Restoring the original data segment selector */
      mov    %ax, %ds      
      mov    %ax, %es
      mov    %ax, %fs
      mov    %ax, %gs
     
      popa                 /* Pushing General-purpose registers out of the stack */
      add    $8, %esp      /* Removing the error code from the stack */
                           /* and put the ISR number there */
      sti                  /* Allowing all interrupts again */
      iret                                /* Returning from the handler */
                           /* at the same time, they are pushed out of the stack */
                           /* values of the CS, EIP, and EFLAGS registers, */
                           /* SS end EIP */


/*------------------------------------------------------------------------------
//
//  Обработчик прерывания INT 50h (INT 80) для организации системного вызова
//
//----------------------------------------------------------------------------*/
.global isr80
isr80:

      push  $0      /* Помещаем в стек фиктивный код ошибки */
      push  $80     /* и номер прерывания (для обработчика на C) */
     
      /* Проталкиваем в стек все РОН  */
      pusha                 
     
      /* Сохраняем текущий селектор сегмента данных */        
      push  %ds
     
      /* Настраиваемся на сегмент данных ядра */
      mov   $0x10, %ax
      mov   %ax, %ds     

      /* Вызываем обработчик на C */
      call  isr_handler
     
      /* Восстанавливаем селектор сегмента данных */
      pop   %ds
          
      /* Выполняем действия, аналогичные команде popa */
      /* за исключением восстановления регистра EAX */
      /* для сохранения значения, возвращенного системным вызовом */
      pop   %edi
      pop   %esi
      pop   %ebp
      add   $4, %esp
      pop   %ebx
      pop   %edx
      pop   %ecx     
     
      add   $12, %esp   /* Очищаем стек */
     
      iret
