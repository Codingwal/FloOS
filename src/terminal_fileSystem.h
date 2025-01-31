#include "defs.h"
#include "stringFormat.h"
#include "io.h"
#include "fileSystem.h"
#include "string.h"

static void terminal_ls(char **args, uint argc, const FileSystem *fs, const char *path)
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
    fileSystem_listFiles(fs, path, recursive);
}
static void terminal_mk(char **args, uint argc, const FileSystem *fs, const char *path)
{
    if (argc != 2)
    {
        print("Expected 2 arguments (\"mk <fullPath>\").\n");
        return;
    }
    char fileName[200];
    string_copy(fileName, path);
    string_append(fileName, "/");
    string_append(fileName, args[1]);

    if (!fileSystem_createFileInfo(fs, fileName))
        PRINT("Failed to create file \"%s\".\n", fileName)
    else
        PRINT("Created file \"%s\".\n", fileName)
}
static void terminal_rm(char **args, uint argc, const FileSystem *fs, const char *path)
{
    if (argc < 2)
    {
        print("Expected at least 2 arguments (\"rm <fullPath>\").\n");
    }

    bool recursive = false;
    char *path = NULL;
    for (uint i = 1; i < (uint)argc; i++)
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