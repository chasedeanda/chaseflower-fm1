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
    {"The Only Exception", "Paramore: verse", 20, PT(FAMILIAR), CF_SC_MIXOLYDIAN, 11, 6, 4, {0,0,4,3}},
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
    {"Only Exception", "Paramore: chorus", 56, PT(FAMILIAR), CF_SC_MIXOLYDIAN, 11, 6, 3, {0,4,3}},
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
    /* Sourced song sections (preserve written chord qualities); see docs/research/SPOTIFY-COVER-SHORTLIST.md.
     * Meter 4 is a practice setup, not a claim about the original arrangement. */
    {"Daylily", "Movements: chorus", 101, PT(FAMILIAR), CF_SC_MAJOR, 2, 4, 4, {3,0,1,0}},
    {"Full Circle", "Movements: verse", 102, PT(FAMILIAR), CF_SC_MAJOR, 4, 4, 3, {3,0,1}},
    {"Kept", "Movements: chorus", 103, PT(FAMILIAR), CF_SC_MINOR, 9, 4, 7, {0,5,0,2,0,5,0}},
    {"Keep This Up", "TSSF: verse", 104, PT(FAMILIAR), CF_SC_MAJOR, 4, 4, 5, {0,5,0,5,3}},
    {"Proper Dose", "TSSF: verse", 105, PT(FAMILIAR), CF_SC_MAJOR, 3, 4, 6, {0,3,0,3,1,3}},
    {"Overstepping", "Belmont: verse", 106, PT(FAMILIAR), CF_SC_MAJOR, 1, 4, 3, {3,5,4}},
    {"Country Girl", "Belmont: chorus", 107, PT(FAMILIAR), CF_SC_MAJOR, 4, 4, 4, {3,5,0,1}},
    {"Placeholder", "TSSF: chorus", 108, PT(FAMILIAR), CF_SC_MAJOR, 2, 4, 5, {3,5,3,5,4}},
    {"Bags", "Clairo: verse", 109, PT(FAMILIAR), CF_SC_MAJOR, 1, 4, 3, {3,5,4}},
    {"Sofia", "Clairo: main loop", 110, PT(FAMILIAR), CF_SC_MAJOR, 0, 4, 4, {1,4,0,3}},
    {"Pretty Girl", "Clairo: chorus", 111, PT(FAMILIAR), CF_SC_MAJOR, 0, 4, 4, {1,0,2,1}},
    {"Not The Same", "Rex: chorus", 112, PT(FAMILIAR), CF_SC_MAJOR, 5, 4, 4, {0,4,1,5}},
    {"The Shade", "Rex: chorus", 113, PT(FAMILIAR), CF_SC_MAJOR, 5, 4, 4, {3,0,3,0}},
    {"Pluto Projector", "Rex: verse", 114, PT(FAMILIAR), CF_SC_MAJOR, 0, 4, 7, {0,2,3,3,4,4,4}},
    {"I.F.L.Y.", "Bazzi: verse", 118, PT(FAMILIAR), CF_SC_MAJOR, 2, 4, 4, {1,4,0,3}},
    {"Intentions", "Bieber: chorus", 119, PT(FAMILIAR), CF_SC_MAJOR, 9, 4, 6, {0,0,5,3,5,3}},
    {"Hurt", "Oliver Tree: pre", 120, PT(FAMILIAR), CF_SC_MINOR, 9, 4, 4, {0,2,6,5}},
    {"Habitual", "Bieber: chorus", 121, PT(FAMILIAR), CF_SC_MAJOR, 4, 4, 3, {3,4,0}},
    {"Confirmation", "Bieber: verse", 122, PT(FAMILIAR), CF_SC_MINOR, 5, 4, 6, {0,6,5,2,3,0}},
    {"Changes", "Bieber: verse", 123, PT(FAMILIAR), CF_SC_MAJOR, 0, 4, 3, {3,2,1}},
    {"Forever", "Bieber: chorus", 124, PT(FAMILIAR), CF_SC_MINOR, 1, 4, 4, {0,2,0,2}},
    {"Yummy", "Bieber: chorus", 125, PT(FAMILIAR), CF_SC_MAJOR, 5, 4, 4, {1,2,1,2}},
    /* Morgan Wallen playlist sections; full sources/tuning in the research notes. */
    {"Last Night", "Wallen: chorus", 128, PT(FAMILIAR), CF_SC_MINOR, 3, 4, 4, {5,6,0,2}},
    {"Cover Me Up", "Wallen: chorus", 129, PT(FAMILIAR), CF_SC_MAJOR, 2, 6, 6, {1,3,5,0,3,0}},
    {"One Thing At A Time", "Wallen: verse", 130, PT(FAMILIAR), CF_SC_MAJOR, 7, 4, 4, {3,4,0,1}},
    {"Everything I Love", "Wallen: prechorus", 131, PT(FAMILIAR), CF_SC_MAJOR, 8, 4, 3, {5,3,0}},
    {"I Can't Love You...", "Ella/Wallen: chorus", 132, PT(FAMILIAR), CF_SC_MINOR, 10, 4, 3, {5,6,0}},
    {"Thought You Should", "Wallen: chorus", 133, PT(FAMILIAR), CF_SC_MAJOR, 6, 4, 3, {0,4,1}},
    {"Cowgirls", "Wallen: chorus", 134, PT(FAMILIAR), CF_SC_MINOR, 5, 4, 4, {0,5,2,6}},
    {"What I Want", "Wallen/Tate: chorus", 135, PT(FAMILIAR), CF_SC_DORIAN, 11, 4, 3, {0,6,3}},
};
#undef PT
_Static_assert(sizeof CF_PROGRESSIONS / sizeof CF_PROGRESSIONS[0] < CF_P_NONE, "browser index fits a byte");

/* Full source harmony. Factory chord colors are data, not global button state.
 * Sources and the audit of unchanged triad sections are in the cover notes. */
static const struct { uint16_t id; cf_source_chord_t chord[8]; } CF_SOURCE[] = {
    /* The Only Exception: B B F#m6 Emaj7 */
    {20, {{0x91u,0,0,-1}, {0x91u,0,0,-1}, {0x289u,4,0,-1}, {0x891u,3,0,-1}}},
    /* Still Into You verse: F5 F5 D5 D5 */
    {55, {{0x81u,0,0,-1}, {0x81u,0,0,-1}, {0x81u,5,0,-1}, {0x81u,5,0,-1}}},
    /* The Only Exception chorus: B F#m6 Emaj7 */
    {56, {{0x91u,0,0,-1}, {0x289u,4,0,-1}, {0x891u,3,0,-1}}},
    /* Daylily: Gmaj7 D Em D */
    {101, {{0x891u,3,0,-1}, {0x91u,0,0,-1}, {0x89u,1,0,-1}, {0x91u,0,0,-1}}},
    /* Sofia: Dm7 G7 Cmaj7 Fmaj7 */
    {110, {{0x489u,1,0,-1}, {0x491u,4,0,-1}, {0x891u,0,0,-1}, {0x891u,3,0,-1}}},
    /* Pretty Girl: Dm7 Cmaj7 Em7 Dm7 */
    {111, {{0x489u,1,0,-1}, {0x891u,0,0,-1}, {0x489u,2,0,-1}, {0x489u,1,0,-1}}},
    /* The Shade: Bbmaj7 Fmaj7 Bbmaj7 Fmaj7 */
    {113, {{0x891u,3,0,-1}, {0x891u,0,0,-1}, {0x891u,3,0,-1}, {0x891u,0,0,-1}}},
    /* Pluto: C Em7 Fmaj7 F#m7b5 G7 G7sus4 G7 */
    {114, {{0x91u,0,0,-1}, {0x489u,2,0,-1}, {0x891u,3,0,-1}, {0x449u,3,1,-1}, {0x491u,4,0,-1}, {0x4a1u,4,0,-1}, {0x491u,4,0,-1}}},
    /* I.F.L.Y.: Em9 A13 Dmaj9 Gmaj7 */
    {118, {{0x4489u,1,0,-1}, {0x204491u,4,0,-1}, {0x4891u,0,0,-1}, {0x891u,3,0,-1}}},
    /* Changes: Fmaj7 Em7 Dm7 */
    {123, {{0x891u,3,0,-1}, {0x489u,2,0,-1}, {0x489u,1,0,-1}}},
    /* Yummy: Gm7 Am7 Gm7 Am7 */
    {125, {{0x489u,1,0,-1}, {0x489u,2,0,-1}, {0x489u,1,0,-1}, {0x489u,2,0,-1}}},
    /* Last Night: B Db Ebm Gb (C D Em G shapes, down a semitone) */
    {128, {{0x91u,5,0,-1}, {0x91u,6,0,-1}, {0x89u,0,0,-1}, {0x91u,2,0,-1}}},
    /* Cover Me Up: Em G Bm D G D */
    {129, {{0x89u,1,0,-1}, {0x91u,3,0,-1}, {0x89u,5,0,-1}, {0x91u,0,0,-1}, {0x91u,3,0,-1}, {0x91u,0,0,-1}}},
    /* One Thing At A Time: C D G Am7 */
    {130, {{0x91u,3,0,-1}, {0x91u,4,0,-1}, {0x91u,0,0,-1}, {0x489u,1,0,-1}}},
    /* Everything I Love: Fm Db Ab (Em C G shapes, capo 1) */
    {131, {{0x89u,5,0,-1}, {0x91u,3,0,-1}, {0x91u,0,0,-1}}},
    /* I Can't Love You Anymore (Ella Langley & Wallen): Gbmaj7 Ab Bbm */
    {132, {{0x891u,5,0,-1}, {0x91u,6,0,-1}, {0x89u,0,0,-1}}},
    /* Thought You Should Know: F# C#/E# G#m7 (G D/F# Am7, down a semitone) */
    {133, {{0x91u,0,0,-1}, {0x91u,4,0,4}, {0x489u,1,0,-1}}},
    /* Cowgirls: Fm Db Ab Eb */
    {134, {{0x89u,0,0,-1}, {0x91u,5,0,-1}, {0x91u,2,0,-1}, {0x91u,6,0,-1}}},
    /* What I Want (with Tate McRae): Bm A E; preserve Dorian's major IV. */
    {135, {{0x89u,0,0,-1}, {0x91u,6,0,-1}, {0x91u,3,0,-1}}},
};
static const cf_progression_t *cfp_draft_view;
static const cf_source_chord_t *cfp_draft_source;
void cf_progression_draft_register(const cf_progression_t *p,const cf_source_chord_t *source)
{ cfp_draft_view=p;cfp_draft_source=source; }
const cf_source_chord_t *cf_progression_source(const cf_progression_t *p)
{
    if (!p) return 0;
    if(p==cfp_draft_view)return cfp_draft_source;
    for (unsigned i=0; i<sizeof CF_SOURCE/sizeof CF_SOURCE[0]; i++)
        if (CF_SOURCE[i].id==p->id) return CF_SOURCE[i].chord;
    return 0;
}
int cf_progression_root(const cf_progression_t *p, unsigned slot, int scale, int tonic)
{
    if (!p || slot>=p->n || tonic<0 || tonic>11 || !cf_harmony_supported(scale,1)) return -1;
    const cf_source_chord_t *s=cf_progression_source(p);
    int degree=s?s[slot].degree:p->degree[slot];
    return (tonic+cf_scale_step(scale,degree)+(s?s[slot].alter:0)+12)%12;
}
uint32_t cf_progression_slot_mask(const cf_progression_t *p, unsigned slot, int scale, int tonic, const cf_harmony_t *h)
{
    if (!p || slot>=p->n || !h || !cf_harmony_supported(scale,1) || tonic<0 || tonic>11) return 0;
    const cf_source_chord_t *s=cf_progression_source(p);
    return s?s[slot].mask:cf_progression_mask(scale,tonic,p->degree[slot],h);
}
int cf_progression_slot_key(const cf_progression_t *p, unsigned slot, const cf_chord_cfg_t *cfg)
{
    if (!cfg || !p || slot>=p->n) return -1;
    int base=cf_progression_key(cfg,p->degree[slot]);
    const cf_source_chord_t *source=cf_progression_source(p);
    if(base<0 || !source || !source[slot].alter) return base;
    int target=cf_key_root_note(cfg,base)+source[slot].alter;
    for(int key=0;key<CF_NKEYS;key++) {
        if(cf_key_root_note(cfg,key)==target) return key;
    }
    return -1;
}

const char *cf_progression_tag(int tag)
{
    static const char *const names[CF_P_NTAGS] = {"All", "Familiar", "Emotional", "Dreamy", "Dark",
        "Uplifting", "Jazzy", "Energetic", "Experimental"};
    return names[tag >= 0 && tag < CF_P_NTAGS ? tag : 0];
}
_Static_assert(sizeof CF_PROGRESSIONS/sizeof CF_PROGRESSIONS[0]<CF_P_DRAFT,"reserved draft index");
unsigned cf_progression_count(void) { return sizeof CF_PROGRESSIONS / sizeof CF_PROGRESSIONS[0]; }
const cf_progression_t *cf_progression_get(unsigned index)
{
    if(index==CF_P_DRAFT)return cfp_draft_view;
    return index < cf_progression_count() ? &CF_PROGRESSIONS[index] : 0;
}
uint8_t cf_progression_pick(unsigned current, int turn)
{
    if(current==CF_P_DRAFT && !turn && cfp_draft_view)return CF_P_DRAFT;
    int n = (int)cf_progression_count();
    int pos = current < (unsigned)n ? (int)current : 0;
    /* Reduce before adding, including INT_MIN/INT_MAX encoder input. */
    return (uint8_t)((pos + turn % n + n) % n);
}
uint16_t cf_progression_highlight(const cf_progression_t *p, int scale, int tonic,
                                 int root, uint32_t mask)
{
    uint16_t active = 0;
    /* Masks describe the unvoiced chord. A high #9 is a color, not a minor
     * third; folding upper extensions would misclassify a major chord. */
    uint32_t thirds = mask & ((1u << 3) | (1u << 4));
    if (!p || !cf_harmony_supported(scale, 1) || tonic < 0 || tonic > 11 || root < 0 || root > 11) return 0;
    for (unsigned i = 0; i < p->n && i < CF_P_MAX; i++) {
        uint16_t used, conflict;
        int pc = cf_progression_root(p,i,scale,tonic);
        cf_harmony_t h; cf_harmony_defaults(&h);
        uint32_t triad = cf_harmony_mask(&h, scale, tonic, pc, 0, 0, &used, &conflict);
        const cf_source_chord_t *source=cf_progression_source(p);
        uint32_t wanted=source?source[i].mask:triad;
        if (root == pc && thirds == (wanted & ((1u<<3)|(1u<<4))) &&
            (!source || (mask & wanted)==wanted)) active |= (uint16_t)(1u << i);
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
