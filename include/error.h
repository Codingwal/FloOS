#pragma once

#include "defs.h"

// Display the error message and halt the kernel
void _panic(const char *errorMessage, const char *file, int line, const char *func, ...) __attribute__((noreturn));

#define panic(errorMessage, ...) _panic(errorMessage, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

// If !condition, display the error message and halt the kernel
void _assert(bool condition, const char *errorMessage, const char *file, int line, const char *func);

#define assert(condition, errorMessage) _assert(condition, errorMessage, __FILE__, __LINE__, __func__)

void halt(void) __attribute__((noreturn));