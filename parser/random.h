#ifndef __RANDOM_H__
#define __RANDOM_H__

#include "numerics.h"

void setRandSeed(INT64 randseed);
INT64 genRandInt(INT64 l, INT64 r);
real genRandFloat(INT64 l, INT64 r, int d);

#endif // __RANDOM_H__
