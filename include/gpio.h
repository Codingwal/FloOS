#pragma once

#include "defs.h"

void mmio_write(uint64 reg, uint value);
uint mmio_read(uint64 reg);

ExitCode gpio_set(uint pinNumber, uint value);
ExitCode gpio_clear(uint pinNumber, uint value);
ExitCode gpio_pull(uint pinNumber, uint value);
ExitCode gpio_function(uint pinNumber, uint value);

ExitCode gpio_useAsAlt3(uint pinNumber);
ExitCode gpio_useAsAlt5(uint pinNumber);
