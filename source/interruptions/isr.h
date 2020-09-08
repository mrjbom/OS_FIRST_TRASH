#ifndef _ISR_H_
#define _ISR_H_

/*
Part of the code is borrowed from the @maisvendoo blog, http://phantomexos.blogspot.ru
Thanks.
*/

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"

/*------------------------------------------------
// Структура для хранения регистров процессора
//----------------------------------------------*/
struct registers {
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, esx, eax;
  uint32_t int_num, err_code; /* Номер прерывания и код ошибки */
  uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed));

typedef struct registers registers_t;

#endif
