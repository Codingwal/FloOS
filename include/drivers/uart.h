#pragma once

#include "defs.h"

ExitCode uart_init();

ExitCode uart_writeByte(char c);
char uart_readByte();