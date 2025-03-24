#pragma once

static inline void cpu_dataSyncBarrier()
{
    asm volatile("dsb SY\n");
}
static inline void cpu_instrSyncBarrier()
{
    asm volatile("isb\n");
}
static inline void cpu_invalidateTLB()
{
    // https://f.osdev.org/viewtopic.php?t=36412
    asm volatile("tlbi VMALLE1\n");
    cpu_dataSyncBarrier();
    cpu_instrSyncBarrier();
}