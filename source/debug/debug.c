#include "debug.h"
#include "../inlineassembly/inlineassembly.h"

void serial_init() {
   outb(PORT_COM1 + 1, 0x00);    // Disable all interrupts
   outb(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT_COM1 + 1, 0x00);    //                  (hi byte)
   outb(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int serial_is_transmit_empty() {
   return inb(PORT_COM1 + 5) & 0x20;
}
 
void serial_write_symbol(char symbol) {
   while (serial_is_transmit_empty() == 0);
      outb(PORT_COM1, symbol);
}

void serial_write_str(const char* str) {
   size_t str_size = strlen(str);
   for(size_t i = 0; i < str_size; i++) {
      serial_write_symbol(str[i]);
   }
}

void serial_printf(const char* fmt, ...) {
   char outstr[1024];
   va_list list;
   va_start(list, fmt);
   vsnprintf(outstr, 1024, fmt, list);
   serial_write_str(outstr);
}
