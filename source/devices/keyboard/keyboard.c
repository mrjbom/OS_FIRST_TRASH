#include "keyboard.h"
#include "../../debug/debug.h"
#include "../../lfbmemory/lfbmemory.h"
#include "../../inlineassembly/inlineassembly.h"

unsigned char keyboard_keys[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
    '9', '0', '-', '=', '\b',	/* Backspace */
    '\t',			/* Tab */
    'q', 'w', 'e', 'r',	/* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
    '\'', '`',   0,		/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
    'm', ',', '.', '/',   0,				/* Right shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/*
8043 Keyboard Controller Status Register
0x64 port
Bit 0: Output Buffer Status
    0: Output buffer empty, dont read yet
    1: Output buffer full, please read me :)
Bit 1: Input Buffer Status
    0: Input buffer empty, can be written
    1: Input buffer full, dont write yet
Bit 2: System flag
    0: Set after power on reset
    1: Set after successfull completion of the keyboard controllers self-test (Basic Assurance Test, BAT)
Bit 3: Command Data
    0: Last write to input buffer was data (via port 0x60)
    1: Last write to input buffer was a command (via port 0x64)
Bit 4: Keyboard Locked
    0: Locked
    1: Not locked
Bit 5: Auxiliary Output buffer full
    PS/2 Systems:
        0: Determins if read from port 0x60 is valid If valid, 0=Keyboard data
        1: Mouse data, only if you can read from port 0x60
    AT Systems:
        0: OK flag
        1: Timeout on transmission from keyboard controller to keyboard. This may indicate no keyboard is present.
Bit 6: Timeout
    0: OK flag
    1: Timeout
        PS/2:
        General Timeout
        AT:
        Timeout on transmission from keyboard to keyboard controller. Possibly parity error (In which case both bits 6 and 7 are set)
Bit 7: Parity error
    0: OK flag, no error
    1: Parity error with last byte
*/

void keyboard_handler(unsigned char key, unsigned char status) {

}

void keyboard_disable() {
    outb(0x64, 0xAD);
}

void keyboard_enable() {
    outb(0x64, 0xAE);
}