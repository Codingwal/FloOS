#pragma once

#include "defs.h"

// CoreLink GIC-400 Generic Interrupt Controller Technical Reference Manual (programmers model)
// ARM Generic Interrupt Controller Architecture Specification

// bcm2711-peripherals.pdf, page 93
#define GIC_BASE 0xFF840000

// Distributor registers
OFFSET_STRUCT(GICDistributorRegs,
              OFFSET_MEMBER_0(volatile uint32 control);
              OFFSET_MEMBER(0x004, volatile uint32 controllerType);
              OFFSET_MEMBER(0x008, volatile uint32 implementerId);
              // ...
              OFFSET_MEMBER(0x100, volatile byte interruptEnable[64]);       // (1b per interrupt)
              OFFSET_MEMBER(0x180, volatile byte interruptDisable[64]);      // (1b per interrupt)
              OFFSET_MEMBER(0x200, volatile byte interruptSetPending[64]);   // (1b per interrupt)
              OFFSET_MEMBER(0x280, volatile byte interruptClearPending[64]); // (1b per interrupt)
              // ...
              OFFSET_MEMBER(0x800, volatile byte interruptTargetProcessor[512]); // (1B per interrupt)
              OFFSET_MEMBER(0xC00, volatile byte interruptConfig[128]););        // (2b per interrupt)
#define GICD_REGS ((volatile GICDistributorRegs *)(GIC_BASE + 0x1000))

// CPU interfaces
OFFSET_STRUCT(GICCPUInterfaces,
              OFFSET_MEMBER_0(volatile uint32 control);
              OFFSET_MEMBER(0x004, volatile uint32 priorityMask);
              OFFSET_MEMBER(0x008, volatile uint32 binaryPoint);
              OFFSET_MEMBER(0x00C, volatile uint32 interruptAcknowledge);
              OFFSET_MEMBER(0x010, volatile uint32 endOfInterrupt);
              OFFSET_MEMBER(0x014, volatile uint32 runningPriority);
              OFFSET_MEMBER(0x018, volatile uint32 highestPriorityPending);
              // ...
              OFFSET_MEMBER(0x1000, volatile uint32 deactivateInterrupt););
#define GICC_REGS ((volatile GICCPUInterfaces *)(GIC_BASE + 0x2000))