#pragma once

// Display the error message and halt the kernel
void _panic(const char *errorMessage, const char *file, int line, const char *func, ...);
#define panic(errorMessage, ...) _panic(errorMessage, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)