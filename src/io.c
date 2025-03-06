#include "io.h"
#include "gpio.h"
#include "stdarg.h"
#include "drivers/uart.h"
#include "string.h"

ExitCode printChar(char c)
{
    return uart_writeByte(c);
}

char readChar()
{
    return uart_readByte();
}

ExitCode print(const char *str)
{
    if (!str)
        return FAILURE;
    while (*str)
    {
        RETURN_ON_FAILURE(printChar(*str++))
    }
    return SUCCESS;
}

ExitCode readLine(char *dest, uint maxCharCount)
{
    for (uint i = 0; i < maxCharCount; i++)
    {
        char c = readChar();
        if (c == '\n')
        {
            dest[i] = '\0';
            return SUCCESS;
        }
        dest[i] = c;
    }
    return SUCCESS;
}

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

ExitCode printf(const char *str, ...)
{
    va_list args;
    va_start(args, str);

    if (!str)
        return FAILURE;

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

        switch (*str)
        {
        case 'd':
        case 'i':
        {
            char tmp[50];
            RETURN_ON_FAILURE(intToString(tmp, va_arg(args, int), 10))
            RETURN_ON_FAILURE(print(tmp))
            break;
        }
        case 'x':
        {
            char tmp[50];
            RETURN_ON_FAILURE(intToString(tmp, va_arg(args, int), 16))
            RETURN_ON_FAILURE(print("0x"))
            RETURN_ON_FAILURE(print(tmp))
            break;
        }
        case 'c':
            RETURN_ON_FAILURE(printChar((char)va_arg(args, int)))
            break;
        case 's':
            RETURN_ON_FAILURE(print(va_arg(args, char *)))
            break;
        default:
            return FAILURE;
        }
    }
    va_end(args);
    return SUCCESS;
}