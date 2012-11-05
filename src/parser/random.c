#include "numerics.h"
#include "random.h"
#include "mt19937-64.h"

void setRandSeed(int64_t randseed)
{
    init_genrand64(randseed);
}

static uint64_t nextRand()
{
    return genrand64_int64();
}

int64_t genRandInt(int64_t l, int64_t r)
{
    uint64_t mod = (uint64_t)(r - l) + 1;
    return l + (nextRand() % mod);
}

long double genRandFloat(int64_t l, int64_t r, int64_t d)
{
    long double d10, x;
    uint64_t len, tmp;
    int64_t tmp1;
    int i, k;

    if (r <= l) return (long double)l;
    len = r-l+1;
    if (!len) len--;

    k = 0; tmp = len;
    while (tmp) {k++; tmp /= 10;}
    if (k + d >= maxIntLen) d = maxIntLen-1-k;

    for (i = 1, d10 = 1; i <= d; i++) d10 *= 10;
    tmp1 = genRandInt(l*d10, r*d10);
    x = tmp1 / (long double)d10;
    return x;
}
