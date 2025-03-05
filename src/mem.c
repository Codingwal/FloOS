#include "mem.h"

void mem_set(void *start, byte value, uint size)
{
    byte *ptr = start;
    for (uint i = 0; i < size; i++)
    {
        ptr[i] = value;
    }
}