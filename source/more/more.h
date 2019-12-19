#ifndef _MORE_H_
#define _MORE_H_

#include "../multiboot/multiboot.h"

extern multiboot_info_t* MBI;

extern void init_multiboot_info(multiboot_info_t* mbi_temp);

#endif
