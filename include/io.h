#pragma once

#include "defs.h"

void uart_init();

ExitCode print(const char *str);
ExitCode printChar(char str);