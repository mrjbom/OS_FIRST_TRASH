#include "../inlineassembly/inlineassembly.h"

uint32_t farpeekl(uint16_t sel, void* off)
{
    uint32_t ret;
    __asm__ ( "push %%fs\n\t"
          "mov  %1, %%fs\n\t"
          "mov  %%fs:(%2), %0\n\t"
          "pop  %%fs"
          : "=r"(ret) : "g"(sel), "r"(off) );
    return ret;
}

void farpokeb(uint16_t sel, void* off, uint8_t v)
{
    __asm__ ( "push %%fs\n\t"
          "mov  %0, %%fs\n\t"
          "movb %2, %%fs:(%1)\n\t"
          "pop %%fs"
          : : "g"(sel), "r"(off), "r"(v) );
}

void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

uint16_t inw(uint16_t p_port)
{
    uint16_t l_ret;
    __asm__ volatile ( "inw %1, %0"
                    : "=a" (l_ret)
                    : "dN" (p_port) );
    return l_ret;
}

void outw (uint16_t p_port,uint16_t p_data)
{
    __asm__ volatile ( "outw %1, %0" : : "dN" (p_port), "a" (p_data) );
}

void io_wait(void)
{
    /* TODO: This is probably fragile. */
    __asm__ volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}

bool are_interrupts_enabled()
{
    unsigned long flags;
    __asm__ volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

unsigned long save_irqdisable(void)
{
    unsigned long flags;
    __asm__ volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    
    //add by me
    return flags;
}

void irqrestore(unsigned long flags)
{
    __asm__ ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

/*
void intended_usage(void)
{
    unsigned long f = save_irqdisable();
    do_whatever_without_irqs();
    irqrestore(f);
}
*/

void lidt(void* base, uint16_t size)
{   // This function works in 32 and 64bit mode
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { size, base };
 
    __asm__ ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void cpuid(int code, uint32_t* a, uint32_t* d)
{
    __asm__ volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

int cpuid_string(int code, uint32_t where[4]) {
  __asm__ volatile("cpuid":"=a"(*where),"=b"(*(where+1)),
               "=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
  return (int)where[0];
}
