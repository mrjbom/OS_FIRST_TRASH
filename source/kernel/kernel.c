#include "kernel.h"
#include "../multiboot/multiboot.h"

multiboot_info_t* MBI;

void initMultibootInfo(multiboot_info_t* mbiTemp)
{
    MBI = mbiTemp;
}
