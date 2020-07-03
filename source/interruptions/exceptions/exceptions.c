#include "exceptions.h"
#include "../../debug/debug.h"
#include "../../memory/mmu/mmu.h"
#include "../../lfbmemory/lfbmemory.h"

void page_fault_exception() {
    serial_printf("page_fault_exception!\n");
    uint32_t cr2 = 0;
    uint32_t error_code = 0;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
    __asm__ volatile ("mov %%eax, %0" : "=r"(error_code));

    uint32_t virtualaddr = cr2;
    uint32_t virtualaddr_aligned = virtualaddr - (virtualaddr % 4096);
    /*
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
            "error_code = %i\n"
            "error P bit = %i\n"
            "error RW bit = %i\n"
            "error US bit = %i\n",
            virtualaddr,
            virtualaddr_aligned,
            error_code,
            error_p_bit,
            error_rw_bit,
            error_us_bit);
    lfb_clear(0xFF0000);
}
