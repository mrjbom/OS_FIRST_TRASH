#include "pcspeaker.h"

#include "../pit/pit.h"
#include "../../inlineassembly/inlineassembly.h"

void pcspeaker_play_sound(uint32_t freq, uint32_t milliseconds) {
    //freq: >= 19 && <= 65535
    uint32_t div;
 	uint8_t tmp;
 
    //Set the PIT to the desired frequency
 	div = 1193180 / freq;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (div) );
 	outb(0x42, (uint8_t) (div >> 8));
 
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
    
    pit_sleep(milliseconds);
    pcspeaker_mute_sound();
    pit_init(freq_copy);
}

void pcspeaker_mute_sound() {
    uint8_t tmp = inb(0x61) & 0xFC;
 	outb(0x61, tmp);
}
