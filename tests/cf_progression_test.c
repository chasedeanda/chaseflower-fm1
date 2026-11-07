/* SPDX-License-Identifier: GPL-3.0-only */
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "cf_progression.h"
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; if (failures <= 10) printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

int main(void)
{
    static const int scale[4] = {CF_SC_MAJOR, CF_SC_MINOR, CF_SC_DORIAN, CF_SC_MIXOLYDIAN};
    /* Independent pitch fixtures, including the seventh's wrap into the next octave. */
    static const int semitone[4][7] = {{0,2,4,5,7,9,11}, {0,2,3,5,7,8,10},
        {0,2,3,5,7,9,10}, {0,2,4,5,7,9,10}};
    static const unsigned counts[4] = {8,8,2,2};
    unsigned tags = 0;
    cf_harmony_t h;
    cf_harmony_defaults(&h);
    CHECK(cf_progression_count() == 20);
    CHECK(!cf_progression_get(CF_P_NONE) && !cf_progression_get(UINT_MAX));
    for (unsigned i = 0; i < cf_progression_count(); i++) {
        const cf_progression_t *p = cf_progression_get(i);
        CHECK(p && p->id && p->name && strlen(p->name) < 20 && p->n >= 3 && p->n <= CF_P_MAX);
        for (unsigned j = 0; j < p->n; j++) CHECK(p->degree[j] < 7);
        for (unsigned j = i+1; j < cf_progression_count(); j++) CHECK(p->id != cf_progression_get(j)->id);
        tags |= p->tags;
    }
    CHECK(tags == (1u << (CF_P_NTAGS-1)) - 1);
    for (int sc = 0; sc < 4; sc++) {
        CHECK(cf_progression_filtered_count(scale[sc], CF_P_ALL) == counts[sc]);
        for (int tag = 0; tag < CF_P_NTAGS; tag++) {
            unsigned n = cf_progression_filtered_count(scale[sc], tag);
            uint8_t first = cf_progression_pick(scale[sc], tag, CF_P_NONE, 0);
            if (!n) { CHECK(first == CF_P_NONE); continue; }
            CHECK(cf_progression_matches(first, scale[sc], tag));
            CHECK(cf_progression_position(scale[sc], tag, first) == 1);
            CHECK(cf_progression_pick(scale[sc], tag, first, INT_MIN) == first);
            uint8_t last = cf_progression_pick(scale[sc], tag, first, INT_MAX);
            CHECK(cf_progression_position(scale[sc], tag, last) == n);
            for (unsigned i = 0; i < n; i++) {
                uint8_t picked = cf_progression_pick(scale[sc], tag, first, (int)i);
                CHECK(cf_progression_position(scale[sc], tag, picked) == i+1);
                CHECK(cf_progression_pick(scale[sc], tag, picked, 0) == picked);
            }
        }
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
    CHECK(cf_progression_pick(-1,0,0,1) == CF_P_NONE);
    CHECK(cf_progression_pick(CF_NSCALES,0,0,1) == CF_P_NONE);
    CHECK(cf_progression_pick(CF_SC_MAJOR,-1,0,1) == CF_P_NONE);
    CHECK(cf_progression_pick(CF_SC_MAJOR,CF_P_NTAGS,0,1) == CF_P_NONE);
    CHECK(cf_progression_pick(CF_SC_LYDIAN,0,0,0) == CF_P_NONE);
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
