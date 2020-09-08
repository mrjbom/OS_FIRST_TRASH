/*------------------------------------------------------------------------------
//
//	Descriptor tables initialization
//	(c) maisvendoo, 06.07.2013
//
//----------------------------------------------------------------------------*/

#include "descriptor_tables.h"
#include "../inlineassembly/inlineassembly.h"
#include "../lib/string.h"

/*******************************************************************************
	GDT
*******************************************************************************/

/* GDT loading function */
extern void gdt_flush(uint32_t);

static void init_gdt(void);

static void gdt_set_gate(int32_t, uint32_t, uint32_t, uint8_t, uint8_t);

static	void init_idt(void);

static	void idt_set_gate(uint8_t, uint32_t, uint16_t, uint8_t);

void write_tss(int32_t num, uint32_t ss0, uint32_t esp0);

gdt_entry_t	gdt_entries[6];

gdt_ptr_t	gdt_ptr;

idt_entry_t	idt_entries[256];
idt_ptr_t	idt_ptr;

tss_entry_t	tss;

extern uint32_t init_esp;

extern void tss_flush();

/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
void init_gdt(void)
{
  gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
  gdt_ptr.base = (uint32_t)&gdt_entries;
  
  gdt_set_gate(0, 0, 0, 0, 0);	/* Null segment */
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Code segment */
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* Data segment */
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User mode code segment */
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User mode data segment */
  gdt_set_gate(5, (uint32_t)&tss, (uint32_t)&tss + sizeof(tss_entry_t), 0xE9, 0xCF);

  gdt_flush((uint32_t)&gdt_ptr);

  uint32_t stack = 0;
  __asm__ volatile ("mov %%esp, %0" : "=r"(stack));

  write_tss(5, 0x10, stack);

  tss_flush();
}

/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
void init_descriptor_tables(void)
{
  init_gdt();
  init_idt();
}

/*------------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------*/
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
/*void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t dpl, u8int type)*/
{
  
  gdt_entries[num].base_low = (base & 0xFFFF);
  gdt_entries[num].base_middle = (base >> 16) & 0xFF;
  gdt_entries[num].base_high = (base >> 24) & 0xFF;
  
  gdt_entries[num].limit_low = (limit & 0xFFFF);
  gdt_entries[num].granularity = (limit >> 16) & 0xF;
  
  gdt_entries[num].granularity |= gran & 0xF0;
  gdt_entries[num].access = access;
}

/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
void write_tss(int32_t num, uint32_t ss0, uint32_t esp0)
{
	memset(&tss, 0, sizeof(tss_entry_t));

	tss.ss0 = ss0;
	tss.esp0 = esp0;

	tss.cs = 0x0B;

	tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;

	tss.iomap = 0xFF;
	tss.iomap_offset = (uint16_t) ( (uint32_t) &tss.iomap - (uint32_t) &tss );
}

/*------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------*/
void set_kernel_stack_in_tss(uint32_t stack)
{
	tss.esp0 = stack;
}

/*******************************************************************************
	IDT
*******************************************************************************/

extern void idt_flush(uint32_t);

/*------------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------*/

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */
 
/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void init_idt(void)
{
  idt_ptr.limit = sizeof(idt_entry_t)*256 - 1;
  idt_ptr.base = (uint32_t) &idt_entries;
  
  memset(&idt_entries, 0, sizeof(idt_entry_t)*256);  
  
  idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
  idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
  idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
  idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
  idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
  idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
  idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
  idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
  
  idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
  idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
  idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
  idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
  idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
  idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
  idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
  idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
  
  idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
  idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
  idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
  idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
  idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
  idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
  idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
  idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
  
  idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
  idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
  idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
  idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
  idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
  idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
  idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
  idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);      
  
  PIC_remap(32, 32 + 7);

  idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E);  
  idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E);
  idt_set_gate(34, (uint32_t)isr34, 0x08, 0x8E);
  idt_set_gate(35, (uint32_t)isr35, 0x08, 0x8E);
  idt_set_gate(36, (uint32_t)isr36, 0x08, 0x8E);
  idt_set_gate(37, (uint32_t)isr37, 0x08, 0x8E);
  idt_set_gate(38, (uint32_t)isr38, 0x08, 0x8E);
  idt_set_gate(39, (uint32_t)isr39, 0x08, 0x8E);
  idt_set_gate(40, (uint32_t)isr40, 0x08, 0x8E);
  idt_set_gate(41, (uint32_t)isr41, 0x08, 0x8E);
  idt_set_gate(42, (uint32_t)isr42, 0x08, 0x8E);
  idt_set_gate(43, (uint32_t)isr43, 0x08, 0x8E);
  idt_set_gate(44, (uint32_t)isr44, 0x08, 0x8E);
  idt_set_gate(45, (uint32_t)isr45, 0x08, 0x8E);
  idt_set_gate(46, (uint32_t)isr46, 0x08, 0x8E);
  idt_set_gate(47, (uint32_t)isr47, 0x08, 0x8E);

  idt_set_gate(80, (uint32_t)isr80, 0x08, 0xEF);
 
  idt_flush((uint32_t) &idt_ptr); 
}

/*------------------------------------------------------------------------------
//	
//----------------------------------------------------------------------------*/
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
  idt_entries[num].base_low = base & 0xFFFF;
  idt_entries[num].base_high = (base >> 16) & 0xFFFF;
  
  idt_entries[num].selector = selector;
  idt_entries[num].allways0 = 0;
  
  idt_entries[num].flags = flags;/* - для пользовательского режима */
}

uint32_t get_tss_esp0(void)
{
	return tss.esp0;
}
