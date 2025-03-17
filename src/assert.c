#include "assert.h"
#include "panic.h"

void _assert(bool condition, const char *errorMessage, const char *file, int line, const char *func)
{
    if (condition)
        return;

    _panic("Assertion failed: %s", file, line, func, errorMessage);
}