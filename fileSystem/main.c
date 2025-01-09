#include <stdio.h>
#include "defs.h"
#include "fileSystem.h"
#include <malloc.h>

int main()
{
    FileSystem *fileSystem = malloc(sizeof(FileSystem));
    fileSystem_init(fileSystem);

    FileInfo *file = fileSystem_getFileInfo(fileSystem, "bin");
    if (file)
    {
        printf("%s: size=%d\n", file->name, file->size);
    }
    else
        printf("Failed to get file info");

    return SUCCESS;
}