.section .text

.global gdt_flush
gdt_flush:
/* Загрузка GDTR указателем полученным из стека */
/* переданным в качестве параметра */
    mov    4(%esp), %eax /* берем параметр из стека */
    lgdt    (%eax)       /* загружаем регистр GDTR */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $0x08, $flush
flush:
    ret

.global idt_flush
idt_flush:
/* Загрузка IDTR указателем полученным из стека */
/* переданным в качестве параметра */
    mov    4(%esp), %eax /* берем параметр из стека */
    lidt    (%eax)       /* загружаем регистр IDTR */
    ret

.global tss_flush
tss_flush:
    mov     $0x2B, %eax
    ltr     %ax
    ret
