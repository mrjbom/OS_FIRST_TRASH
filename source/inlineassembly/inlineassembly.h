#ifndef _INLINEASSEMBLY_H_
#define _INLINEASSEMBLY_H_

#include "../kernel/kernel.h"
#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"

//----Memory access----

//FAR_PEEKx
extern uint32_t farpeekl(uint16_t sel, void* off);

//FAR_POKEx
extern void farpokeb(uint16_t sel, void* off, uint8_t v);

//I/O access
extern void outb(uint16_t port, uint8_t val);

extern uint8_t inb(uint16_t port);

//IO_WAIT
extern void io_wait(void);

//----Interrupt-related functions----

//Interrupts Enabled?
extern bool are_interrupts_enabled();

//Push/pop interrupt flag
extern unsigned long save_irqdisable(void);

extern void irqrestore(unsigned long flags);

//extern void intended_usage(void);

//LIDT
extern void lidt(void* base, uint16_t size);

//----CPU-related functions----

//CPUID
extern void cpuid(int code, uint32_t* a, uint32_t* d);

extern int cpuid_string(int code, uint32_t where[4]);

#endif
