#ifndef __NUMERICS_H__
#define __NUMERICS_H__

#include <stdint.h>
#include <stdlib.h>

#define LLONG_MAX_LEN 19

#if defined(_MSC_VER)
#define ConvertStringToInt64 _strtoi64
#else
#define ConvertStringToInt64 strtoll
#endif

#endif // __NUMERICS_H__
