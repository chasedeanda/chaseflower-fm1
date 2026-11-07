/* SPDX-License-Identifier: GPL-3.0-only */
#include "cf_song.h"
#if !defined(CF_UNIT) && !defined(CF_EMU)
#include <string.h>
#endif
static int csg_metres(unsigned beats) { return beats>=2 && beats<=7; }
static unsigned csg_count(uint32_t v) { unsigned n=0;while(v){n+=v&1u;v>>=1;}return n; }
static void csg_slot(cf_song_slot_t *s)
{ memset(s,0,sizeof *s);s->bars=1;s->bass=-1;s->gate=100;s->quarters=4; }
int cf_song_valid(const cf_song_t *s)
{
    if(!s || !s->n || s->n>CF_SONG_MAX || s->tonic>11 || !cf_harmony_supported(s->scale,CF_Q_BASIC) ||
       !csg_metres(s->beats) || s->bpm<30 || s->bpm>300 || s->swing>100 || (s->rhythm>255 && s->rhythm<65534) || s->drums_only>1 || s->repeat_off>1 ||
       !s->name[0] || s->name[CF_SONG_NAME])return 0;
    int end=0;for(unsigned i=0;i<=CF_SONG_NAME;i++) {
        unsigned c=(uint8_t)s->name[i];if(!c)end=1;else if(end || c<32 || c>126)return 0;
    }
    for(unsigned i=0;i<s->n;i++) {
        const cf_song_slot_t *v=&s->slot[i];
        if(v->degree>6 || v->alter< -1 || v->alter>1 || v->inversion< -1 || v->inversion>3 || v->reg< -2 || v->reg>2 ||
           !v->bars || v->bars>8 || v->bass< -1 || v->bass>11 || v->bass_oct< -2 || v->bass_oct>1 || v->bass_off>1 ||
           v->quality>=CF_SONG_NQUALITY || v->color>=CF_MOD_N || v->gate<20 || v->gate>100 || v->tie>1 ||
           v->unit>CF_SONG_QUARTERS || !v->quarters || v->quarters>128 ||
           (v->mask && (!(v->mask&1u) || v->mask>0xFFFFFFu || csg_count(v->mask)>6)))return 0;
    }
    return 1;
}
int cf_song_clone(cf_song_t *s,const cf_progression_t *p)
{
    if(!s || !p || !p->n || p->n>CF_SONG_MAX)return 0;
    cf_song_t next={0};unsigned n=0;while(n<CF_SONG_NAME && p->name[n]){next.name[n]=p->name[n];n++;}
    next.source_id=p->id;next.bpm=120;next.rhythm=65535;next.tonic=p->tonic;next.scale=p->scale;next.beats=p->beats;next.n=p->n;
    const cf_source_chord_t *src=cf_progression_source(p);
    for(unsigned i=0;i<next.n;i++) {
        csg_slot(&next.slot[i]);next.slot[i].degree=p->degree[i];
        if(src){next.slot[i].mask=src[i].mask;next.slot[i].alter=src[i].alter;next.slot[i].bass=src[i].bass;}
    }
    if(!cf_song_valid(&next))return 0;*s=next;return 1;
}
uint32_t cf_song_mask(const cf_song_t *s,unsigned slot,int scale)
{
    if(!cf_song_valid(s) || slot>=s->n || !cf_harmony_supported(scale,CF_Q_BASIC))return 0;
    const cf_song_slot_t *v=&s->slot[slot];cf_harmony_t h;cf_harmony_defaults(&h);
    static const uint32_t masks[]={0,0x91u,0x89u,0x49u,0x111u,0x81u};
    if(v->quality>=CF_SONG_NQUALITY || v->degree>6 || v->color>=CF_MOD_N)return 0;
    uint32_t mask=v->quality?masks[v->quality]:v->mask?v->mask:cf_progression_mask(scale,s->tonic,v->degree,&h);
    if(v->color){uint32_t stamp[CF_H_SLOTS]={1};uint16_t used,conflict;h.slot[0]=v->color;mask=cf_harmony_apply(&h,mask,1,stamp,&used,&conflict);}
    return cf_harmony_limit(mask,6);
}
int cf_song_resolve(const cf_song_t *s,unsigned slot,const cf_chord_cfg_t *cfg,
                    const uint8_t *previous,unsigned nprevious,cf_song_chord_t *out)
{
    if(!out || !cfg || !cf_song_valid(s) || slot>=s->n || cfg->tonic>11 || cfg->octave< -3 || cfg->octave>3 ||
       !cf_harmony_supported(cfg->scale,CF_Q_BASIC) || nprevious>6 || (nprevious && !previous))return 0;
    for(unsigned i=0;i<nprevious;i++)if(previous[i]>127)return 0;
    const cf_song_slot_t *v=&s->slot[slot];cf_song_chord_t next={0};
    next.mask=cf_song_mask(s,slot,cfg->scale);if(!next.mask)return 0;
    int root=60+cfg->tonic-(cfg->tonic>6?12:0)+cf_scale_step(cfg->scale,v->degree)+v->alter+12*(cfg->octave+CF_OCT_BASE+v->reg);
    int inv=v->inversion;int8_t tones[12];int nt=cf_mask_tones(next.mask,tones);
    while(root<0)root+=12;while(root+tones[nt-1]>127)root-=12;
    next.root=(uint8_t)root;
    if(inv<0){inv=cf_auto_inversion(root,next.mask,previous,nprevious);if(inv>=nt){root-=12;inv-=nt;}}
    if(inv>=nt)inv=nt-1;
    /* Reserve octave room for the requested inversion before calling cf_voice. */
    while(root<0)root+=12;while(root+tones[nt-1]+(inv?12:0)>127)root-=12;
    uint8_t notes[CF_MAX_NOTES];unsigned nn=cf_voice(root,next.mask,inv,notes);
    if(!nn || nn>6)return 0;for(unsigned i=0;i<nn;i++)
        if(!next.n || notes[i]!=next.note[next.n-1])next.note[next.n++]=notes[i];
    next.ticks=v->unit==CF_SONG_BARS?v->bars*s->beats*96u:v->quarters*96u*(s->beats==6?2u:1u)/4u;
    next.gate=v->gate;next.tie=v->tie;next.bass_off=v->bass_off;next.bass_oct=v->bass_oct;
    next.bass_pc=(int8_t)((next.root+(v->bass<0?0:v->bass))%12);*out=next;return 1;
}
int cf_song_insert(cf_song_t *s,unsigned at,int duplicate)
{
    if(!cf_song_valid(s) || s->n==CF_SONG_MAX || at>s->n || (duplicate && at>=s->n))return 0;
    cf_song_slot_t value;if(duplicate){value=s->slot[at];at++;}else csg_slot(&value);
    for(unsigned i=s->n;i>at;i--)s->slot[i]=s->slot[i-1];s->slot[at]=value;s->n++;return 1;
}
int cf_song_remove(cf_song_t *s,unsigned at)
{
    if(!cf_song_valid(s) || s->n<=1 || at>=s->n)return 0;
    for(unsigned i=at;i+1<s->n;i++)s->slot[i]=s->slot[i+1];s->n--;memset(&s->slot[s->n],0,sizeof s->slot[0]);return 1;
}
int cf_song_draft_begin(cf_song_draft_t *d,const cf_song_t *s)
{
    if(!d || !cf_song_valid(s) || (d->have && d->dirty))return 0;
    cf_song_t next=*s;memset(d,0,sizeof *d);d->song=next;d->have=1;return 1;
}
int cf_song_draft_commit(cf_song_draft_t *d,const cf_song_t *s)
{
    if(!d || !d->have || !cf_song_valid(&d->song) || !cf_song_valid(s))return 0;
    uint8_t before[CF_SONG_WIRE],after[CF_SONG_WIRE];
    cf_song_encode(&d->song,before);cf_song_encode(s,after);if(!memcmp(before,after,sizeof before))return 0;
    cf_song_t next=*s;d->previous=d->song;d->song=next;d->dirty=d->can_undo=1;return 1;
}
int cf_song_draft_undo(cf_song_draft_t *d)
{
    if(!d || !d->have || !d->can_undo || !cf_song_valid(&d->previous))return 0;
    d->song=d->previous;d->can_undo=0;/* dirty remains: undo is not a saved-Song confirmation */return 1;
}
static void csg_u16(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static unsigned csg_r16(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t csg_hash(const uint8_t *p,unsigned n){uint32_t h=2166136261u;while(n--){h^=*p++;h*=16777619u;}return h;}
static void csg_u32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(i*8));}
static uint32_t csg_r32(const uint8_t *p){uint32_t v=0;for(unsigned i=0;i<4;i++)v|=(uint32_t)p[i]<<(i*8);return v;}
int cf_song_encode(const cf_song_t *s,uint8_t out[CF_SONG_WIRE])
{
    if(!out || !cf_song_valid(s))return 0;memset(out,0,CF_SONG_WIRE);
    out[0]='C';out[1]='S';out[2]='G';out[3]='1';out[4]=1;csg_u16(out+6,CF_SONG_WIRE);
    memcpy(out+8,s->name,CF_SONG_NAME);csg_u16(out+32,s->source_id);csg_u16(out+34,s->bpm);csg_u16(out+36,s->rhythm);
    out[38]=s->tonic;out[39]=s->scale;out[40]=s->beats;out[41]=s->n;out[42]=s->kit;out[43]=s->swing;out[44]=s->drums_only;out[45]=s->repeat_off;
    for(unsigned i=0;i<s->n;i++) {
        const cf_song_slot_t *v=&s->slot[i];uint8_t *p=out+52+i*18;csg_u32(p,v->mask);
        p[4]=v->degree;p[5]=(uint8_t)v->alter;p[6]=(uint8_t)v->inversion;p[7]=(uint8_t)v->reg;p[8]=v->bars;
        p[9]=(uint8_t)v->bass;p[10]=(uint8_t)v->bass_oct;p[11]=v->bass_off;p[12]=v->quality;p[13]=v->color;p[14]=v->gate;p[15]=v->tie;p[16]=v->unit;p[17]=v->quarters;
    }
    csg_u32(out+CF_SONG_WIRE-4,csg_hash(out,CF_SONG_WIRE-4));return 1;
}
int cf_song_decode(cf_song_t *s,const uint8_t *p,unsigned size)
{
    if(!s || !p || size!=CF_SONG_WIRE || p[0]!='C' || p[1]!='S' || p[2]!='G' || p[3]!='1' || p[4]!=1 || p[5] ||
       csg_r16(p+6)!=size || csg_r32(p+size-4)!=csg_hash(p,size-4))return 0;
    for(unsigned i=46;i<52;i++)if(p[i])return 0;
    cf_song_t next={0};memcpy(next.name,p+8,CF_SONG_NAME);next.source_id=csg_r16(p+32);next.bpm=csg_r16(p+34);next.rhythm=csg_r16(p+36);
    next.tonic=p[38];next.scale=p[39];next.beats=p[40];next.n=p[41];next.kit=p[42];next.swing=p[43];next.drums_only=p[44];next.repeat_off=p[45];
    if(!next.n || next.n>CF_SONG_MAX)return 0;
    for(unsigned i=0;i<CF_SONG_MAX;i++) {
        const uint8_t *v=p+52+i*18;if(i>=next.n){for(unsigned j=0;j<18;j++)if(v[j])return 0;continue;}
        cf_song_slot_t *o=&next.slot[i];o->mask=csg_r32(v);o->degree=v[4];o->alter=(int8_t)v[5];o->inversion=(int8_t)v[6];o->reg=(int8_t)v[7];o->bars=v[8];
        o->bass=(int8_t)v[9];o->bass_oct=(int8_t)v[10];o->bass_off=v[11];o->quality=v[12];o->color=v[13];o->gate=v[14];o->tie=v[15];o->unit=v[16];o->quarters=v[17];
    }
    if(!cf_song_valid(&next))return 0;*s=next;return 1;
}
