#include "exceptions.h"
#include "../../debug/debug.h"
#include "../../memory/mmu/mmu.h"

void page_fault_exception() {
    dprintf("page_fault_exception!\n");

    uint32_t cr2 = 0;
    uint32_t error_code = 0;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
    __asm__ volatile ("pop %0" : "=r"(error_code));

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
    */
    bool error_p_bit = (error_code >> 0) & 1;
    bool error_rw_bit = (error_code >> 1) & 1;
    bool error_us_bit = (error_code >> 2) & 1;
    //uint32_t pdindex = (uint32_t)virtualaddr_aligned >> 22;
	//uint32_t ptindex = (uint32_t)virtualaddr_aligned >> 12 & 0x03FF;
    bool virtual_p_bit = (virtualaddr >> 0) & 1;
    bool virtual_w_bit = (virtualaddr >> 1) & 1;
    bool virtual_u_bit = (virtualaddr >> 2) & 1;
    bool virtual_r_bit = (virtualaddr >> 3) & 1;
    bool virtual_i_bit = (virtualaddr >> 4) & 1;
    dprintf("------\n"
            "virtualaddr: 0x%X\n"
            "virtualaddr_aligned: 0x%X\n"
            "error_code = %I\n"
            "error P bit = %I\n"
            "error RW bit = %I\n"
            "error US bit = %I\n"
            "virtual P bit = %I\n"
            "virtual W bit = %I\n"
            "virtual U bit = %I\n"
            "virtual R bit = %I\n"
            "virtual I bit = %I\n",
            virtualaddr,
            virtualaddr_aligned,
            error_code,
            error_p_bit,
            error_rw_bit,
            error_us_bit,
            virtual_p_bit,
            virtual_w_bit,
            virtual_u_bit,
            virtual_r_bit,
            virtual_i_bit);
}
