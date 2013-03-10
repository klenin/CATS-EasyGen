#ifndef __LIB_NUMERICS_H__
#define __LIB_NUMERICS_H__

#include <stdint.h>

#define LLONG_MAX_LEN 19

#if defined (__GNUC__)

    #include <inttypes.h>

#elif defined (_MSC_VER)

    #define PRId64 "I64d"
    #define strtoll _strtoi64

#endif

#endif // __LIB_NUMERICS_H__
