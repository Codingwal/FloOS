#include "exceptions.h"
#include "defs.h"
#include "error.h"
#include "cpu.h"
#include "io.h"

extern byte exceptionVector[];

void exceptions_undefined(void)
{
    print("Undefined exception\n");
    halt();
}

void exceptions_sync(void)
{
    print("Synchronous exception\n");
}
void exceptions_irq(void)
{
    print("Interrupt request exception\n");
    halt();
}
void exceptions_fiq(void)
{
    print("Fast interrupt request exception\n");
    halt();
}
void exceptions_serror(void)
{
    print("System error exception\n");
    halt();
}

void exceptions_init(void)
{
    uint64 exceptionVectorAddr = (uint64)&exceptionVector;
    cpu_sysregs_vbar_el1_write(exceptionVectorAddr);
}