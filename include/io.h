#pragma once

#include "defs.h"

ExitCode printChar(char c);
ExitCode print(const char *str);
char readChar();
ExitCode readLine(char *dest, uint maxCharCount);