#include "vm.h"
#include "pageAlloc.h"
#include "mem.h"
#include "error.h"
#include "cpu.h"
#include "io.h"
#include "cpu.h"

// See https://developer.arm.com/documentation/ddi0487/fc/
// page 2571: virtual address structure
// page 2589: table descriptor structure
// page 2591: page descriptor structure
// page 2595: page descriptor attribute fields
// page 2600: access permissions
// 2598

enum
{
    VALID = 1,
    TABLE_DESCRIPTOR = 2, // 0b10
    PAGE_DESCRIPTOR = 2,  // 0b10
};

typedef union PagetableEntry
{
    struct
    {
        uint64 valid : 1;
        uint64 entryType : 1;
    };
    struct TableDescriptor
    {
        uint64 valid : 1;     // should be 1
        uint64 entryType : 1; // should be 1
        uint64 ta : 14;
        uint64 : 32; // address bits [47:16]
        uint64 : 16;
    } tableDescriptor;
    struct PageDescriptor
    {
        uint64 valid : 1;     // should be 1
        uint64 entryType : 1; // should be 1
        uint64 attrIndx : 3;  // index into mair_elX
        uint64 ns : 1;        // non secure bit
        uint64 ap : 2;        // ap[2:1] access permissions
        uint64 sh : 2;        // shareability field
        uint64 af : 1;        // access flag
        uint64 ng : 1;        // not global
        uint64 : 4;
        uint64 : 32; // address bits [47:16]
        uint64 : 4;
        uint64 contiguous : 1; // contiguous bit
        uint64 pxn : 1;        // privileged execute-never
        uint64 xn : 1;         // (unpriviledged-) execute-never
        uint64 softwareFlags : 4;
        uint64 : 5;
    } pageDescriptor;
    struct BlockDescriptor
    {
        uint64 valid : 1;     // should be 1
        uint64 entryType : 1; // should be 0
        uint64 lowerAttr : 10;
        uint64 oa : 4;
        uint64 nT : 1;
        uint64 : 31; // address bits [47:n] // L1: n=42, L2: n=29
        uint64 : 2;
        uint64 upperAttr : 14;
    } blockDescriptor;
    uint64 address; // set address using entry.address |= ptr
    uint64 raw;
} PagetableEntry;
REQUIRE_SIZE(PagetableEntry, 8);
int x = sizeof(struct PageDescriptor);
struct Pagetable
{
    PagetableEntry entries[512];
};

Pagetable *kernelPagetable;

#define PTE2PA(x) ((void *)(x & (BITMASK(48) - BITMASK(12))))
#define PA2PTE(x) ((uint64)x)

static PagetableEntry vm_getFlags(enum MairIdxs mairIdx, bool pxn, enum AccessPerms ap)
{
    PagetableEntry entry = {0};
    entry.valid = true;
    entry.entryType = 1;
    entry.pageDescriptor.attrIndx = mairIdx;
    entry.pageDescriptor.ap = ap;
    entry.pageDescriptor.pxn = pxn;
    entry.pageDescriptor.af = 1;                               // cpu generates a fault if this is set to 0
    entry.pageDescriptor.sh = (mairIdx == IDX_NORMAL) ? 3 : 0; // inner shareable if normal memory
    return entry;
}

// Get the page table entry at level 3, which points to a 4KB memory region
static PagetableEntry *vm_getPTE(Pagetable *table, void *virtualAddr)
{
    assert(table != NULL, "table is NULL");

    if (vm_isEnabled())
    {
        // Shift indices down and use 511 as the first index (kernelPagetable[511] points to kernelPagetable)
        uint64 va = (uint64)virtualAddr;
        va >>= 9;                  // loops once -> L1 index will be the actual L0 index and so on
        va &= ~BITMASK(3);         // Remove the 3 least significant bits. [11:3] are the L3 index
        va |= ((uint64)511 << 39); // L0 index needs to be the last entry, which is the loop entry
        return (PagetableEntry *)va;
    }

    uint lsb = 39;
    for (uint level = 0; true; level++)
    {
        uint index = ((uint64)virtualAddr >> lsb) & BITMASK(9); // Extract [47..39], [38..30], [29..21] or [20..12], depending on level, and shift it down

        // If the entry points to a memory region and not another page table (true for all entries in a level 3 page table), return a pointer to it
        if (level == 3)
            return &table->entries[index];

        PagetableEntry entry = table->entries[index];
        assert(entry.valid, "invalid entry");
        table = (Pagetable *)PTE2PA(entry.raw); // Extract [47..12], which is the next level page table adress ([11..00] is always zero)

        lsb -= 9;
    }
}

void *vm_va2pa(Pagetable *table, void *virtualAddr)
{
    assert(table != NULL, "table is NULL");

    PagetableEntry *entryPtr = vm_getPTE(table, virtualAddr);
    if (entryPtr->valid)
        return PTE2PA(entryPtr->address);
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

    PagetableEntry flags = vm_getFlags(mairIdx, pxn, ap);

    byte *lastPage = va + size;
    while (va < lastPage)
    {
        PagetableEntry *entry = vm_getPTE(table, va);
        assert(!entry->valid || replace, "entry exists but replacing is disallowed");
        *entry = flags;
        entry->address |= (uint64)pa;

        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }

    cpu_invalidateTLB();
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
        PagetableEntry *entry = vm_getPTE(table, va);
        entry->raw = 0;
        va += PAGE_SIZE;
    }

    cpu_invalidateTLB();
}

void *vm_getVaRange(Pagetable *table, uint size)
{
    assert(table != NULL, "table is NULL");
    assert(size % PAGE_SIZE == 0, "size must be page aligned");
    size /= PAGE_SIZE;

    uint count = 0;
    for (byte *va = (byte *)V_RAM_START; (uint64)va < V_RAM_END; va += PAGE_SIZE)
    {
        PagetableEntry *entryPtr = vm_getPTE(table, va);

        // If there already is a physical address mapped to this virtual address, reset the free PTEs counter and continue
        if (entryPtr->valid)
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

// Get the page table entry at level 3, which points to a 4KB memory region
static PagetableEntry *vm_getPTEAllocating(Pagetable *table, void *virtualAddr)
{
    assert(table != NULL, "table is NULL");

    uint lsb = 39;
    for (uint level = 0; true; level++)
    {
        uint index = ((uint64)virtualAddr >> lsb) & BITMASK(9); // Extract [47..39], [38..30], [29..21] or [20..12], depending on level, and shift it down

        PagetableEntry *entry = &table->entries[index];

        // If the entry points to a memory region and not another page table (true for all entries in a level 3 page table), return a pointer to it
        if (level == 3)
            return entry;

        if (!entry->valid)
        {
            void *ptr = pageAlloc_alloc();
            mem_set(ptr, 0, PAGE_SIZE);
            *entry = vm_getFlags(IDX_NORMAL_NO_CACHE, true, PRIV_RW);
            entry->address |= (uint64)ptr;
        }

        table = (Pagetable *)PTE2PA(entry->address); // Extract [47..12], which is the next level page table adress

        lsb -= 9;
    }
}
static void vm_mapAllocating(Pagetable *table, void *virtualAddr, void *physicalAddr, uint size, enum MairIdxs mairIdx, bool pxn, enum AccessPerms ap)
{
    assert(!vm_isEnabled(), "virtual memory must be disabled");
    assert(table != NULL, "table is NULL");
    assert(size % PAGE_SIZE == 0, "size must be page aligned");
    assert((uint64)virtualAddr % PAGE_SIZE == 0, "virtual address must be page aligned");
    assert((uint64)physicalAddr % PAGE_SIZE == 0, "physical address must be page aligned");

    byte *va = virtualAddr;
    byte *pa = physicalAddr;

    PagetableEntry flags = vm_getFlags(mairIdx, pxn, ap);

    byte *lastPage = va + size;
    while (va < lastPage)
    {
        PagetableEntry *entry = vm_getPTEAllocating(table, va);
        assert(!entry->valid, "entry already exists");
        *entry = flags;
        entry->address = (uint64)pa;

        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }
}

static void vm_setConfig(const Pagetable *kernelPagetable)
{
    assert(((uint64)kernelPagetable & ~BITMASK(7)) == (uint64)kernelPagetable, "kernel pagetable is not sufficiently aligned");

    // Set translation table base address
    ttbr0_el1 ttbr0 = {0};
    ttbr0.baddr = (uint64)kernelPagetable;
    cpu_sysregs_ttbr0_el1_write(ttbr0);

    // Set information about the virtual address structure
    tcr_el1 tcr = {0};
    tcr.t0sz = 16; // size of the region addressed by ttbr0 =2^(64-16)=2^48 (=> 48bits)
    tcr.epd0 = 0;  // enable ttbr0 translation table walks
    tcr.irgn0 = 1; // ttbr0 inner cacheability: Write-Back Read-Allocate Write-Allocate Cacheable
    tcr.orgn0 = 1; // ttbr0 outer cacheability: Write-Back Read-Allocate Write-Allocate Cacheable
    tcr.sh0 = 3;   // ttbr0: inner shareable
    tcr.tg0 = 0;   // 4kB granule size for ttbr0
    tcr.t1sz = 16; // size of the region addressed by ttbr0 =2^(64-16)=2^48 (=> 48bits)
    tcr.a1 = 0;    // ttbr0 defines the ASID (ttbr1.ASID is ignored)
    tcr.epd1 = 0;  // enable ttbr1 translation table walks
    tcr.irgn1 = 1; // ttbr1 inner cacheability: Write-Back Read-Allocate Write-Allocate Cacheable
    tcr.orgn1 = 1; // ttbr1 outer cacheability: Write-Back Read-Allocate Write-Allocate Cacheable
    tcr.sh1 = 3;   // ttbr1: inner shareable
    tcr.tg1 = 2;   // 4kB granule size for ttbr1
    tcr.ips = 5;   // intermediate physical address size = 48 bits
    tcr.as = 0;    // the upper 8 bits of ttbrX are ignored by hardware
    tcr.tbi0 = 1;  // ignore top byte for ttbr0 addresses
    tcr.tbi1 = 1;  // ignore top byte for ttbr1 addresses
    cpu_sysregs_tcr_el1_write(tcr);

    // Set memory attributes
    // https://documentation-service.arm.com/static/63a43e333f28e5456434e18b (learn_the_architecture_-_aarch64_memory_attributes_and_properties)
    // IMPORTANT: this must match the MairIdxs enum
    mair_el1 mair = {0};
    mair.attr0 = 0xFF; // normal
    mair.attr1 = 68;   // normal no cache (68 = 0b01000100)
    mair.attr2 = 0;    // device
    cpu_sysregs_mair_el1_write(mair);

    cpu_instrSyncBarrier();
}

void vm_enable(void)
{
    sctlr_el1 reg = cpu_sysregs_sctlr_el1_read();
    reg.m = 1; // Enable virtual memory
    reg.i = 1; // Enable instruction caches at el1
    cpu_sysregs_sctlr_el1_write(reg);

    cpu_instrSyncBarrier();
}

bool vm_isEnabled(void)
{
    sctlr_el1 sysControlReg = cpu_sysregs_sctlr_el1_read();
    return sysControlReg.m == 1; // Check the virtual memory enabled bit
}

void vm_init(void)
{
    kernelPagetable = (Pagetable *)pageAlloc_alloc();
    mem_set(kernelPagetable, 0, PAGE_SIZE);

    // Map kernel addresses (peripherals, kernel code, ...)

    // Kernel executable
    vm_mapAllocating(kernelPagetable, _text_start, _text_start, PAGE_ROUND_UP(kernelExecutableSize()), IDX_NORMAL, false, PRIV_R);

    // Kernel data
    vm_mapAllocating(kernelPagetable, _data_start, _data_start, PAGE_ROUND_UP(kernelDataSize()), IDX_NORMAL, true, PRIV_RW);

    // Stack
    vm_mapAllocating(kernelPagetable, (void *)_stack_bottom, (void *)_stack_bottom, PAGE_ROUND_UP(kernelStackSize()), IDX_NORMAL, true, PRIV_RW);

    // Peripherals (uart, timer, ...)
    vm_mapAllocating(kernelPagetable, (void *)PERIPHERAL_BASE, (void *)PERIPHERAL_BASE, PERIPHERAL_SIZE, IDX_DEVICE, true, PRIV_RW);

    // GIC400 peripherals
    vm_mapAllocating(kernelPagetable, (void *)GIC_BASE, (void *)GIC_BASE, GIC_SIZE, IDX_DEVICE, true, PRIV_RW);

    // Setup and enable the mmu and virtual memory
    vm_setConfig(kernelPagetable);
    vm_enable();
}
