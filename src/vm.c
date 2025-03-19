#include "vm.h"
#include "kalloc.h"
#include "mem.h"
#include "error.h"
#include "mmu.h"
#include "io.h"

// See https://developer.arm.com/documentation/ddi0487/fc/
// page 2571: virtual adress structure
// page 2591: page table entry structure
// page 2600: access permissions

enum
{
    VALID = 1 << 0,
    // READ = 1 << 1,
    // WRITE = 1 << 2
};

Pagetable *kernelPagetable;

#define PTE2PA(x) ((void *)(x & (BITMASK(48) - BITMASK(12))))
#define PA2PTE(x) ((uint64)x)

// Get the page table entry at level 3, which points to a 4KB memory region
static uint64 *vm_getPTE(Pagetable *table, void *virtualAddr, bool alloc)
{
    assert(table != NULL, "table is NULL");

    uint lsb = 39;
    for (uint level = 0; level < 4; level++)
    {
        uint index = ((uint64)virtualAddr >> lsb) & BITMASK(9); // Extract [47..39], [38..30], [29..21] or [20..12], depending on level, and shift it down

        // If the entry points to a memory region and not another page table (true for all entries in a level 3 page table, at least in this os), return a pointer to it
        if (level == 3)
            return &table->entries[index];

        uint64 entry = table->entries[index];
        if (entry & VALID) // Entry exists
        {
            table = (Pagetable *)PTE2PA(entry); // Extract [47..12], which is the next level page table adress ([11..00] is always zero)
        }
        else // Entry does not exist
        {
            assert(alloc, "entry does not exist but allocation is disallowed");
            Pagetable *newTable = kalloc(); // kalloc allocates 4KB pages which is exactly the size of a page table
            assert(newTable != NULL, "newTable allocation failed");
            mem_set(newTable, 0, PAGE_SIZE);

            table->entries[index] = PA2PTE(newTable);
            table = newTable;
        }
        lsb -= 9;
    }
    return NULL; // Never reached
}

void *vm_va2pa(Pagetable *table, void *virtualAddr)
{
    assert(table != NULL, "table is NULL");

    uint64 *entryPtr = vm_getPTE(table, virtualAddr, false);
    return PTE2PA(*entryPtr);
}

void vm_map(Pagetable *table, void *virtualAddr, void *physicalAddr, uint size, uint64 flags, bool replace)
{
    assert(table != NULL, "table is NULL");
    assert(size % PAGE_SIZE == 0, "size must be page aligned");
    assert((uint64)virtualAddr % PAGE_SIZE == 0, "virtual address must be page aligned");
    assert((uint64)physicalAddr % PAGE_SIZE == 0, "physical address must be page aligned");

    byte *va = virtualAddr;
    byte *pa = physicalAddr;

    uint c = size / PAGE_SIZE;
    for (uint i = 0; i < c; i++)
    {
        uint64 *entry = vm_getPTE(table, va, true);
        assert(!(*entry & VALID && !replace), "entry exists but replacing is disallowed");
        *entry = (uint64)pa | flags;

        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }
}

void vm_unmap(Pagetable *table, void *virtualAddr, uint size)
{
    assert(table != NULL, "table is NULL");
    assert(size % PAGE_SIZE == 0, "size must be page aligned");
    assert((uint64)virtualAddr % PAGE_SIZE == 0, "virtual address must be page aligned");

    byte *va = virtualAddr;

    uint c = size / PAGE_SIZE;
    for (uint i = 0; i < c; i++)
    {
        uint64 *entry = vm_getPTE(table, va, false);
        *entry = 0;
        va += PAGE_SIZE;
    }
}

void *vm_getVaRange(Pagetable *table, uint size)
{
    assert(table != NULL, "table is NULL");
    assert(size % PAGE_SIZE == 0, "size must be page aligned");
    size /= PAGE_SIZE;

    const uint MAX_TRIES = 10000; // Stop after trying MAX_TRIES PTEs
    uint count = 0;
    for (byte *va = 0; (uint64)va < MAX_TRIES * PAGE_SIZE; va += PAGE_SIZE)
    {
        uint64 *entryPtr = vm_getPTE(table, va, true);

        // If there already is a physical address mapped to this virtual address, reset the free PTEs counter and continue
        if (*entryPtr & VALID)
        {
            count = 0;
            continue;
        }

        count++;

        // If there are size free PTEs in a row, return the virtual address
        if (count == size)
        {
            return va - size * PAGE_SIZE;
        }
    }
    return NULL;
}

void vm_init()
{
    // Allocate L0 kernel pagetable
    kernelPagetable = (Pagetable *)kalloc();
    assert(kernelPagetable != NULL, "kalloc failed");
    mem_set(kernelPagetable, 0, PAGE_SIZE);
    kernelPagetable->entries[511] = PA2PTE(kernelPagetable); // Last entry points to itself. This is used to get the PAs of pagetables

    // Map kernel addresses (peripherals, kernel code, ...)
    // TODO: Flags needed?
    vm_map(kernelPagetable, (void *)PERIPHERAL_BASE, (void *)PERIPHERAL_BASE, PAGE_ROUND_UP(PERIPHERAL_SIZE), 0, false); // Peripherals (uart, ...)
    vm_map(kernelPagetable, _start, _start, PAGE_ROUND_UP(_end - _start), 0, false);                                     // Kernel executable
    vm_map(kernelPagetable, STACK_BOTTOM, STACK_BOTTOM, STACK_SIZE, 0, false);

    print("Prepared pagetables\n");

    // Setup and enable the mmu and virtual memory
    mmu_setConfig(kernelPagetable);
    print("Prepared mmu\n");
    mmu_enable();
    print("Enabled mmu\n");

    assert(mmu_isEnabled(), "mmu is not enabled");
}
