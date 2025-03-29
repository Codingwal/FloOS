#pragma once

#include "defs.h"

void interrupts_init(void);

void interrupts_enableARMCInterrupt(uint armcId);
void interrupts_addARMCInterruptHandler(uint armcId, Func handler);

void interrupts_handleIRQ(void);