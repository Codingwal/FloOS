#pragma once
#include "defs.h"

inline static uint64 sysregs_sctlr_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, sctlr_el1\n" : "=r"(value) :);
    return value;
}
inline static void sysregs_sctlr_el1_write(uint64 value)
{
    asm volatile("msr sctlr_el1, %0\n" : : "r"(value));
}

inline static uint64 sysregs_ttbr0_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, ttbr0_el1\n" : "=r"(value) :);
    return value;
}
inline static void sysregs_ttbr0_el1_write(uint64 value)
{
    asm volatile("msr ttbr0_el1, %0\n" : : "r"(value));
}

inline static uint64 sysregs_ttbr1_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, ttbr1_el1\n" : "=r"(value) :);
    return value;
}
inline static void sysregs_ttbr1_el1_write(uint64 value)
{
    asm volatile("msr ttbr1_el1, %0\n" : : "r"(value));
}

inline static uint64 sysregs_tcr_el1_read()
{
    uint64 value;
    asm volatile("mrs %0,tcr_el1\n" : "=r"(value) :);
    return value;
}
inline static void sysregs_tcr_el1_write(uint64 value)
{
    asm volatile("msr tcr_el1, %0\n" : : "r"(value));
}

inline static uint64 sysregs_mair_el1_read()
{
    uint64 value;
    asm volatile("mrs %0, mair_el1\n" : "=r"(value) :);
    return value;
}
inline static void sysregs_mair_el1_write(uint64 value)
{
    asm volatile("msr mair_el1, %0\n" : : "r"(value));
}

inline static uint64 sysregs_CurrentEL_read()
{
    uint64 value;
    asm volatile("mrs %0, CurrentEL\n" : "=r"(value) :);
    return value;
}