/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef CF_FX_TYPES_H
#define CF_FX_TYPES_H
#include <stdint.h>
enum { CFX_FILTER, CFX_REVERB, CFX_DELAY, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_N };
typedef struct {
    uint8_t order[CFX_N]; /* first to last, a permutation of effect IDs */
    uint8_t on;           /* bit e enables effect e */
    uint8_t v[CFX_N][3];  /* 0..100; filter/reverb type 0..2, cutoff 0..99 */
} cf_fx_t;
#endif
