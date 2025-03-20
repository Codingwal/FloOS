#pragma once

// Manages the physical memory and allocates single 4096 byte pages

void pageAlloc_init();

// Allocate a single physical page. Will never return NULL. (Out of memory triggers an assert)
void *pageAlloc_alloc();

// Free a previously allocated page
void pageAlloc_free(void *ptr);