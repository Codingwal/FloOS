#include "exceptions.h"
#include "defs.h"
#include "error.h"
#include "cpu.h"
#include "io.h"

extern byte exceptionVector[];

static void exceptions_printDebugInfo(void)
{
    uint64 esr = cpu_sysregs_esr_el1_read();
    uint ec = (esr >> 26) & BITMASK(6); // [31:26] Exception class
    uint iss = esr & BITMASK(25);       // [24:0] Instruction specific syndrome

    printf("Faulting instruction address: %p\n\n", cpu_sysregs_elr_el1_read());
    printf("Exception class = %b ", ec);

    switch (ec)
    {
    case 37: // 0b100101
        print("(data abort without a change in exception level)\n");
        uint dataFaultStatusCode = iss & BITMASK(6);
        switch (dataFaultStatusCode >> 2)
        {
        case 0:
            print("Address size fault");
            break;
        case 1:
            print("Translation fault");
            break;
        case 2:
            print("Access flag fault");
            break;
        case 3:
            print("Permission fault");
            break;
        default:
            printf("Unknown fault (dfsc = %b)", dataFaultStatusCode);
            break;
        }
        printf(", L%d\n", dataFaultStatusCode & 3);
        break;

    default:
        print("(unknown exception class)\n");
        break;
    }

    printf("\nInstruction specific syndrome = %b\n", iss);
    printf("Fault address register: %p\n", cpu_sysregs_far_el1_read());
}
static void exceptions_unifiedHandler(void)
{
    exceptions_printDebugInfo();
    halt();
}

void exceptions_undefined(void)
{
    print("\n\nUndefined exception\n");
    exceptions_unifiedHandler();
}

void exceptions_sync(void)
{
    print("\n\nSynchronous exception\n");
    exceptions_unifiedHandler();
}
void exceptions_fiq(void)
{
    print("\n\nFast interrupt request exception\n");
    exceptions_unifiedHandler();
}
void exceptions_serror(void)
{
    print("\n\nSystem error exception\n");
    exceptions_unifiedHandler();
}

void exceptions_init(void)
{
    // Set vector table address
    uint64 exceptionVectorAddr = (uint64)&exceptionVector;
    cpu_sysregs_vbar_el1_write(exceptionVectorAddr);
}
