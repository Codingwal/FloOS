#include "kalloc.h"
#include "pageAlloc.h"
#include "mem.h"
#include "vm.h"
#include "error.h"
#include "io.h"

#define MAX_ALLOCS 512

typedef struct Allocation
{
    void *virtualAddr;
    uint size;
} Allocation;

Allocation allocs[MAX_ALLOCS];

void kalloc_init()
{
    assert(vm_isEnabled(), "virtual memory is not initialized");
    for (uint i = 0; i < MAX_ALLOCS; i++)
    {
        allocs[i].virtualAddr = NULL;
        allocs[i].size = 0;
    }
}

void *kalloc(uint size)
{
    size = PAGE_ROUND_UP(size);

    // Allocate physical memory and map it to virtual addresses
    void *virtualAddr = vm_getVaRange(kernelPagetable, size);
    for (byte *va = virtualAddr; (uint64)va < (uint64)virtualAddr + size; va += PAGE_SIZE)
    {
        void *pa = pageAlloc_alloc();
        printf("pa: %x\n", (int)(int64)virtualAddr);
        vm_map(kernelPagetable, va, pa, PAGE_SIZE, false, IDX_NORMAL, true, PRIV_RW);
    }

    // Store info in an empty slot in allocs
    for (uint i = 0; true; i++)
    {
        assert(i != MAX_ALLOCS, "reached maximum allocation count");
        if (allocs[i].virtualAddr != NULL)
            continue;

        allocs[i].virtualAddr = virtualAddr;
        allocs[i].size = size;
    }

    return virtualAddr;
}

void kfree(void *virtualAddr)
{
    assert((uint64)virtualAddr % PAGE_SIZE == 0, "virtual address is not page aligned");

    // Find allocation
    Allocation *info = NULL;
    for (uint i = 0; i < MAX_ALLOCS; i++)
    {
        if (allocs[i].virtualAddr == virtualAddr)
        {
            info = &allocs[i];
            break;
        }
    }
    assert(info != NULL, "invalid virtual address");

    // Free physical memory
    for (byte *va = virtualAddr; (uint64)va < (uint64)virtualAddr + info->size; va += PAGE_SIZE)
    {
        pageAlloc_free(vm_va2pa(kernelPagetable, va));
    }

    // Remove mappings
    vm_unmap(kernelPagetable, virtualAddr, info->size);

    // Clear entry in allocs
    info->virtualAddr = NULL;
    info->size = 0;
}