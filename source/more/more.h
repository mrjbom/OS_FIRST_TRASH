#ifndef _MORE_H_
#define _MORE_H_

//Various auxiliary functions and things
#include "../multiboot/multiboot.h"

#include "../lfbmemory/lfbmemory.h"

extern multiboot_info_t* MBI;

extern void multiboot_info_init(multiboot_info_t* mbi_temp);

extern void show_base_info(ssfn_text_cursor_t* text_cursor);

extern void setl(uint32_t*, uint32_t);

#endif
