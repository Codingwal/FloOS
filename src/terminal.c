#include "terminal.h"
#include "string.h"
#include "io.h"
#include "fileSystem.h"
#include "logo.h"

enum
{
    MAX_FILE_PATH_LENGTH = 200,
};

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
static const char *exitCodeToString(ExitCode exitCode)
{
    switch ((int)exitCode)
    {
    // Standard
    case SUCCESS:
        return "[SUCCESS]";
    case FAILURE:
        return "[FAILURE]";
    case FAILURE_NOT_IMPLEMENTED:
        return "[FAILURE] Not implemented";

    // FileSystem
    case FAILURE_INVALID_INPUT:
        return "[FAILURE] Invalid input";
    case FAILURE_FILE_NOT_FOUND:
        return "[FAILURE] File not found";
    case FAILURE_IS_FOLDER:
        return "[FAILURE] Expected a file but found a folder";

    // Unspecified
    default:
        if (exitCode < 10)
            return "[FAILURE] Unspecified standard error";
        else if (exitCode < 20)
            return "[FAILURE] Unspecified file system error";
        else if (exitCode < 100)
            return "[FAILURE] Unspecified OS error";
        else
            return "[FAILURE] Unspecified user-defined error";
    }
}
static void printExitCode(ExitCode exitCode)
{
    if (exitCode != SUCCESS)
        printf("%s (ExitCode %d)\n", exitCodeToString(exitCode), exitCode);
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

        char filePathArr[MAX_FILE_PATH_LENGTH];
        char *filePath = &filePathArr[0];
        if (string_length(path) + 1 + string_length(fileName) > MAX_FILE_PATH_LENGTH)
        {
            printf("File path exceeds maximum length of %d", MAX_FILE_PATH_LENGTH);
            return;
        }
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
                printExitCode(FAILURE);
            else
                printExitCode(SUCCESS);
        }
        else if (cmd == RM)
        {
            printExitCode(fileSystem_deleteFileInfo(fs, filePath, recursive));
        }
        else if (cmd == LS)
        {
            printExitCode(fileSystem_listFiles(fs, filePath, recursive));
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
                printExitCode((ExitCode)FAILURE_FILE_NOT_FOUND);
                return;
            }
            string_copy(path, filePath);
        }
    }
    else if (cmd == LOGO)
    {
        printf("\n%s\n", logo);
    }
    else
    {
        printf("Unknown command \"%s\".\n", cmd);
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
        printf("%s> ", path);
        readLine(str, 200);
        terminal_execCmd(str, fs, path);
    }
}