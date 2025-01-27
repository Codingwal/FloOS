#include "alloc.h"
#include "defs.h"
#include "stringFormat.h"
#include "io.h"

#ifndef OS
#include <malloc.h>
#endif

#define SECTOR_COUNT 4

typedef struct AllocationSector
{
    uint sizePerElement;
    byte *buffer;
    uint64 memoryMap; // 0 = free, 1 = occupied
} AllocationSector;

typedef struct Allocator
{
    AllocationSector sectors[SECTOR_COUNT];
} Allocator;

Allocator allocator;

static ExitCode createAllocationSector(AllocationSector *s, uint sizePerElement)
{
    s->sizePerElement = sizePerElement;

#ifndef OS
    s->buffer = malloc(sizePerElement * 64);
#else
#error This has not been implemented yet
#endif

    if (!s->buffer)
        return FAILURE;
    s->memoryMap = 0;
    return SUCCESS;
}
static void *allocInSector(AllocationSector *s)
{
    uint64 mask = 1;
    for (uint i = 0; i < 64; i++)
    {
        if ((s->memoryMap & mask) == 0)
        {
            s->memoryMap |= mask; // Set bit to 1 (-> occupied)
            return s->buffer + i * s->sizePerElement;
        }
        mask <<= 1;
    }
    return NULL;
}
static ExitCode freeInSector(AllocationSector *s, byte *addr)
{
    uint index = (addr - s->buffer) / s->sizePerElement;
    if (index >= 64)
        return FAILURE;
    uint64 mask = 1 << index;
    if ((s->memoryMap & mask) == 0)
        return FAILURE;
    s->memoryMap &= ~mask; // Set bit to 0 (-> free)
    return SUCCESS;
}

ExitCode allocator_init()
{
    if (createAllocationSector(&allocator.sectors[0], 8) == FAILURE)
        return FAILURE;
    if (createAllocationSector(&allocator.sectors[1], 32) == FAILURE)
        return FAILURE;
    if (createAllocationSector(&allocator.sectors[2], 128) == FAILURE)
        return FAILURE;
    if (createAllocationSector(&allocator.sectors[3], 512) == FAILURE)
        return FAILURE;
    return SUCCESS;
}
ExitCode allocator_dispose()
{
    // PRINT("Found %i memory leaks!\n", allocator.allocationCount);

    for (uint i = 0; i < SECTOR_COUNT; i++)
    {
        free(allocator.sectors[i].buffer);
        if (allocator.sectors[i].memoryMap != 0)
            print("Found memory leak(s)!\n");
    }
    return SUCCESS;
}
void *alloc(uint size)
{
    if (size == 0)
        return NULL;

    for (uint i = 0; i < SECTOR_COUNT; i++)
    {
        if (size < allocator.sectors[i].sizePerElement)
        {
            return allocInSector(&allocator.sectors[i]);
        }
    }
    return NULL;
}
ExitCode freeAlloc(void *ptr)
{
    for (uint i = 0; i < SECTOR_COUNT; i++)
    {
        if (freeInSector(&allocator.sectors[i], ptr) == SUCCESS)
            return SUCCESS;
    }
    return FAILURE;
}

void allocator_print()
{
    for (uint i = 0; i < SECTOR_COUNT; i++)
    {
        AllocationSector *s = &allocator.sectors[i];
        PRINT("Sector %i: sizePerElement = %i\nMemoryMap: ", i, s->sizePerElement);
        uint64 mask = 1;
        for (uint j = 0; j < 64; j++)
        {
            if ((s->memoryMap & mask))
                printChar('X');
            else
                printChar('-');
            mask <<= 1;
        }
        printChar('\n');
    }
    printChar('\n');
}