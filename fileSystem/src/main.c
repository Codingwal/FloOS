#include "defs.h"
#include "fileSystem.h"
#include "alloc.h"
#include "stringFormat.h"
#include "io.h"

int main()
{
    // Initialize allocator
    allocator_init(&allocator);

    // Initialize fileSystem
    FileSystem *fileSystem = fileSystem_create();

    fileSystem_createFileInfo(fileSystem, "user/documents");

    fileSystem_createFileInfo(fileSystem, "user/documents/test.txt");

    fileSystem_createFileInfo(fileSystem, "abc");
    fileSystem_createFileInfo(fileSystem, "abc/def.test");

    // Delete "user/documents"
    if (fileSystem_deleteFileInfo(fileSystem, "user", true) == FAILURE)
        print("Failed to delete user/documents\n");

    fileSystem_printAllFileInfos(fileSystem);

    fileSystem_dispose(fileSystem);
    fileSystem = NULL;

    allocator_dispose(&allocator);
}