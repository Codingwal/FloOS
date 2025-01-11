#include <stdio.h>
#include "defs.h"
#include "fileSystem.h"
#include <malloc.h>

int main()
{
    // Initialize fileSystem
    FileSystem *fileSystem = malloc(sizeof(FileSystem));
    fileSystem_init(fileSystem);

    // Create "user/documents"
    FileInfo *documents = fileSystem_createFileInfo(fileSystem, "user/documents");
    if (!documents)
        printf("Failed to create user/documents\n");

    // Create "user/documents/test.txt"
    FileInfo *test = fileSystem_createFileInfo(fileSystem, "user/documents/test.txt");
    if (!test)
        printf("Failed to create user/documents/test.txt\n");

    // Create "user/documents/test.txt"
    if (fileSystem_deleteFileInfo(fileSystem, "user/documents/test.txt", false) == FAILURE)
        printf("Failed to delete user/documents/test.txt\n");

    // Get "user/documents/test.txt"
    FileInfo *file = fileSystem_getFileInfo(fileSystem, "user/documents");
    if (file)
    {
        fileSystem_printFileInfo(file);
    }
    else
        printf("Failed to get file info\n");
    
    fileSystem_printFileInfo(fileSystem_getFileInfo(fileSystem, "user/documents/test.txt"));
}