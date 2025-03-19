#include "io.h"
#include "drivers/uart.h"
#include "kalloc.h"
#include "vm.h"
#include "sysregs.h"
#include "error.h"

int main()
{
    uart_init();
    print("Initialized uart\n");

    assert(sysregs_CurrentEL_read() >> 2 == 1, "exception level should be EL1");

    kallocInit();
    print("Initialized allocator\n");

    vm_init();
    print("Enabled the mmu and virtual memory\n");

    print("\nFinished initializing flOSse.\n\n");

    while (true)
    {
    }
}