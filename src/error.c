#include "error.h"
#include "io.h"
#include "stdarg.h"

void _assert(bool condition, const char *errorMessage, const char *file, int line, const char *func)
{
    if (condition)
        return;

    _panic("Assertion failed: %s", file, line, func, errorMessage);
}

void _panic(const char *errorMessage, const char *file, int line, const char *func, ...)
{
    printf("Fatal error in function \"%s\" (file %s, line %d):\n", func, file, line);

    va_list args;
    va_start(args, func);
    vprintf(errorMessage, args);
    va_end(args);

    halt();
}

void halt(void)
{
    print("\nStopping kernel.\n");
    while (true)
    {
    }
}