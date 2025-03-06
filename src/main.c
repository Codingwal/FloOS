#include "io.h"
#include "drivers/uart.h"
#include "kalloc.h"
#include "mem.h"
#include "assert.h"

int main()
{
    uart_init();
    print("Initialized uart\n");

    kallocInit();
    print("Initialized allocator\n");

    print("\nFinished initializing flOSse.\n\n");

    while (true)
    {
    }
}