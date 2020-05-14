#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../../lib/cstdlib.h"
#include "../../lib/stddef.h"
#include "../../lib/stdint.h"
#include "../../lib/string.h"

extern unsigned char keyboard_keys[128];

extern void keyboard_handler(byte key, byte status);

#endif