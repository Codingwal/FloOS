#pragma once

#include "defs.h"

typedef struct Pagetable
{
    uint64 entries[512];
} Pagetable;

// Create kernel page tables and switch to the virtual adress space
void vm_init();

// Map a region of virtual memory to a region of physical memory
void vm_map(Pagetable *table, void *virtualAddr, void *physicalAddr, uint size, uint64 flags, bool replace);