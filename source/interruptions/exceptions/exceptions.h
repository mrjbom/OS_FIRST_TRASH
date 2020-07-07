#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../lib/string.h"

extern void invalid_opcode_exception(uint16_t cs, uint32_t eip);
extern void general_protection_fault_exception(uint32_t error_code);
extern void page_fault_exception(uint32_t error_code);

#endif
