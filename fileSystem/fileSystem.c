#include "fileSystem.h"
#include <malloc.h>
#include "string.h"
#include <stdio.h>

FileInfo *fileSystem_allocFile()
{
    return malloc(sizeof(FileInfo));
}
FileInfo *fileSystem_createFile(FileSystem *fs, FileInfo *parent, char *name)
{
    if (!parent || !name || !fs)
        return NULL;

    // Create and initialize file
    FileInfo *file = fileSystem_allocFile();
    if (!file)
        return NULL;
    file->name = name;
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

    fileSystem_createFile(fs, fs->root, "bin");
    fileSystem_createFile(fs, fs->root, "user");

    return SUCCESS;
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

    return file;
}
