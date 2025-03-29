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
    EXVEC_ENTRY exceptions_sync         // Synchronous exception
    EXVEC_ENTRY interrupts_handleIRQ    // Interrupt request
    EXVEC_ENTRY exceptions_fiq          // Fast interrupt request
    EXVEC_ENTRY exceptions_serror       // System error

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
    