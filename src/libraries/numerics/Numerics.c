#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "Numerics.h"

int ConvertStringToLongDouble(char *str, long double *ldvalue)
{
    int result;
#if defined __MINGW64__
    *ldvalue = strtold(str, NULL);
    result = errno != ERANGE;
#elif defined __MINGW32__
    // MinGW has 80-bit long double, but MSVCRT uses 64-bit long double,
    // so just read double and convert it to long double.
    *ldvalue = strtod(str, NULL);
    result = errno != ERANGE;
#else
    result = sscanf(str, "%" SCLD, ldvalue) == 1;
#endif
    return result;
}
