#include "io.h"
#include "gpio.h"

#include "drivers/uart.h"

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