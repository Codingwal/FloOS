#pragma once

#include "defs.h"

// If !condition, display the error message and halt the kernel
void _assert(bool condition, const char *errorMessage, const char *file, int line, const char *func);

#define assert(condition, errorMessage) _assert(condition, errorMessage, __FILE__, __LINE__, __FUNCTION__)