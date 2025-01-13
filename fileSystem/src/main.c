#include "defs.h"
#include "fileSystem.h"
#include "alloc.h"
#include "stringFormat.h"
#include "io.h"

int main()
{
    // Initialize fileSystem
    FileSystem *fileSystem = alloc(sizeof(FileSystem));
    fileSystem_init(fileSystem);

    // Create "user/documents"
    FileInfo *documents = fileSystem_createFileInfo(fileSystem, "user/documents");
    if (!documents)
        print("Failed to create user/documents\n");

    // Create "user/documents/test.txt"
    FileInfo *test = fileSystem_createFileInfo(fileSystem, "user/documents/test.txt");
    if (!test)
        print("Failed to create user/documents/testas.txt\n");

    // Delete "user/documents"
    if (fileSystem_deleteFileInfo(fileSystem, "user/documents", true) == FAILURE)
        print("Failed to delete user/documents\n");

    fileSystem_printAllFileInfos(fileSystem);
}