/* SPDX-License-Identifier: GPL-3.0-only
 * Small allocation-free helpers shared by static CF views and optional scenes. */
#ifndef FLORA_MATH_H
#define FLORA_MATH_H
#include <stdint.h>
#include "flora_lut.h"

static int32_t fl_fsin(int32_t a)
{
    a &= 1023;
    if (a < 256)
        return fl_qsin[a];
    if (a < 512)
        return fl_qsin[512 - a];
    if (a < 768)
        return -fl_qsin[a - 512];
    return -fl_qsin[1024 - a];
}
static int32_t fl_fcos(int32_t a) { return fl_fsin(a + 256); }

static uint32_t fl_hash32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

#endif
