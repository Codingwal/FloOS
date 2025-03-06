#pragma once

#include "defs.h"

// If !condition, display the error message and halt the kernel
void assert(bool condition, char *errorMessage);