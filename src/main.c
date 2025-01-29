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

    terminal_run();

end:
    allocator_dispose();
#ifdef OS
    while (true)
    {
    }
#endif
}