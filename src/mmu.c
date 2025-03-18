#include "mmu.h"
#include "sysregs.h"

void mmu_setConfig(const Pagetable *kernelPagetable)
{
    // Set the address of the L0 pagetable
    sysregs_ttbr0_el1_write((uint64)kernelPagetable);
    sysregs_ttbr1_el1_write((uint64)kernelPagetable);

    // Set information about the virtual address structure
    sysregs_tcr_el1_write(0x5b5103510);

    // Set memory attributes
    // Everything to 1 -> everything normal memory
    sysregs_mair_el1_write((uint64)-1);

    asm volatile("isb\n");
}

void mmu_enable()
{
    uint64 sysControlReg = sysregs_sctlr_el1_read();
    sysControlReg |= 0b1;       // Enable virtual memory
    sysControlReg |= 0b1 << 12; // Enable instruction caches at el1
    sysregs_sctlr_el1_write(sysControlReg);

    asm volatile("isb\n");
}

bool mmu_isEnabled()
{
    uint64 sysControlReg = sysregs_sctlr_el1_read();
    return sysControlReg & 0b1; // Check the virtual memory enabled bit
}