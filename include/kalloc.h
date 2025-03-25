#pragma once
#include "defs.h"

// Kernel allocator
// Allocates physical pages and maps them to a virtual address

void kalloc_init(void);

// Allocate PAGE_ROUND_UP(size) bytes
// The returned ptr is page aligned
void *kalloc(uint size);

// Free a previously with kalloc allocated memory region
void kfree(void *ptr);