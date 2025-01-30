#include "io.h"
#include "terminal.h"
#include "stringFormat.h"

int main()
{
    uart_init();

    FileSystem *fs = fileSystem_create();
  
    print("Initialized flOSse\n");

    terminal_run(fs);

    fileSystem_dispose(fs);
    allocator_dispose();

    while (true)
    {
      
    }
}