#pragma once

#include "defs.h"

/* instruction wrappers */

static inline void cpu_instrSyncBarrier(void)
{
    asm volatile("isb\n");
}
static inline void cpu_dataSyncBarrier(void)
{
    asm volatile("dsb SY\n");
}

static inline void cpu_exceptionReturn(void)
{
    asm volatile("eret\n");
}

/* sysregs */

// System control register
typedef union sctlr_el1
{
    struct
    {
        uint64 m : 1;   // mmu enable
        uint64 a : 1;   // alignment check enable
        uint64 c : 1;   // stage 1 cacheability control
        uint64 sa : 1;  // sp alignment check enable (el1)
        uint64 sa0 : 1; // sp alignment check enable (el0)
        uint64 : 4;
        uint64 uma : 1; // user mask access (0 = trap el0 attempt to access PSTATE.{D,A,I,F}, 1 = don't trap)
        uint64 : 2;
        uint64 i : 1; // stage 1 instruction access cacheability control (el0 & el1)
        uint64 : 1;
        uint64 dze : 1;
        uint64 uct : 1;
        uint64 : 48; // ...
    };
    uint64 raw;
} sctlr_el1;
REQUIRE_SIZE(sctlr_el1, 8);
static inline sctlr_el1 cpu_sysregs_sctlr_el1_read(void)
{
    sctlr_el1 value;
    asm volatile("mrs %0, sctlr_el1\n" : "=r"(value.raw) :);
    return value;
}
static inline void cpu_sysregs_sctlr_el1_write(sctlr_el1 value)
{
    asm volatile("msr sctlr_el1, %0\n" : : "r"(value.raw));
}

// Translation table base register 0
typedef union ttbr0_el1
{
    struct
    {
        uint64 baddr : 48;
        uint64 asid : 16;
    };
    uint64 raw;
} ttbr0_el1;
REQUIRE_SIZE(ttbr0_el1, 8);
static inline ttbr0_el1 cpu_sysregs_ttbr0_el1_read(void)
{
    ttbr0_el1 value;
    asm volatile("mrs %0, ttbr0_el1\n" : "=r"(value.raw) :);
    return value;
}
static inline void cpu_sysregs_ttbr0_el1_write(ttbr0_el1 value)
{
    asm volatile("msr ttbr0_el1, %0\n" : : "r"(value.raw));
}

// Translation table base register 1
typedef union ttbr1_el1
{
    struct
    {
        uint64 : 1;
        uint64 baddr : 47;
        uint64 asid : 16;
    };
    uint64 raw;
} ttbr1_el1;
REQUIRE_SIZE(ttbr1_el1, 8);
static inline ttbr1_el1 cpu_sysregs_ttbr1_el1_read(void)
{
    ttbr1_el1 value;
    asm volatile("mrs %0, ttbr1_el1\n" : "=r"(value.raw) :);
    return value;
}
static inline void cpu_sysregs_ttbr1_el1_write(ttbr1_el1 value)
{
    asm volatile("msr ttbr1_el1, %0\n" : : "r"(value.raw));
}

// Translation control register
typedef union tcr_el1
{
    struct
    {
        uint64 t0sz : 6; // Size offset of the mem region addressed by ttbr0. region size = 2^(64-val) bytes. (= 48 bits)
        uint64 : 1;
        uint64 epd0 : 1;  // Translation table walk disable for ttbr0 (0 = enable ttbr0 translation table walks)
        uint64 irgn0 : 2; // Inner cacheability attribute for mem associated with ttbr0 (0b01 = Write-Back Read-Allocate Write-Allocate Cacheable)
        uint64 orgn0 : 2; // Outer cacheability attribute for mem associated with ttbr0 (0b01 = Write-Back Read-Allocate Write-Allocate Cacheable)
        uint64 sh0 : 2;   // Shareabilty attribute for mem associated with ttbr0 (0b00 = inner shareable)
        uint64 tg0 : 2;   // Granule size for ttbr0 (0b00 = 4KB)
        uint64 t1sz : 6;  // Size offset of the mem region addressed by ttbr1. region size = 2^(64-val) bytes. (= 48 bits)
        uint64 a1 : 1;    // ttbr0 defines the ASID (ttbr1.ASID is ignored)
        uint64 epd1 : 1;  // Translation table walk disable for ttbr1 (1 = disable ttbr1 translation table walks)
        uint64 irgn1 : 2; // Inner cacheability attribute for mem associated with ttbr1 (0b01 = Write-Back Read-Allocate Write-Allocate Cacheable)
        uint64 orgn1 : 2; // Outer cacheability attribute for mem associated with ttbr1 (0b01 = Write-Back Read-Allocate Write-Allocate Cacheable)
        uint64 sh1 : 2;   // Shareabilty attribute for mem associated with ttbr1 (0b00 = innter shareable)
        uint64 tg1 : 2;   // Granule size for ttbr1 (0b10 = 4KB)
        uint64 ips : 3;   // Intermediate physical address size (0b101 = 48 bits)
        uint64 : 1;
        uint64 as : 1;   // the upper 8 bits of ttbrX are ignored by hardware
        uint64 tbi0 : 1; // 1 = Top byte ignored in address calculation with ttbr0
        uint64 tbi1 : 1; // 1 = Top byte ignored in address calculation with ttbr1
    };
    uint64 raw;
} tcr_el1;
REQUIRE_SIZE(tcr_el1, 8);
static inline tcr_el1 cpu_sysregs_tcr_el1_read(void)
{
    tcr_el1 value;
    asm volatile("mrs %0,tcr_el1\n" : "=r"(value.raw) :);
    return value;
}
static inline void cpu_sysregs_tcr_el1_write(tcr_el1 value)
{
    asm volatile("msr tcr_el1, %0\n" : : "r"(value.raw));
}

// Memory attribute indirection register
typedef union mair_el1
{
    struct
    {
        byte attr0;
        byte attr1;
        byte attr2;
        byte attr3;
        byte attr4;
        byte attr5;
        byte attr6;
        byte attr7;
    };
    uint64 raw;
} mair_el1;
REQUIRE_SIZE(mair_el1, 8);
static inline mair_el1 cpu_sysregs_mair_el1_read(void)
{
    mair_el1 value;
    asm volatile("mrs %0, mair_el1\n" : "=r"(value.raw) :);
    return value;
}
static inline void cpu_sysregs_mair_el1_write(mair_el1 value)
{
    asm volatile("msr mair_el1, %0\n" : : "r"(value.raw));
}

// Vector base address register
typedef union vbar_el1
{
    uint64 vba;
    uint64 raw;
} vbar_el1;
REQUIRE_SIZE(vbar_el1, 8);
static inline vbar_el1 cpu_sysregs_vbar_el1_read(void)
{
    vbar_el1 value;
    asm volatile("mrs %0, vbar_el1\n" : "=r"(value.raw) :);
    return value;
}
static inline void cpu_sysregs_vbar_el1_write(vbar_el1 value)
{
    asm volatile("msr vbar_el1, %0\n" : : "r"(value.raw));
}

// Interrupt mask bits
typedef union daif
{
    struct
    {
        uint64 : 6;
        uint64 f : 1; // mask FIQ exceptions
        uint64 i : 1; // mask IRQ exceptions
        uint64 a : 1; // mask SError exceptions
        uint64 d : 1; // mask watchpoint, breakpoint and software step exceptions targeted at the current el
        uint64 : 54;
    };
    uint64 raw;
} daif;
REQUIRE_SIZE(daif, 8);
static inline daif cpu_sysregs_daif_read(void)
{
    daif value;
    asm volatile("mrs %0, daif\n" : "=r"(value.raw) :);
    return value;
}
static inline void cpu_sysregs_daif_write(daif value)
{
    asm volatile("msr daif, %0\n" : : "r"(value.raw));
}

// Exception syndrome register
typedef union esr_el1
{
    struct
    {
        uint64 iss : 25; // instruction specific syndrome
        uint64 il : 1;   // instruction length (0=16b, 1=32b)
        uint64 ec : 6;   // exception class
        uint64 : 32;
    };
    uint64 raw;
} esr_el1;
REQUIRE_SIZE(esr_el1, 8);
static inline esr_el1 cpu_sysregs_esr_el1_read(void)
{
    esr_el1 value;
    asm volatile("mrs %0, esr_el1\n" : "=r"(value.raw) :);
    return value;
}

// Exception link register
typedef union elr_el1
{
    uint64 address; // The address to return to
    uint64 raw;
} elr_el1;
REQUIRE_SIZE(elr_el1, 8);
static inline elr_el1 cpu_sysregs_elr_el1_read(void)
{
    elr_el1 value;
    asm volatile("mrs %0, elr_el1\n" : "=r"(value.raw) :);
    return value;
}

// Fault address register
typedef union far_el1
{
    uint64 address; // The faulting virtual address
    uint64 raw;
} far_el1;
REQUIRE_SIZE(far_el1, 8);
static inline far_el1 cpu_sysregs_far_el1_read(void)
{
    far_el1 value;
    asm volatile("mrs %0, far_el1\n" : "=r"(value.raw) :);
    return value;
}

// Current exception level
typedef union CurrentEL
{
    struct
    {
        uint64 : 2;
        uint64 el : 2; // Current exception level
        uint64 : 60;
    };
    uint64 raw;
} CurrentEL;
REQUIRE_SIZE(CurrentEL, 8);
static inline CurrentEL cpu_sysregs_CurrentEL_read(void)
{
    CurrentEL value;
    asm volatile("mrs %0, CurrentEL\n" : "=r"(value.raw) :);
    return value;
}

/* Utility functions */

static inline uint cpu_exceptionLevel(void)
{
    return cpu_sysregs_CurrentEL_read().el;
}

static inline void cpu_invalidateTLB(void)
{
    // see https://f.osdev.org/viewtopic.php?t=36412
    asm volatile("tlbi vmalle1\n");
    cpu_dataSyncBarrier();
    cpu_instrSyncBarrier();
}