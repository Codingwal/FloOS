#include "vm.h"
#include "pageAlloc.h"
#include "mem.h"
#include "error.h"
#include "sysregs.h"
#include "io.h"

// See https://developer.arm.com/documentation/ddi0487/fc/
// page 2571: virtual adress structure
// page 2589: table descriptor structure
// page 2591: page descriptor structure
// page 2595: page descriptor attribute fields
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

static uint64 vm_getFlags(enum MairIdxs mairIdx, bool pxn, enum AccessPerms ap)
{
    uint64 flags = 0;
    flags |= (uint64)(pxn & 1) << 53;                   // [53] Privileged execute never
    flags |= 1 << 10;                                   // [10] Access flag
    flags |= (mairIdx == IDX_NORMAL) ? (0b11 << 8) : 0; // [9:8] Shareability field (Only normal, chacheable memory)
    flags |= (ap & 0b11) << 6;                          // [7:6] Access permissions
    flags |= 1 << 5;                                    // [5] Non secure access control (1 = access for secure and non-secure)
    flags |= (mairIdx & 0b111) << 2;                    // [4:2] Attribute index
    flags |= PAGE_DESCRIPTOR;                           // [1] Descriptor type
    flags |= VALID;                                     // [0] Valid
    return flags;
}

static void vm_createPagetables()
{
    uint64 flags = vm_getFlags(IDX_NORMAL_NO_CACHE, true, PRIV_RW);

    // Create L0 kernel pagetable (0TB - 256TB)
    kernelPagetable = (Pagetable *)pageAlloc_alloc();
    mem_set(kernelPagetable, 0, PAGE_SIZE);
    kernelPagetable->entries[511] = PA2PTE(kernelPagetable) | flags; // Last entry points to itself. This is used to get the PAs of pagetables

    // Create L1 pagetable (0GB - 512GB)
    Pagetable *L1 = pageAlloc_alloc();
    mem_set(L1, 0, PAGE_SIZE);
    kernelPagetable->entries[0] = PA2PTE(L1) | flags;

    // Create L2 pagetable (0GB - 1GB)
    Pagetable *L2 = pageAlloc_alloc();
    mem_set(L2, 0, PAGE_SIZE);
    L1->entries[0] = PA2PTE(L2) | flags;

    // Create L2 device pagetable (3GB - 4GB)
    Pagetable *L2periph = pageAlloc_alloc();
    mem_set(L2periph, 0, PAGE_SIZE);
    L1->entries[3] = PA2PTE(L2periph) | flags;

    // L3 pagetables
    {
        // Stack and kernel (0MB - 2MB)
        Pagetable *kernel = pageAlloc_alloc();
        L2->entries[0] = PA2PTE(kernel) | flags;
        mem_set(kernel, 0, PAGE_SIZE);

        // RAM (2MB - 4MB)
        Pagetable *ram = pageAlloc_alloc();
        L2->entries[1] = PA2PTE(ram) | flags;
        mem_set(ram, 0, PAGE_SIZE);

        // AUX & GPIO (4066MB - 4068MB) (4066MB = 3GB + 994MB; 994 / 2 = 497)
        Pagetable *aux = pageAlloc_alloc();
        L2periph->entries[497] = PA2PTE(aux) | flags;
        mem_set(aux, 0, PAGE_SIZE);
    }
}

// Get the page table entry at level 3, which points to a 4KB memory region
static uint64 *vm_getPTE(Pagetable *table, void *virtualAddr)
{
    assert(table != NULL, "table is NULL");

    if (vm_isEnabled())
    {
        // Shift indices down and use 511 as the first index (kernelPagetable[511] points to kernelPagetable)
        uint64 va = (uint64)virtualAddr;
        va >>= 9;                  // loops once -> L1 index will be the actual L0 index and so on
        va &= ~0b111;              // Remove the 3 least significant bits. [11:3] are the L3 index
        va |= ((uint64)511 << 39); // L0 index needs to be the last entry, which is the loop entry
        return (uint64 *)va;
    }

    uint lsb = 39;
    for (uint level = 0; true; level++)
    {
        uint index = ((uint64)virtualAddr >> lsb) & BITMASK(9); // Extract [47..39], [38..30], [29..21] or [20..12], depending on level, and shift it down

        // If the entry points to a memory region and not another page table (true for all entries in a level 3 page table), return a pointer to it
        if (level == 3)
            return &table->entries[index];

        uint64 entry = table->entries[index];
        assert(entry & VALID, "invalid entry");
        table = (Pagetable *)PTE2PA(entry); // Extract [47..12], which is the next level page table adress ([11..00] is always zero)

        lsb -= 9;
    }
}

void *vm_va2pa(Pagetable *table, void *virtualAddr)
{
    assert(table != NULL, "table is NULL");

    uint64 *entryPtr = vm_getPTE(table, virtualAddr);
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

    uint64 flags = vm_getFlags(mairIdx, pxn, ap);

    byte *lastPage = va + size;
    while (va < lastPage)
    {
        uint64 *entry = vm_getPTE(table, va);
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
        uint64 *entry = vm_getPTE(table, va);
        *entry = 0;
        va += PAGE_SIZE;
    }
}

void *vm_getVaRange(Pagetable *table, uint size)
{
    assert(table != NULL, "table is NULL");
    assert(size % PAGE_SIZE == 0, "size must be page aligned");
    size /= PAGE_SIZE;

    uint count = 0;
    for (byte *va = (byte *)V_RAM_START; (uint64)va < V_RAM_END; va += PAGE_SIZE)
    {
        uint64 *entryPtr = vm_getPTE(table, va);

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
            return (void *)((uint64)va - (size - 1) * PAGE_SIZE); // If size = 0, nothing needs to be subtracted, as va is incremented afterwards
        }
    }
    panic("Failed to get a free virtual address range");
    return NULL; // Never reached
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
    vm_createPagetables();

    // Map kernel addresses (peripherals, kernel code, ...)
    vm_map(kernelPagetable, (void *)LOWEST_USED_PERIPHERAL, (void *)LOWEST_USED_PERIPHERAL, USED_PERIPHERAL_SIZE, false, IDX_DEVICE, true, PRIV_RW); // Peripherals (uart, ...)
    vm_map(kernelPagetable, _text_start, _text_start, PAGE_ROUND_UP(kernelExecutableSize()), false, IDX_NORMAL, false, PRIV_R);                      // Kernel executable
    vm_map(kernelPagetable, _data_start, _data_start, PAGE_ROUND_UP(kernelDataSize()), false, IDX_NORMAL, true, PRIV_RW);                            // Kernel data
    vm_map(kernelPagetable, (void *)_stack_bottom, (void *)_stack_bottom, PAGE_ROUND_UP(kernelStackSize()), false, IDX_NORMAL, true, PRIV_RW);       // Stack

    // Setup and enable the mmu and virtual memory
    vm_setConfig(kernelPagetable);
    vm_enable();
}
