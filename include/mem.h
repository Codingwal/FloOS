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