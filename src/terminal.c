#include "terminal.h"
#include "stringFormat.h"
#include "string.h"
#include "io.h"
#include "fileSystem.h"

void terminal_run(FileSystem *fs)
{
    char str[200];
    while (true)
    {
        readLine(str, 200);

        str[string_length(str) - 1] = '\0'; // Remove "\r\n" at the end of the string

        // Tokenize input str
        char *args[20];
        int argc = string_tokenize(args, str, ' ', 20);
        if (argc < 1)
            return;

        // Find command (first argument)
        char *cmd = args[0];
        if (string_compare(cmd, "exit"))
        {
            print("Exiting application.\n");
            return;
        }
        else if (string_compare(cmd, "mkdir"))
        {
            if (argc != 2)
                print("Expected 2 arguments (\"mkdir <fullPath>\").\n");
            else
            {
                if (!fileSystem_createFileInfo(fs, args[1])) // Always failes
                    PRINT("Failed to create file \'%s\'", args[1]);
            }
        }
        else if (string_compare(cmd, "lsall"))
        {
            fileSystem_printAllFileInfos(fs);
        }
        else
        {
            PRINT("Unknown command \"%s\".\n", cmd);
        }
    }
}