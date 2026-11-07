/* SPDX-License-Identifier: GPL-3.0-only
 * Read-only, scale-relative suggestions. No playback or persisted state. */
#ifndef CF_PROGRESSION_H
#define CF_PROGRESSION_H
#include "cf_chord.h"

enum { CF_P_ALL, CF_P_FAMILIAR, CF_P_EMOTIONAL, CF_P_DREAMY, CF_P_DARK,
       CF_P_UPLIFTING, CF_P_JAZZY, CF_P_ENERGETIC, CF_P_EXPERIMENTAL, CF_P_NTAGS };
#define CF_P_MAX 4
#define CF_P_NONE 255u
typedef struct {
    const char *name;
    uint32_t scales;
    uint16_t id, tags;
    uint8_t n, degree[CF_P_MAX]; /* zero-based scale degrees */
} cf_progression_t;

const char *cf_progression_tag(int tag);
unsigned cf_progression_count(void);
const cf_progression_t *cf_progression_get(unsigned index);
int cf_progression_matches(unsigned index, int scale, int tag);
unsigned cf_progression_filtered_count(int scale, int tag);
/* Keep current if compatible, otherwise choose the first match. Turn within the
 * filtered list, clamping at its ends. NONE for invalid input or no matches. */
uint8_t cf_progression_pick(int scale, int tag, unsigned current, int turn);
unsigned cf_progression_position(int scale, int tag, unsigned index); /* 1-based, 0 absent */
/* Underlying Auto triad, without physical/latched modifiers or live state. */
uint32_t cf_progression_mask(int scale, int tonic, int degree, const cf_harmony_t *h);
/* Physical root key in the central tonic octave, or -1 if unavailable. */
int cf_progression_key(const cf_chord_cfg_t *cfg, int degree);
#endif
