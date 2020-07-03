#ifndef _PCSPEAKER_H_
#define _PCSPEAKER_H_

#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"

extern void pcspeaker_play_sound(uint32_t freq, uint32_t milliseconds);

//mute pcspeaker
extern void pcspeaker_mute_sound();

#endif
