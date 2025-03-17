#include "panic.h"
#include "io.h"
#include "stdarg.h"

void _panic(const char *errorMessage, const char *file, int line, const char *func, ...)
{
    printf("Fatal error in function %s (file %s, line %d): ", func, file, line);

    va_list args;
    va_start(args, func);
    vprintf(errorMessage, args);
    va_end(args);

    print("\nStopping kernel.\n");

    while (true)
    {
    }
}