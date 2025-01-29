#include "terminal.h"
#include "stringFormat.h"
#include "string.h"
#include "io.h"

void terminal_run()
{
    char str[200];
    while (true)
    {
        readLine(str, 200);

        str[string_length(str) - 1] = '\0'; // Remove "\r\n" at the end of the string

        // Tokenize input str
        char *tokenArr[20];
        int tokenCount = string_tokenize(tokenArr, str, ' ', 20);
        if (tokenCount < 1)
            return;

        // Find command (first argument)
        char *cmd = tokenArr[0];
        if (string_compare(cmd, "exit"))
        {
            print("Exiting application.\n");
            return;
        }
        else if (string_compare(cmd, "mkdir"))
        {
            print("Not implemented!\n");
        }
        else
        {
            PRINT("Unknown command \"%s\".\n", cmd);
        }
    }
}