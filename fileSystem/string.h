#pragma once

#include "defs.h"

/// @brief Returns the length of the null-terminated string, excluding the '\0' character
uint string_length(const char *str);

/// @brief Compares two strings.
/// @return Returns true if the strings are equal and false if they are not
bool string_compare(const char *lhs, const char *rhs);

/// @brief Copies the string src to dest.
/// @param dest The destination string. Must be at least as large as src!
/// @param src The source string.
ExitCode string_copy(char *dest, const char *src);

/// @brief Splits the string into multiple null-terminated strings and stores their start adress in destArr.
/// @param destArr The array where the "new strings" (char pointers) will be stored.
/// @param str The string to tokenize. Will be modified. Copy beforehand if you need the original afterwards.
/// @param delimeter The char that causes a split (is replaced by '\0').
/// @param maxTokenCount The maximum count of tokens (usually the size of destArr).
/// @return The amount of tokens the string has been split to (less or equal to maxTokenCount) or -1 on error.
int string_tokenize(char **destArr, char *str, char delimeter, uint maxTokenCount);