bits 32

%define MULTIBOOT_MAGIC 0x1BADB002
%define MULTIBOOT_FLAGS (1<<0 | 1<<1 | 1<<2)

section .text
align 4
multiboot_header:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 800 ; width
    dd 600 ; height
    dd 32 ; bbp

;global descriptor table
  gdt:
 
  gdt_null:
  dq 0
 
  gdt_code:
  dw 0FFFFh
  dw 0
 
  db 0
  db 10011010b
  db 11001111b
  db 0
 
  gdt_data:
  dw 0FFFFh
  dw 0
 
  db 0
  db 10010010b
  db 11001111b
  db 0
 
  gdt_end:
 
  gdt_desc:
   dw gdt_end - gdt - 1
   dd gdt
 
  ;load gdt
  load_gdt:
    cli  ;disable interrupts
    lgdt [gdt_desc]  ;load GDT
    sti  ;enable interrupts

global start
extern kmain

start:
  cli
  mov esp, stack_space
  push ebx
  push eax
  jmp load_gdt
  call kmain
  hlt

section .bss
resb 8192
stack_space: