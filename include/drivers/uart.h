#pragma once

#include "defs.h"

ExitCode uart_init();

ExitCode uart_writeByte(byte c);
byte uart_readByte();