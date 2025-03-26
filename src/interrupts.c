#include "interrupts.h"
#include "defs.h"
#include "error.h"
#include "cpu.h"

typedef enum Exception
{
    RESET,
    NMI,
    HARD_FAULT,
    MPU_FAULT,
    BUS_FAULT,
    USAGE_FAULT,
    SECURE_FAULT,
    SVC,
    DEBUG_MONITOR
} Exception;

typedef enum Interrupt
{
    INT0,
    INT1,
} Interrupt;

static void interrupts_handleException(Exception ex)
{
    panic("exception triggered! (%d)", (int)ex);
}
static void interrupts_handleInterrupt(Interrupt i)
{
    panic("interrupt triggered! (%d)", (int)i);
}

static void interrupts_handler_reset(void) { interrupts_handleException(RESET); }
static void interrupts_handler_nmi(void) { interrupts_handleException(NMI); }
static void interrupts_handler_hardFault(void) { interrupts_handleException(HARD_FAULT); }
static void interrupts_handler_mpuFault(void) { interrupts_handleException(MPU_FAULT); }
static void interrupts_handler_busFault(void) { interrupts_handleException(BUS_FAULT); }
static void interrupts_handler_usageFault(void) { interrupts_handleException(USAGE_FAULT); }
static void interrupts_handler_secureFault(void) { interrupts_handleException(SECURE_FAULT); }
static void interrupts_handler_svc(void) { interrupts_handleException(SVC); }
static void interrupts_handler_debugMonitor(void) { interrupts_handleException(DEBUG_MONITOR); }

static void interrupts_handler_int0(void) { interrupts_handleInterrupt(INT0); }
static void interrupts_handler_int1(void) { interrupts_handleInterrupt(INT1); }

uint32 exceptionVectors[16] __attribute__((aligned(512))) = {NULL};

void interrupts_init(void)
{
    exceptionVectors[1] = (uint32)(uint64)interrupts_handler_reset | 1;       // [1] Reset handler
    exceptionVectors[2] = (uint32)(uint64)interrupts_handler_nmi | 1;         // [2] NMI handler
    exceptionVectors[3] = (uint32)(uint64)interrupts_handler_hardFault | 1;   // [3] Hard fault handler
    exceptionVectors[4] = (uint32)(uint64)interrupts_handler_mpuFault | 1;    // [4] MPU fault handler
    exceptionVectors[5] = (uint32)(uint64)interrupts_handler_busFault | 1;    // [5] Bus fault handler
    exceptionVectors[6] = (uint32)(uint64)interrupts_handler_usageFault | 1;  // [6] Usage fault handler
    exceptionVectors[7] = (uint32)(uint64)interrupts_handler_secureFault | 1; // [7] Secure fault handler

    exceptionVectors[11] = (uint32)(uint64)interrupts_handler_svc | 1;          // [11] SVC handler
    exceptionVectors[12] = (uint32)(uint64)interrupts_handler_debugMonitor | 1; // [12] Debug monitor handler

    exceptionVectors[14] = (uint32)(uint64)interrupts_handler_int0 | 1; // [14] Interrupt 0 handler
    exceptionVectors[15] = (uint32)(uint64)interrupts_handler_int1 | 1; // [15] Interrupt 1 handler

    assert(((uint64)&exceptionVectors & ~BITMASK(7)) == (uint64)&exceptionVectors, "exception vector table is not sufficiently aligned");
    uint64 *vtorAddr = (uint64 *)0xE000ED08;
    *vtorAddr = (uint64)&exceptionVectors;
    cpu_dataSyncBarrier();
    cpu_instrSyncBarrier();
}