/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef CF_BROWSER_H
#define CF_BROWSER_H
#include "cf_progression.h"
#define CPB_MAX 128u
#define CPB_BYTES (CPB_MAX / 8u)
enum { CPB_CATALOG, CPB_ALPHA, CPB_SHUFFLE, CPB_FAVORITES, CPB_MODES };
typedef struct {
    uint8_t order[CPB_MAX], favorites[CPB_BYTES], n, mode;
    uint32_t rng;
} cf_browser_t;
void cf_browser_order(cf_browser_t *b, unsigned mode, uint32_t seed);
uint8_t cf_browser_pick(const cf_browser_t *b, unsigned current, int turn);
int cf_browser_favorite(const cf_browser_t *b, unsigned index);
void cf_browser_star(cf_browser_t *b, unsigned index, int on);
#endif
