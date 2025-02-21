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
    RETURN_ON_FAILURE(string_reverse(dest, i))

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
            RETURN_ON_FAILURE(printChar(*str))
            continue;
        }
        if (*++str == '%') // double '%'
        {
            RETURN_ON_FAILURE(printChar('%'))
            continue;
        }

        if (argIndex >= argc)
            return FAILURE;
        int64 arg = argv[argIndex++];

        switch (*str)
        {
        case 'd':
        case 'i':
        {
            char tmp[50];
            RETURN_ON_FAILURE(intToString(tmp, (int)arg, 10))
            RETURN_ON_FAILURE(print(tmp))
            break;
        }
        case 'x':
        {
            char tmp[50];
            RETURN_ON_FAILURE(intToString(tmp, (int)arg, 16))
            RETURN_ON_FAILURE(print("0x"))
            RETURN_ON_FAILURE(print(tmp))
            break;
        }
        case 'c':
            RETURN_ON_FAILURE(printChar((char)arg))
            break;
        case 's':
            RETURN_ON_FAILURE(print((char *)arg))
            break;
        default:
            return FAILURE;
        }
    }
    return SUCCESS;
}