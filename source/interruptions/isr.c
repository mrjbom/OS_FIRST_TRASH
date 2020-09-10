#include "isr.h"
#include "../debug/debug.h"
#include "../inlineassembly/inlineassembly.h"
#include "../devices/pit/pit.h"
#include "../scheduler/scheduler.h"

void PIC_sendEOI(uint8_t intnum)
{
  //PIC1 PIC2
  //32   39
  //33   40
  //34   41
  //35   42
  //36   43
  //37   44
  //38   45
  if(intnum >= 32) {
	  if(intnum >= 39)
		  outb(0xA0, 0x20); //EOI to PIC2
	  outb(0x20, 0x20); //EOI to PIC1
  }
}

void isr_handler(registers_t* regs)
{
  if(regs->int_num == 6) {
    scheduler_thread_show_list();
    PIC_sendEOI(regs->int_num);
    return;
  }
  if(regs->int_num == 32) {
    pit_handler();
    PIC_sendEOI(regs->int_num);
    return;
  }
  serial_printf("unhandled interrupt: %u\n", regs->int_num);
  PIC_sendEOI(regs->int_num);
}
