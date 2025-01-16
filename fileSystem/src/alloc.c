#include "alloc.h"
#include <malloc.h>
#include <stdio.h>
#include "defs.h"
#include "stringFormat.h"
#include "io.h"

typedef struct Allocator
{
    uint allocationCount;
} Allocator;

Allocator allocator;

ExitCode allocator_init()
{
    allocator.allocationCount = 0;
    return SUCCESS;
}
ExitCode allocator_dispose()
{
    if (allocator.allocationCount != 0)
        PRINT("Found %i memory leaks!\n", allocator.allocationCount);
    return SUCCESS;
}
void *alloc(uint size)
{
    allocator.allocationCount++;
    return malloc(size);
}
void freeAlloc(void *ptr)
{
    allocator.allocationCount--;
    free(ptr);
}