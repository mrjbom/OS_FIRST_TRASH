#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../lib/string.h"

extern unsigned char keyboard_keys[128];

extern void keyboard_handler(unsigned char key, unsigned char status);

#endif