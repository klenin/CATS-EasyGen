#ifndef __LIB_NUMERICS_H__
#define __LIB_NUMERICS_H__

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define LLONG_MAX_LEN 19

#if defined (_MSC_VER)
#define strtoll _strtoi64
long double strtold(char* str, char** dummy);
#endif

#endif // __LIB_NUMERICS_H__
