#include "defs.h"
#include "fileSystem.h"
#include "alloc.h"
#include "stringFormat.h"
#include "io.h"
#include "terminal.h"

int main()
{
    // Initialize allocator
    if (allocator_init() != SUCCESS)
        goto end;

    FileSystem *fs = fileSystem_create();

    terminal_run(fs);

    fileSystem_dispose(fs);
    allocator_dispose();

end:
#ifdef OS
    while (true)
    {
    }
#endif
}