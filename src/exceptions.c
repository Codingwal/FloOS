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
    printf("Exception class = %b; Instruction specific syndrome = %b\n", ec, iss);
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
}
