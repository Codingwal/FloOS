#pragma once

#include "defs.h"

#define PAGE_SIZE 4096

#define PERIPHERAL_BASE 0xFE000000                         // 0xFE00_0000
#define PERIPHERAL_END 0xFE216000                          // 0xFE21_6000
#define PERIPHERAL_SIZE (PERIPHERAL_END - PERIPHERAL_BASE) // In bytes

#define P_RAM_START 0x40000000               // 0x4000_0000
#define P_RAM_END 0xFC000000                 // 0xFC00_0000
#define P_RAM_SIZE (P_RAM_END - P_RAM_START) // In bytes

#define V_RAM_START 0x200000                 // 0x20_0000 (2MiB)
#define V_RAM_SIZE 0x800000                  // 0x80_0000 (8MiB)
#define V_RAM_END (V_RAM_START + V_RAM_SIZE) // In bytes

#define GIC_BASE 0xFF840000           // 0xFF84_0000 (bcm2711-peripherals.pdf, page 93)
#define GIC_END 0xFF850000            // 0xFF85_0000
#define GIC_SIZE (GIC_END - GIC_BASE) // In bytes

#define PAGE_MASK (PAGE_SIZE - 1)
#define PAGE_ROUND_DOWN(addr) ROUND_DOWN(addr, PAGE_SIZE)
#define PAGE_ROUND_UP(addr) ROUND_UP(addr, PAGE_SIZE)

// Addresses set by linker

extern char _start[]; // = _text_start
extern char _end[];   // = _data_end

extern char _text_start[]; // Page aligned
extern char _text_end[];
static inline uint kernelExecutableSize(void) { return _text_end - _text_start; }

extern char _data_start[]; // Page aligned (First empty page after _text_end (PAGE_ROUND_DOWN(_text_end) + 1))
extern char _data_end[];
static inline uint kernelDataSize(void) { return _data_end - _data_start; }

extern char _stack_bottom[]; // Page aligned
extern char _stack_top[];    // Page aligned
static inline uint kernelStackSize(void) { return _stack_top - _stack_bottom; }

void mem_set(void *start, byte value, uint size);