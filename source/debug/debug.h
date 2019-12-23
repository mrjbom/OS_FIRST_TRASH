#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"

#define PORT 0x3f8   /* COM1 */
 
//serial PORT debug

extern void init_serial();

extern int is_transmit_empty();

extern void write_serial_ch(char ch);

extern void write_serial_str(const char* str);

extern void dprintf(const char* s2, ...);

#endif