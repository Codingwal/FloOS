#pragma once

#include "defs.h"

void timer_init(void);

void timer_setTimer(uint value);
uint timer_getTimer(void);

void timer_check(void);