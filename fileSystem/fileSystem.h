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

FileInfo *fileSystem_createFileInfo(FileSystem *fs, const char *path);
FileInfo *fileSystem_getFileInfo(const FileSystem *fs, const char *path);
ExitCode fileSystem_deleteFileInfo(FileSystem *fs, const char *path, bool recursive);
ExitCode fileSystem_printFileInfo(const FileInfo *file);
ExitCode fileSystem_printAllFileInfos(const FileSystem *fs);