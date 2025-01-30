#include "alloc.h"
#include "defs.h"
#include "stringFormat.h"
#include "io.h"

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
    bool initialized;
} Allocator;

Allocator allocator;
byte buffer[1024 * 1024];

static ExitCode createAllocationSector(AllocationSector *s, uint sizePerElement, uint *idx)
{
    if (*idx + sizePerElement * 64 > sizeof(buffer))
        return FAILURE;
    s->buffer = &buffer[*idx];
    s->sizePerElement = sizePerElement;
    s->memoryMap = 0;
    *idx += sizePerElement * 64;
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
    uint idx = 0;
    RETURN_ON_FAILURE(createAllocationSector(&allocator.sectors[0], 8, &idx))
    RETURN_ON_FAILURE(createAllocationSector(&allocator.sectors[1], 32, &idx))
    RETURN_ON_FAILURE(createAllocationSector(&allocator.sectors[2], 128, &idx))
    RETURN_ON_FAILURE(createAllocationSector(&allocator.sectors[3], 512, &idx))
    allocator.initialized = true;
    return SUCCESS;
}
ExitCode allocator_dispose()
{
    if (!allocator.initialized)
        return FAILURE;

    for (uint i = 0; i < SECTOR_COUNT; i++)
    {
        if (allocator.sectors[i].memoryMap != 0)
            print("Found memory leak(s)!\n");
    }
    return SUCCESS;
}
void *alloc(uint size)
{
    if (size == 0 || !allocator.initialized)
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
    if (!allocator.initialized)
        return FAILURE;

    for (uint i = 0; i < SECTOR_COUNT; i++)
    {
        if (freeInSector(&allocator.sectors[i], ptr) == SUCCESS)
            return SUCCESS;
    }
    return FAILURE;
}

void allocator_print()
{
    if (!allocator.initialized)
        return;

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