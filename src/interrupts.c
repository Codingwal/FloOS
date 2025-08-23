#include "interrupts.h"
#include "mem.h"
#include "error.h"
#include "cpu.h"
#include "io.h"

// GIC-400 Information:
// CoreLink GIC-400 Generic Interrupt Controller Technical Reference Manual (programmers model)
// ARM Generic Interrupt Controller Architecture Specification

// GIC-400 Distributor registers
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

// GIC-400 CPU interfaces
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

Func interruptHandlers[128] = {NULL};

static void interrupts_enableInterrupt(uint id)
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
    interrupts_enableInterrupt(id);
    ARMC_REGS->irq0_set_en_2 |= 1 << armcId;
}
void interrupts_addARMCInterruptHandler(uint armcId, Func handler)
{
    assert(interruptHandlers[armcId + 64] == NULL, "handler already present");
    interruptHandlers[armcId + 64] = handler;
}

// The entire value of interruptAcknowledge is preserved for compatability,
// but [9:0] are the ones containing the relevant id
void interrupts_handleIRQ(void)
{
    uint32 interruptId = GICC_REGS->interruptAcknowledge;

    // Handle IRQ
    uint32 id = interruptId & BITMASK(10);
    assert(interruptHandlers[id] != NULL, "missing interrupt handler");
    interruptHandlers[id]();

    // Notify GIC400 that the IRQ has been handled
    GICD_REGS->interruptClearPending[interruptId / 8] |= 1 << (interruptId % 8);
    GICC_REGS->endOfInterrupt = interruptId;
    GICC_REGS->deactivateInterrupt = interruptId;

    ARMC_REGS->irq0_pending2 = 0;
}

void interrupts_enable(void)
{
    cpu_sysregs_daif_write(0);
}
void interrupts_disable(void)
{
    cpu_sysregs_daif_write(BITMASK(4) << 6); // Set [9:6] to 1
}

void interrupts_init(void)
{
    GICC_REGS->priorityMask = 0xFFFF; // Enable interrupts of all priorities (Only priority values lower than this one are forwarded)
    GICD_REGS->control = 1;           // Enable distributor (interrupt forwarding)
    GICC_REGS->control = 1;           // Enable cpu interface (signaling of interrupts)
}