#pragma once

#include "defs.h"

/* instruction wrappers */

static inline void cpu_instrSyncBarrier()
{
    asm volatile("isb\n");
}
static inline void cpu_dataSyncBarrier()
{
    asm volatile("dsb SY\n");
}

/* sysregs operation wrappers */

inline static uint64 cpu_sysregs_sctlr_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, sctlr_el1\n" : "=r"(value) :);
    return value;
}
inline static void cpu_sysregs_sctlr_el1_write(uint64 value)
{
    asm volatile("msr sctlr_el1, %0\n" : : "r"(value));
}

inline static uint64 cpu_sysregs_ttbr0_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, ttbr0_el1\n" : "=r"(value) :);
    return value;
}
inline static void cpu_sysregs_ttbr0_el1_write(uint64 value)
{
    asm volatile("msr ttbr0_el1, %0\n" : : "r"(value));
}

inline static uint64 cpu_sysregs_ttbr1_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, ttbr1_el1\n" : "=r"(value) :);
    return value;
}
inline static void cpu_sysregs_ttbr1_el1_write(uint64 value)
{
    asm volatile("msr ttbr1_el1, %0\n" : : "r"(value));
}

inline static uint64 cpu_sysregs_tcr_el1_read()
{
    uint64 value;
    asm volatile("mrs %0,tcr_el1\n" : "=r"(value) :);
    return value;
}
inline static void cpu_sysregs_tcr_el1_write(uint64 value)
{
    asm volatile("msr tcr_el1, %0\n" : : "r"(value));
}

inline static uint64 cpu_sysregs_mair_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, mair_el1\n" : "=r"(value) :);
    return value;
}
inline static void cpu_sysregs_mair_el1_write(uint64 value)
{
    asm volatile("msr mair_el1, %0\n" : : "r"(value));
}

inline static uint64 cpu_sysregs_CurrentEL_read()
{
    uint64 value;
    asm volatile("mrs %0, CurrentEL\n" : "=r"(value) :);
    return value;
}

/* Utility functions */

static inline uint cpu_getExceptionLevel()
{
    return cpu_sysregs_CurrentEL_read() >> 2;
}

static inline void cpu_invalidateTLB()
{
    // https://f.osdev.org/viewtopic.php?t=36412
    asm volatile("tlbi VMALLE1\n");
    cpu_dataSyncBarrier();
    cpu_instrSyncBarrier();
}