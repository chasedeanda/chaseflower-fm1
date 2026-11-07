/* SPDX-License-Identifier: GPL-3.0-only */
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "cf_progression.h"
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; if (failures <= 10) printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

static void song_seed_fixtures(void)
{
    /* Explicit sounding chord roots/qualities from the documented section
     * reductions, independent of the catalog's degree/scale representation.
     * Pitch classes use C=0; minor=1 means a minor triad, 0 a major triad. */
    static const struct {
        uint16_t id;
        uint8_t n, root[CF_P_MAX], minor[CF_P_MAX];
    } songs[] = {
        {101, 4, {7,2,4,2},       {0,0,1,0}},       /* Daylily: G D Em D */
        {102, 3, {9,4,6},         {0,0,1}},         /* Full Circle: A E F#m */
        {103, 7, {9,5,9,0,9,5,9}, {1,0,1,0,1,0,1}}, /* Kept: Am F Am C Am F Am */
        {104, 5, {4,1,4,1,9},     {0,1,0,1,0}},     /* Keep This Up: E C#m E C#m A */
        {105, 6, {3,8,3,8,5,8},   {0,0,0,0,1,0}},   /* Proper Dose: Eb Ab Eb Ab Fm Ab */
        {106, 3, {6,10,8},        {0,1,0}},         /* Overstepping: Gb Bbm Ab */
        {107, 4, {9,1,4,6},       {0,1,0,1}},       /* Country Girl: A C#m E F#m */
        {108, 5, {7,11,7,11,9},   {0,1,0,1,0}},     /* Placeholder: G Bm G Bm A */
        {109, 3, {6,10,8},        {0,1,0}},         /* Bags: Gb Bbm Ab */
        {110, 4, {2,7,0,5},       {1,0,0,0}},       /* Sofia: Dm G C F */
        {111, 4, {2,0,4,2},       {1,0,1,1}},       /* Pretty Girl: Dm C Em Dm */
        {112, 4, {5,0,7,2},       {0,0,1,1}},       /* Not The Same: F C Gm Dm */
        {113, 4, {10,5,10,5},     {0,0,0,0}},       /* The Shade: Bb F Bb F */
        {114, 4, {0,4,5,7},       {0,1,0,0}},       /* Pluto Projector adaptation: C Em F G */
        {115, 4, {2,9,6,6},       {0,0,1,1}},       /* Bloody Valentine: D A F#m F#m */
        {116, 3, {1,5,3},         {0,1,0}},         /* my ex's best friend: Db Fm Eb */
        {117, 6, {0,4,2,0,7,2},   {0,1,0,0,0,0}},   /* I Think I'm OKAY: C Em D C G D */
        {118, 4, {4,9,2,7},       {1,0,0,0}},       /* I.F.L.Y. adaptation: Em A D G */
        {119, 6, {9,9,6,2,6,2},   {0,0,1,0,1,0}},   /* Intentions: A A F#m D F#m D */
        {120, 4, {9,0,7,5},       {1,0,0,0}},       /* Hurt: Am C G F */
        {121, 3, {9,11,4},        {0,0,0}},         /* Habitual: A B E */
        {122, 6, {5,3,1,8,10,5},  {1,0,0,0,1,1}},   /* Confirmation: Fm Eb Db Ab Bbm Fm */
        {123, 3, {5,4,2},         {0,1,1}},         /* Changes: F Em Dm */
        {124, 4, {1,4,1,4},       {1,0,1,0}},       /* Forever: C#m E C#m E */
        {125, 4, {7,9,7,9},       {1,1,1,1}},       /* Yummy: Gm Am Gm Am */
        {126, 4, {11,4,6,8},      {0,0,0,1}},       /* Concert for Aliens: B E F# G#m */
        {127, 4, {8,4,11,1},      {1,0,0,1}},       /* Sick and Tired: G#m E B C#m */
    };
    cf_harmony_t h;
    cf_harmony_defaults(&h);
    for (unsigned i = 0; i < sizeof songs / sizeof songs[0]; i++) {
        const cf_progression_t *p = 0;
        for (unsigned j = 0; j < cf_progression_count(); j++) {
            const cf_progression_t *candidate = cf_progression_get(j);
            if (candidate->id == songs[i].id) { p = candidate; break; }
        }
        CHECK(p != 0);
        if (!p) continue;
        CHECK(p->n == songs[i].n);
        for (unsigned j = 0; j < songs[i].n && j < p->n; j++) {
            int root = (p->tonic + cf_scale_step(p->scale, p->degree[j])) % 12;
            uint32_t expected = 1u | (1u << (songs[i].minor[j] ? 3 : 4)) | (1u << 7);
            CHECK(root == songs[i].root[j]);
            CHECK(cf_progression_mask(p->scale, p->tonic, p->degree[j], &h) == expected);
        }
    }
}

int main(void)
{
    static const int scale[7] = {CF_SC_MAJOR, CF_SC_MINOR, CF_SC_DORIAN, CF_SC_MIXOLYDIAN, CF_SC_LYDIAN, CF_SC_PHRYGIAN, CF_SC_LOCRIAN};
    /* Independent pitch fixtures, including the seventh's wrap into the next octave. */
    static const int semitone[7][7] = {{0,2,4,5,7,9,11}, {0,2,3,5,7,8,10},
        {0,2,3,5,7,9,10}, {0,2,4,5,7,9,10}, {0,2,4,6,7,9,11},
        {0,1,3,5,7,8,10}, {0,1,3,5,6,8,10}};
    unsigned modes=0, meters=0;
    unsigned tags = 0;
    cf_harmony_t h;
    cf_harmony_defaults(&h);
    CHECK(cf_progression_count() == 127);
    song_seed_fixtures();
    CHECK(!cf_progression_get(CF_P_NONE) && !cf_progression_get(UINT_MAX));
    for (unsigned i = 0; i < cf_progression_count(); i++) {
        const cf_progression_t *p = cf_progression_get(i);
        CHECK(p && p->id && p->name && strlen(p->name) < 20 && p->n >= 3 && p->n <= CF_P_MAX);
        CHECK(p->context && p->tonic<12 && cf_harmony_supported(p->scale,1));
        CHECK(p->beats>=2 && p->beats<=7);
        modes |= 1u << p->scale; meters |= 1u << p->beats;
        for (unsigned j = 0; j < p->n; j++) CHECK(p->degree[j] < 7);
        for (unsigned j = 0; j < p->n; j++) {
            int root=(p->tonic+cf_scale_step(p->scale,p->degree[j]))%12;
            uint32_t mask=cf_progression_mask(p->scale,p->tonic,p->degree[j],&h);
            uint8_t expected=0;
            for(unsigned k=0;k<p->n;k++) if(p->degree[k]==p->degree[j]) expected|=(uint8_t)(1u<<k);
            CHECK(cf_progression_highlight(p,p->scale,p->tonic,root,mask | (1u<<10) | (1u<<14))==expected);
            CHECK(cf_progression_highlight(p,p->scale,p->tonic,root,mask | (1u<<15) | (1u<<16))==expected);
            CHECK(!cf_progression_highlight(p,p->scale,p->tonic,root,mask ^ (1u<<3) ^ (1u<<4)));
            CHECK(!cf_progression_highlight(p,p->scale,p->tonic,(root+1)%12,mask));
        }
        for (unsigned j = i+1; j < cf_progression_count(); j++) CHECK(p->id != cf_progression_get(j)->id);
        tags |= p->tags;
    }
    CHECK(tags == (1u << (CF_P_NTAGS-1)) - 1);
    CHECK(meters==0xFCu);
    unsigned expected_modes=0; for(int i=0;i<7;i++) expected_modes |= 1u << scale[i];
    CHECK(modes==expected_modes);
    for(unsigned i=0;i<cf_progression_count();i++) {
        CHECK(cf_progression_pick(i,1)==(i+1)%cf_progression_count());
        CHECK(cf_progression_pick(i,-1)==(i+cf_progression_count()-1)%cf_progression_count());
        CHECK(cf_progression_pick(i,0)==i);
        CHECK(cf_progression_pick(i,INT_MIN)<cf_progression_count());
        CHECK(cf_progression_pick(i,INT_MAX)<cf_progression_count());
    }
    CHECK(cf_progression_pick(CF_P_NONE,0)==0 && cf_progression_pick(UINT_MAX,0)==0);
    for (int sc = 0; sc < 7; sc++) {
        for (int tonic = 0; tonic < 12; tonic++) for (int degree = 0; degree < 7; degree++) {
            cf_chord_cfg_t cfg;
            cf_chord_defaults(&cfg);
            cfg.algo = CF_ALG_TOP_LOCK; cfg.scale = (uint8_t)scale[sc]; cfg.tonic = (uint8_t)tonic;
            for (int style = CF_H_BASIC; style <= CF_H_CUSTOM; style++) {
                h.style = (uint8_t)style; h.custom = CF_MOD_13;
                int idx = degree + 2;
                int third = semitone[sc][idx%7] + 12*(idx/7) - semitone[sc][degree];
                unsigned expected = 1u | (1u << (third < 4 ? 3 : 4)) | (1u << 7);
                CHECK(cf_progression_mask(scale[sc], tonic, degree, &h) == expected);
                for (int quant = CF_Q_BASIC; quant <= CF_Q_FULL; quant++) for (int oct = -3; oct <= 3; oct++) {
                    cfg.quantize = (uint8_t)quant; cfg.octave = (int8_t)oct;
                    int key = cf_progression_key(&cfg, degree);
                    int expected_note = 60 + tonic - (tonic > 6 ? 12 : 0) + semitone[sc][degree] + 12*(oct-1);
                    CHECK(key >= 0 && cf_key_root_note(&cfg, key) == expected_note);
                    cf_chord_t model; cf_chord_init(&model, &cfg);
                    cf_harmony_t auto_h = h; auto_h.mode = CF_H_AUTO;
                    cf_chord_harmony(&model, &auto_h); cf_chord_key(&model, key, 1);
                    CHECK(model.out.sounding && model.out.mask == expected && model.out.root_pc == expected_note % 12);
                }
            }
        }
    }
    static const char *const names[4] = {"C", "G", "Am", "F"};
    cf_spell_t sp; cf_key_spell(0,CF_SC_MAJOR,&sp); h.style=CF_H_BASIC;
    const cf_progression_t *p = cf_progression_get(0);
    for (unsigned i=0;i<4;i++) {
        char name[16]; int root=semitone[0][p->degree[i]];
        cf_chord_name(root,cf_progression_mask(CF_SC_MAJOR,0,p->degree[i],&h),-1,&sp,name);
        CHECK(!strcmp(name,names[i]));
    }
    CHECK(!cf_progression_mask(CF_SC_MAJOR,0,7,&h));
    CHECK(!cf_progression_mask(CF_SC_MINOR,12,0,&h));
    CHECK(!cf_progression_mask(CF_SC_MAJOR,0,0,0));
    CHECK(!cf_progression_mask(CF_SC_MAJ_PENT,0,0,&h));
    CHECK(cf_progression_key(0,0) == -1);
    cf_chord_cfg_t cfg; cf_chord_defaults(&cfg); cfg.quantize=CF_Q_OFF;
    CHECK(cf_progression_key(&cfg,0) == -1);
    cfg.quantize=CF_Q_FULL; cfg.octave=127;
    CHECK(cf_progression_key(&cfg,0) == -1);
    printf("progression library: %u checks, %u failures\n",checks,failures);
    return failures != 0;
}
