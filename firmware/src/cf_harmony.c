/* SPDX-License-Identifier: GPL-3.0-only */
#include "cf_harmony.h"

static const char *const CF_MOD_NAMES[CF_MOD_N] = {
    "None", "6", "7", "9", "Add9", "11", "13", "Sus2", "Sus4", "Add2", "Add4",
    "Major", "Minor", "Dim", "Aug", "Power"
};
const char *cf_modifier_name(int m) { return CF_MOD_NAMES[m >= 0 && m < CF_MOD_N ? m : 0]; }
const char *cf_modifier_label(int m)
{
    static const char *const labels[CF_MOD_N] = {"-","6","7","9","+9","11","13","s2","s4","+2","+4","M","m","d","+","5"};
    return labels[m >= 0 && m < CF_MOD_N ? m : 0];
}

void cf_harmony_defaults(cf_harmony_t *h)
{
    static const uint8_t slots[CF_H_SLOTS] = {
        CF_MOD_6, CF_MOD_7, CF_MOD_9, CF_MOD_SUS2, CF_MOD_SUS4,
        CF_MOD_ADD2, CF_MOD_11, CF_MOD_ADD9, CF_MOD_MAJ, CF_MOD_MIN, CF_MOD_NONE
    };
    h->mode = CF_H_MANUAL; h->style = CF_H_BASIC; h->smooth = h->momentary = 0;
    h->custom = CF_MOD_ADD9;
    for (int i = 0; i < CF_H_SLOTS; i++) {
        h->slot[i] = slots[i]; h->inv[i] = 0; h->bass[i] = 0;
    }
}
void cf_harmony_sanitize(cf_harmony_t *h)
{
    if (h->mode > CF_H_AUTO) h->mode = CF_H_MANUAL;
    if (h->style >= CF_H_NSTYLES) h->style = CF_H_BASIC;
    h->smooth = !!h->smooth; h->momentary = !!h->momentary;
    if (h->custom >= CF_MOD_N) h->custom = CF_MOD_NONE;
    for (int i = 0; i < CF_H_SLOTS; i++) {
        if (h->slot[i] >= CF_MOD_N) h->slot[i] = CF_MOD_NONE;
        if (h->inv[i] < CF_INV_AUTO || h->inv[i] > 3) h->inv[i] = 0;
        if (h->bass[i] > 5) h->bass[i] = 0;
    }
}
int cf_harmony_supported(int scale, int quantize)
{
    return quantize != 0 && scale >= 0 && scale < CF_NSCALES && cf_scale_size(scale) == 7;
}
static uint32_t cfh_degree(int scale, int idx, int degree)
{
    int s = cf_scale_step(scale, idx + degree - 1) - cf_scale_step(scale, idx);
    return s >= 0 && s < 24 ? 1u << s : 0;
}
static int cfh_quality(int m) { return m >= CF_MOD_MAJ && m <= CF_MOD_POWER; }
static uint32_t cfh_add(uint32_t mask, int mod, int scale, int idx)
{
    if (mod == CF_MOD_7 || mod == CF_MOD_9 || mod == CF_MOD_11 || mod == CF_MOD_13)
        mask |= cfh_degree(scale, idx, 7);
    if (mod == CF_MOD_9 || mod == CF_MOD_ADD9 || mod == CF_MOD_11 || mod == CF_MOD_13)
        mask |= cfh_degree(scale, idx, 9);
    if (mod == CF_MOD_11) mask |= cfh_degree(scale, idx, 11);
    if (mod == CF_MOD_13) mask |= cfh_degree(scale, idx, 13);
    if (mod == CF_MOD_6) mask |= cfh_degree(scale, idx, 6);
    if (mod == CF_MOD_ADD2) mask |= cfh_degree(scale, idx, 2);
    if (mod == CF_MOD_ADD4) mask |= cfh_degree(scale, idx, 4);
    return mask;
}
uint32_t cf_harmony_mask(const cf_harmony_t *h, int scale, int tonic, int root,
                         uint16_t slots, const uint32_t stamp[CF_H_SLOTS],
                         uint16_t *used, uint16_t *conflict)
{
    int q = -1, sus = -1, idx;
    uint32_t m;
    *used = *conflict = 0;
    if (!cf_harmony_supported(scale, 1)) return 1u;
    idx = cf_scale_index(scale, root - tonic);
    if (idx < 0) return 1u;
    for (int i = 0; i < CF_H_SLOTS; i++) if ((slots >> i) & 1u) {
        int mod = h->slot[i];
        if (cfh_quality(mod) && (q < 0 || (int32_t)(stamp[i] - stamp[q]) >= 0)) q = i;
        if ((mod == CF_MOD_SUS2 || mod == CF_MOD_SUS4) &&
            (sus < 0 || (int32_t)(stamp[i] - stamp[sus]) >= 0)) sus = i;
    }
    m = cf_diatonic_mask(scale, tonic, root, 3);
    if (q >= 0) {
        static const uint8_t types[] = {CF_T_MAJ, CF_T_MIN, CF_T_DIM, CF_T_AUG, CF_T_POWER};
        cf_form_t f = {types[h->slot[q] - CF_MOD_MAJ], 0, 0, 0};
        m = cf_form_mask(&f);
    }
    if (sus >= 0) {
        /* Remove the actual scale's third, including unusual seven-note scales. */
        m &= ~cfh_degree(scale, idx, 3);
        m &= ~((1u << 3) | (1u << 4));
        m |= cfh_degree(scale, idx, h->slot[sus] == CF_MOD_SUS2 ? 2 : 4);
    }
    if (h->style == CF_H_RICH || h->style == CF_H_DREAMY) m = cfh_add(m, CF_MOD_7, scale, idx);
    if (h->style == CF_H_DREAMY) m = cfh_add(m, CF_MOD_ADD9, scale, idx);
    if (h->style == CF_H_CUSTOM) m = cfh_add(m, h->custom, scale, idx);
    for (int i = 0; i < CF_H_SLOTS; i++) if ((slots >> i) & 1u) {
        int mod = h->slot[i];
        if (mod == CF_MOD_NONE) continue;
        if ((cfh_quality(mod) && q >= 0 && mod != h->slot[q]) ||
            ((mod == CF_MOD_SUS2 || mod == CF_MOD_SUS4) && sus >= 0 && mod != h->slot[sus])) {
            *conflict |= (uint16_t)(1u << i); continue;
        }
        *used |= (uint16_t)(1u << i);
        m = cfh_add(m, mod, scale, idx);
    }
    return cf_harmony_limit(m, 6);
}
uint32_t cf_harmony_limit(uint32_t mask, int limit)
{
    uint32_t out = 0;
    /* Character tones before colors, fifths last; repeatable for all callers. */
    static const uint8_t priority[] = {0,3,4,2,5,10,11,6,8,14,13,15,17,18,16,21,20,22,9,1,7,12,19,23};
    for (unsigned i = 0; i < sizeof priority && limit > 0; i++) {
        uint32_t bit = 1u << priority[i];
        if (mask & bit) { out |= bit; limit--; }
    }
    return out;
}
uint16_t cf_harmony_event_buttons(uint32_t mask) { return (uint16_t)(CF_H_EVENT | ((mask >> 16) & 255u)); }
uint32_t cf_harmony_event_mask(uint16_t mask, uint16_t buttons)
{
    return (buttons & CF_H_EVENT) ? (uint32_t)mask | (uint32_t)(buttons & 255u) << 16 : mask;
}
