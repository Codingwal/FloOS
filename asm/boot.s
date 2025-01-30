.section ".text.boot"

.global _start

_start:
    // Check processor ID is zero (=main core), halt otherwise
    mrs x1, mpidr_el1   // Get info from system register
    and x1, x1, #3
    cbz x1, 2f
 
1:  // Halt
    wfe     // Wait for event
    b 1b    // Loop back (-> infinite loop)

2:  // Init
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

