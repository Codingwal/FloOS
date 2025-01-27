#pragma once

#include "defs.h"

ExitCode uart_init();

ExitCode print(const char *str);
ExitCode printChar(char c);