#include "fileSystem.h"
#include "alloc.h"
#include "string.h"
#include "stringFormat.h"
#include "io.h"

struct FileInfo
{
    char *name;
    uint size;                 // File: size in bytes, Directory: count of subfiles
    struct FileInfo *children; // File: NULL
    struct FileInfo *next;     // File headers are implemented as a linked list
};
struct FileSystem
{
    FileInfo *root;
    uint fileCount;
};

ExitCode fileSystem_printFileInfo(const FileInfo *file)
{
    if (!file)
        return FAILURE;
    // printf("FileInfo of \"%s\": size = %d, isFolder = %d\n", file->name, file->size, (bool)file->children);
    return SUCCESS;
}
ExitCode fileSystem_printFileAndChildren(const FileInfo *file, uint indentation)
{
    if (!file)
        return NULL;

    for (uint i = 0; i < indentation; i++)
        print("  ");

    if (file->children)
    {
        print_s("%s/\n", file->name);
        file = file->children;
        indentation++;
        while (file)
        {
            if (fileSystem_printFileAndChildren(file, indentation) == FAILURE)
                return FAILURE;
            file = file->next;
        }
    }
    else
        print_s("%s\n", file->name);
    return SUCCESS;
}
ExitCode fileSystem_printAllFileInfos(const FileSystem *fs)
{
    if (!fs)
        return NULL;
    return fileSystem_printFileAndChildren(fs->root, 0);
}
FileInfo *fileSystem_allocFile()
{
    return alloc(sizeof(FileInfo));
}
FileInfo *fileSystem_createChildFileInfo(FileSystem *fs, FileInfo *parent, const char *name)
{
    if (!fs || !parent || !name)
        return NULL;

    // Create and initialize file
    FileInfo *file = fileSystem_allocFile();
    if (!file)
        return NULL;

    file->name = alloc(string_length(name) + 1);
    string_copy(file->name, name);
    file->size = 0;
    file->children = NULL;
    file->next = NULL;

    if (parent->children)
        file->next = parent->children;

    parent->children = file;

    fs->fileCount++;

    return file;
}

FileInfo *fileSystem_getChildFileInfo(const FileInfo *parent, const char *name)
{
    if (!parent || !name)
        return NULL;

    FileInfo *file = parent->children;
    while (file && !string_compare(file->name, name))
    {
        file = file->next;
    }
    return file;
}
FileInfo *fileSystem_getFileInfo(const FileSystem *fs, const char *name)
{
    if (!fs || !name)
        return NULL;

    if (string_length(name) == 0)
        return fs->root;

    char *nameCopy = alloc(string_length(name));
    if (!nameCopy)
        return NULL;
    if (string_copy(nameCopy, name) == FAILURE)
        return NULL;

    char *tokens[10];
    int tokenCount = string_tokenize(tokens, nameCopy, '/', 10);
    if (tokenCount == -1)
        return NULL;

    FileInfo *file = fs->root;
    for (uint i = 0; i < tokenCount; i++)
    {
        file = fileSystem_getChildFileInfo(file, tokens[i]);
        if (!file)
            return NULL;
    }
    freeAllocation(nameCopy);
    return file;
}
ExitCode fileSystem_deleteFileInfoAndChildren(FileInfo *file)
{
    if (!file)
        return FAILURE;

    FileInfo *child = file->children;
    while (child)
    {
        if (fileSystem_deleteFileInfoAndChildren(child) == FAILURE)
            return FAILURE;
        child = child->next;
    }
    freeAllocation(file->name);
    freeAllocation(file);
    return SUCCESS;
}
ExitCode fileSystem_deleteFileInfoHelper(FileInfo **prevNodeNextPtr, bool recursive)
{
    if (!prevNodeNextPtr || !*prevNodeNextPtr)
        return FAILURE;

    FileInfo *fileToDelete = (*prevNodeNextPtr);

    if (fileToDelete->children) // Folder
    {
        if (!recursive)
            return FAILURE;
        *prevNodeNextPtr = fileToDelete->next; // Remove fileToDelete from the linked list
        return fileSystem_deleteFileInfoAndChildren(fileToDelete);
    }
    else // File
    {
        *prevNodeNextPtr = fileToDelete->next; // Remove fileToDelete from the linked list
        freeAllocation(fileToDelete->name);
        freeAllocation(fileToDelete);
        return SUCCESS;
    }
}
ExitCode fileSystem_deleteFileInfo(FileSystem *fs, const char *path, bool recursive)
{
    if (!fs || !path)
        return FAILURE;

    const char *name = string_findFromBack(path, string_length(path), '/');
    char *pathExceptName;
    if (!name) // string doesn't contain '/' -> file with root as parent
    {
        name = path;
        pathExceptName = alloc(1);
        pathExceptName[0] = '\0';
    }
    else
    {
        pathExceptName = string_substring(path, name - path);
        name++; // Don't include '/'
    }

    FileInfo *parent = fileSystem_getFileInfo(fs, pathExceptName);

    freeAllocation(pathExceptName);

    if (!parent || !parent->children)
        return FAILURE;

    if (string_compare(parent->children->name, name))
    {
        return fileSystem_deleteFileInfoHelper(&parent->children, recursive);
    }

    FileInfo *file = parent->children;
    while (file->next)
    {
        if (string_compare(file->next->name, name))
        {
            return fileSystem_deleteFileInfoHelper(&file->next, recursive);
        }
        file = file->next;
    }
    return FAILURE;
}

FileInfo *fileSystem_createFileInfo(FileSystem *fs, const char *path)
{
    if (!fs || !path)
        return NULL;

    const char *name = string_findFromBack(path, string_length(path), '/');
    char *pathExceptName;
    if (!name) // string doesn't contain '/' -> file with root as parent
    {
        name = path;
        pathExceptName = alloc(1);
        pathExceptName[0] = '\0';
    }
    else
    {
        pathExceptName = string_substring(path, name - path);
        name++; // Don't include '/'
    }

    FileInfo *parent = fileSystem_getFileInfo(fs, pathExceptName);

    freeAllocation(pathExceptName);

    return fileSystem_createChildFileInfo(fs, parent, name);
}

FileSystem *fileSystem_create()
{
    FileSystem *fs = alloc(sizeof(FileSystem));
    if (!fs)
        return NULL;

    fs->root = fileSystem_allocFile();
    if (!fs->root)
    {
        freeAllocation(fs);
        return NULL;
    }
    fs->root->name = alloc(5);
    if (!fs->root->name)
    {
        freeAllocation(fs);
        freeAllocation(fs->root);
        return NULL;
    }
    string_copy(fs->root->name, "root");
    fs->root->size = 0;
    fs->root->children = NULL;
    fs->root->next = NULL;
    fs->fileCount = 1;

    if (!fileSystem_createChildFileInfo(fs, fs->root, "bin") || !fileSystem_createChildFileInfo(fs, fs->root, "user"))
    {
        freeAllocation(fs);
        freeAllocation(fs->root);
        freeAllocation(fs->root->name);
        return NULL;
    }

    return fs;
}
ExitCode fileSystem_dispose(FileSystem *fs)
{
    if (fileSystem_deleteFileInfoAndChildren(fs->root) == FAILURE)
        return FAILURE;
    freeAllocation(fs);
    return SUCCESS;
}