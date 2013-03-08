#ifndef __NUMERICS_H__
#define __NUMERICS_H__

#include <stdint.h>
#include <stdlib.h>

#define LLONG_MAX_LEN 19

#if defined (_MSC_VER)
#define strtoll _strtoi64
#endif

#endif // __NUMERICS_H__
