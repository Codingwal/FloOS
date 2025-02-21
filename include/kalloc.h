#pragma once

// Allocates 4096B pages

void kallocInit();
void *kalloc();
void kfree(void *ptr);