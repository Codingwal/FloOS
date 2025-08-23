#pragma once

#include "defs.h"

// Initializes interrupts but does not enable them
void interrupts_init(void);

void interrupts_enable(void);
void interrupts_disable(void);

void interrupts_enableARMCInterrupt(uint armcId);
void interrupts_addARMCInterruptHandler(uint armcId, Func handler);
