#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>

void setRandSeed(int64_t randseed);
int64_t genRandInt(int64_t l, int64_t r);
long double genRandFloat(int64_t l, int64_t r, int d);

#endif // __RANDOM_H__
