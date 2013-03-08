#include "Numerics.h"

#if defined (_MSC_VER)

#include <errno.h>
#include <math.h>
#include <stdlib.h>

long double strtold(char* str, char** dummy)
{
    int retval;
    _CRT_DOUBLE value;

    retval = _atodbl(&value, str);
    if (retval != 0)
    {
        errno = ERANGE;
        return 0;
    }

    return value.x;
}

#endif // defined (_MSC_VER)
