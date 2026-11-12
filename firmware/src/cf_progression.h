/* SPDX-License-Identifier: GPL-3.0-only
 * Scale-relative ideas, complete-list browsing and live chord matching. */
#ifndef CF_PROGRESSION_H
#define CF_PROGRESSION_H
#include "cf_chord.h"

enum { CF_P_ALL, CF_P_FAMILIAR, CF_P_EMOTIONAL, CF_P_DREAMY, CF_P_DARK,
       CF_P_UPLIFTING, CF_P_JAZZY, CF_P_ENERGETIC, CF_P_EXPERIMENTAL, CF_P_NTAGS };
#ifndef CF_STUDIO
#define CF_STUDIO 1
#endif
#define CF_P_MAX 16
#define CF_P_COMPOSED 249u
#define CF_P_ENGINE 253u
#define CF_P_DRAFT 254u
#define CF_P_NONE 255u
typedef struct {
    const char *name;
    const char *context;        /* genre, or artist and song section */
    uint16_t id, tags;
    uint8_t scale, tonic, beats; /* suggested setup; 6 means 6/8, other meters /4 */
    uint8_t n, degree[CF_P_MAX]; /* zero-based scale degrees */
} cf_progression_t;

void cf_progression_engine_register(const cf_progression_t *p,const cf_source_chord_t *source);
void cf_progression_draft_register(const cf_progression_t *p,const cf_source_chord_t *source);
const cf_source_chord_t *cf_progression_source(const cf_progression_t *p);
int cf_progression_root(const cf_progression_t *p, unsigned slot, int scale, int tonic);
uint32_t cf_progression_slot_mask(const cf_progression_t *p, unsigned slot, int scale, int tonic, const cf_harmony_t *h);
int cf_progression_slot_key(const cf_progression_t *p, unsigned slot, const cf_chord_cfg_t *cfg);
const char *cf_progression_tag(int tag);
unsigned cf_progression_count(void);
const cf_progression_t *cf_progression_get(unsigned index);
/* One wrapping list, independent of the current key, scale and descriptive tags. */
uint8_t cf_progression_pick(unsigned current, int turn);
/* All matching cards light, including repeated chords. Generic entries match
 * root/third quality; source entries require their full intervals too. Inversion
 * does not affect matching. Caller excludes silence/single notes. */
uint16_t cf_progression_highlight(const cf_progression_t *p, int scale, int tonic,
                                 int root, uint32_t mask);
/* Underlying Auto triad, without physical/latched modifiers or live state. */
uint32_t cf_progression_mask(int scale, int tonic, int degree, const cf_harmony_t *h);
/* Physical root key in the central tonic octave, or -1 if unavailable. */
int cf_progression_key(const cf_chord_cfg_t *cfg, int degree);
#endif
