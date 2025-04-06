.macro EXVEC_ENTRY callback
.balign 0x80
    b \callback
.endm

.global exceptionVector
.balign 0x1000
exceptionVector:
    // Current EL using sp_el0
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined

    // Current EL using sp_elX
    EXVEC_ENTRY exceptions_sync // Synchronous exception
    EXVEC_ENTRY irq_handler // Interrupt request
    EXVEC_ENTRY exceptions_fiq // Fast interrupt request
    EXVEC_ENTRY exceptions_serror // System error

    // Lower EL, at least one lower EL is AArch64
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined

    // Lower EL, all lower ELs are AArch32
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined
    EXVEC_ENTRY exceptions_undefined

irq_handler:
    // The processor does the following things before executing this code:
    // - Save PSTATE in spsr_el1
    // - The address of the first not completed instruction is written to elr_el1
    // - Disable IRQs

    // Save caller-saved registers (all others will be done by C)
    stp x0, x1, [sp, #-16]! // sp -= 16; *sp = (x0, x1);
    stp x2, x3, [sp, #-16]! // sp -= 16; *sp = (x2, x3);
    str lr, [sp, #-8]! // sp -= 8; *sp = lr;

    // Jump to the C interrupt handler
    bl interrupts_handleIRQ

    // Load caller-saved registers (all others will be done by C)
    ldr lr, [sp], #8 // lr = *sp; sp += 8;
    ldp x2, x3, [sp], #16 // (x2, x3) = *sp; sp += 16;
    ldp x0, x1, [sp], #16 // (x0, x1) = *sp; sp += 16;

    // Exception return
    // This causes the processor to do the following things:
    // - Restore previous processor state from spsr_el1
    // - Branch to the return address in elr_el1
    // - Reenable IRQs
    eret
