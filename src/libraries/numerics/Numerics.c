#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "Exceptions.h"
#include "Numerics.h"

int64_t ConvertStringToInt64(char* num)
{
    uint32_t i;
    uint32_t length = strlen(num);
    int64_t result = 0;
    int negative = num[0] == '-';

    if ((negative ? (length - 1): length) > LLONG_MAX_LEN)
    {
        throw(IllegalArgumentException, "too long integer number");
    }

    for (i = (negative ? 1: 0); i < length; ++i)
    {
        if (!isdigit(num[i]))
        {
            throw(IllegalArgumentException, "unexpected character");
        }
        result = result * 10 + num[i] - '0';
    }

    if (negative)
    {
        result = -result;
    }

    return result;
}
