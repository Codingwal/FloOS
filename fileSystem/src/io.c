#include "io.h"
#include <stdarg.h>
#include <stdio.h>

ExitCode print(const char *str)
{
    if (!str || *str == '\0')
        return FAILURE;
    if (printf(str) < 0)
        return FAILURE;
    else
        return SUCCESS;
}
ExitCode printChar(char c)
{
    if (putchar(c) == EOF)
        return FAILURE;
    else
        return SUCCESS;
}