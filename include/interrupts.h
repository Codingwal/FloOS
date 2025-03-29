#pragma once

#include "defs.h"

void interrupts_init(void);

void interrupts_enableARMCInterrupt(uint armcId);

void interrupts_check(void);