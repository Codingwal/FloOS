.section ".text.boot"

.global _start

_start:
    // Check processor ID is zero (=main core), halt otherwise
    mrs x1, mpidr_el1   // Get info from system register
    and x1, x1, #3
    cbz x1, set_el
 
1:  // Halt
    wfe     // Wait for event
    b 1b    // Loop back (-> infinite loop)

set_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    cmp x0, #3
    beq el3_entry
    cmp x0, #2
    beq el2_entry
    b el1_entry

el3_entry:
    // Zero sctlr_el2 and hcr_el2, might be configured later
    msr sctlr_el2, xzr
    msr hcr_el2, xzr

    // Set execution state and security of EL2
    mrs x0, scr_el3
    orr x0, x0, #(1<<10) // RW: EL2 Execution state is AArch64.
    orr x0, x0, #(1<<0)  // NS: EL1 is Non-secure world.
    msr scr_el3, x0

    // define EL2 as exception level and sp_el2 as stack pointer
    mov x0, #0b01001
    msr spsr_el3, x0 // M[3:0]=1001 

    // Set the return address of an EL3 exception return to the EL2 entry
    adr x0, el2_entry // el2_entry points to the first instruction of  EL2 code
    msr elr_el3, x0

    eret

el2_entry:
    // Zero sctlr_el1, might be configured later (for example to enable the mmu)
    msr sctlr_el1, xzr

    // Set execution state of EL1
    mrs x0, hcr_el2
    orr x0, x0, #(1<<31) // RW: EL2 Execution state is AArch64.
    msr hcr_el2, x0

    // define EL1 as exception level and sp_el1 as stack pointer
    mov x0, #0b00101
    msr spsr_el2, x0 // M[3:0]=0101

    // Set the return address of an EL2 exception return to the EL1 entry
    adr x0, el1_entry // el1_entry points to the first instruction of EL1 code
    msr elr_el2, x0

    eret

el1_entry:
    // Set stack to start below the code section [____<-Stack|.text|.bss, .data, ...]
    ldr x1, =_start
    mov sp, x1
    
    // Clean the BSS section
    ldr x1, = __bss_start
    ldr w2, = __bss_size
3:  cbz w2, 4f
    str xzr, [x1], #8   // xzr is the zero register
    sub w2, w2, #1
    cbnz w2, 3b

    // Jump to main() in C
4:  bl main // Call main()
    b 1b    // Halt in case it returns (it shouldn't)

