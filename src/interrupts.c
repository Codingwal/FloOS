#include "interrupts.h"
#include "defs.h"
#include "mem.h"
#include "io.h"
#include "error.h"

// bcm2711-peripherals.pdf, page 93
#define GIC_BASE 0xFF840000

// Distributor registers
// CoreLink GIC-400 Generic Interrupt Controller Technical Reference Manual (programmers model)
// ARM Generic Interrupt Controller Architecture Specification
OFFSET_STRUCT(GICDistributorRegs,
              OFFSET_MEMBER_0(volatile uint32 control);
              OFFSET_MEMBER(0x004, volatile uint32 controllerType);
              OFFSET_MEMBER(0x008, volatile uint32 implementerId);
              // ...
              OFFSET_MEMBER(0x100, volatile byte interruptEnable[64]);
              OFFSET_MEMBER(0x180, volatile byte interruptDisable[64]);
              // ...
              OFFSET_MEMBER(0x800, volatile byte interruptTargetProcessor[512]);
              OFFSET_MEMBER(0xC00, volatile byte interruptConfig[128]););

#define GICD_CONTROL ((uint32 *)(GIC_BASE + 0x000))
#define GICD_CONTOLLER_TYPE ((uint32 *)(GIC_BASE + 0x004)) // RO
#define GICD_IMPLEMENTER_ID ((uint32 *)(GIC_BASE + 0x008)) // RO
// ...
#define GICD_IENABLE ((byte *)(GIC_BASE + 0x100))  // byte[64] (1b per int) (512 * 1 / 8)
#define GICD_IDISABLE ((byte *)(GIC_BASE + 0x180)) // byte[64] (1b per int) (512 * 1 / 8)
// ...
#define GICD_ITARGET_PROCESSOR ((byte *)(GIC_BASE + 800)) // byte[512] (1B per int) (512 * 8 / 8)
#define GICD_ICONFIG ((byte *)(GIC_BASE + 0xC00))         // byte[128] (2b per int) (512 * 2 / 8)

// ARMC interrupts
// bcm2711-peripherals.pdf, page 101
struct ARMCRegs
{
    // Enabled interrupt pending (will be OR-ed with core interrupt line)
    volatile uint32 irq0_pending0;
    volatile uint32 irq0_pending1;
    volatile uint32 irq0_pending2;

    // Write to set interrupt enable bits
    volatile uint32 irq0_set_en_0 __attribute__((aligned(0x010)));
    volatile uint32 irq0_set_en_1;
    volatile uint32 irq0_set_en_2;

    // Write to clear interrupt enable bits
    volatile uint32 irq0_clr_en_0 __attribute__((aligned(0x010)));
    volatile uint32 irq0_clr_en_1;
    volatile uint32 irq0_clr_en_2;

    // Interrupt line bits (unmasked -> also shows disabled interrupts)
    volatile uint32 irq_status0 __attribute__((aligned(0x010)));
    volatile uint32 irq_status1;
    volatile uint32 irq_status2;

    // More regs for cpu cores 1, 2 and 3 and FIQs
};
#define ARMC_BASE (PERIPHERAL_BASE + 0xb000)
#define ARMC_REGS ((volatile struct ARMCRegs *)(ARMC_BASE + 0x200))
_Static_assert(offsetof(struct ARMCRegs, irq0_set_en_0) == 0x010, "Invalid offset");
_Static_assert(offsetof(struct ARMCRegs, irq0_clr_en_0) == 0x020, "Invalid offset");
_Static_assert(offsetof(struct ARMCRegs, irq_status0) == 0x030, "Invalid offset");

static void interrupts_enable(uint id)
{
    // Enable interrupt
    uint n = id / 8;
    uint offset = id % 8;
    GICD_IENABLE[n] |= 1 << offset;

    // Set target core to cpu0
    GICD_ITARGET_PROCESSOR[id] = 1;
}
void interrupts_enableARMCInterrupt(uint armcId)
{
    uint id = armcId + 64;
    interrupts_enable(id);
    ARMC_REGS->irq0_set_en_2 |= 1 << armcId;
}

void interrupts_init(void)
{
    *GICD_CONTROL = 1; // Enable interrupt forwarding
}

void interrupts_check(void)
{
    printf("%ub, ", ARMC_REGS->irq0_pending2);
}
