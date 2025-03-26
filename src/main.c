#include "io.h"
#include "cpu.h"
#include "error.h"

// Included because they need to be initialized
#include "drivers/uart.h"
#include "interrupts.h"
#include "pageAlloc.h"
#include "vm.h"
#include "kalloc.h"

int main(void)
{
    uart_init();
    print("Initialized uart\n");

    assert(cpu_getExceptionLevel() == 1, "exception level should be EL1");

    interrupts_init();
    print("Initialized interrupts\n");

    pageAlloc_init();
    print("Initialized page allocator\n");

    // vm_init();
    // print("Enabled the mmu and virtual memory\n");

    // kalloc_init();
    // print("Initialized kernel allocator\n");

    print("\nFinished initializing flOSse.\n\n");

    print("Test 1\n");
    int a = 0;
    int b = 10 / a;
    printf("Success (a = %d)\n", b);

    while (true)
    {
    }
}