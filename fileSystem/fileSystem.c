#include "fileSystem.h"
#include <malloc.h>
#include "string.h"
#include <stdio.h>

ExitCode fileSystem_printFileInfo(const FileInfo *file)
{
    if (!file)
        return FAILURE;
    printf("FileInfo of \"%s\": size = %d, isFolder = %d\n", file->name, file->size, (bool)file->children);
    return SUCCESS;
}
FileInfo *fileSystem_allocFile()
{
    return malloc(sizeof(FileInfo));
}
FileInfo *fileSystem_createChildFileInfo(FileSystem *fs, FileInfo *parent, const char *name)
{
    if (!fs || !parent || !name)
        return NULL;

    // Create and initialize file
    FileInfo *file = fileSystem_allocFile();
    if (!file)
        return NULL;

    file->name = malloc(string_length(name) + 1);
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
    FileInfo *file = parent->children;
    while (file && !string_compare(file->name, name))
    {
        file = file->next;
    }
    return file;
}
FileInfo *fileSystem_getFileInfo(const FileSystem *fs, const char *name)
{
    if (!name)
        return NULL;

    char *nameCopy = malloc(string_length(name));
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

    free(nameCopy);

    return file;
}

ExitCode fileSystem_deleteFileInfoHelper(FileInfo **prevNodeNextPtr, bool recursive)
{
    FileInfo *fileToDelete = (*prevNodeNextPtr);
    // Handle folders
    if (fileToDelete->children)
    {
        if (!recursive)
            return FAILURE;
    }

    *prevNodeNextPtr = fileToDelete->next;
    free(fileToDelete->name);
    free(fileToDelete);
    return SUCCESS;
}
ExitCode fileSystem_deleteFileInfo(FileSystem *fs, const char *path, bool recursive)
{
    char *name = string_findFromBack(path, string_length(path), '/') + 1;
    char *pathExceptName = string_substring(path, name - path - 1);

    printf("%s\n", name);
    printf("%s\n", pathExceptName);

    FileInfo *parent = fileSystem_getFileInfo(fs, pathExceptName);

    if (!parent || !parent->children)
        return FAILURE;

    if (string_compare(parent->children->name, name))
    {
        return fileSystem_deleteFileInfoHelper(&parent->children, recursive);
    }

    FileInfo *file = parent->children;
    while (file)
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
    char *name = string_findFromBack(path, string_length(path), '/') + 1;
    char *pathExceptName = string_substring(path, name - path - 1);

    FileInfo *parent = fileSystem_getFileInfo(fs, pathExceptName);

    free(pathExceptName);
    return fileSystem_createChildFileInfo(fs, parent, name);
}

ExitCode fileSystem_init(FileSystem *fs)
{
    fs->root = fileSystem_allocFile();
    if (!fs->root)
        return FAILURE;
    fs->root->name = "root/";
    fs->root->size = 0;
    fs->root->children = NULL;
    fs->root->next = NULL;
    fs->fileCount = 1;

    fileSystem_createChildFileInfo(fs, fs->root, "bin");
    fileSystem_createChildFileInfo(fs, fs->root, "user");

    return SUCCESS;
}