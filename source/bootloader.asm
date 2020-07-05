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

global stack_bottom
global stack_top
global gdt
start:
  cli
  mov esp, stack_top
  push ebx
  push eax

  jmp load_gdt

  gdt:
  ;global descriptor table (kernel)
  gdt_kernel:

  gdt_null:
  dq 0

  gdt_kernel_code:
  ;Kernel Space code (Offset: 0x8 bytes)
	dw 0FFFFh 			;limit low
	dw 0 				    ;base low
	db 0 				    ;base middle
	db 10011010b		;access - Notice that bits 5 and 6 (privilege level) are 0 for Ring 0
	db 11001111b		;granularity
	db 0 		        ;base high

  gdt_kernel_data:
  ;Kernel Space data (Offset: 16 (0x10) bytes
	dw 0FFFFh 			;limit low
	dw 0 				    ;base low
	db 0 				    ;base middle
	db 10010010b 		;access - Notice that bits 5 and 6 (privilege level) are 0 for Ring 0
	db 11001111b 		;granularity
	db 0				    ;base high
  gdt_kernel_end:

  gdt_userspace:
  gdt_userspace_code:
  ;User Space code (Offset: 24 (0x18) bytes)
	dw 0FFFFh 			;limit low
	dw 0 				    ;base low
	db 0 				    ;base middle
	db 11111010b 	  ;access - Notice that bits 5 and 6 (privilege level) are 11b for Ring 3
	db 11001111b 		;granularity
	db 0 				    ;base high
 gdt_userspace_data:
  ;User Space data (Offset: 32 (0x20) bytes
	dw 0FFFFh 			;limit low
	dw 0 				    ;base low
	db 0 				    ;base middle
	db 11110010b 		;access - Notice that bits 5 and 6 (privilege level) are 11b for Ring 3
	db 11001111b 		;granularity
	db 0				    ;base high
  gdt_userspace_end:
  gdt_end:

  gdt_kernel_desc:
  dw gdt_kernel_end - gdt_kernel - 1
  dd gdt_kernel

  load_gdt:
  lgdt [gdt_kernel_desc]  ;load GDT
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:.setcs
  .setcs:
  
  call kmain
  .hltloop:
  hlt
  jmp .hltloop

section .bss
stack_bottom:
resb 8192
stack_top: