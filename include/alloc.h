#pragma once

#include "defs.h"

void allocator_init();
void allocator_dispose();

void *alloc(uint size);
void freeAlloc(void *ptr);

void allocator_print();