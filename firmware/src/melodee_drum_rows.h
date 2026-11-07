/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio)
 * Copyright (C) 2026 Chase FM-1 contributors (recipe adaptation)
 *
 * Data-only adaptations of Melodee's drum_cr78.c CR_SOUNDS and drum_fm.c FMK_SOUNDS:
 * https://github.com/keremimo/melodee/tree/165d177a425944f3d30b0b669255a6ed0050c3a5/firmware/src
 * Pinned source: 165d177a425944f3d30b0b669255a6ed0050c3a5. Included inside DS_KITS.
 *
 * These use CoralFlower's existing ds_snd_t renderer, not Melodee's engines. CR-78 pulse-excited
 * resonators, ringing retriggers, separate modal decays and noise attack are approximated. FM has
 * no feedback here and its index follows the carrier envelope, rather than a separate decay.
 * Source carrier/noise frequencies and ratios are retained where this renderer permits them.
 * A single shared filter replaces the source's independently filtered components; Q is at most 2.
 *
 * Mappings (rounded): pitch = 16 * (69 + 12 * log2(Hz / 440)); ratio Q8 -> Q12 by *16;
 * source -40 dB time t_ms -> tau = t_ms / ln(100), then DS time = 10 * log2(tau / 0.3 ms).
 * Cutoff = 127 * log(Hz / 30) / log(16000 / 30); bend = source semitones * 4.
 * DSW_FM's maximum initial index is about 2*pi radians: o2 = source index * 127 / (32*pi),
 * where source index is radians *16. Its hat/cymbal depths are clamped to this smaller range.
 * CR-78's 8% tom pitch drop becomes about 1.25 semitones. The one tom row uses the source's
 * middle tom; existing GM offsets supply the other pitches. CLAP plays the retro tambourine.
 * Source gains do not transfer directly between these renderers: conservative initial balances
 * are provided for audition. No added grit, room, morph or random variation; A and B are identical.
 */

/* Existing DS field order: wave, noise, filter, teeth, route, pitch, metal pitch, ratio,
 * bend, btau, odec, o2, olev, cut, res, ndec, nlev, fm, gap, click, drive, level.
 * Role order: kick, snare, clap, closed hat, open hat, tom, rim, bell, cymbal. */
#define MELODEE_RETRO_ROWS \
    /* 62 Hz round kick, 260 ms to -40 dB; its source 900 Hz tone filter. */ \
    DS(DSW_SINE, DSN_NONE, DSF_LP, 0, 1, 561, 0, 0, 0, 0, 76, 0, 110, 69, 0, 0, 0, 0, 0, 0, 0, 96), \
    /* 190 + 330 Hz body, 70 ms; 1800 Hz high-passed noise, 170 ms. */ \
    DS(DSW_ADD, DSN_WHITE, DSF_HP, 0, 0, 871, 0, 7114, 0, 0, 57, 127, 60, 83, 50, 69, 100, 0, 0, 0, 0, 96), \
    /* Tambourine: 5100 + 7300 Hz squares, 180 ms; shared 6500 Hz BP for jingles/noise. */ \
    DS(DSW_SQ2, DSN_WHITE, DSF_BP, 0, 1, 1783, 0, 5863, 0, 0, 70, 127, 40, 109, 113, 73, 95, 0, 0, 0, 0, 96), \
    /* Noise-only hats, 7000 Hz HP; 45 / 320 ms to -40 dB. */ \
    DS(DSW_SINE, DSN_WHITE, DSF_HP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 50, 50, 110, 0, 0, 0, 0, 94), \
    DS(DSW_SINE, DSN_WHITE, DSF_HP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 50, 79, 110, 0, 0, 0, 0, 88), \
    /* 165 Hz middle tom, 200 ms decay and 8% pitch drop; source LP 3500 Hz. */ \
    DS(DSW_SINE, DSN_NONE, DSF_LP, 0, 1, 832, 0, 0, 5, 62, 72, 0, 110, 96, 0, 0, 0, 0, 0, 0, 0, 96), \
    /* 520 + 1650 Hz rim; one 18 ms envelope approximates the two source networks. */ \
    DS(DSW_ADD, DSN_NONE, DSF_LP, 0, 1, 1150, 0, 12997, 0, 0, 37, 64, 127, 121, 0, 0, 0, 0, 0, 0, 0, 90), \
    /* Cowbell's 587 + 845 Hz squares through a 900 Hz BP; source Q3 clamped to Q2. */ \
    DS(DSW_SQ2, DSN_NONE, DSF_BP, 0, 1, 1184, 0, 5896, 0, 0, 73, 127, 100, 69, 127, 0, 0, 0, 0, 0, 0, 96), \
    /* Noise cymbal, 8500 Hz BP, 1100 ms to -40 dB. */ \
    DS(DSW_SINE, DSN_WHITE, DSF_BP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 114, 75, 96, 100, 0, 0, 0, 0, 90)

#define MELODEE_FM_ROWS \
    /* 52 Hz / ratio 1 kick, index 1.5 rad; 24-semitone fall in 45 ms. */ \
    DS(DSW_FM, DSN_NONE, DSF_LP, 0, 0, 512, 0, 4096, 96, 50, 83, 30, 110, 127, 0, 0, 0, 0, 0, 0, 0, 96), \
    /* 185 Hz / ratio 1.56 snare, index 2 rad; 1500 Hz high-passed noise. */ \
    DS(DSW_FM, DSN_WHITE, DSF_HP, 0, 0, 864, 0, 6384, 28, 42, 67, 40, 80, 79, 113, 70, 74, 0, 0, 0, 0, 96), \
    /* Source four noise bursts 10 ms apart, followed by a 160 ms tail. */ \
    DS(DSW_SINE, DSN_WHITE, DSF_BP, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 113, 69, 110, 0, 88, 0, 0, 100), \
    /* 3020 Hz / ratio 1.414 hats through 6000 Hz HP; index capped, no feedback. */ \
    DS(DSW_FM, DSN_NONE, DSF_HP, 0, 1, 1638, 0, 5792, 0, 0, 50, 127, 93, 107, 50, 0, 0, 0, 0, 0, 0, 96), \
    DS(DSW_FM, DSN_NONE, DSF_HP, 0, 1, 1638, 0, 5792, 0, 0, 80, 127, 82, 107, 50, 0, 0, 0, 0, 0, 0, 90), \
    /* Source middle tom: 130 Hz / ratio 1, 7-semitone fall, index 1.1875 rad. */ \
    DS(DSW_FM, DSN_NONE, DSF_LP, 0, 0, 766, 0, 4096, 28, 63, 79, 24, 110, 127, 0, 0, 0, 0, 0, 0, 0, 94), \
    /* 460 Hz / ratio 3.46 rim, index 2.5 rad, 30 ms to -40 dB. */ \
    DS(DSW_FM, DSN_NONE, DSF_LP, 0, 0, 1116, 0, 14176, 0, 0, 44, 51, 110, 127, 0, 0, 0, 0, 0, 0, 0, 94), \
    /* 830 Hz / ratio 3.5 bell, index 3 rad, 900 ms to -40 dB. */ \
    DS(DSW_FM, DSN_NONE, DSF_LP, 0, 0, 1280, 0, 14336, 0, 0, 93, 61, 90, 127, 0, 0, 0, 0, 0, 0, 0, 90), \
    /* 2100 Hz / ratio 2.31 cymbal with a little noise; shared output HP 4000 Hz. */ \
    DS(DSW_FM, DSN_WHITE, DSF_HP, 0, 1, 1537, 0, 9456, 0, 0, 99, 127, 75, 99, 50, 95, 18, 0, 0, 0, 0, 84)

    {"M Retro", {MELODEE_RETRO_ROWS}, {MELODEE_RETRO_ROWS}, 0, 0, 1, 0, 0, 0},
    {"M FM", {MELODEE_FM_ROWS}, {MELODEE_FM_ROWS}, 0, 0, 1, 0, 0, 0},

#undef MELODEE_RETRO_ROWS
#undef MELODEE_FM_ROWS
