#include "stringFormat.h"
#include "io.h"
#include "string.h"

// ExitCode print_ic(const char *str, int i, char c)
// {
//     print_i(str, i);
// }
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
ExitCode print_(const char *str, uint argc, int64 *argv)
{
    if (!str)
        return FAILURE;

    uint argIndex = 0;

    for (; *str != '\0'; str++)
    {
        if (*str != '%')
        {
            if (printChar(*str) == FAILURE)
                return FAILURE;
            continue;
        }
        if (*++str == '%') // double '%'
        {
            if (printChar('%') == FAILURE)
                return FAILURE;
            continue;
        }

        if (argIndex >= argc)
            return FAILURE;
        int64 arg = argv[argIndex++];

        switch (*str)
        {
        case 'd':
        case 'i':
            char tmp[50];
            if (intToString(tmp, (int)arg, 10) == FAILURE)
                return FAILURE;
            if (print(tmp) == FAILURE)
                return FAILURE;
            break;
        case 'c':
            if (printChar((char)arg) == FAILURE)
                return FAILURE;
            break;
        case 's':
            if (print((char *)arg) == FAILURE)
                return FAILURE;
            break;
        default:
            return FAILURE;
        }
    }
    return SUCCESS;
}