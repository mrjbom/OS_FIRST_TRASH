#include "more.h"

multiboot_info_t* MBI;

void init_multiboot_info(multiboot_info_t* mbi_temp) {
    MBI = mbi_temp;
}
