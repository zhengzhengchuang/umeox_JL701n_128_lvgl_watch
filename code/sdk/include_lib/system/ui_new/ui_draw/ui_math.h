#ifndef __MATH_H__
#define __MATH_H__

#include "generic/typedef.h"

typedef struct {
    u16 i;
    u16 f;
} sqrt_res_t;

#define MATH_MIN(a, b) ((a) < (b) ? (a) : (b))
#define MATH_MAX(a, b) ((a) > (b) ? (a) : (b))
#define MATH_ABS(x) ((x) > 0 ? (x) : (-(x)))

#define TRIGO_SHIFT 15

int _trigo_sin(int angle);
int _trigo_cos(int angle);
void _sqrt(u32 x, sqrt_res_t *q, u32 mask);

AT_UI_RAM
static inline void sqrt_approx(sqrt_res_t *q, sqrt_res_t *ref, u32 x)
{
    // printf("_sq0: %d, %d\n", ref->i, x);
    x = x << 8;

    u32 raw = (ref->i << 4) + (ref->f >> 4);
    u32 raw2 = raw * raw;

    int d = x - raw2;
    d = (int)d / (int)(2 * raw) + raw;

    q->i = d >> 4;
    q->f = (d & 0xF) << 4;
    // printf("_sq1: %d, %d\n", q->i, q->f);
}

#endif

