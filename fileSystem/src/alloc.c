#include "alloc.h"
#include <malloc.h>
#include <stdio.h>
#include "defs.h"
#include "stringFormat.h"
#include "io.h"

Allocator allocator;

ExitCode allocator_init(Allocator *alloc)
{
    alloc->allocationCount = 0;
    return SUCCESS;
}
ExitCode allocator_dispose(Allocator *alloc)
{
    if (alloc->allocationCount != 0)
        print_i("Found %i memory leaks!\n", alloc->allocationCount);
    return SUCCESS;
}
void *alloc(uint size)
{
    allocator.allocationCount++;
    return malloc(size);
}
void freeAllocation(void *ptr)
{
    allocator.allocationCount--;
    free(ptr);
}