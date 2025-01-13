#pragma once

#include "defs.h"

typedef struct Allocator
{
    uint allocationCount;
} Allocator;

extern Allocator allocator;

ExitCode allocator_init(Allocator *allocator);
ExitCode allocator_dispose(Allocator*allocator);

void *alloc(uint size);
void freeAllocation(void *ptr);