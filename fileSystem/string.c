#include "string.h"
#include <stdio.h>

uint string_length(const char *str)
{
    const char *start = str;
    while (*str++)
    {
    }
    return str - start - 1;
}

bool string_compare(const char *lhs, const char *rhs)
{
    while (*lhs == *rhs)
    {
        if (*lhs == '\0')
            return true;
        lhs++;
        rhs++;
    }
    return false;
}

ExitCode string_copy(char *dest, const char *src)
{
    if (dest == NULL || src == NULL)
        return FAILURE;

    while ((*dest++ = *src++))
    {
    }
    return SUCCESS;
}

int string_tokenize(char **destArr, char *str, char delimeter, uint maxTokenCount)
{
    if (destArr == NULL || str == NULL || delimeter == '\0')
        return -1;

    for (uint i = 0; i < maxTokenCount; i++)
    {
        destArr[i] = str;
        while (*str != delimeter)
        {
            if (*str == '\0')
                return i + 1;
            str++;
        }
        str[0] = '\0';
        str++;
    }
    return maxTokenCount;
}
