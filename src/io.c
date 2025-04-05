#include "io.h"
#include "drivers/uart.h"
#include "string.h"
#include "error.h"

void printChar(char c)
{
    uart_writeByte(c);
}

char readChar(void)
{
    return uart_readByte();
}

void print(const char *str)
{
    assert(str != 0, "str is NULL");
    while (*str != '\0')
    {
        printChar(*str++);
    }
}

void printf(const char *str, ...)
{
    assert(str != 0, "str is NULL");
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

static void uintToString(char *dest, uint64 value, int base)
{
    assert(dest != 0, "dest buffer is NULL");
    if (value == 0)
    {
        dest[0] = '0';
        dest[1] = '\0';
        return;
    }

    uint i = 0;
    while (value != 0)
    {
        int remainder = value % base;
        if (remainder < 10)
            dest[i++] = remainder + '0';
        else
            dest[i++] = remainder - 10 + 'A';
        value /= base;
    }
    string_reverse(dest, i);
    dest[i] = '\0';
}

static void intToString(char *dest, int64 value, int base)
{
    assert(dest != 0, "dest buffer is NULL");
    if (value == 0)
    {
        dest[0] = '0';
        dest[1] = '\0';
        return;
    }

    bool isNegative = false;
    if (value < 0)
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
            dest[i++] = remainder - 10 + 'A';
        value /= base;
    }
    if (isNegative)
        dest[i++] = '-';
    string_reverse(dest, i);
    dest[i] = '\0';
}

static void printPtr(void *ptr)
{
    print("0x");
    byte *arr = (byte *)&ptr;
    for (int i = 7; i >= 0; i--)
    {
        byte top = arr[i] >> 4;
        if (top < 10)
            printChar(top + '0');
        else
            printChar(top - 10 + 'A');

        byte bottom = arr[i] & 0xF;
        if (bottom < 10)
            printChar(bottom + '0');
        else
            printChar(bottom - 10 + 'A');
    }
}

static void printInt(va_list *args, bool u, bool l, uint base)
{
    char tmp[50];
    if (u && l)
        uintToString(tmp, va_arg(*args, uint64), base);
    else if (u)
        uintToString(tmp, va_arg(*args, uint), base);
    else if (l)
        intToString(tmp, va_arg(*args, int64), base);
    else
        intToString(tmp, va_arg(*args, int), base);
    print(tmp);
}

void vprintf(const char *str, va_list args)
{
    assert(str != 0, "str is NULL");

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

        bool u = false;
        if (*str == 'u')
        {
            u = true;
            str++;
        }
        bool l = false;
        if (*str == 'l')
        {
            l = true;
            str++;
        }
        switch (*str)
        {
        case 'c':
            assert(!l && !u, "invalid format modifier (format specifier = %c)");
            printChar((char)va_arg(args, int));
            break;
        case 's':
            assert(!l && !u, "invalid format modifier (format specifier = %s)");
            print(va_arg(args, char *));
            break;
        case 'd':
        case 'i':
            printInt(&args, u, l, 10);
            break;
        case 'x':
            print("0x");
            printInt(&args, u, l, 16);
            break;
        case 'b':
            print("0b");
            printInt(&args, u, l, 2);
            break;
        case 'B':
            assert(!l && !u, "invalid format modifier (format specifier = %B)");
            if (va_arg(args, int))
                print("true");
            else
                print("false");
            break;
        case 'p':
            assert(!l && !u, "invalid format modifier (format specifier = %p)");
            printPtr((void *)va_arg(args, uint64));
            break;
        default:
            panic("invalid format specifier");
        }
    }
}