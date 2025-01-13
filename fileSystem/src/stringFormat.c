#include "stringFormat.h"
#include "io.h"
#include "string.h"

ExitCode intToString(char *dest, int value, int base)
{
    if (!dest)
        return FAILURE;

    if (value == 0)
    {
        dest[0] = '0';
        dest[1] = '\0';
        return SUCCESS;
    }

    bool isNegative = false;
    if (base == 10 && value < 0)
    {
        isNegative = true;
        value = -value;
    }
    uint i = 0;
    while (value != 0)
    {
        int remainder = value % base;
        if (remainder < 10)
            dest[i++] = remainder + '0';
        else
            dest[i++] = remainder - 10 + 'a';
        value /= base;
    }

    if (isNegative)
        dest[i++] = '-';
    string_reverse(dest, i);

    dest[i] = '\0';
    return SUCCESS;
}
ExitCode print_i(const char *str, int i)
{
    if (!str)
        return FAILURE;

    for (; *str != '\0'; str++)
    {
        if (*str != '%')
        {
            if (printChar(*str) == FAILURE)
                return FAILURE;
            continue;
        }
        switch (*++str)
        {
        case '%':
            if (printChar('%') == FAILURE)
                return FAILURE;
            continue;
        case 'd':
        case 'i':
            char tmp[50];
            if (intToString(tmp, i, 10) == FAILURE)
                return FAILURE;
            if (print(tmp) == FAILURE)
                return FAILURE;
            break;
        default:
            return FAILURE;
        }
    }
    return SUCCESS;
}
ExitCode print_c(const char *str, char c)
{
    if (!str)
        return FAILURE;

    for (; *str != '\0'; str++)
    {
        if (*str != '%')
        {
            if (printChar(*str) == FAILURE)
                return FAILURE;
            continue;
        }
        switch (*++str)
        {
        case '%':
            if (printChar('%') == FAILURE)
                return FAILURE;
            continue;
        case 'c':
            if (printChar(c) == FAILURE)
                return FAILURE;
            break;
        default:
            return FAILURE;
        }
    }
    return SUCCESS;
}
ExitCode print_s(const char *str, const char *s)
{
    if (!str)
        return FAILURE;

    for (; *str != '\0'; str++)
    {
        if (*str != '%')
        {
            if (printChar(*str) == FAILURE)
                return FAILURE;
            continue;
        }
        switch (*++str)
        {
        case '%':
            if (printChar('%') == FAILURE)
                return FAILURE;
            continue;
        case 's':
            if (print(s) == FAILURE)
                return FAILURE;
            break;
        default:
            return FAILURE;
        }
    }
    return SUCCESS;
}