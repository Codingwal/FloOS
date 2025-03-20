#include "io.h"
#include "drivers/uart.h"
#include "vm.h"
#include "sysregs.h"
#include "error.h"
#include "pageAlloc.h"
#include "kalloc.h"

int main()
{
    uart_init();
    print("Initialized uart\n");

    assert(sysregs_CurrentEL_read() >> 2 == 1, "exception level should be EL1");

    pageAlloc_init();
    print("Initialized page allocator\n");

    vm_init();
    print("Enabled the mmu and virtual memory\n");

    kalloc_init();
    print("Initialized kernel allocator\n");

    print("\nFinished initializing flOSse.\n\n");

    while (true)
    {
    }
}