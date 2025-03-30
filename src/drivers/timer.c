#include "drivers/timer.h"
#include "mem.h"
#include "interrupts.h"
#include "io.h"

// BCM2711 Arm Peripherals, page 160 following (Chapter 12: Timer (ARM side))

struct TimerRegs
{
    volatile uint32 load;
    volatile uint32 value; // Read-Only
    volatile uint32 control;
    volatile uint32 irqClear;  // Write-Only
    volatile uint32 rawIrq;    // Read-Only
    volatile uint32 maskedIrq; // Read-Only
    volatile uint32 reload;
    volatile uint32 preDiv;
    volatile uint32 freeCounter;
};
#define TIMER_BASE (PERIPHERAL_BASE + 0xb400)
#define TIMER ((volatile struct TimerRegs *)TIMER_BASE)

static void timer_clearIrq(void)
{
    TIMER->irqClear = 1;
}

void timer_setTimer(uint value)
{
    value *= 4000;
    TIMER->load = value;
    timer_clearIrq();
}

static void timer_handleIRQ(void)
{
    print("Handling timer IRQ.\n");
    timer_clearIrq();
}

void timer_init(void)
{
    uint32 control = 0;
    // [0] reserved
    control |= 1 << 1; // [1] 32BIT -> 32bit counter
    control |= 2 << 2; // [3:2] DIV -> pre-scale is clock / 256
    // [4] reserved
    control |= 1 << 5; // [5] IE -> timer interrupt enabled
    // [6] reserved
    control |= 1 << 7; // [7] ENABLE -> timer enabled
    control |= 0 << 8; // [8] DBGHALT -> timer keeps running if ARM is in debug halted mode
    control |= 1 << 9; // [9] ENAFREE -> free running counter enabled
    // [15:10] reserved
    control |= 0x3e; // [32:16] FREEDIV (For freeCounter reg) -> default value
    TIMER->control = control;

    uint32 preDiv = 0;
    preDiv |= 125; // [9:0] PREDIV -> default value
    TIMER->preDiv = preDiv;

    interrupts_enableARMCInterrupt(0);
    interrupts_addARMCInterruptHandler(0, timer_handleIRQ);
}