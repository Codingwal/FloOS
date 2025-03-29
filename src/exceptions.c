#include "exceptions.h"
#include "defs.h"
#include "error.h"
#include "cpu.h"
#include "io.h"

extern byte exceptionVector[];

static void exceptions_unifiedHandler(void)
{
    uint64 esr = cpu_sysregs_esr_el1_read();
    uint ec = (esr >> 26) & BITMASK(6); // [31:26] Exception class
    uint iss = esr & BITMASK(24);       // [24:0] Instruction specific syndrome
    printf("Exception class = %b; Instruction specific syndrome = %b", ec, iss);
    cpu_exceptionReturn();
}

void exceptions_undefined(void)
{
    print("Undefined exception\n");
    exceptions_unifiedHandler();
}

void exceptions_sync(void)
{
    print("Synchronous exception\n");
    exceptions_unifiedHandler();
}
void exceptions_irq(void)
{
    print("Interrupt request exception\n");
    exceptions_unifiedHandler();
}
void exceptions_fiq(void)
{
    print("Fast interrupt request exception\n");
    exceptions_unifiedHandler();
}
void exceptions_serror(void)
{
    print("System error exception\n");
    exceptions_unifiedHandler();
}

void exceptions_init(void)
{
    // Set vector table address
    uint64 exceptionVectorAddr = (uint64)&exceptionVector;
    cpu_sysregs_vbar_el1_write(exceptionVectorAddr);

    // Remove interrupt masking
    // uint64 daif = 0;
    // daif |= 0 << 6; // [6] F (FIQ mask) -> Enable FIQ interrupts
    // daif |= 0 << 7; // [7] I (IRQ mask) -> Enable IRQ interrupts
    // daif |= 0 << 8; // [8] A (SError mask) -> Enable SError interrupts
    // daif |= 0 << 9; // [9] D (D mask) -> Enable watchpoint, breakpoint & software step exceptions
    cpu_sysregs_daif_write(0);

    // cpu_sysregs_daifclr_write(2);
    asm volatile("msr daifclr, #2\n");
}
