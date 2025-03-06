#include "fileSystem.h"
#include "alloc.h"
#include "string.h"
#include "io.h"

enum
{
    MAX_FILE_NAME_LENGTH = 50,
};

struct FileInfo
{
    char *name;
    uint size;                 // File: size in bytes, Directory: count of subfiles
    struct FileInfo *children; // File: data ptr
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
        return FAILURE_INVALID_INPUT;
    printf("FileInfo of \"%s\": size = %d, isFolder = %d\n", file->name, file->size, (bool)file->children);
    return SUCCESS;
}
static ExitCode fileSystem_printFile(const FileInfo *file, uint indentation, bool printChildren)
{
    if (!file)
        return FAILURE_INVALID_INPUT;

    for (uint i = 0; i < indentation; i++)
        print("  ");

    if (file->children)
    {
        printf("%s/\n", file->name);
        file = file->children;
        indentation++;
        while (file)
        {
            if (printChildren)
            {
                RETURN_ON_FAILURE(fileSystem_printFile(file, indentation, true))
            }
            else
                printf("    %s\n", file->name);
            file = file->next;
        }
    }
    else
        printf("%s\n", file->name);
    return SUCCESS;
}
ExitCode fileSystem_listFiles(const FileSystem *fs, const char *path, bool recursive)
{
    if (!fs || !path)
        return FAILURE_INVALID_INPUT;
    FileInfo *file = fileSystem_getFileInfo(fs, path);
    return fileSystem_printFile(file, 0, recursive);
}
static FileInfo *fileSystem_allocFile()
{
    return alloc(sizeof(FileInfo));
}
static FileInfo *fileSystem_createChildFileInfo(FileSystem *fs, FileInfo *parent, const char *name)
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

static FileInfo *fileSystem_getChildFileInfo(const FileInfo *parent, const char *name)
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
    if (string_copy(nameCopy, name) != SUCCESS)
        goto error;

    char *tokens[10];
    int tokenCount = string_tokenize(tokens, nameCopy, '/', 10);
    if (tokenCount == -1)
        goto error;

    FileInfo *file = fs->root;
    for (uint i = 0; i < (uint)tokenCount; i++)
    {
        file = fileSystem_getChildFileInfo(file, tokens[i]);
        if (!file)
            goto error;
    }
    freeAlloc(nameCopy);
    return file;

error:
    freeAlloc(nameCopy);
    return NULL;
}
static ExitCode fileSystem_deleteFileInfoAndChildren(FileInfo *file)
{
    if (!file)
        return FAILURE_INVALID_INPUT;

    FileInfo *child = file->children;
    while (child)
    {
        RETURN_ON_FAILURE(fileSystem_deleteFileInfoAndChildren(child))
        child = child->next;
    }
    freeAlloc(file->name);
    freeAlloc(file);
    return SUCCESS;
}
static ExitCode fileSystem_deleteFileInfoHelper(FileInfo **prevNodeNextPtr, bool recursive)
{
    if (!prevNodeNextPtr || !*prevNodeNextPtr)
        return FAILURE_INVALID_INPUT;

    FileInfo *fileToDelete = (*prevNodeNextPtr);

    if (fileToDelete->children) // Folder
    {
        if (!recursive)
            return (ExitCode)FAILURE_IS_FOLDER;
        *prevNodeNextPtr = fileToDelete->next; // Remove fileToDelete from the linked list
        return fileSystem_deleteFileInfoAndChildren(fileToDelete);
    }
    else // File
    {
        *prevNodeNextPtr = fileToDelete->next; // Remove fileToDelete from the linked list
        freeAlloc(fileToDelete->name);
        freeAlloc(fileToDelete);
        return SUCCESS;
    }
}
ExitCode fileSystem_deleteFileInfo(FileSystem *fs, const char *path, bool recursive)
{
    if (!fs || !path)
        return FAILURE_INVALID_INPUT;

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

    freeAlloc(pathExceptName);

    if (!parent || !parent->children)
        return (ExitCode)FAILURE_FILE_NOT_FOUND;

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
    return (ExitCode)FAILURE_FILE_NOT_FOUND;
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

    if (string_length(name) > MAX_FILE_NAME_LENGTH)
        return NULL;

    FileInfo *parent = fileSystem_getFileInfo(fs, pathExceptName);

    freeAlloc(pathExceptName);

    return fileSystem_createChildFileInfo(fs, parent, name);
}

FileSystem *fileSystem_create()
{
    FileSystem *fs = alloc(sizeof(FileSystem));
    if (!fs)
        return NULL;

    fs->root = fileSystem_allocFile();
    if (!fs->root)
        goto error_free1;

    fs->root->name = alloc(5);
    if (!fs->root->name)
        goto error_free2;
    string_copy(fs->root->name, "root");

    fs->root->size = 0;
    fs->root->children = NULL;
    fs->root->next = NULL;
    fs->fileCount = 1;

    if (!fileSystem_createChildFileInfo(fs, fs->root, "bin") || !fileSystem_createChildFileInfo(fs, fs->root, "user"))
        goto error_free3;

    return fs;

error_free3:
    freeAlloc(fs->root->name);
error_free2:
    freeAlloc(fs->root);
error_free1:
    freeAlloc(fs);
    return NULL;
}
ExitCode fileSystem_dispose(FileSystem *fs)
{
    RETURN_ON_FAILURE(fileSystem_deleteFileInfoAndChildren(fs->root))
    RETURN_ON_FAILURE(freeAlloc(fs));
    return SUCCESS;
}