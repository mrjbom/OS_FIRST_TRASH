.text
.global vm_load_page_directory
vm_load_page_directory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

.text
.global vm_enable_paging
vm_enable_paging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret

.text
.global vm_tlb_flush
vm_tlb_flush:
mov %cr3, %eax
mov %eax, %cr3
ret
