#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "../multiboot/multiboot.h"

#define BASE_TEXT_MODE_ADDRESS 0xB8000

#define true 0x00
#define false 0x01
#define NULL 0

typedef unsigned char               bool;
typedef unsigned char               byte;
typedef unsigned int                size_t;
typedef unsigned char               uint8_t;
typedef unsigned short              uint16_t;
typedef unsigned int                uint32_t;
typedef unsigned long long          uint64_t;
typedef char               int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;

enum base_textmode_colors
{
    TEXT_MODE_BLACK = 0x0,
    TEXT_MODE_BLUE = 0x1,
    TEXT_MODE_GREEN = 0x2,
    TEXT_MODE_CYAN = 0x3,
    TEXT_MODE_RED = 0x04,
    TEXT_MODE_MAGENTA = 0x05,
    TEXT_MODE_BROWN = 0x06,
    TEXT_MODE_LIGHT_GRAY = 0x07,
    TEXT_MODE_DARK_GRAY = 0x08,
    TEXT_MODE_LIGHT_BLUE = 0x09,
    TEXT_MODE_LIGHT_GREEN = 0x0A,
    TEXT_MODE_LIGHT_CYAN = 0x0B,
    TEXT_MODE_LIGHT_RED = 0x0C,
    TEXT_MODE_LIGHT_MAGENTA = 0x0D,
    TEXT_MODE_LIGNT_BROWN = 0x0E,
    TEXT_MODE_WHITE = 0x0F
};

extern multiboot_info_t* MBI;

extern void initMultibootInfo(multiboot_info_t* mbiTemp);

#endif
