#pragma once

#include "defs.h"
#include "stdarg.h"

void printChar(char c);
void print(const char *str);

// Format specifiers:
// %s: string
// %c: char
// %d: integer
// %i: integer
// %x: integer in hex
// %b: integer in binary
// %B: boolean
// %p: pointer (same as %ulx but always with 16 digits)
// Format modifiers:
// u: unsigned
// l: long
void printf(const char *fmtStr, ...);
void vprintf(const char *fmtStr, va_list args);

char readChar(void);
void readLine(char *dest, uint maxCharCount);