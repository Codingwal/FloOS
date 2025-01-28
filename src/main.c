#include "defs.h"
#include "fileSystem.h"
#include "alloc.h"
#include "stringFormat.h"
#include "io.h"

int main()
{
    // Initialize allocator
    if (allocator_init() != SUCCESS)
        goto end;

    char str[200];
    if (readLine(str, 200) != SUCCESS)
        goto end;
    PRINT("You wrote: %s", str);

end:
    allocator_dispose();
#ifdef OS
    while (true)
    {
    }
#endif
}