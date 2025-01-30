#include "io.h"
#include "terminal.h"
#include "stringFormat.h"
#include "alloc.h"

int main()
{
    uart_init();

    allocator_init();

    FileSystem *fs = fileSystem_create();
    if (!fs)
    {
        print("Failed to initialize fileSystem.\n");
        goto end;
    }

    print("Successfully initialized flOSse.\n");

    terminal_run(fs);

    fileSystem_dispose(fs);
    allocator_dispose();

end:
    while (true)
    {
    }
}