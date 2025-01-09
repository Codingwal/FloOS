#pragma once

#include "defs.h"

typedef struct FileInfo
{
    char *name;
    uint size;                 // File: size in bytes, Directory: count of subfiles
    struct FileInfo *children; // File: NULL
    struct FileInfo *next;     // File headers are implemented as a linked list
} FileInfo;

typedef struct FileSystem
{
    FileInfo *root;
    uint fileCount;
} FileSystem;

ExitCode fileSystem_init(FileSystem *fs);

FileInfo *fileSystem_getFileInfo(const FileSystem *fs, const char *name);