#pragma once

#include "defs.h"

void mmio_write(uint64 reg, uint value);
uint mmio_read(uint64 reg);

void gpio_set(uint pinNumber, uint value);
void gpio_clear(uint pinNumber, uint value);
void gpio_pull(uint pinNumber, uint value);
void gpio_function(uint pinNumber, uint value);

void gpio_useAsAlt3(uint pinNumber);
void gpio_useAsAlt5(uint pinNumber);
