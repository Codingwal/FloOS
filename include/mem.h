#pragma once

#include "defs.h"

enum
{
    PAGE_SIZE = 4096,

    PERIPHERAL_BASE = 0xFE000000, // 0xFE00_0000
    PERIPHERAL_SIZE = 0x300000,   // 0x30_0000

    // PERIPHERAL_END = 0x100000000, // 0x1_0000_0000
    // PERIPHERAL_SIZE = PERIPHERAL_END - PERIPHERAL_BASE,

    MEM_START = 0x40000000,           // 0x4000_0000
    MEM_END = 0xFC000000,             // 0xFC00_0000
    MEM_SIZE = (MEM_END - MEM_START), // In bytes

    STACK_TOP = 0x80000,                     // 0x8_0000
    STACK_BOTTOM = 0x70000,                  // 0x1_0000
    STACK_SIZE = (STACK_TOP - STACK_BOTTOM), // In bytes
};

#define PAGE_MASK (PAGE_SIZE - 1)
#define PAGE_ROUND_DOWN(addr) ((addr) & ~PAGE_MASK)
#define PAGE_ROUND_UP(addr) (PAGE_ROUND_DOWN(addr) + PAGE_SIZE)

// The start and end of the kernel (executable and data)
extern char _start[]; // 0x8_0000
extern char _end[];

void mem_set(void *start, byte value, uint size);