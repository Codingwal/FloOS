#include "terminal.h"
#include "stringFormat.h"
#include "string.h"
#include "io.h"
#include "fileSystem.h"

void terminal_execCmd(char *str, FileSystem *fs)
{
    // Tokenize input str
    char *args[20];
    int argc = string_tokenize(args, str, ' ', 20);
    if (argc < 1)
        return;

    // Find command (first argument)
    char *cmd = args[0];
    if (string_compare(cmd, "mk"))
    {
        if (argc != 2)
            print("Expected 2 arguments (\"mk <fullPath>\").\n");
        else
        {
            if (!fileSystem_createFileInfo(fs, args[1]))
                PRINT("Failed to create file \"%s\".\n", args[1])
            else
                PRINT("Created file \"%s\".\n", args[1])
        }
    }
    else if (string_compare(cmd, "ls"))
    {
        bool recursive = false;
        for (uint i = 1; i < argc; i++)
        {
            if (args[i][0] != '-')
            {
                print("Invalid argument, expected option (-<option>).\n");
                return;
            }
            switch (args[i][1])
            {
            case 'r':
                recursive = true;
                break;
            default:
                PRINT("Invalid option '%c'.\n", args[i][1])
                return;
            }
        }

        fileSystem_listFiles(fs, "", recursive);
    }
    else if (string_compare(cmd, "rm"))
    {
        if (argc < 2)
        {
            print("Expected at least 2 arguments (\"rm <fullPath>\").\n");
        }

        bool recursive = false;
        char *path = NULL;
        for (uint i = 1; i < argc; i++)
        {
            if (args[i][0] == '-')
            {
                switch (args[i][1])
                {
                case 'r':
                    recursive = true;
                    break;
                default:
                    PRINT("Invalid option '%c'.\n", args[i][1])
                    return;
                }
            }
            else
            {
                path = args[i];
            }
        }
        if (!path)
            print("You must specify the file to delete.\n");
        if (fileSystem_deleteFileInfo(fs, path, recursive) != SUCCESS)
            PRINT("Failed to delete file \"%s\".\n", args[1])
        else
            PRINT("Deleted file \"%s\".\n", args[1])
    }
    else if (string_compare(cmd, "logo"))
    {
        print("\n");
        print(logo);
        print("\n");
    }
    else
    {
        PRINT("Unknown command \"%s\".\n", cmd)
    }
}
void terminal_run(FileSystem *fs)
{
    print("Running terminal.\n");
    char str[200];
    while (true)
    {
        print("> ");
        readLine(str, 200);
        terminal_execCmd(str, fs);
    }
}