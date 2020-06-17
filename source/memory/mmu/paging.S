.text
.global load_page_directory
load_page_directory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

.text
.global enable_paging
enable_paging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret

.text
.global tlb_flush
tlb_flush:
mov %cr3, %eax
mov %eax, %cr3
ret
