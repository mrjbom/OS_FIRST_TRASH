#include "debug.h"
#include "../inlineassembly/inlineassembly.h"

void init_serial() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}
 
void write_serial_ch(char ch) {
   while (is_transmit_empty() == 0);
      outb(PORT, ch);
}

void write_serial_str(const char* str) {
   size_t str_size = strlen(str);
   for(size_t i = 0; i < str_size; i++)
   {
      write_serial_ch(str[i]);
   }
}

//formats
//c - char
//i - int32
//I - uint32
//X - uint32 address
//s - string
void dprintf(const char* s2, ...) {
   char outstr[1024];
   va_list list;
   va_start(list, s2);
   vsprintf(outstr, s2, list);
   write_serial_str(outstr);
}
