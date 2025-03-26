#include "exceptions.h"
#include "defs.h"
#include "error.h"
#include "cpu.h"
#include "io.h"

extern byte exceptionVector[];

void exceptions_undefined(void)
{
    print("Undefined exception\n");
    exceptions_unifiedHandler();
}

// 0b111100 = BRK instruction execution in AArch64 state
static void exceptions_unifiedHandler(void)
{
    uint64 esr = cpu_sysregs_esr_el1_read();
    uint ec = (esr >> 26) & BITMASK(6); // [31:26] Exception class
    uint iss = esr & BITMASK(24);       // [24:0] Instruction specific syndrome
    printf("Exception class = %b; Instruction specific syndrome = %b", ec, iss);
    cpu_exceptionReturn();
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
    uint64 exceptionVectorAddr = (uint64)&exceptionVector;
    cpu_sysregs_vbar_el1_write(exceptionVectorAddr);
}
