#ifndef _DEBUG_H_
#define _DEBUG_H_

//serial PORT debug

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"

#define PORT_COM1 0x3f8   /* COM1 */

extern void init_serial();

extern int is_transmit_empty();

extern void write_serial_ch(char ch);

extern void write_serial_str(const char* str);

//debug serial printf
//formats
//c - char
//i - int32
//I - uint32
//l - int64
//L - uint64
//X - uint32 address
//Z - uint64 address
//s - string
extern void dprintf(const char* s2, ...);

#endif