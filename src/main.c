#include "defs.h"
#include "fileSystem.h"
#include "alloc.h"
#include "stringFormat.h"
#include "io.h"

int main()
{
    // Initialize allocator
    allocator_init();

    // Initialize fileSystem
    FileSystem *fileSystem = fileSystem_create();

    fileSystem_createFileInfo(fileSystem, "user/documents");

    fileSystem_createFileInfo(fileSystem, "user/documents/test.txt");

    // Delete "user/documents"
    if (fileSystem_deleteFileInfo(fileSystem, "user/documents", true) != SUCCESS)
        print("Failed to delete user/documents\n");

    fileSystem_printAllFileInfos(fileSystem);

    fileSystem_dispose(fileSystem);
    fileSystem = NULL;

    allocator_dispose();

#ifdef OS
    while (true)
    {
    }
#endif
}