#pragma once

#include "defs.h"

ExitCode allocator_init();
ExitCode allocator_dispose();

void *alloc(uint size);
ExitCode freeAlloc(void *ptr);

void allocator_print();