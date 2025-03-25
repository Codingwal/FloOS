#pragma once

#include "defs.h"

void uart_init(void);

void uart_writeByte(byte c);
byte uart_readByte(void);