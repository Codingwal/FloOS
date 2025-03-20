#include "vm.h"
#include "kalloc.h"
#include "mem.h"
#include "error.h"
#include "sysregs.h"
#include "io.h"

// See https://developer.arm.com/documentation/ddi0487/fc/
// page 2571: virtual adress structure
// page 2591: page table entry structure
// page 2600: access permissions
// 2598

enum
{
    VALID = 1,
    TABLE_DESCRIPTOR = 0b10,
    PAGE_DESCRIPTOR = 0b10,
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

            table->entries[index] = PA2PTE(newTable) | TABLE_DESCRIPTOR | VALID;
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
    if (*entryPtr & VALID)
        return PTE2PA(*entryPtr);
    else
        return NULL;
}

void vm_map(Pagetable *table, void *virtualAddr, void *physicalAddr, uint size, bool replace, enum MairIdxs mairIdx, bool pxn, enum AccessPerms ap)
{
    assert(table != NULL, "table is NULL");
    assert(size % PAGE_SIZE == 0, "size must be page aligned");
    assert((uint64)virtualAddr % PAGE_SIZE == 0, "virtual address must be page aligned");
    assert((uint64)physicalAddr % PAGE_SIZE == 0, "physical address must be page aligned");

    byte *va = virtualAddr;
    byte *pa = physicalAddr;

    uint64 flags = 0;
    flags |= (uint64)(pxn & 1) << 53;                   // [53] Privileged execute never
    flags |= 1 << 10;                                   // [10] Access flag
    flags |= (mairIdx == IDX_NORMAL) ? (0b11 << 8) : 0; // [9:8] Shareability field (Only normal, chacheable memory)
    flags |= (ap & 0b11) << 6;                          // [7:6] Access permissions
    flags |= 1 << 5;                                    // [5] Non secure access control (1 = access for secure and non-secure)
    flags |= (mairIdx & 0b111) << 2;                    // [4:2] Attribute index
    flags |= PAGE_DESCRIPTOR;                           // [1] Descriptor type
    flags |= VALID;                                     // [0] Valid

    byte *lastPage = va + size;
    while (va < lastPage)
    {
        uint64 *entry = vm_getPTE(table, va, true);
        assert(!(*entry & VALID) || replace, "entry exists but replacing is disallowed");
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

static void vm_setConfig(const Pagetable *kernelPagetable)
{
    // Set the address of the L0 pagetable
    sysregs_ttbr0_el1_write((uint64)kernelPagetable);
    sysregs_ttbr1_el1_write((uint64)kernelPagetable);

    // Set information about the virtual address structure
    sysregs_tcr_el1_write(0x5b5103510);

    // Set memory attributes
    // https://documentation-service.arm.com/static/63a43e333f28e5456434e18b (learn_the_architecture_-_aarch64_memory_attributes_and_properties)
    byte normal = 0xFF;
    byte normalNoCache = 0b01000100;
    byte device = 0;
    sysregs_mair_el1_write((normal << (IDX_NORMAL * 8)) | (normalNoCache << (IDX_NORMAL_NO_CACHE * 8)) | (device << (IDX_DEVICE * 8)));

    asm volatile("isb\n");
}

void vm_enable()
{
    uint64 sysControlReg = sysregs_sctlr_el1_read();
    sysControlReg |= 0b1;       // Enable virtual memory
    sysControlReg |= 0b1 << 12; // Enable instruction caches at el1
    sysregs_sctlr_el1_write(sysControlReg);

    asm volatile("isb\n");
}

bool vm_isEnabled()
{
    uint64 sysControlReg = sysregs_sctlr_el1_read();
    return sysControlReg & 0b1; // Check the virtual memory enabled bit
}

void vm_init()
{
    // Allocate L0 kernel pagetable
    kernelPagetable = (Pagetable *)kalloc();
    assert(kernelPagetable != NULL, "kalloc failed");
    mem_set(kernelPagetable, 0, PAGE_SIZE);
    kernelPagetable->entries[511] = PA2PTE(kernelPagetable) | TABLE_DESCRIPTOR | VALID; // Last entry points to itself. This is used to get the PAs of pagetables

    // Map kernel addresses (peripherals, kernel code, ...)
    vm_map(kernelPagetable, (void *)PERIPHERAL_BASE, (void *)PERIPHERAL_BASE, PAGE_ROUND_UP(PERIPHERAL_SIZE), false, IDX_DEVICE, true, PRIV_RW); // Peripherals (uart, ...)
    print("Mapped peripherals\n");
    vm_map(kernelPagetable, _start, _start, PAGE_ROUND_UP(kernelExecutableSize()), false, IDX_NORMAL, false, PRIV_RW); // Kernel executable
    print("Mapped kernel executable\n");
    vm_map(kernelPagetable, (void *)_stack_bottom, (void *)_stack_bottom, PAGE_ROUND_UP(kernelStackSize()), false, IDX_NORMAL, true, PRIV_RW); // Stack
    print("Mapped stack\n");

    print("Prepared pagetables\n");

    // Setup and enable the mmu and virtual memory
    vm_setConfig(kernelPagetable);
    print("Prepared mmu\n");
    vm_enable();

    print("Enabled mmu\n");

    assert(vm_isEnabled(), "mmu is not enabled");
}
