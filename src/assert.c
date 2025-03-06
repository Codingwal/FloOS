#include "assert.h"
#include "panic.h"

void assert(bool condition, char *errorMessage)
{
    if (condition)
        return;

    panic("Assertion failed: %s", errorMessage);
}