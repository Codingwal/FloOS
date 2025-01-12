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
ExitCode fileSystem_printFileAndChildren(const FileInfo *file, uint indentation)
{
    for (uint i = 0; i < indentation; i++)
        printf("  ");

    if (file->children)
    {
        printf("%s/\n", file->name);
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
        printf("%s\n", file->name);
    return SUCCESS;
}
ExitCode fileSystem_printAllFileInfos(const FileSystem *fs)
{
    return fileSystem_printFileAndChildren(fs->root, 0);
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

    if (string_length(name) == 0)
        return fs->root;

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
    free(file->name);
    free(file);
    return SUCCESS;
}
ExitCode fileSystem_deleteFileInfoHelper(FileInfo **prevNodeNextPtr, bool recursive)
{
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
        free(fileToDelete->name);
        free(fileToDelete);
        return SUCCESS;
    }
}
ExitCode fileSystem_deleteFileInfo(FileSystem *fs, const char *path, bool recursive)
{
    const char *name = string_findFromBack(path, string_length(path), '/');
    char *pathExceptName;
    if (!name) // string doesn't contain '/' -> file with root as parent
    {
        name = path;
        pathExceptName = malloc(1);
        pathExceptName[0] = '\0';
    }
    else
    {
        pathExceptName = string_substring(path, name - path);
        name++; // Don't include '/'
    }

    FileInfo *parent = fileSystem_getFileInfo(fs, pathExceptName);

    free(pathExceptName);

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
    const char *name = string_findFromBack(path, string_length(path), '/');
    char *pathExceptName;
    if (!name) // string doesn't contain '/' -> file with root as parent
    {
        name = path;
        pathExceptName = malloc(1);
        pathExceptName[0] = '\0';
    }
    else
    {
        pathExceptName = string_substring(path, name - path);
        name++; // Don't include '/'
    }

    FileInfo *parent = fileSystem_getFileInfo(fs, pathExceptName);

    free(pathExceptName);

    return fileSystem_createChildFileInfo(fs, parent, name);
}

ExitCode fileSystem_init(FileSystem *fs)
{
    fs->root = fileSystem_allocFile();
    if (!fs->root)
        return FAILURE;
    fs->root->name = "root";
    fs->root->size = 0;
    fs->root->children = NULL;
    fs->root->next = NULL;
    fs->fileCount = 1;

    fileSystem_createChildFileInfo(fs, fs->root, "bin");
    fileSystem_createChildFileInfo(fs, fs->root, "user");

    return SUCCESS;
}