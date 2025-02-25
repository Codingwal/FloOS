#pragma once

#include "defs.h"

enum
{
    FAILURE_FILE_NOT_FOUND = 10,
    FAILURE_IS_FOLDER,
};

typedef struct FileInfo FileInfo;
typedef struct FileSystem FileSystem;

FileSystem *fileSystem_create();
ExitCode fileSystem_dispose(FileSystem *fs);

FileInfo *fileSystem_getFileInfo(const FileSystem *fs, const char *path);
FileInfo *fileSystem_createFileInfo(FileSystem *fs, const char *path);
ExitCode fileSystem_deleteFileInfo(FileSystem *fs, const char *path, bool recursive);

ExitCode fileSystem_printFileInfo(const FileInfo *file);
ExitCode fileSystem_listFiles(const FileSystem *fs, const char *path, bool recursive);