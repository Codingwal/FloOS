#pragma once

#include "defs.h"

ExitCode allocator_init();
ExitCode allocator_dispose();

void *alloc(uint size);
void freeAlloc(void *ptr);