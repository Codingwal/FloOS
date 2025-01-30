#include "io.h"
#include "stringFormat.h"

int main()
{
    uart_init();

    print("Hello, World!\n");

    char str[500];
    while (true)
    {
        readLine(str, 500);
        PRINT("You wrote: \"%s\"\n> ", str);
    }
}