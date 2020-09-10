/*------------------------------------------------------------------------------
//
//	Description of descriptor tables structures
//	(c) mainsvendoo, 06.07.2013
//
//----------------------------------------------------------------------------*/

#ifndef _DESCRIPTOR_TABLES_H_
#define _DESCRIPTOR_TABLES_H_

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"

/*------------------------------------------------------------------------------
//	TSS descriptor
//----------------------------------------------------------------------------*/
struct tss_descriptor
{
	uint16_t limit_15_0;
	uint16_t base_15_0;
	uint8_t	base_23_16;
	uint8_t	type:4;
	uint8_t	sys:1;
	uint8_t	DPL:2;
	uint8_t	present:1;
	uint8_t	limit_19_16:4;
	uint8_t	AVL:1;
	uint8_t	allways_zero:2;
	uint8_t	gran:1;
	uint8_t	base_31_24;

}__attribute__((packed));

typedef	struct	tss_descriptor	tss_descriptor_t;

/*------------------------------------------------------------------------------
//	Task state segment (TSS)
//----------------------------------------------------------------------------*/
struct	tss_entry
{
	uint32_t	prev_tss;
	uint32_t	esp0;
	uint32_t	ss0;
	uint32_t	esp1;
	uint32_t	ss1;
	uint32_t	esp2;
	uint32_t	ss2;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax;
	uint32_t	ecx;
	uint32_t	edx;
	uint32_t	ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint32_t	es;
	uint32_t	cs;
	uint32_t	ss;
	uint32_t	ds;
	uint32_t	fs;
	uint32_t	gs;
	uint32_t	ldtr;
	uint16_t	task_flags;
	uint16_t	iomap_offset;
	uint8_t	iomap;

};

typedef struct tss_entry tss_entry_t;

/*------------------------------------------------------------------------------
//	Entry in Global Descriptor Table (GDT)
//----------------------------------------------------------------------------*/
struct gdt_entry_struct
{
 
  uint16_t	limit_low;
  uint16_t	base_low;
  uint8_t	base_middle;
  uint8_t	access;
  uint8_t	granularity;
  uint8_t	base_high;
  
}__attribute__((packed));

typedef	struct	gdt_entry_struct	gdt_entry_t;

/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
struct gdt_ptr_struct
{
  
  uint16_t	limit;
  uint32_t	base;
  
}__attribute__((packed));

typedef	struct	gdt_ptr_struct	gdt_ptr_t;

/*------------------------------------------------------------------------------
//	Entry in Interrup Descriptor Table (IDT)
//----------------------------------------------------------------------------*/
struct idt_entry_struct
{
  
  uint16_t	base_low;
  uint16_t	selector;
  uint8_t	allways0;
  uint8_t	flags;
  uint16_t	base_high;  
  
}__attribute__((packed));

typedef	struct	idt_entry_struct	idt_entry_t;

/*------------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------*/
struct	idt_ptr_struct
{
 
  uint16_t	limit;
  uint32_t	base;
  
}__attribute__((packed));

typedef	struct	idt_ptr_struct idt_ptr_t;

/* GDT inmitialization */
void init_descriptor_tables(void);

/* External function for interrupt processing */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);

extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);

extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);

extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

//irq
extern void isr32(void);
extern void isr33(void);
extern void isr34(void);
extern void isr35(void);
extern void isr36(void);
extern void isr37(void);
extern void isr38(void);
extern void isr39(void);
extern void isr40(void);
extern void isr41(void);
extern void isr42(void);
extern void isr43(void);
extern void isr44(void);
extern void isr45(void);
extern void isr46(void);
extern void isr47(void);

extern void isr80(void);

extern uint32_t get_tss_esp0(void);

#endif
