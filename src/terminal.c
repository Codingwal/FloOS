#include "terminal.h"
#include "stringFormat.h"
#include "string.h"
#include "io.h"
#include "fileSystem.h"
#include "logo.h"

static char *simplifyPath(char *path)
{
    uint len = string_length(path);
    path[len] = '/';
    path[len + 1] = '\0';

    // Handle '.'
    for (uint i = 0; path[i] != '\0'; i++)
    {
        if (path[i] != '.')
            continue;
        if (i != 0 && path[i - 1] != '/')
            continue;
        if (path[i + 1] != '\0' && path[i + 1] != '/')
            continue;
        path = &path[i + 1];
    }

    // Handle "\"
    for (uint i = 0; path[i] != '\0'; i++)
    {
        if (path[i] == '\\')
            path[i] = '/';
    }

    // Handle "//"
    for (uint i = 0; path[i] != '\0'; i++)
    {
        if (path[i] == '/' && path[i + 1] == '/')
        {
            string_copy(&path[i], &path[i + 1]);
        }
    }

    // Handle ".."
    char *ptr = string_findString(path, "/../");
    while (ptr)
    {
        char *p = ptr - 1;
        while (*p != '/' && p > path)
        {
            p--;
        }
        string_copy(p, ptr + 3);

        ptr = string_findString(p, "/../");
    }

    // Handle "/" as first token
    if (path[0] == '/')
        path += 1;

    // Handle "/" as last token
    len = string_length(path);
    if (path[len - 1] == '/')
    {
        path[len - 1] = '\0';
    }

    return path;
}
typedef enum Command
{
    NONE,

    MK,
    RM,
    LS,
    CD,

    LOGO,
} Command;
static void terminal_execCmd(char *str, FileSystem *fs, char *path)
{
    // Tokenize input str
    char *args[20];
    int argc = string_tokenize(args, str, ' ', 20);
    if (argc < 1)
        return;

    Command cmd = NONE;
    if (string_compare(args[0], "mk"))
        cmd = MK;
    else if (string_compare(args[0], "rm"))
        cmd = RM;
    else if (string_compare(args[0], "ls"))
        cmd = LS;
    else if (string_compare(args[0], "cd"))
        cmd = CD;
    else if (string_compare(args[0], "logo"))
        cmd = LOGO;

    if (cmd == MK || cmd == RM || cmd == LS || cmd == CD)
    {
        bool recursive = false;
        char *fileName = NULL;

        for (uint i = 1; i < (uint)argc; i++)
        {
            if (args[i][0] == '-') // Modifier
            {
                switch (args[i][1])
                {
                case 'r':
                    recursive = true;
                    break;
                default:
                    break;
                }
            }
            else // Parameter
            {
                fileName = args[i];
            }
        }

        char filePathArr[200];
        char *filePath = &filePathArr[0];
        string_copy(filePath, path);
        string_append(filePath, "/");
        string_append(filePath, fileName);
        filePath = simplifyPath(filePath);

        if (cmd == MK)
        {
            if (recursive)
            {
                print("Invalid modifier \"-r\"\n");
                return;
            }
            if (!fileSystem_createFileInfo(fs, filePath))
                PRINT("Failed to create file \"%s\".\n", filePath)
            else
                PRINT("Created file \"%s\".\n", filePath)
        }
        else if (cmd == RM)
        {
            if (fileSystem_deleteFileInfo(fs, filePath, recursive) != SUCCESS)
                PRINT("Failed to delete file \"%s\".\n", filePath)
            else
                PRINT("Deleted file \"%s\".\n", filePath)
        }
        else if (cmd == LS)
        {
            if (fileSystem_listFiles(fs, filePath, recursive) != SUCCESS)
                PRINT("Failed to list files at \"%s\".\n", filePath)
        }
        else if (cmd == CD)
        {
            if (recursive)
            {
                print("Invalid modifier \"-r\"\n");
                return;
            }
            if (!fileSystem_getFileInfo(fs, filePath))
            {
                PRINT("Path \"%s\" doesn't exist.\n", filePath)
                return;
            }
            string_copy(path, filePath);
        }
    }
    else if (cmd == LOGO)
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
    char path[200];
    path[0] = '\0';
    char str[200];
    while (true)
    {
        PRINT("%s> ", path)
        readLine(str, 200);
        terminal_execCmd(str, fs, path);
    }
}