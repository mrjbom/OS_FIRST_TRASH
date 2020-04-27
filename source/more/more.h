#ifndef _MORE_H_
#define _MORE_H_

//Various auxiliary functions and things
#include "../multiboot/multiboot.h"

#include "../lfbmemory/lfbmemory.h"
#include "../inlineassembly/inlineassembly.h"
#include "../interruptions/interruptions.h"
#include "../memory/memdetect/memdetect.h"
#include "../memory/memmmu/memmmu.h"
#include "../task/task.h"
#include "../debug/debug.h"
#include "../devices/cpu/cpu.h"

extern multiboot_info_t* MBI;

extern void init_multiboot_info(multiboot_info_t* mbi_temp);

extern void show_base_info();

#endif
