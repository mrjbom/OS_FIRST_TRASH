#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../lib/string.h"

extern unsigned char keyboard_keys[128];

//0x60 key, 0x64 status ports
extern void keyboard_handler(unsigned char key, unsigned char status);

extern void keyboard_disable();

//after turning on the keyboard, all the keys pressed during the keyboard disable will work.
extern void keyboard_enable();

#endif