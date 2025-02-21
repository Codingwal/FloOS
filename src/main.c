#include "io.h"
#include "stringFormat.h"
#include "drivers/uart.h"
#include "kalloc.h"
#include "mem.h"

int main()
{
    uart_init();
    print("Initialized uart\n");

    print("Initializing allocator\n");
    kallocInit();
    print("Initialized allocator\n");

    print("Successfully initialized flOSse.\n");

    while (true)
    {
    }
}