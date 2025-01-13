#pragma once

typedef unsigned int uint;
typedef unsigned char byte;

#ifndef bool
    #define bool _Bool
    #define true 1
    #define false 0
#endif
#ifndef NULL
    #define NULL 0
#endif

typedef enum
{
    SUCCESS = 0,
    FAILURE = 1,
} ExitCode;