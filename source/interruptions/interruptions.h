#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../lib/string.h"

//GDT

//gdt data from bootloader.asm
extern char gdt[];

struct gdt_ptr_struct
{
	unsigned short limit;
	unsigned int base;
};
//struct gdt_ptr_struct* GDT_kernel = (struct gdt_ptr_struct*)gdt_kernel;

//IDT
struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};
struct IDT_entry IDT[256];

extern void invalid_opcode_handler(uint16_t cs, uint32_t eip);

extern void general_protection_fault_handler(uint32_t error_code);

extern void page_fault_handler(uint32_t error_code);

extern void idt_init(void);

extern void irq0_handler(void);

extern void irq1_handler(void);

extern void irq2_handler(void);

extern void irq3_handler(void);

extern void irq4_handler(void);

extern void irq5_handler(void);

extern void irq6_handler(void);

extern void irq7_handler(void);

extern void irq8_handler(void);

extern void irq9_handler(void);

extern void irq10_handler(void);

extern void irq11_handler(void);

extern void irq12_handler(void);

extern void irq13_handler(void);

extern void irq14_handler(void);

extern void irq15_handler(void);


#endif