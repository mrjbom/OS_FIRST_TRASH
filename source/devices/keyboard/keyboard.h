#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../../kernel/kernel.h"
#include "../../lib/stdint.h"

extern unsigned char keyboard_keys[128];

extern void keyboard_handler(byte key, byte status);

#endif