/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include <string.h>
#include "cf_chord.h"
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; fprintf(stderr, "FAIL %d: %s\n", __LINE__, #x); } } while (0)
static uint32_t bits(int a, int b, int c) { return (1u << a) | (1u << b) | (1u << c); }
static void name_is(int root, uint32_t mask, const char *want)
{
    cf_spell_t sp; char name[16]; cf_key_spell(0, CF_SC_MAJOR, &sp);
    cf_chord_name(root, mask, -1, &sp, name);
    CHECK(!strcmp(name, want));
}
static int button(int algo, int idx)
{
    for (int k = 0; k < CF_NKEYS; k++) { int n; if (cf_key_role(algo, k, &n) == CF_ROLE_BUTTON && n == idx) return k; }
    return -1;
}
int main(void)
{
    cf_harmony_t h; uint32_t stamp[CF_H_SLOTS] = {0}; uint16_t used, conflict;
    cf_harmony_defaults(&h); h.mode = CF_H_AUTO;
    struct { int scale, tonic; int roots[7], thirds[7], fifths[7], sevenths[7]; } keys[] = {
        {CF_SC_MAJOR,0,{0,2,4,5,7,9,11},{4,3,3,4,4,3,3},{7,7,7,7,7,7,6},{11,10,10,11,10,10,10}},
        {CF_SC_MAJOR,7,{7,9,11,0,2,4,6},{4,3,3,4,4,3,3},{7,7,7,7,7,7,6},{11,10,10,11,10,10,10}},
        {CF_SC_MINOR,9,{9,11,0,2,4,5,7},{3,3,4,3,3,4,4},{7,6,7,7,7,7,7},{10,10,11,10,10,11,10}},
        {CF_SC_DORIAN,2,{2,4,5,7,9,11,0},{3,3,4,4,3,3,4},{7,7,7,7,7,6,7},{10,10,11,10,10,10,11}},
        {CF_SC_MIXOLYDIAN,7,{7,9,11,0,2,4,5},{4,3,3,4,3,3,4},{7,7,6,7,7,7,7},{10,10,10,11,10,10,11}}
    };
    /* Independent semitone fixtures, tested through the actual input model. */
    for (unsigned k = 0; k < sizeof keys / sizeof *keys; k++) for (int d = 0; d < 7; d++)
    for (int a = 0; a < CF_NALGS; a++) for (int q = CF_Q_BASIC; q <= CF_Q_FULL; q++) {
        cf_chord_cfg_t g; cf_chord_t c; cf_chord_defaults(&g);
        g.algo = (uint8_t)a; g.scale = (uint8_t)keys[k].scale; g.tonic = (uint8_t)keys[k].tonic; g.quantize = (uint8_t)q;
        cf_chord_init(&c, &g); h.style = CF_H_BASIC; cf_chord_harmony(&c, &h);
        int key = CF_KEY_EXT + 60 + keys[k].roots[d];
        cf_chord_key(&c, key, 1);
        uint32_t triad = bits(0,keys[k].thirds[d],keys[k].fifths[d]);
        CHECK(c.out.mask == triad && c.out.n == 3 && !c.out.single && c.out.automatic && !c.out.fallback);
        h.style = CF_H_RICH; cf_chord_harmony(&c, &h);
        CHECK(c.out.mask == (triad | 1u << keys[k].sevenths[d]) && c.out.n == 4);
        cf_chord_key(&c, key, 0); CHECK(!c.out.sounding && !c.out.n);
    }
    cf_harmony_defaults(&h); h.mode = CF_H_AUTO;
    h.slot[0] = CF_MOD_7;
    uint32_t m = cf_harmony_mask(&h, CF_SC_MAJOR, 0, 0, 1, stamp, &used, &conflict);
    name_is(0,m,"Cmaj7");
    h.slot[0] = CF_MOD_9; m = cf_harmony_mask(&h,0,0,0,1,stamp,&used,&conflict); name_is(0,m,"Cmaj9");
    h.slot[0] = CF_MOD_ADD9; m = cf_harmony_mask(&h,0,0,0,1,stamp,&used,&conflict); name_is(0,m,"Cadd9");
    CHECK(!(m & ((1u << 10) | (1u << 11))));
    h.style=CF_H_DREAMY; m=cf_harmony_mask(&h,0,0,0,0,stamp,&used,&conflict); name_is(0,m,"Cmaj9");
    h.style=CF_H_CUSTOM; h.custom=CF_MOD_13; m=cf_harmony_mask(&h,0,0,0,0,stamp,&used,&conflict); name_is(0,m,"Cmaj13");
    h.style=CF_H_BASIC;
    h.slot[0]=CF_MOD_SUS2; m=cf_harmony_mask(&h,CF_SC_PHRYGIAN,0,0,1,stamp,&used,&conflict); name_is(0,m,"Csusb2");
    h.slot[0]=CF_MOD_SUS4; m=cf_harmony_mask(&h,CF_SC_LYDIAN,0,0,1,stamp,&used,&conflict); name_is(0,m,"Csus#4");
    h.slot[0]=CF_MOD_ADD4; m=cf_harmony_mask(&h,CF_SC_LYDIAN,0,0,1,stamp,&used,&conflict); name_is(0,m,"Cadd#4");
    h.slot[0]=CF_MOD_AUG; h.slot[1]=CF_MOD_9; m=cf_harmony_mask(&h,0,0,0,3,stamp,&used,&conflict); name_is(0,m,"Cmaj9#5");
    h.slot[0]=CF_MOD_POWER; h.slot[1]=CF_MOD_9; m=cf_harmony_mask(&h,0,0,0,3,stamp,&used,&conflict); name_is(0,m,"Cmaj9no3");
    h.slot[0] = CF_MOD_SUS4; h.slot[1] = CF_MOD_9;
    m = cf_harmony_mask(&h,0,0,0,3,stamp,&used,&conflict); name_is(0,m,"Cmaj9sus4"); CHECK(!(m & (1u << 4)));
    h.slot[0] = CF_MOD_9; m = cf_harmony_mask(&h,0,0,11,1,stamp,&used,&conflict); name_is(11,m,"Bm7b5b9");
    h.slot[0] = CF_MOD_MIN; h.slot[1] = CF_MOD_MAJ; stamp[0]=2; stamp[1]=1;
    m = cf_harmony_mask(&h,0,0,0,3,stamp,&used,&conflict); CHECK(m == bits(0,3,7) && used == 1 && conflict == 2);
    h.slot[0] = CF_MOD_SUS2; h.slot[1] = CF_MOD_SUS4;
    m = cf_harmony_mask(&h,0,0,0,3,stamp,&used,&conflict); CHECK(m == bits(0,2,7) && conflict == 2);
    /* Every modifier pair in every scale: bounded notes, stable serialization,
     * in-scale non-overrides, deterministic output, and exact mask round trips. */
    for (int s=0;s<CF_NSCALES;s++) if (cf_harmony_supported(s,1)) for (int d=0;d<7;d++)
    for (int x=0;x<CF_MOD_N;x++) for (int y=0;y<CF_MOD_N;y++) {
        int root=cf_scale_step(s,d); h.slot[0]=(uint8_t)x; h.slot[1]=(uint8_t)y;
        m=cf_harmony_mask(&h,s,0,root,3,stamp,&used,&conflict);
        int8_t tones[12]; int n=cf_mask_tones(m,tones);
        CHECK(n>=1 && n<=6 && (m & 1));
        CHECK(cf_harmony_event_mask((uint16_t)m,cf_harmony_event_buttons(m))==m);
        CHECK(cf_harmony_mask(&h,s,0,root,3,stamp,&used,&conflict)==m);
        if (x<CF_MOD_MAJ && y<CF_MOD_MAJ) for(int i=0;i<n;i++) CHECK(cf_scale_index(s, root+tones[i])>=0);
    }
    for (int a=0;a<CF_NALGS;a++) for(int moment=0;moment<2;moment++) {
        cf_chord_cfg_t g; cf_chord_t c; cf_chord_defaults(&g); g.algo=(uint8_t)a;
        cf_harmony_defaults(&h); h.mode=CF_H_AUTO; h.momentary=(uint8_t)moment; h.slot[0]=CF_MOD_7;
        cf_chord_init(&c,&g); cf_chord_harmony(&c,&h);
        int b=button(a,0); cf_chord_key(&c,b,1); cf_chord_key(&c,7,1);
        CHECK(c.out.n==4); cf_chord_key(&c,b,0);
        CHECK(c.out.n==((moment || !(a&1))?3:4));
        cf_chord_key(&c,7,0); CHECK(!c.out.sounding);
        g.quantize=CF_Q_OFF; cf_chord_config(&c,&g); cf_chord_key(&c,7,1); CHECK(c.out.single && c.out.fallback && c.out.n==1);
        g.quantize=CF_Q_FULL; g.scale=CF_SC_MAJ_PENT; cf_chord_config(&c,&g); CHECK(c.out.single && c.out.fallback);
        h.mode=CF_H_MANUAL; cf_chord_harmony(&c,&h); CHECK(!c.out.automatic && c.out.single);
        CHECK(!memcmp(c.cfg.top,g.top,sizeof g.top) && !memcmp(c.cfg.side,g.side,sizeof g.side));
    }
    /* Saturated MIDI registers can put a clamped root outside the scale;
     * that case must be an explicit single-note fallback, with safe note-offs. */
    for(int octave=-3;octave<=3;octave++) for(int midi=0;midi<128;midi++) {
        cf_chord_cfg_t cfg; cf_chord_t model; cf_chord_defaults(&cfg);
        cfg.tonic=1; cfg.octave=(int8_t)octave;
        cf_harmony_defaults(&h); h.mode=CF_H_AUTO; h.style=CF_H_DREAMY;
        cf_chord_init(&model,&cfg); cf_chord_harmony(&model,&h);
        cf_chord_key(&model,CF_KEY_EXT+midi,1);
        CHECK(model.out.n>=1 && model.out.n<=6);
        for(int n=0;n<model.out.n;n++) CHECK(model.out.note[n]<=127 && (!n || model.out.note[n]>model.out.note[n-1]));
        if(cf_scale_index(cfg.scale,model.out.root_pc-cfg.tonic)<0) CHECK(model.out.fallback && model.out.single);
        cf_chord_key(&model,CF_KEY_EXT+midi,0); CHECK(!model.out.sounding && !model.out.n);
    }
    cf_chord_cfg_t g; cf_chord_t c; cf_chord_defaults(&g); cf_chord_init(&c,&g);
    cf_harmony_defaults(&h); h.mode=CF_H_AUTO; cf_chord_harmony(&c,&h);
    cf_chord_key(&c,7,1); c.keep=1;
    cf_chord_key(&c,3,1); cf_chord_key(&c,3,0); CHECK(c.out.n==3 && c.out.buttons==(1u<<1));
    cf_chord_key(&c,9,1); CHECK(c.out.n==4 && c.out.root_pc==2);
    cf_chord_key(&c,25,1); cf_chord_key(&c,25,0); cf_chord_key(&c,7,0); cf_chord_key(&c,9,0); CHECK(c.out.hold && c.out.sounding);
    cf_chord_key(&c,11,1); CHECK(c.nroots==1 && c.out.root_pc==4 && c.out.n==4);
    cf_chord_key(&c,11,0); cf_chord_key(&c,25,1); cf_chord_key(&c,25,0); CHECK(!c.out.sounding);
    /* Rapid retrigger/config changes never retain roots after release. */
    for(int i=0;i<1000;i++) {
        h.smooth=(uint8_t)(i&1); cf_chord_harmony(&c,&h); cf_chord_key(&c,7,1); cf_chord_key(&c,7,0); CHECK(!c.out.sounding && c.out.n==0);
    }
    printf("cf_harmony: %u checks, %u failures\n",checks,failures); return failures?1:0;
}
