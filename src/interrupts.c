#include "interrupts.h"
#include "gic400.h"
#include "mem.h"
#include "io.h"
#include "error.h"
#include "cpu.h"

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
static void interrupts_clearPending(uint id)
{
    uint n = id / 8;
    uint offset = id % 8;
    GICD_REGS->interruptClearPending[n] |= 1 << offset;
}

void interrupts_enableARMCInterrupt(uint armcId)
{
    uint id = armcId + 64;
    interrupts_enable(id);
    ARMC_REGS->irq0_set_en_2 |= 1 << armcId;
}

Func interruptHandlers[128] = {NULL};
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
    printf("Interrupt request (id = %d)\n", interruptId);

    // Handle IRQ
    uint32 id = interruptId & BITMASK(10);
    assert(interruptHandlers[id] != NULL, "missing interrupt handler");
    interruptHandlers[id]();

    // Notify GIC400 that the IRQ has been handled
    interrupts_clearPending(interruptId);
    GICC_REGS->endOfInterrupt = interruptId;
    GICC_REGS->deactivateInterrupt = interruptId;

    ARMC_REGS->irq0_pending2 = 0;

    print("Reset pending interrupts line\n");

    assert(cpu_exceptionLevel() == 1, "wrong exception level");

    cpu_sysregs_daif_write(0);
    // cpu_exceptionReturn();
}

void interrupts_init(void)
{
    GICC_REGS->priorityMask = 0xFFFF; // Enable interrupts of all priorities (Only priority values lower than this one are forwarded)
    GICD_REGS->control = 1;           // Enable distributor (interrupt forwarding)
    GICC_REGS->control = 1;           // Enable cpu interface (signaling of interrupts)
}

void interrupts_check(void)
{
    printf("%ub, ", ARMC_REGS->irq0_pending2);
}