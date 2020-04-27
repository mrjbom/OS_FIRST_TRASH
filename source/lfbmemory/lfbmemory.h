#ifndef _LFBMEMORY_H_
#define _LFBMEMORY_H_

#include "../lib/cstdlib.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/string.h"
#include "../more/more.h"

//#define SSFN_NOIMPLEMENTATION               /* don't include the normal renderer implementation */
//#define SSFN_CONSOLEBITMAP_HICOLOR          /* use the special renderer for hicolor packed pixels */
//#define SSFN_CONSOLEBITMAP_CLEARBG          /* added by me, for availability of changing the ssfn_bg variable */
//#include "../lib/ssfn/ssfn.h"

//sfn start addr from obj
//FreeSans.sfn
extern unsigned char _binary_FSfont_sfn_start;
//FreeSerif.sfn
extern unsigned char _binary_FSifont_sfn_start;

extern uint32_t* lfb_framebuffer_addr;

extern void init_lfb_mem();

extern void clear_lfb_mem(uint32_t color);

extern void put_pixel_lfb_mem(uint32_t x, uint32_t y, uint32_t color);

extern void draw_line_lfb_mem(int x0, int y0, int x1, int y1, uint32_t color);

extern void test_func();

#endif
