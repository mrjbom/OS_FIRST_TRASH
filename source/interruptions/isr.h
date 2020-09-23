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
                      //offset
  uint32_t ds;        //+0
  uint32_t edi;       //+4
  uint32_t esi;       //+8
  uint32_t ebp;       //+12
  uint32_t esp;       //+16
  uint32_t ebx;       //+20
  uint32_t edx;       //+24
  uint32_t ecx;       //+28
  uint32_t eax;       //+32
  uint32_t int_num;   //+36
  uint32_t err_code;  //+40
  uint32_t eip;       //+44
  uint32_t cs;        //+48
  uint32_t eflags;    //+52
  uint32_t useresp;   //+56
  uint32_t ss;        //+60
};
typedef struct registers registers_t;

#endif
