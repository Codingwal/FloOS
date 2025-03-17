#pragma once

#include "defs.h"

typedef struct Pagetable
{
    uint64 entries[512];
} Pagetable;

extern Pagetable *kernelPagetable;

// Create kernel page tables and switch to the virtual adress space
void vm_init();

// Enable / disable virtual memory
void vm_enable(Pagetable *table);
void vm_disable();

// Map a region of virtual memory to a region of physical memory
void vm_map(Pagetable *table, void *virtualAddr, void *physicalAddr, uint size, uint64 flags, bool replace);

void vm_unmap(Pagetable *table, void *virtualAddr, uint size);

// Get a range of free virtual adress space
void *vm_getVaRange(Pagetable *table, uint size);

// Walk the page table to find the physical adress mapped for the virtual address
void *vm_va2pa(Pagetable *table, void *virtualAddr);