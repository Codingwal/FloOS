#pragma once

typedef unsigned int uint;
typedef unsigned char byte;
typedef long long int int64;
typedef long long unsigned int uint64;
typedef int int32;
typedef unsigned int uint32;

typedef void (*Func)(void);

_Static_assert(sizeof(byte) == 1, "size error");
_Static_assert(sizeof(int64) == 8, "size error");
_Static_assert(sizeof(uint64) == 8, "size error");
_Static_assert(sizeof(int32) == 4, "size error");
_Static_assert(sizeof(uint32) == 4, "size error");

#ifndef bool
#define bool _Bool
#define true 1
#define false 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

typedef enum ExitCode
{
    // 0 - 9: Standard
    SUCCESS = 0,
    FAILURE = 1,
    FAILURE_NOT_IMPLEMENTED,
    FAILURE_INVALID_INPUT,

    // 10 - 19: FileSystem
} ExitCode;

// Creates a bitmask where the first n bits are turned on
#define BITMASK(n) (((uint64)1 << n) - 1)

// Round x to the next / previous multiple of n (n must be a power of 2)
#define ROUND_DOWN(x, n) ((x) & ~(n - 1))
#define ROUND_UP(x, n) (ROUND_DOWN(x + n - 1, n))

// Useful standard macros

// NARGS counts the number of args
#define NARGS_SEQ(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

// CAT lets macros expand before concating them
#define PRIMITIVE_CAT(x, y) x##y
#define CAT(x, y) PRIMITIVE_CAT(x, y)

// APPLY calls macro on each argument passed in
#define APPLY(macro, ...) CAT(APPLY_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_1(m, x1) m(x1)
#define APPLY_2(m, x1, x2) m(x1), m(x2)
#define APPLY_3(m, x1, x2, x3) m(x1), m(x2), m(x3)
#define APPLY_4(m, x1, x2, x3, x4) m(x1), m(x2), m(x3), m(x4)
#define APPLY_5(m, x1, x2, x3, x4, x5) m(x1), m(x2), m(x3), m(x4), m(x5)
#define APPLY_6(m, x1, x2, x3, x4, x5, x6) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define APPLY_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define APPLY_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)

// Useful for creating structs with specific alignments
#define OFFSET_STRUCT(name, members) \
    typedef union name               \
    {                                \
        members                      \
    } name

// User OFFSET_MEMBER_0 if offset is zero
#define OFFSET_MEMBER(offset, member)  \
    struct __attribute__((__packed__)) \
    {                                  \
        char pad##offset[offset];      \
        member;                        \
    }

#define OFFSET_MEMBER_0(member)        \
    struct __attribute__((__packed__)) \
    {                                  \
        member;                        \
    }
