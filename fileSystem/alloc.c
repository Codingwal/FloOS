#include "alloc.h"
#include <malloc.h>

void *alloc(uint size)
{
    return malloc(size);
}
void freeAllocation(void *ptr)
{
    free(ptr);
}