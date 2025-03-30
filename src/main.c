#include "io.h"
#include "cpu.h"
#include "error.h"

// Included because they need to be initialized
#include "drivers/uart.h"
#include "exceptions.h"
#include "interrupts.h"
#include "pageAlloc.h"
#include "vm.h"
#include "kalloc.h"
#include "drivers/timer.h"

int main(void)
{
    uart_init();
    print("Initialized uart\n");

    assert(cpu_exceptionLevel() == 1, "exception level should be EL1");

    exceptions_init();
    print("Initialized exceptions\n");

    interrupts_init();
    print("Initialized interrupts\n");

    pageAlloc_init();
    print("Initialized page allocator\n");

    vm_init();
    print("Enabled the mmu and virtual memory\n");

    kalloc_init();
    print("Initialized kernel allocator\n");

    timer_init();
    print("Initialized timer\n");

    print("\nFinished initializing flOSse.\n\n");

    timer_setTimer(10 * 1000 * 1000);
    print("Set timer\n");

    while (true)
    {
        print("Waiting...\n");
        // Waste time
        for (uint i = 0; i < 1000 * 1000; i++)
        {
            print("");
        }
    }

    halt();
}