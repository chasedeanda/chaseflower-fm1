/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include <string.h>
#include "cf_lab.h"
#include "cf_progression.h"
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;if(failures<30)printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static cf_lab_chord_t chord(int root,uint32_t mask,int inv)
{cf_lab_chord_t c={0};c.root=root;c.mask=mask;c.bass=-1;c.n=cf_voice(root,mask,inv,c.note);return c;}
static void theory(void)
{
    cf_lab_info_t info;cf_lab_chord_t b=chord(59,0x891,0);cf_lab_describe(&b,CF_SC_MAJOR,4,&info);
    CHECK(!strcmp(info.name,"Bmaj7") && !strcmp(info.degree,"V") && info.n==4);
    CHECK(!strcmp(info.note[0],"B") && !strcmp(info.note[1],"D#") && !strcmp(info.note[2],"F#") && !strcmp(info.note[3],"A#"));
    CHECK(!info.outside[0] && !info.outside[1] && !info.outside[2] && info.outside[3]);
    CHECK(!strcmp(info.interval[0],"1") && !strcmp(info.interval[1],"3") && !strcmp(info.interval[2],"5") && !strcmp(info.interval[3],"7"));
    static const int modes[]={CF_SC_MAJOR,CF_SC_MINOR,CF_SC_DORIAN,CF_SC_MIXOLYDIAN,CF_SC_LYDIAN,CF_SC_PHRYGIAN,CF_SC_LOCRIAN};
    cf_harmony_t h;cf_harmony_defaults(&h);h.mode=CF_H_AUTO;h.slot[0]=CF_MOD_7;
    for(unsigned mode=0;mode<7;mode++)for(int tonic=0;tonic<12;tonic++)for(int degree=0;degree<7;degree++)for(int mod=0;mod<CF_MOD_N;mod++) {
        int scale=modes[mode],root=tonic+48+cf_scale_step(scale,degree);h.slot[0]=mod;uint32_t stamp[CF_H_SLOTS]={1};uint16_t used,conflict;uint32_t mask=cf_harmony_mask(&h,scale,tonic,root%12,1,stamp,&used,&conflict);
        int8_t tones[12];int nt=cf_mask_tones(mask,tones);
        for(int inv=0;inv<nt;inv++) {
            cf_lab_chord_t c=chord(root,mask,inv);cf_lab_describe(&c,scale,tonic,&info);CHECK(info.n==c.n && info.name[0]);
            for(unsigned i=0;i<c.n;i++)CHECK(info.note[i][0] && info.interval[i] && info.outside[i]==(cf_scale_index(scale,(c.note[i]%12+12-tonic)%12)<0));
        }
    }
    cf_lab_chord_t ninth=chord(48,0x4000u|0x91u,2);cf_lab_describe(&ninth,CF_SC_MAJOR,0,&info);
    for(unsigned i=0;i<ninth.n;i++)if(ninth.note[i]%12==2)CHECK(!strcmp(info.interval[i],"9"));
    cf_lab_chord_t altered=chord(48,0x8089,0);cf_lab_describe(&altered,CF_SC_MINOR,0,&info);
    CHECK(!strcmp(info.interval[1],"b3") && !strcmp(info.interval[3],"#9"));
    cf_lab_chord_t fifth=chord(59,0x89,0);cf_lab_describe(&fifth,CF_SC_LOCRIAN,11,&info);CHECK(info.outside[2]);
}
static void ranking(void)
{
    cf_lab_history_t h={0};cf_lab_chord_t c=chord(48,0x91,0);CHECK(cf_lab_remember(&h,CF_SC_MAJOR,0,&c));
    c=chord(60,0x91,2);CHECK(!cf_lab_remember(&h,CF_SC_MAJOR,0,&c) && h.n==1);
    c=chord(60,0x891,0);CHECK(cf_lab_remember(&h,CF_SC_MAJOR,0,&c) && h.n==2);
    for(unsigned scale=0;scale<CF_NSCALES;scale++)for(unsigned tonic=0;tonic<12;tonic++) {
        memset(&h,0,sizeof h);h.scale=scale;h.tonic=tonic;
        for(unsigned n=0;n<4;n++) {
            cf_lab_choice_t a[3]={{0}},b[3]={{0}};int fallback,other;cf_lab_history_t before=h;
            unsigned na=cf_lab_suggest(&h,a,&fallback),nb=cf_lab_suggest(&h,b,&other);
            CHECK(!memcmp(&h,&before,sizeof h) && na==nb && fallback==other);
            if(!cf_harmony_supported(scale,CF_Q_BASIC)){CHECK(!na);continue;}
            CHECK(na==3 && a[0].degree!=a[1].degree && a[1].degree!=a[2].degree && a[0].degree!=a[2].degree);
            for(unsigned i=0;i<na;i++){CHECK(a[i].mask && a[i].degree<7 && a[i].degree==b[i].degree && a[i].mask==b[i].mask);c=chord(48+(tonic+cf_scale_step(scale,a[i].degree))%12,a[i].mask,0);CHECK(c.n>=3 && c.n<=6);}
            c=chord(48+(tonic+cf_scale_step(scale,n))%12,a[n%3].mask,0);cf_lab_remember(&h,scale,tonic,&c);CHECK(h.n<=3);
        }
    }
    memset(&h,0,sizeof h);h.scale=CF_SC_DORIAN;h.tonic=2;cf_lab_choice_t out[3];int fallback;CHECK(cf_lab_suggest(&h,out,&fallback)==3 && fallback);
}
int main(void){theory();ranking();printf("Chord Lab theory/ranking: %u checks, %u failures\n",checks,failures);return failures!=0;}
