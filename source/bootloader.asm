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

global start
extern kmain

global kernel_stack_bottom
global kernel_stack_top

start:
  cli
  mov esp, kernel_stack_top
  push ebx
  push eax
  
  call kmain
  .hltloop:
  hlt
  jmp .hltloop

section .bss
kernel_stack_bottom:
resb 8192
kernel_stack_top: