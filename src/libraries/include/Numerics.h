#ifndef __LIB_NUMERICS_H__
#define __LIB_NUMERICS_H__

#include <stdint.h>

#define LLONG_MAX_LEN 19

#if defined __MINGW32__

    #define PRId64 "I64d"
    #define PRLD "f"
    #define PRLDType double
    #define SCLD PRLD

#elif defined __GNUC__

    #include <inttypes.h>
    #define PRLD "Lf"
    #define PRLDType long double
    #define SCLD PRLD

#elif defined _MSC_VER

    #define strtoll _strtoi64
    #define PRId64 "I64d"
    #define PRLD "Lf"
    #define PRLDType long double
    #define SCLD PRLD

#endif

int ConvertStringToLongDouble(char *str, long double *value);

#endif // __LIB_NUMERICS_H__
