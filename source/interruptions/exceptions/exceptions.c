#include "exceptions.h"
#include "../../debug/debug.h"
#include "../../memory/mmu/mmu.h"
#include "../../lfbmemory/lfbmemory.h"

//Intel® 64 and IA-32 architectures software developer’s manual combined volumes 1, 2A, 2B, 2C, 2D, 3A, 3B, 3C, 3D, and 4
//6-42 Vol. 3A
void general_protection_fault_exception(uint32_t error_code) {
    serial_printf("general_protection_fault_exception!\n");
    serial_printf("error_code = %u\n", error_code);
    for(int32_t i = 31; i >= 0; --i) {
        serial_printf("%u", get_n_bit(error_code, i));
    }
    serial_printf("\n");
    lfb_clear(0xFF0000);
}

void page_fault_exception(uint32_t error_code) {
    serial_printf("page_fault_exception!\n");
    uint32_t cr2 = 0;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));

    uint32_t virtualaddr = cr2;
    uint32_t virtualaddr_aligned = virtualaddr - (virtualaddr % 4096);
    /*
    error code
    Bit 0 (P) is the Present flag.
    Bit 1 (R/W) is the Read/Write flag.
    Bit 2 (U/S) is the User/Supervisor flag.

    US RW  P - Description
    0  0  0 - Supervisory process tried to read a non-present page entry
    0  0  1 - Supervisory process tried to read a page and caused a protection fault
    0  1  0 - Supervisory process tried to write to a non-present page entry
    0  1  1 - Supervisory process tried to write a page and caused a protection fault
    1  0  0 - User process tried to read a non-present page entry
    1  0  1 - User process tried to read a page and caused a protection fault
    1  1  0 - User process tried to write to a non-present page entry
    1  1  1 - User process tried to write a page and caused a protection fault

    P bit	  When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.
    R/W bit	  When set, the page fault was caused by a write access. When not set, it was caused by a read access.
    U/S bit	  When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
    */
    bool error_p_bit = get_n_bit(error_code, 0);
    bool error_rw_bit = get_n_bit(error_code, 1);
    bool error_us_bit = get_n_bit(error_code, 2);
    //uint32_t pdindex = (uint32_t)virtualaddr_aligned >> 22;
	//uint32_t ptindex = (uint32_t)virtualaddr_aligned >> 12 & 0x03FF;
    serial_printf(
            "virtualaddr: 0x%x\n"
            "virtualaddr_aligned: 0x%x\n"
            "error_code = %u\n"
            "error P bit = %u\n"
            "error RW bit = %u\n"
            "error US bit = %u\n",
            virtualaddr,
            virtualaddr_aligned,
            error_code,
            error_p_bit,
            error_rw_bit,
            error_us_bit);

    lfb_clear(0xFF0000);

    //you must handle the error or exception will loop.
    //for example
    //vm_set_page_flags(current_directory_table, (void*)virtualaddr_aligned, PAGE_PRESENT);
}
