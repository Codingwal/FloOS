#include "interrupts.h"
#include "defs.h"
#include "mem.h"
#include "io.h"
#include "error.h"

// bcm2711-peripherals.pdf, page 93

// Distributor registers
// CoreLink GIC-400 Generic Interrupt Controller Technical Reference Manual (programmers model)
// ARM Generic Interrupt Controller Architecture Specification
OFFSET_STRUCT(GICDistributorRegs,
              OFFSET_MEMBER_0(volatile uint32 control);
              OFFSET_MEMBER(0x004, volatile uint32 controllerType);
              OFFSET_MEMBER(0x008, volatile uint32 implementerId);
              // ...
              OFFSET_MEMBER(0x100, volatile byte interruptEnable[64]);  // (1b per interrupt)
              OFFSET_MEMBER(0x180, volatile byte interruptDisable[64]); // (1b per interrupt)
              // ...
              OFFSET_MEMBER(0x800, volatile byte interruptTargetProcessor[512]); // (1B per interrupt)
              OFFSET_MEMBER(0xC00, volatile byte interruptConfig[128]););        // (2b per interrupt)
#define GIC_BASE 0xFF840000
#define GICD_REGS ((volatile GICDistributorRegs *)GIC_BASE)

// ARMC interrupt registers
// bcm2711-peripherals.pdf, page 101
OFFSET_STRUCT(ARMCRegs,
              // Enabled interrupt pending (will be OR-ed with core interrupt line)
              OFFSET_MEMBER(0x200, volatile uint32 irq0_pending0);
              OFFSET_MEMBER(0x204, volatile uint32 irq0_pending1);
              OFFSET_MEMBER(0x208, volatile uint32 irq0_pending2);

              // Write to set interrupt enable bits
              OFFSET_MEMBER(0x210, volatile uint32 irq0_set_en_0);
              OFFSET_MEMBER(0x214, volatile uint32 irq0_set_en_1);
              OFFSET_MEMBER(0x218, volatile uint32 irq0_set_en_2);

              // Write to clear interrupt enable bits
              OFFSET_MEMBER(0x220, volatile uint32 irq0_clr_en_0);
              OFFSET_MEMBER(0x224, volatile uint32 irq0_clr_en_1);
              OFFSET_MEMBER(0x228, volatile uint32 irq0_clr_en_2);

              // Interrupt line bits (unmasked -> also shows disabled interrupts)
              OFFSET_MEMBER(0x230, volatile uint32 irq_status0);
              OFFSET_MEMBER(0x234, volatile uint32 irq_status1);
              OFFSET_MEMBER(0x238, volatile uint32 irq_status2););
#define ARMC_BASE (PERIPHERAL_BASE + 0xb000)
#define ARMC_REGS ((volatile ARMCRegs *)ARMC_BASE)

static void interrupts_enable(uint id)
{
    // Enable interrupt
    uint n = id / 8;
    uint offset = id % 8;
    GICD_REGS->interruptEnable[n] |= 1 << offset;

    // Set target core to cpu0
    GICD_REGS->interruptTargetProcessor[id] = 1;
}
void interrupts_enableARMCInterrupt(uint armcId)
{
    uint id = armcId + 64;
    interrupts_enable(id);
    ARMC_REGS->irq0_set_en_2 |= 1 << armcId;
}

void interrupts_init(void)
{
    GICD_REGS->control = 1; // Enable interrupt forwarding
}

void interrupts_check(void)
{
    printf("%ub, ", ARMC_REGS->irq0_pending2);
}
