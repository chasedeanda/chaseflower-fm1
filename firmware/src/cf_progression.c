/* SPDX-License-Identifier: GPL-3.0-only */
#include "cf_progression.h"

#define PT(t) (1u << (CF_P_##t - 1))
static const cf_progression_t CF_PROGRESSIONS[] = {
    {"Open Road", "Pop punk", 1, PT(UPLIFTING), CF_SC_MAJOR, 4, 4, 4, {0,4,5,3}},
    {"Homeward", "Pop punk", 2, PT(EMOTIONAL), CF_SC_MAJOR, 2, 4, 4, {5,3,0,4}},
    {"Front Row", "Pop punk", 3, PT(ENERGETIC), CF_SC_MAJOR, 9, 4, 4, {0,3,5,4}},
    {"Summer Drive", "Pop punk", 4, PT(UPLIFTING), CF_SC_MAJOR, 4, 4, 4, {3,0,4,5}},
    {"Last Chorus", "Pop punk", 5, PT(ENERGETIC), CF_SC_MAJOR, 2, 4, 4, {3,4,0,5}},
    {"Old Friends", "Pop punk", 6, PT(FAMILIAR), CF_SC_MAJOR, 7, 4, 4, {0,5,3,4}},
    {"Basement Show", "Pop punk", 7, PT(DARK), CF_SC_MAJOR, 11, 4, 4, {5,4,3,4}},
    {"Long Way Home", "Pop punk", 8, PT(FAMILIAR), CF_SC_MAJOR, 9, 4, 4, {0,3,1,4}},
    {"After the Show", "Pop punk", 9, PT(EMOTIONAL), CF_SC_MAJOR, 4, 4, 4, {0,2,3,4}},
    {"Still Into You", "Paramore: chorus", 10, PT(FAMILIAR), CF_SC_MAJOR, 5, 4, 4, {3,4,5,0}},
    {"Porch Light", "Songwriter", 11, PT(FAMILIAR), CF_SC_MAJOR, 7, 4, 4, {0,4,3,0}},
    {"Empty Room", "Songwriter", 12, PT(EMOTIONAL), CF_SC_MAJOR, 0, 6, 4, {0,2,5,3}},
    {"Handwritten", "Songwriter", 13, PT(EMOTIONAL), CF_SC_MAJOR, 2, 4, 4, {5,2,3,0}},
    {"Slow Morning", "Songwriter", 14, PT(DREAMY), CF_SC_MAJOR, 7, 6, 4, {3,0,1,4}},
    {"Postcard", "Songwriter", 15, PT(FAMILIAR), CF_SC_MAJOR, 0, 4, 4, {0,5,1,4}},
    {"Coming Home", "Songwriter", 16, PT(EMOTIONAL), CF_SC_MAJOR, 4, 4, 4, {3,4,2,5}},
    {"Late Train", "Songwriter", 17, PT(DARK), CF_SC_MINOR, 9, 6, 4, {0,6,5,6}},
    {"Window Seat", "Songwriter", 18, PT(EMOTIONAL), CF_SC_MINOR, 4, 4, 4, {0,3,6,2}},
    {"Quiet Ending", "Songwriter", 19, PT(FAMILIAR), CF_SC_MAJOR, 2, 6, 4, {0,3,0,4}},
    {"The Only Exception", "Paramore: verse", 20, PT(FAMILIAR), CF_SC_MIXOLYDIAN, 11, 6, 4, {0,4,3,3}},
    {"Night Drive", "Emo", 21, PT(ENERGETIC), CF_SC_MINOR, 6, 4, 4, {0,5,2,6}},
    {"Heavy Weather", "Emo", 22, PT(DARK), CF_SC_MINOR, 1, 4, 4, {0,6,5,3}},
    {"Half Awake", "Emo", 23, PT(EMOTIONAL), CF_SC_MINOR, 4, 4, 4, {5,2,6,0}},
    {"Slow Return", "Emo", 24, PT(EMOTIONAL), CF_SC_MINOR, 11, 6, 4, {0,5,6,0}},
    {"Undertow", "Emo", 25, PT(DARK), CF_SC_MINOR, 2, 4, 4, {0,3,5,6}},
    {"Unsent", "Emo", 26, PT(EMOTIONAL), CF_SC_MINOR, 9, 6, 4, {5,3,0,6}},
    {"Fading Light", "Emo", 27, PT(DREAMY), CF_SC_MINOR, 6, 4, 4, {0,2,6,3}},
    {"Holding On", "Emo", 28, PT(EMOTIONAL), CF_SC_MAJOR, 2, 4, 4, {3,5,4,0}},
    {"Small Hours", "Emo", 29, PT(DREAMY), CF_SC_MAJOR, 4, 4, 4, {1,3,5,4}},
    {"Last Light", "Emo", 30, PT(EMOTIONAL), CF_SC_MAJOR, 9, 6, 4, {5,2,3,1}},
    {"Glass Steps", "Math rock", 31, PT(JAZZY), CF_SC_MAJOR, 4, 4, 4, {3,2,5,1}},
    {"Open Strings", "Math rock", 32, PT(DREAMY), CF_SC_MAJOR, 2, 4, 4, {0,1,3,2}},
    {"Sunroom", "Math rock", 33, PT(UPLIFTING), CF_SC_MAJOR, 9, 4, 4, {1,2,3,0}},
    {"Side Streets", "Math rock", 34, PT(EXPERIMENTAL), CF_SC_MAJOR, 4, 5, 4, {5,1,4,2}},
    {"Five Windows", "Math rock", 35, PT(EXPERIMENTAL), CF_SC_MAJOR, 2, 5, 4, {3,1,2,5}},
    {"Uneven Ground", "Math rock", 36, PT(EXPERIMENTAL), CF_SC_DORIAN, 2, 7, 4, {0,3,6,3}},
    {"Bright Corners", "Instrumental", 37, PT(UPLIFTING), CF_SC_LYDIAN, 0, 4, 4, {0,1,2,1}},
    {"Floating", "Instrumental", 38, PT(DREAMY), CF_SC_LYDIAN, 2, 5, 4, {0,1,4,0}},
    {"Mosaic", "Math rock", 39, PT(JAZZY), CF_SC_DORIAN, 4, 7, 4, {0,3,1,6}},
    {"Return Path", "Instrumental", 40, PT(JAZZY), CF_SC_MAJOR, 9, 4, 4, {2,5,1,3}},
    {"Low Lantern", "Modal / Phrygian", 41, PT(DARK), CF_SC_PHRYGIAN, 4, 4, 4, {0,1,6,0}},
    {"Close Quarters", "Modal / Phrygian", 42, PT(ENERGETIC), CF_SC_PHRYGIAN, 2, 2, 4, {0,1,3,0}},
    {"Stone Spiral", "Modal / Phrygian", 43, PT(EXPERIMENTAL), CF_SC_PHRYGIAN, 6, 3, 4, {0,6,5,1}},
    {"Desert Glass", "Modal / Phrygian", 44, PT(DREAMY), CF_SC_PHRYGIAN, 9, 6, 4, {0,3,1,0}},
    {"Fault Line", "Modal / Locrian", 45, PT(DARK), CF_SC_LOCRIAN, 11, 4, 4, {0,1,4,3}},
    {"Narrow Bridge", "Modal / Locrian", 46, PT(EXPERIMENTAL), CF_SC_LOCRIAN, 6, 5, 4, {0,3,1,6}},
    {"Crooked Waltz", "Modal / Locrian", 47, PT(EXPERIMENTAL), CF_SC_LOCRIAN, 4, 3, 4, {0,1,5,0}},
    {"Shadow Steps", "Modal / Locrian", 48, PT(DARK), CF_SC_LOCRIAN, 2, 7, 4, {0,6,4,1}},
    {"Two Step", "Modal / Dorian", 49, PT(ENERGETIC), CF_SC_DORIAN, 9, 2, 4, {0,3,1,0}},
    {"Dorian Waltz", "Modal / Dorian", 50, PT(JAZZY), CF_SC_DORIAN, 2, 3, 4, {0,3,6,0}},
    {"Wide Sky", "Modal / Mixolydian", 51, PT(UPLIFTING), CF_SC_MIXOLYDIAN, 7, 4, 4, {0,6,3,0}},
    {"Wandering Waltz", "Modal / Mixolydian", 52, PT(FAMILIAR), CF_SC_MIXOLYDIAN, 2, 3, 4, {0,3,6,0}},
    {"Prism Waltz", "Modal / Lydian", 53, PT(DREAMY), CF_SC_LYDIAN, 4, 3, 4, {0,1,4,2}},
    {"Seven Skylights", "Modal / Lydian", 54, PT(EXPERIMENTAL), CF_SC_LYDIAN, 9, 7, 4, {0,1,2,4}},
    {"Still Into You", "Paramore: verse", 55, PT(FAMILIAR), CF_SC_MAJOR, 5, 4, 4, {0,0,5,5}},
    {"Only Exception", "Paramore: chorus", 56, PT(FAMILIAR), CF_SC_MIXOLYDIAN, 11, 6, 4, {0,4,3,0}},
    {"Three Chords", "Pop / Country", 57, PT(FAMILIAR), CF_SC_MAJOR, 0, 4, 3, {0,3,4}},
    {"Rock Circle", "Rock / Blues roots", 58, PT(ENERGETIC), CF_SC_MAJOR, 4, 4, 4, {0,3,4,3}},
    {"Rock Return", "Rock", 59, PT(FAMILIAR), CF_SC_MAJOR, 9, 4, 4, {0,4,3,4}},
    {"Back Porch", "Country", 60, PT(FAMILIAR), CF_SC_MAJOR, 7, 2, 4, {0,4,0,3}},
    {"Bluegrass Turn", "Bluegrass", 61, PT(ENERGETIC), CF_SC_MAJOR, 2, 2, 4, {3,0,3,4}},
    {"Folk Waltz", "Folk", 62, PT(FAMILIAR), CF_SC_MAJOR, 7, 3, 4, {0,3,4,0}},
    {"Country Road", "Country", 63, PT(EMOTIONAL), CF_SC_MAJOR, 9, 6, 4, {0,3,5,3}},
    {"River Bend", "Americana", 64, PT(FAMILIAR), CF_SC_MAJOR, 2, 3, 4, {3,4,3,0}},
    {"Two Five One", "Jazz: triad seed", 65, PT(JAZZY), CF_SC_MAJOR, 0, 4, 3, {1,4,0}},
    {"Turnaround", "Jazz: triad seed", 66, PT(JAZZY), CF_SC_MAJOR, 5, 4, 4, {5,1,4,0}},
    {"Passing Through", "Jazz: triad seed", 67, PT(JAZZY), CF_SC_MAJOR, 10, 4, 4, {2,5,1,4}},
    {"Warm Keys", "Soul", 68, PT(DREAMY), CF_SC_MAJOR, 3, 6, 4, {0,1,2,5}},
    {"Four Way Soul", "Soul", 69, PT(EMOTIONAL), CF_SC_MAJOR, 8, 4, 4, {3,2,1,0}},
    {"Midnight Soul", "R&B", 70, PT(DREAMY), CF_SC_MINOR, 0, 4, 4, {0,3,6,2}},
    {"Disco Floor", "Disco", 71, PT(ENERGETIC), CF_SC_DORIAN, 4, 4, 4, {0,3,0,6}},
    {"Night Funk", "Funk", 72, PT(ENERGETIC), CF_SC_DORIAN, 2, 4, 4, {0,1,3,0}},
    {"Dub Horizon", "Dub / Reggae", 73, PT(DARK), CF_SC_MINOR, 9, 4, 4, {0,6,3,0}},
    {"Island Light", "Reggae", 74, PT(UPLIFTING), CF_SC_MIXOLYDIAN, 9, 4, 4, {0,3,0,6}},
    {"Soft Focus", "Ambient", 75, PT(DREAMY), CF_SC_MAJOR, 0, 6, 4, {0,2,1,3}},
    {"Drifting Signal", "Ambient", 76, PT(DREAMY), CF_SC_LYDIAN, 7, 6, 4, {0,4,1,0}},
    {"Canyon Echo", "Cinematic", 77, PT(DREAMY), CF_SC_DORIAN, 2, 6, 4, {0,6,3,1}},
    {"Moonlit Circle", "Cinematic", 78, PT(DARK), CF_SC_MINOR, 4, 3, 4, {0,3,6,2}},
    {"Canon", "Classical: root seed", 79, PT(FAMILIAR), CF_SC_MAJOR, 2, 4, 8, {0,4,5,2,3,0,3,4}},
    {"Classic Cadence", "Classical: triads", 80, PT(FAMILIAR), CF_SC_MAJOR, 0, 3, 5, {0,3,0,4,0}},
    {"Adapted Fifths", "Classical / Modal", 81, PT(EXPERIMENTAL), CF_SC_MAJOR, 5, 7, 8, {0,3,6,2,5,1,4,0}},
    {"Minor Cadence", "Folk / Natural minor", 82, PT(EMOTIONAL), CF_SC_MINOR, 2, 6, 4, {0,3,4,0}},
    {"Phrygian Dance", "Modal / Phrygian", 83, PT(ENERGETIC), CF_SC_PHRYGIAN, 4, 2, 4, {0,1,0,6}},
    {"Dark Waltz", "Modal / Phrygian", 84, PT(DARK), CF_SC_PHRYGIAN, 2, 3, 4, {0,3,6,1}},
    {"Copper Steps", "Modal / Phrygian", 85, PT(EXPERIMENTAL), CF_SC_PHRYGIAN, 0, 5, 4, {0,5,1,3}},
    {"Seven Torches", "Modal / Phrygian", 86, PT(EXPERIMENTAL), CF_SC_PHRYGIAN, 5, 7, 4, {0,1,3,6}},
    {"Half Light", "Modal / Locrian", 87, PT(DREAMY), CF_SC_LOCRIAN, 0, 6, 4, {0,3,5,1}},
    {"Unsteady", "Modal / Locrian", 88, PT(ENERGETIC), CF_SC_LOCRIAN, 6, 2, 4, {0,1,3,4}},
    {"Edges", "Modal / Locrian", 89, PT(EXPERIMENTAL), CF_SC_LOCRIAN, 11, 5, 4, {0,4,5,3}},
    {"Seven Shadows", "Modal / Locrian", 90, PT(DARK), CF_SC_LOCRIAN, 9, 7, 4, {0,6,5,1}},
    {"Lydian Sprint", "Modal / Lydian", 91, PT(UPLIFTING), CF_SC_LYDIAN, 2, 2, 4, {0,1,4,1}},
    {"Long Arc", "Modal / Lydian", 92, PT(DREAMY), CF_SC_LYDIAN, 5, 4, 4, {0,2,1,4}},
    {"Silver Current", "Modal / Lydian", 93, PT(DREAMY), CF_SC_LYDIAN, 7, 6, 4, {0,1,5,4}},
    {"Wide Angle", "Modal / Mixolydian", 94, PT(EXPERIMENTAL), CF_SC_MIXOLYDIAN, 0, 5, 4, {0,6,3,4}},
    {"Off Beat", "Modal / Mixolydian", 95, PT(ENERGETIC), CF_SC_MIXOLYDIAN, 4, 7, 4, {0,3,4,6}},
    {"Coastal Dance", "Modal / Mixolydian", 96, PT(UPLIFTING), CF_SC_MIXOLYDIAN, 2, 2, 4, {0,6,0,3}},
    {"Dorian Five", "Modal / Dorian", 97, PT(JAZZY), CF_SC_DORIAN, 0, 5, 4, {0,3,1,6}},
    {"Seven Current", "Modal / Dorian", 98, PT(EXPERIMENTAL), CF_SC_DORIAN, 11, 7, 4, {0,1,3,5}},
    {"Minor Two", "Folk / Natural minor", 99, PT(ENERGETIC), CF_SC_MINOR, 2, 2, 4, {0,6,5,4}},
    {"Minor Five", "Cinematic", 100, PT(DARK), CF_SC_MINOR, 5, 5, 4, {0,2,5,3}},
};
#undef PT
_Static_assert(sizeof CF_PROGRESSIONS / sizeof CF_PROGRESSIONS[0] < CF_P_NONE, "browser index fits a byte");

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
uint8_t cf_progression_pick(unsigned current, int turn)
{
    int n = (int)cf_progression_count();
    int pos = current < (unsigned)n ? (int)current : 0;
    /* Reduce before adding, including INT_MIN/INT_MAX encoder input. */
    return (uint8_t)((pos + turn % n + n) % n);
}
uint8_t cf_progression_highlight(const cf_progression_t *p, int scale, int tonic,
                                 int root, uint32_t mask)
{
    uint8_t active = 0;
    /* Masks describe the unvoiced chord. A high #9 is a color, not a minor
     * third; folding upper extensions would misclassify a major chord. */
    uint32_t thirds = mask & ((1u << 3) | (1u << 4));
    if (!p || !cf_harmony_supported(scale, 1) || tonic < 0 || tonic > 11 || root < 0 || root > 11) return 0;
    for (unsigned i = 0; i < p->n && i < CF_P_MAX; i++) {
        uint16_t used, conflict;
        int pc = (tonic + cf_scale_step(scale, p->degree[i])) % 12;
        cf_harmony_t h; cf_harmony_defaults(&h);
        uint32_t triad = cf_harmony_mask(&h, scale, tonic, pc, 0, 0, &used, &conflict);
        if (root == pc && thirds == (triad & ((1u << 3) | (1u << 4)))) active |= (uint8_t)(1u << i);
    }
    return active;
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
