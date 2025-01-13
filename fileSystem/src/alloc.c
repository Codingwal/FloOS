#include "alloc.h"
#include <malloc.h>
#include <stdio.h>

int memoryLeaks = 0;
void *alloc(uint size)
{
    memoryLeaks++;
    return malloc(size);
}
void freeAllocation(void *ptr)
{
    memoryLeaks--;
    printf("%i!!!\n", memoryLeaks);
    free(ptr);
}