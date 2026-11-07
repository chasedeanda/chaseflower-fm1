/* SPDX-License-Identifier: GPL-3.0-only
 * Auto harmony: independent of hardware, UI and persisted manual chord forms. */
#ifndef CF_HARMONY_H
#define CF_HARMONY_H
#include "cf_theory.h"

enum { CF_H_MANUAL, CF_H_AUTO };
/* Legacy CHH1 style values, retained only for saved-record compatibility. */
enum { CF_H_BASIC, CF_H_RICH, CF_H_DREAMY, CF_H_CUSTOM, CF_H_NSTYLES };
enum { CF_MOD_NONE, CF_MOD_6, CF_MOD_7, CF_MOD_9, CF_MOD_ADD9, CF_MOD_11, CF_MOD_13,
       CF_MOD_SUS2, CF_MOD_SUS4, CF_MOD_ADD2, CF_MOD_ADD4, CF_MOD_MAJ, CF_MOD_MIN,
       CF_MOD_DIM, CF_MOD_AUG, CF_MOD_POWER, CF_MOD_N };
#define CF_H_SLOTS 11
typedef struct {
    uint8_t mode, style, smooth, momentary; /* only mode is active; other bytes reserved */
    uint8_t slot[CF_H_SLOTS];
    uint8_t custom;             /* reserved (formerly Custom style extension) */
    int8_t inv[CF_H_SLOTS];
    uint8_t bass[CF_H_SLOTS];
} cf_harmony_t;
/* A source chord keeps its actual quality/colors, chromatic alteration and
 * root-relative slash bass. This is factory data, never a saved-record field. */
typedef struct {
    uint32_t mask;
    int8_t degree, alter, bass; /* bass=-1 means root position */
} cf_source_chord_t;
uint32_t cf_harmony_apply(const cf_harmony_t *h, uint32_t base,
                         uint16_t slots, const uint32_t stamp[CF_H_SLOTS],
                         uint16_t *used, uint16_t *conflict);
void cf_harmony_defaults(cf_harmony_t *h);
void cf_harmony_sanitize(cf_harmony_t *h);
int cf_harmony_supported(int scale, int quantize);
/* Degree label for the underlying major/minor Auto triad. */
void cf_harmony_numeral(int scale, int tonic, int root, char out[8]);
const char *cf_modifier_name(int mod);
const char *cf_modifier_label(int mod);
/* Returns a root-relative interval mask. Last quality/suspension wins; the
 * returned used mask identifies applied slots, conflict identifies ignored slots. */
uint32_t cf_harmony_mask(const cf_harmony_t *h, int scale, int tonic, int root,
                         uint16_t slots, const uint32_t stamp[CF_H_SLOTS],
                         uint16_t *used, uint16_t *conflict);
/* Bound a generated chord before voicing/recording: root, third/sus, seventh,
 * requested colors, fifth. Stable output independent of allocation order. */
uint32_t cf_harmony_limit(uint32_t mask, int limit);
/* Auto takes store the full interval mask in existing event fields. Bit 15
 * of btns is outside all physical slots; low 8 bits then hold mask[16..23]. */
#define CF_H_EVENT 0x8000u
uint16_t cf_harmony_event_buttons(uint32_t mask);
uint32_t cf_harmony_event_mask(uint16_t mask, uint16_t buttons);
#endif
