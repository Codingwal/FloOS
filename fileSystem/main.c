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

    // Delete "user/documents"
    if (fileSystem_deleteFileInfo(fileSystem, "user/documents", true) == FAILURE)
        printf("Failed to delete user/documents\n");

    // Try to print file infos
    fileSystem_printFileInfo(fileSystem_getFileInfo(fileSystem, "user/documents"));
    fileSystem_printFileInfo(fileSystem_getFileInfo(fileSystem, "user/documents/test.txt"));

    fileSystem_printAllFileInfos(fileSystem);
}