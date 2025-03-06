#include "panic.h"
#include "io.h"
#include "stdarg.h"

void panic(const char *errorMessage, ...)
{
    print("Fatal error: ");

    va_list args;
    va_start(args, errorMessage);
    vprintf(errorMessage, args);
    va_end(args);

    print("\nStopping kernel.\n");

    while (true)
    {
    }
}