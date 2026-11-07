/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include <string.h>
#include "cf_song.h"
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;if(failures<30)printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static uint32_t hash(const uint8_t *p,unsigned n){uint32_t h=2166136261u;while(n--){h^=*p++;h*=16777619u;}return h;}
static void check_hash(uint8_t *p){uint32_t h=hash(p,CF_SONG_WIRE-4);for(unsigned i=0;i<4;i++)p[CF_SONG_WIRE-4+i]=(uint8_t)(h>>(8*i));}
static void catalogue(void)
{
    for(unsigned idea=0;idea<cf_progression_count();idea++) {
        const cf_progression_t *p=cf_progression_get(idea);cf_song_t s,restored;
        CHECK(cf_song_clone(&s,p));CHECK(cf_song_valid(&s) && s.source_id==p->id && s.n==p->n);
        uint8_t bytes[CF_SONG_WIRE],again[CF_SONG_WIRE];CHECK(cf_song_encode(&s,bytes));CHECK(cf_song_decode(&restored,bytes,sizeof bytes));
        CHECK(cf_song_encode(&restored,again) && !memcmp(bytes,again,sizeof bytes));
        cf_harmony_t h;cf_harmony_defaults(&h);
        for(unsigned i=0;i<s.n;i++) {
            CHECK(cf_song_mask(&s,i,s.scale)==cf_progression_slot_mask(p,i,p->scale,p->tonic,&h));
            for(unsigned tonic=0;tonic<12;tonic++) {
                cf_chord_cfg_t cfg;cf_chord_defaults(&cfg);cfg.tonic=tonic;cfg.scale=s.scale;cf_song_chord_t out;
                CHECK(cf_song_resolve(&s,i,&cfg,0,0,&out));CHECK(out.root%12==cf_progression_root(p,i,s.scale,tonic));
                CHECK(out.ticks==s.beats*96 && out.n>=2 && out.n<=6);
                for(unsigned j=0;j<out.n;j++)CHECK(out.note[j]<=127 && (!j || out.note[j]>out.note[j-1]));
                const cf_source_chord_t *src=cf_progression_source(p);
                CHECK(out.bass_pc==(out.root+(src && src[i].bass>=0?src[i].bass:0))%12);
            }
        }
    }
}
static void draft_and_structure(void)
{
    cf_song_t original;CHECK(cf_song_clone(&original,cf_progression_get(0)));
    cf_song_draft_t d={0};CHECK(cf_song_draft_begin(&d,&original));CHECK(!d.dirty && !d.can_undo);
    cf_song_t next=original;next.slot[2].quality=CF_SONG_MINOR;next.slot[2].color=CF_MOD_9;next.slot[2].bars=3;
    CHECK(cf_song_draft_commit(&d,&next) && d.dirty && d.can_undo);CHECK(!memcmp(&original,&d.previous,sizeof original));
    CHECK(!cf_song_draft_begin(&d,&original) && !memcmp(&d.song,&next,sizeof next));
    CHECK(!cf_song_draft_commit(&d,&next) && !memcmp(&d.previous,&original,sizeof original));
    CHECK(cf_song_draft_undo(&d) && !memcmp(&d.song,&original,sizeof original) && d.dirty);CHECK(!cf_song_draft_undo(&d));
    d.dirty=0;CHECK(cf_song_draft_begin(&d,&d.song) && cf_song_valid(&d.song));
    CHECK(cf_song_draft_commit(&d,&next));CHECK(cf_song_draft_commit(&d,&d.previous) && !memcmp(&d.song,&original,sizeof original));
    cf_song_t s=original;cf_song_slot_t slot=s.slot[2];CHECK(cf_song_insert(&s,2,1) && s.n==5 && !memcmp(&slot,&s.slot[3],sizeof slot));
    CHECK(cf_song_insert(&s,1,0) && s.slot[1].degree==0 && !s.slot[1].mask && s.slot[1].bars==1);
    while(s.n<CF_SONG_MAX)CHECK(cf_song_insert(&s,s.n,0));next=s;CHECK(!cf_song_insert(&s,0,0) && !memcmp(&s,&next,sizeof s));
    CHECK(!cf_song_insert(&s,99,1));while(s.n>1)CHECK(cf_song_remove(&s,0));next=s;CHECK(!cf_song_remove(&s,0) && !memcmp(&s,&next,sizeof s));
    CHECK(!cf_song_remove(&s,1));CHECK(!memcmp(&original,&d.song,sizeof original));
}
static void timing_and_voicing(void)
{
    cf_song_t s;CHECK(cf_song_clone(&s,cf_progression_get(0)));cf_chord_cfg_t cfg;cf_chord_defaults(&cfg);cfg.scale=s.scale;cfg.tonic=s.tonic;
    cf_song_chord_t a,b;cf_song_slot_t *v=&s.slot[0];v->bars=8;v->gate=37;v->tie=1;v->bass_off=1;v->bass_oct=-2;
    CHECK(cf_song_resolve(&s,0,&cfg,0,0,&a) && a.ticks==8*4*96 && a.gate==37 && a.tie && a.bass_off && a.bass_oct==-2);
    s.beats=6;CHECK(cf_song_resolve(&s,0,&cfg,0,0,&a) && a.ticks==8*6*96);
    v->unit=CF_SONG_QUARTERS;
    for(unsigned q=1;q<=128;q++){v->quarters=q;s.beats=4;CHECK(cf_song_resolve(&s,0,&cfg,0,0,&a) && a.ticks==q*24);s.beats=6;CHECK(cf_song_resolve(&s,0,&cfg,0,0,&a) && a.ticks==q*48);}
    v->quality=CF_SONG_POWER;v->inversion=3;CHECK(cf_song_resolve(&s,0,&cfg,0,0,&a) && a.n==2); /* clamps to available voices */
    v->quality=CF_SONG_MAJOR;v->color=CF_MOD_7;CHECK(cf_song_mask(&s,0,s.scale)==0x891u);
    v->quality=CF_SONG_MINOR;CHECK(cf_song_mask(&s,0,s.scale)==0x489u);
    for(int oct=-3;oct<=3;oct++)for(int reg=-2;reg<=2;reg++)for(int inv=-1;inv<=3;inv++)for(unsigned tonic=0;tonic<12;tonic++) {
        cfg.octave=oct;cfg.tonic=tonic;v->reg=reg;v->inversion=inv;CHECK(cf_song_resolve(&s,0,&cfg,0,0,&a));
        CHECK(cf_song_resolve(&s,0,&cfg,a.note,a.n,&b));for(unsigned i=0;i<b.n;i++)CHECK(b.note[i]<=127 && (!i || b.note[i]>b.note[i-1]));
    }
    /* Exact masks may include octave duplicates: inversion must not double-attack a pitch. */
    v->quality=CF_SONG_FOLLOW;v->color=0;v->mask=1u|(1u<<4)|(1u<<12);v->inversion=1;
    CHECK(cf_song_resolve(&s,0,&cfg,0,0,&a));for(unsigned i=1;i<a.n;i++)CHECK(a.note[i]>a.note[i-1]);
    v->mask=0;v->color=CF_MOD_7;
    /* Deliberate Cmaj7 in F stays outside scale, without altering the live 7 rule. */
    s.tonic=5;s.scale=CF_SC_MAJOR;v->degree=4;v->quality=CF_SONG_MAJOR;CHECK(cf_song_mask(&s,0,s.scale)==0x891u);
}
static void wire(void)
{
    cf_song_t s,output;CHECK(cf_song_clone(&s,cf_progression_get(0)));while(s.n<CF_SONG_MAX)CHECK(cf_song_insert(&s,s.n,0));
    s.source_id=0x1234;s.bpm=300;s.rhythm=65534;s.kit=255;s.swing=100;s.drums_only=s.repeat_off=1;
    memcpy(s.name,"123456789012345678901234",25);s.slot[15].alter=-1;s.slot[15].reg=-2;s.slot[15].inversion=-1;s.slot[15].bass=11;
    uint8_t bytes[CF_SONG_WIRE],bad[CF_SONG_WIRE];CHECK(cf_song_encode(&s,bytes));CHECK(bytes[32]==0x34 && bytes[33]==0x12);CHECK(cf_song_decode(&output,bytes,sizeof bytes));
    CHECK(!strcmp(output.name,s.name) && output.slot[15].alter==-1 && output.slot[15].inversion==-1 && output.slot[15].bass==11);
    cf_song_t saved=output;
    for(unsigned n=0;n<CF_SONG_WIRE;n++)CHECK(!cf_song_decode(&output,bytes,n) && !memcmp(&output,&saved,sizeof saved));
    for(unsigned bit=0;bit<CF_SONG_WIRE*8;bit++){memcpy(bad,bytes,sizeof bytes);bad[bit/8]^=1u<<(bit%8);CHECK(!cf_song_decode(&output,bad,sizeof bad) && !memcmp(&output,&saved,sizeof saved));}
    const unsigned offsets[]={4,5,6,38,39,40,41,43,44,45,46,52+4,52+5,52+6,52+7,52+8,52+9,52+10,52+11,52+12,52+13,52+14,52+15,52+16,52+17};
    for(unsigned i=0;i<sizeof offsets/sizeof offsets[0];i++){memcpy(bad,bytes,sizeof bytes);bad[offsets[i]]=offsets[i]==52+17?0:0x7F;check_hash(bad);CHECK(!cf_song_decode(&output,bad,sizeof bad) && !memcmp(&output,&saved,sizeof saved));}
    s.n=1;CHECK(cf_song_encode(&s,bytes));memcpy(bad,bytes,sizeof bytes);bad[52+18]=1;check_hash(bad);CHECK(!cf_song_decode(&output,bad,sizeof bad));
    s.name[24]='x';CHECK(!cf_song_valid(&s) && !cf_song_encode(&s,bytes));
}
int main(void){catalogue();draft_and_structure();timing_and_voicing();wire();printf("semantic phrase: %u checks, %u failures\n",checks,failures);return failures!=0;}
