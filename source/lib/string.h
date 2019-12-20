#ifndef _STRING_H_
#define _STRING_H_

#include "../lib/stdint.h"
#include "../lib/stdarg.h"
#include "../lib/cstdlib.h"
#include "../debug/debug.h"

extern size_t strlen(const char *s);
extern char* strcpy(char* s1, const char* s2);
extern char* strncpy(char* s1, const char* s2, size_t n);
extern void* memcpy(void* buf1, const void* buf2, size_t bytes);
extern void* memset(void* buf1, uint8_t value, size_t bytes);
extern int strcmp(const char* s1, const char* s2);
extern int strncmp(const char* s1, const char* s2, uint32_t n);
extern char* strcat(char* s1, const char* s2);
extern char* strext(char* buf, const char* str, char sym);
extern int strspn(char* str, const char* accept);
extern int strcspn(char* str, const char* rejected);
extern char* strchr(const char* str, char ch);
extern char* strtok_r(char* str, const char* delims, char** save_ptr);
extern char* memext(void* buff_dst, uint32_t n, const void* buff_src, char sym);
extern char* itoa(unsigned int value, char* str, unsigned int base);
extern unsigned int atou(char* str);
extern char* strinv(char* str);
extern unsigned int sprintf(char* s1, const char* s2, ...);
extern unsigned int snprintf(char* s1, uint32_t n, const char* s2, ...);
extern unsigned int vsprintf(char* s1, const char* s2, va_list list);
extern unsigned int vsnprintf(char* s1, unsigned int n, const char* s2, va_list list);

#endif
