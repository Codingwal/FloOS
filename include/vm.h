#pragma once

#include "defs.h"

typedef struct Pagetable Pagetable;
extern Pagetable *kernelPagetable;

enum MairIdxs
{
    IDX_NORMAL = 0,
    IDX_NORMAL_NO_CACHE = 1,
    IDX_DEVICE = 2,
};
enum AccessPerms
{
    PRIV_RW = 0,
    RW = 1,
    PRIV_R = 2,
    R_PRIV_R = 3,
};

// Create kernel page tables and switch to the virtual adress space
void vm_init(void);

// Enable / disable virtual memory
void vm_enable(void);
void vm_disable(void);
bool vm_isEnabled(void);

// Map a region of virtual memory to a region of physical memory
/// @param mairIdx Index in the memory attribute indirection register
/// @param pxn Privileged execute never (0 = execution in exception level higher than EL0 allowed)
/// @param ap Access permissions
void vm_map(Pagetable *table, void *virtualAddr, void *physicalAddr, uint size, bool replace, enum MairIdxs mairIdx, bool pxn, enum AccessPerms ap);

void vm_unmap(Pagetable *table, void *virtualAddr, uint size);

// Get a range of free virtual adress space (size in bytes)
void *vm_getVaRange(Pagetable *table, uint size);

// Walk the page table to find the physical adress mapped for the virtual address
void *vm_va2pa(Pagetable *table, void *virtualAddr);
