#pragma once

#include "defs.h"

enum
{
    PAGE_SIZE = 4096,

    PERIPHERAL_BASE = 0xFE000000, // 0xFE00_0000
    LOWEST_USED_PERIPHERAL = PERIPHERAL_BASE + 0x200000,
    HIGHEST_USED_PERIPHERAL = PERIPHERAL_BASE + 0x216000,
    USED_PERIPHERAL_SIZE = (HIGHEST_USED_PERIPHERAL - LOWEST_USED_PERIPHERAL),

    P_RAM_START = 0x40000000,               // 0x4000_0000
    P_RAM_END = 0xFC000000,                 // 0xFC00_0000
    P_RAM_SIZE = (P_RAM_END - P_RAM_START), // In bytes

    V_RAM_START = 0x200000, // 0x20_0000 (after 2MB)
    V_RAM_SIZE = 0x100000,  // 0x10_0000 (1MB)
    V_RAM_END = (V_RAM_START + V_RAM_SIZE)
};

#define PAGE_MASK (PAGE_SIZE - 1)
#define PAGE_ROUND_DOWN(addr) ((addr) & ~PAGE_MASK)
#define PAGE_ROUND_UP(addr) (PAGE_ROUND_DOWN(addr + PAGE_SIZE - 1))

// Addresses set by linker

extern char _start[]; // = _text_start
extern char _end[];   // = _data_end

extern char _text_start[]; // Page aligned
extern char _text_end[];
static inline uint kernelExecutableSize() { return _text_end - _text_start; }

extern char _data_start[]; // Page aligned (First empty page after _text_end (PAGE_ROUND_DOWN(_text_end) + 1))
extern char _data_end[];
static inline uint kernelDataSize() { return _data_end - _data_start; }

extern char _stack_bottom[]; // Page aligned
extern char _stack_top[];    // Page aligned
static inline uint kernelStackSize() { return _stack_top - _stack_bottom; }

void mem_set(void *start, byte value, uint size);