#include "string.h"
#include "alloc.h"
#include "error.h"

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
    if (!lhs || !rhs)
        return false;
    while (*lhs == *rhs)
    {
        if (*lhs == '\0')
            return true;
        lhs++;
        rhs++;
    }
    return false;
}

void string_copy(char *dest, const char *src)
{
    assert(dest != NULL, "dest is NULL");
    assert(src != NULL, "src is NULL");

    while (true)
    {
        *dest = *src;
        dest++;
        src++;
        if (*src == '\0')
            break;
    }
}

int string_tokenize(char **destArr, char *str, char delimeter, uint maxTokenCount)
{
    if (!destArr || !str || delimeter == '\0')
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

char *string_find(const char *str, char c)
{
    while (*str && *str != c)
    {
        str++;
    }
    return (char *)str;
}
char *string_findString(const char *str, const char *seq)
{
    for (uint i = 0; str[i] != '\0'; i++)
    {
        for (uint j = 0; seq[j] == str[i + j]; j++)
        {
            if (seq[j + 1] == '\0')
                return (char *)&str[i];
        }
    }
    return NULL;
}

char *string_findFromBack(const char *str, uint strLength, char c)
{
    const char *it = str + strLength;
    while (*it != c)
    {
        if (it == str)
            return NULL;
        it--;
    }
    return (char *)it;
}

char *string_substring(const char *str, uint length)
{
    if (!str || length == 0)
        return NULL;

    char *res = alloc(length + 1);
    if (!res)
        return NULL;

    uint i = 0;
    for (; i < length && str[i] != '\0'; i++)
    {
        res[i] = str[i];
    }
    res[i] = '\0';
    return res;
}

void string_reverse(char *str, uint length)
{
    assert(str != NULL, "string_reverse: dest is NULL");
    assert(length != 0, "string_reverse: length is zero");

    char *end = str + length - 1;

    while (str < end)
    {
        char tmp = *end;
        *end = *str;
        *str = tmp;

        end--;
        str++;
    }
}

void string_append(char *dest, const char *src)
{
    string_copy(dest + string_length(dest), src);
}