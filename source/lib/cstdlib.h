#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "../lib/stdint.h"

extern char* itoaINT32(int32_t value, char* result, int base);

extern char* itoaUINT32(uint32_t value, char* result, int base);

extern char* itoaINT64(int64_t value, char* result, int base);

extern char* itoaUINT64(uint64_t value, char* result, int base);

#endif
