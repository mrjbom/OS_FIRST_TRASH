#include "../lfbmemory/lfbmemory.h"
#include "../kernel/kernel.h"

uint32_t* LFBFRAMEBUFFER_ADDR = NULL;

void initLFBMemory()
{
    LFBFRAMEBUFFER_ADDR = (uint32_t*)((uint32_t)MBI->framebuffer_addr);
}

void clearLFBMemory(uint32_t color)
{
    for (uint32_t pixelPos = 0; pixelPos < MBI->framebuffer_width * MBI->framebuffer_height; pixelPos++)
        LFBFRAMEBUFFER_ADDR[pixelPos] = color;
}

void writePixelToLFB(uint32_t line, uint32_t column, uint32_t color)
{
    uint32_t pixelPos = line * MBI->framebuffer_width + column;
    LFBFRAMEBUFFER_ADDR[pixelPos] = color;
}

void writeSymbolToLFB(uint32_t line, uint32_t column, char symbol)
{

}
