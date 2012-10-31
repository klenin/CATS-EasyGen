#ifndef __NUMERICS_H__
#define __NUMERICS_H__

#include <stdlib.h>

#define real long double

#ifdef _MSC_VER
#define INT64 __int64
#define UINT64 unsigned __int64
#elif __GNUC__
#define INT64 long long
#define UINT64 unsigned long long
#endif

#define maxIntValue 9223372036854775807LL
#define maxIntLen 19
#define maxFloatDigits 12
#define eps 1e-13

INT64 getMaxIntValue();
size_t getMaxIntLen();
int getMaxFloatDigits();
real getEps();

#endif // __NUMERICS_H__
