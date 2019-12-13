#ifndef _CPU_H_
#define _CPU_H_

#define cpuid(in, a, b, c, d) __asm__("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

extern int do_intel(void);
extern int do_amd(void);
extern void printregs(int eax, int ebx, int ecx, int edx);

extern int detect_cpu(void);

extern char *Intel[];

extern char *Intel_Other[];

extern int do_intel(void);

extern void printregs(int eax, int ebx, int ecx, int edx);

extern int do_amd(void);

#endif