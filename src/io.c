#include "io.h"
#include "gpio.h"
#include "drivers/uart.h"
#include "string.h"
#include "assert.h"
#include "panic.h"

void printChar(char c)
{
    uart_writeByte(c);
}

char readChar()
{
    return uart_readByte();
}

void print(const char *str)
{
    assert(str != 0, "print: str is NULL");
    while (*str)
    {
        printChar(*str++);
    }
}

void printf(const char *str, ...)
{
    assert(str != 0, "printf: str is NULL");
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}

void readLine(char *dest, uint maxCharCount)
{
    for (uint i = 0; i < maxCharCount; i++)
    {
        char c = readChar();
        if (c == '\n')
        {
            dest[i] = '\0';
            return;
        }
        dest[i] = c;
    }
}

void intToString(char *dest, int value, int base)
{
    assert(dest != 0, "intToString: dest buffer is NULL");
    if (value == 0)
    {
        dest[0] = '0';
        dest[1] = '\0';
        return;
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
}

void vprintf(const char *str, va_list args)
{
    assert(str != 0, "vprintf: str is NULL");

    for (; *str != '\0'; str++)
    {
        if (*str != '%')
        {
            printChar(*str);
            continue;
        }
        if (*++str == '%') // double '%'
        {
            printChar('%');
            continue;
        }

        switch (*str)
        {
        case 'd':
        case 'i':
        {
            char tmp[50];
            intToString(tmp, va_arg(args, int), 10);
            print(tmp);
            break;
        }
        case 'x':
        {
            char tmp[50];
            intToString(tmp, va_arg(args, int), 16);
            print("0x");
            print(tmp);
            break;
        }
        case 'c':
            printChar((char)va_arg(args, int));
            break;
        case 's':
            print(va_arg(args, char *));
            break;
        default:
            panic("printf: invalid format specifier");
        }
    }
}