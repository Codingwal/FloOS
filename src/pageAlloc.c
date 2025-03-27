#include "pageAlloc.h"
#include "mem.h"
#include "error.h"

#define MAX_ALLOCS 16384
typedef struct FreePagesStack
{
    void *pages[MAX_ALLOCS];
    int index;
} FreePagesStack;

static FreePagesStack stack;

void pageAlloc_init(void)
{
    assert((uint64)P_RAM_START % PAGE_SIZE == 0, "P_RAM_START is not page aligned");
    byte *ptr = (byte *)P_RAM_START;
    for (uint i = 0; i < MAX_ALLOCS; i++)
    {
        stack.pages[i] = ptr;
        ptr += PAGE_SIZE;
    }
    stack.index = MAX_ALLOCS - 1;
}

void *pageAlloc_alloc(void)
{
    assert(stack.index != -1, "no more free pages");
    void *ptr = stack.pages[stack.index];
    stack.index--;
    return ptr;
}

void pageAlloc_free(void *ptr)
{
    assert((uint64)ptr % PAGE_SIZE == 0, "ptr is not page aligned");
    stack.index++;
    stack.pages[stack.index] = ptr;
    assert(stack.index < MAX_ALLOCS, "freed more allocations than existing");
}
