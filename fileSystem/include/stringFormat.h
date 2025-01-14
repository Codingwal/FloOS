#pragma once

#include "defs.h"

ExitCode intToString(char *dest, int value, int base);
ExitCode print_(const char *str, uint argc, int64 *argv);
// ExitCode print_i(const char *str, int i);
// ExitCode print_c(const char *str, char c);
// ExitCode print_s(const char *str, const char *s);

#define PRINT_EACH(x) (int64) x
#define PRINT(str, ...)                                  \
    {                                                    \
        int64 argv[] = {APPLY(PRINT_EACH, __VA_ARGS__)}; \
        print_(str, sizeof(argv), argv);                 \
    }
    