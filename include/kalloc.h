#pragma once

// Allocates 4KB=4096B pages

void kallocInit();
void *kalloc();
void kfree(void *ptr);