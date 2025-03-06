#pragma once

#include "defs.h"
#include "stdarg.h"

void printChar(char c);
void print(const char *str);
void printf(const char *fmtStr, ...);
void vprintf(const char *fmtStr, va_list args);

char readChar();
void readLine(char *dest, uint maxCharCount);