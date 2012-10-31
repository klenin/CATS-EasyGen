#include "random.h"
#include "mt19937-64.h"

void setRandSeed(INT64 randseed)
{
    init_genrand64(randseed);
}

static UINT64 nextRand()
{
    return genrand64_int64();
}

INT64 genRandInt(INT64 l, INT64 r)
{
    UINT64 mod = (UINT64)(r - l) + 1;
    return l + (nextRand() % mod);
}

real genRandFloat(INT64 l, INT64 r, int d)
{
    real d10, x;
    UINT64 len, tmp;
    INT64 tmp1;
    int i, k;

    if (r <= l) return (real)l;
    len = r-l+1;
    if (!len) len--;

    k = 0; tmp = len;
    while (tmp) {k++; tmp /= 10;}
    if (k + d >= maxIntLen) d = maxIntLen-1-k;

    for (i = 1, d10 = 1; i <= d; i++) d10 *= 10;
    tmp1 = genRandInt(l*d10, r*d10);
    x = tmp1 / (real)d10;
    return x;
}
