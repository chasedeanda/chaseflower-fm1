/* SPDX-License-Identifier: GPL-3.0-only */
#include "cf_progression.h"

#define PT(t) (1u << (CF_P_##t - 1))
#define PS(s) (1u << CF_SC_##s)
static const cf_progression_t CF_PROGRESSIONS[] = {
    {"Open Road",     PS(MAJOR),  1, PT(FAMILIAR)|PT(UPLIFTING), 4, {0,4,5,3}},
    {"Homeward",      PS(MAJOR),  2, PT(FAMILIAR)|PT(EMOTIONAL), 4, {5,3,0,4}},
    {"Old Friends",   PS(MAJOR),  3, PT(FAMILIAR)|PT(EMOTIONAL), 4, {0,5,3,4}},
    {"Two Five One",  PS(MAJOR),  4, PT(JAZZY)|PT(FAMILIAR),     3, {1,4,0}},
    {"Three Chords",  PS(MAJOR),  5, PT(FAMILIAR)|PT(UPLIFTING), 3, {0,3,4}},
    {"Long Way Home", PS(MAJOR),  6, PT(JAZZY)|PT(UPLIFTING),   4, {0,3,1,4}},
    {"Warm Memory",   PS(MAJOR),  7, PT(EMOTIONAL)|PT(DREAMY),   4, {0,2,5,3}},
    {"Round Again",   PS(MAJOR),  8, PT(JAZZY)|PT(FAMILIAR),     4, {0,5,1,4}},
    {"Night Drive",   PS(MINOR),  9, PT(FAMILIAR)|PT(ENERGETIC), 4, {0,5,2,6}},
    {"Drifting",      PS(MINOR), 10, PT(DREAMY)|PT(EMOTIONAL),   4, {0,3,6,2}},
    {"After Dark",    PS(MINOR), 11, PT(DARK)|PT(ENERGETIC),     4, {0,6,5,6}},
    {"Slow Return",   PS(MINOR), 12, PT(EMOTIONAL)|PT(DREAMY),   4, {0,5,6,0}},
    {"Quiet Tension", PS(MINOR), 13, PT(DARK)|PT(FAMILIAR),      4, {0,3,4,0}},
    {"Rising",        PS(MINOR), 14, PT(UPLIFTING)|PT(ENERGETIC),4, {5,6,0,0}},
    {"Moonlit",       PS(MINOR), 15, PT(EMOTIONAL)|PT(DREAMY),   4, {0,2,6,3}},
    {"Uneven Ground", PS(MINOR), 16, PT(DARK)|PT(EXPERIMENTAL), 4, {0,1,5,4}},
    {"Dorian Light",  PS(DORIAN),17, PT(DREAMY)|PT(UPLIFTING),   4, {0,3,0,3}},
    {"Side Street",   PS(DORIAN),18, PT(EXPERIMENTAL)|PT(DREAMY),4, {0,6,3,0}},
    {"Wide Sky",      PS(MIXOLYDIAN),19, PT(FAMILIAR)|PT(ENERGETIC),4, {0,6,3,0}},
    {"Sunlit Turn",   PS(MIXOLYDIAN),20, PT(UPLIFTING)|PT(ENERGETIC),4, {0,3,6,0}},
};
#undef PT
#undef PS

const char *cf_progression_tag(int tag)
{
    static const char *const names[CF_P_NTAGS] = {"All", "Familiar", "Emotional", "Dreamy", "Dark",
        "Uplifting", "Jazzy", "Energetic", "Experimental"};
    return names[tag >= 0 && tag < CF_P_NTAGS ? tag : 0];
}
unsigned cf_progression_count(void) { return sizeof CF_PROGRESSIONS / sizeof CF_PROGRESSIONS[0]; }
const cf_progression_t *cf_progression_get(unsigned index)
{
    return index < cf_progression_count() ? &CF_PROGRESSIONS[index] : 0;
}
int cf_progression_matches(unsigned index, int scale, int tag)
{
    const cf_progression_t *p = cf_progression_get(index);
    return p && scale >= 0 && scale < CF_NSCALES && tag >= 0 && tag < CF_P_NTAGS &&
        ((p->scales >> scale) & 1u) && (!tag || (p->tags & (1u << (tag - 1))));
}
unsigned cf_progression_filtered_count(int scale, int tag)
{
    unsigned n = 0;
    for (unsigned i = 0; i < cf_progression_count(); i++) n += cf_progression_matches(i, scale, tag);
    return n;
}
unsigned cf_progression_position(int scale, int tag, unsigned index)
{
    unsigned n = 0;
    for (unsigned i = 0; i < cf_progression_count(); i++) if (cf_progression_matches(i, scale, tag)) {
        n++;
        if (i == index) return n;
    }
    return 0;
}
uint8_t cf_progression_pick(int scale, int tag, unsigned current, int turn)
{
    unsigned n = cf_progression_filtered_count(scale, tag);
    unsigned pos = cf_progression_position(scale, tag, current), target;
    if (!n) return CF_P_NONE;
    target = pos ? pos - 1 : 0;
    /* Bound before adding, including extreme encoder/test inputs. */
    if (turn > 0) target = (unsigned)turn >= n - target ? n - 1 : target + (unsigned)turn;
    if (turn < 0) target = turn < -(int)target ? 0 : (unsigned)((int)target + turn);
    for (unsigned i = 0; i < cf_progression_count(); i++) if (cf_progression_matches(i, scale, tag)) {
        if (!target) return (uint8_t)i;
        target--;
    }
    return CF_P_NONE;
}
uint32_t cf_progression_mask(int scale, int tonic, int degree, const cf_harmony_t *h)
{
    uint16_t used, conflict;
    if (!h || !cf_harmony_supported(scale, 1) || tonic < 0 || tonic > 11 || degree < 0 || degree > 6) return 0;
    /* slots=0: the underlying triad only; never touch live chord/inversion state. */
    return cf_harmony_mask(h, scale, tonic, (tonic + cf_scale_step(scale, degree)) % 12,
                           0, 0, &used, &conflict);
}
int cf_progression_key(const cf_chord_cfg_t *cfg, int degree)
{
    int root;
    if (!cfg || !cf_harmony_supported(cfg->scale, cfg->quantize) ||
        cfg->tonic > 11 || degree < 0 || degree > 6) return -1;
    root = 60 + cfg->tonic - (cfg->tonic > 6 ? 12 : 0) + cf_scale_step(cfg->scale, degree) +
        12 * (cfg->octave + CF_OCT_BASE);
    if (root < 0 || root > 127) return -1;
    for (int key = 0; key < CF_NKEYS; key++) {
        int index;
        if (cf_key_role(cfg->algo, key, &index) == CF_ROLE_ROOT && cf_key_root_note(cfg, key) == root) return key;
    }
    return -1;
}
