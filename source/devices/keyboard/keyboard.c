#include "keyboard.h"
#include "../../debug/debug.h"
#include "../../lfbmemory/lfbmemory.h"

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

ssfn_text_cursor_t* handler_text_cursor = NULL;
int handler_ssfn_context_index = -1;
void keyboard_handler(byte key, byte status) {
if(!handler_text_cursor && handler_ssfn_context_index == -1) {
    handler_ssfn_context_index = ssfn_init_new_context(&_binary_UbuntuBold_sfn_start);
    if(handler_ssfn_context_index == -1) {
            dprintf("ssfn_init_new_context() error!");
            lfb_clear(0xFF0000);
            return;
        }
        if(!ssfn_select_font(handler_ssfn_context_index,
            SSFN_MY_FAMILY_SANS,
            SSFN_MY_STYLE_BOLD,
            16)) {
                dprintf("ssfn_cache_glyphs() error!");
                lfb_clear(0xFF0000);
                return;
            }
        handler_text_cursor = ssfn_create_cursor(handler_ssfn_context_index);
        if(!handler_text_cursor) {
            dprintf("ssfn_create_cursor() error!");
            lfb_clear(0xFF0000);
            return;
        }
        ssfn_setup_cursor(handler_text_cursor, 5, 700, 20, 0xFFFFFF);
    } else {
        //if(keyboard_keys[key] >= 32 && keyboard_keys[key] >= 128) {
            dprintf("%c(%i)\n", keyboard_keys[key], keyboard_keys[key]);
            tprintf(handler_text_cursor, "%c", keyboard_keys[key]);
        //}
    }
}
