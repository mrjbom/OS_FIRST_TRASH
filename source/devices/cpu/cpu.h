#ifndef _CPU_H_
#define _CPU_H_

#include "../../lib/string.h"

#define cpuidm(in, a, b, c, d) __asm__("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

typedef struct cpuinfo {
    //intel - 1
    //amd - 2
    //other - 3
    int intel_or_amd;
    //Intel Specific Features
    int intel_model;
    int intel_family;
    int intel_type;
    int intel_brand;
    int intel_stepping;
    int intel_reserved;
    int intel_extended_family;
    int intel_signature;
    char* intel_model_str;
    char* intel_family_str;
    char* intel_type_str;
    char* intel_brand_str;
    char* intel_specific_brand;
    char* intel_specific_brand_other;
    //AMD Specific Features
    int amd_model;
    int amd_family;
    int amd_stepping;
    int amd_reserved;
    char* amd_model_str;
    char* amd_family_str;
    char* amd_brand_str;

} cpuinfo_t;

extern cpuinfo_t cpu_info;

extern void detect_cpu(void);
extern int do_intel(void);
extern int do_amd(void);
extern void printregs(int eax, int ebx, int ecx, int edx);

extern char *Intel[];

extern char *Intel_Other[];

//extern int do_intel(void);

#endif