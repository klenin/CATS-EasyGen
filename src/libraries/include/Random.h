#ifndef __LIB_RANDOM_H__
#define __LIB_RANDOM_H__

#include <stdint.h>

void SetRandSeed(int64_t randseed);
int64_t GenerateRandInt(int64_t l, int64_t r);
long double GenerateRandFloat(int64_t l, int64_t r, int64_t d);

#endif // __LIB_RANDOM_H__
