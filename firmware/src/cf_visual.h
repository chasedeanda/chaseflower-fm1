/* SPDX-License-Identifier: GPL-3.0-only
 * Default chord-first build has no decorative scene engine or scene storage.
 * CF_SCENES=1 retains the exploration renderer for compatibility comparisons. */
#ifndef CF_VISUAL_H
#define CF_VISUAL_H
#ifndef CF_SCENES
#define CF_SCENES 0
#endif
#include "flora/flora.h"
#include "flora/flora_pal.h"
#include "flora/flora_math.h"
#if CF_SCENES
#include "flora/flora_canvas.c"
#include "flora/flora.c"
#endif
#endif
