#pragma once

#include "vm.h"

// Set mmu/vm related system registers
// This is needed before virtual memory can be enabled
void mmu_setConfig(const Pagetable *kernelPagetable);

// Enable the mmu and therefor vm
void mmu_enable();

// Returns 1 if the mmu is enabled and zero if it is not
// Can't use bool as return type as this function is implemented in assembly
bool mmu_isEnabled();