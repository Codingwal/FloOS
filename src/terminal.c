#include "terminal.h"
#include "stringFormat.h"
#include "string.h"
#include "io.h"
#include "fileSystem.h"
#include "logo.h"
#include "terminal_fileSystem.h"

static char *simplifyPath(char *path)
{

    uint len = string_length(path);
    path[len] = '/';
    path[len + 1] = '\0';

    PRINT("Simplifiying path \"%s\"\n", path)

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
        char *p = ptr;
        while (*p != '/')
        {
            p--;
        }
        string_copy(p, ptr);

        ptr = string_findString(ptr + 1, "/../");
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

    PRINT("Simplified path to \"%s\".\n", path)

    return path;
}
static void terminal_execCmd(char *str, FileSystem *fs, char *path)
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
        terminal_mk(args, argc, fs, path);
    }
    else if (string_compare(cmd, "ls"))
    {
        terminal_ls(args, argc, fs, path);
    }
    else if (string_compare(cmd, "rm"))
    {
        terminal_rm(args, argc, fs, path);
    }
    else if (string_compare(cmd, "cd"))
    {
        if (argc != 2)
            print("Expected 2 arguments (\"cd <path>\").\n");

        char newPathArr[200];
        char *newPath = &newPathArr[0];

        string_copy(newPath, path);
        string_append(newPath, "/");
        string_append(newPath, args[1]);
        newPath = simplifyPath(newPath);

        if (!fileSystem_getFileInfo(fs, newPath))
        {
            print("Path doesn't exist.\n");
            return;
        }
        string_copy(path, newPath);
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