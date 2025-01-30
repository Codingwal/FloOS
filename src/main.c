#include "io.h"
#include "stringFormat.h"

int main()
{
    uart_init();

    print("Hello, World!\n");

    int i = 0;
    while (true)
    {
        PRINT("%d; ", i);
        i++;
        if (i % 10 == 0)
            print("\n");
    }
}