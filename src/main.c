#include "defs.h"
#include "fileSystem.h"
#include "alloc.h"
#include "stringFormat.h"
#include "io.h"

int main()
{
    // Initialize allocator
    if (allocator_init() != SUCCESS)
        goto end;

    // Initialize fileSystem
    FileSystem *fileSystem = fileSystem_create();
    if (!fileSystem)
        goto end;

    fileSystem_createFileInfo(fileSystem, "user/documents");

    fileSystem_createFileInfo(fileSystem, "user/documents/test.txt");

    // Delete "user/documents"
    if (fileSystem_deleteFileInfo(fileSystem, "user/documents", true) != SUCCESS)
        print("Failed to delete user/documents\n");

    fileSystem_printAllFileInfos(fileSystem);

    fileSystem_dispose(fileSystem);
    fileSystem = NULL;

    allocator_dispose();

end:
#ifdef OS
    while (true)
    {
    }
#endif
}