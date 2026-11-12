/* SPDX-License-Identifier: GPL-3.0-only
 * Foreground theory presentation and deterministic offline continuation rules.
 */
#include "cf_lab.h"
#include "cf_progression.h"
#if !defined(CF_UNIT) && !defined(CF_EMU)
#include <string.h>
#endif
static const char *const CFL_INTERVAL[24]={"1","b2","2","b3","3","4","b5","5","#5","6","b7","7",
    "8","b9","9","#9","10","11","#11","12","b13","13","b14","14"};
/* The degree follows the root and its case follows the chord being played. */
void cf_lab_numeral(int scale,int tonic,int root,uint32_t mask,char out[8])
{
    cf_numeral(scale,tonic,root,out);
    int type=cf_mask_type(mask);
    for(unsigned i=0;out[i];i++){
        char v=out[i];
        if((uint8_t)v==0xB0 || v=='+'){out[i]=0;break;}
        if(v=='i'||v=='v')v-=32;
        if((type==CF_T_MIN || type==CF_T_DIM) && (v=='I'||v=='V'))v+=32;
        out[i]=v;
    }
}
void cf_lab_describe(const cf_lab_chord_t *c,int scale,int tonic,cf_lab_info_t *o)
{
    memset(o,0,sizeof *o);if(!c || !c->n || scale<0 || scale>=CF_NSCALES)return;
    cf_spell_t sp;cf_key_spell(tonic,scale,&sp);cf_pc_name(tonic,&sp,o->key);
    cf_chord_name(c->root%12,c->mask,c->bass>=0 && c->bass!=c->root%12?c->bass:-1,&sp,o->name);
    cf_lab_numeral(scale,tonic,c->root%12,c->mask,o->degree);
    o->n=c->n>CF_MAX_NOTES?CF_MAX_NOTES:c->n;
    for(unsigned i=0;i<o->n;i++) {
        int pc=c->note[i]%12,rel=(pc+12-c->root%12)%12,iv=rel;
        /* Keep the mask's semantic compound interval even in an inversion. */
        if((c->mask&(1u<<(rel+12))) && (!(c->mask&(1u<<rel)) || c->note[i]>=c->root+12))iv+=12;
        o->interval[i]=CFL_INTERVAL[iv];cf_pc_name(pc,&sp,o->note[i]);
        o->outside[i]=cf_scale_index(scale,(pc+12-tonic)%12)<0;
    }
}
int cf_lab_remember(cf_lab_history_t *h,int scale,int tonic,const cf_lab_chord_t *c)
{
    if(!c || !c->n || !c->mask || scale<0 || scale>=CF_NSCALES || tonic<0 || tonic>11)return 0;
    if(h->scale!=scale || h->tonic!=tonic){memset(h,0,sizeof *h);h->scale=scale;h->tonic=tonic;}
    if(h->n && h->chord[h->n-1].root%12==c->root%12 && h->chord[h->n-1].mask==c->mask)return 0;
    if(h->n==CF_LAB_HISTORY){for(unsigned i=1;i<h->n;i++)h->chord[i-1]=h->chord[i];h->n--;}
    h->chord[h->n++]=*c;return 1;
}
unsigned cf_lab_suggest(const cf_lab_history_t *h,cf_lab_choice_t out[CF_LAB_CHOICES],int *fallback)
{
    *fallback=1;
    if(!cf_harmony_supported(h->scale,CF_Q_BASIC))return 0;
    unsigned score[7]={0},examples=0,matches=0;
    cf_harmony_t harmony;cf_harmony_defaults(&harmony);harmony.mode=CF_H_AUTO;
    int last=h->n?cf_scale_index(h->scale,(h->chord[h->n-1].root%12+12-h->tonic)%12):-1;
    for(unsigned p=0;p<cf_progression_count();p++) {
        const cf_progression_t *v=cf_progression_get(p);
        if(v->scale!=h->scale)continue;
        examples++;
        for(unsigned i=0;i<v->n;i++) {
            if(last<0 || v->degree[i]!=last)continue;
            uint32_t mask=cf_progression_slot_mask(v,i,h->scale,h->tonic,&harmony);
            if(cf_mask_type(mask)!=cf_mask_type(h->chord[h->n-1].mask))continue;
            unsigned weight=1;
            for(unsigned j=1;j<h->n && j<v->n;j++) {
                int d=cf_scale_index(h->scale,(h->chord[h->n-1-j].root%12+12-h->tonic)%12);
                if(d!=v->degree[(i+v->n-j)%v->n])break;
                weight*=4;
            }
            score[v->degree[(i+1)%v->n]]+=weight;matches++;
        }
    }
    *fallback=examples<3 || !matches;
    /* Include a home route, a continuation and a contrasting route. Counts
     * remain private ranking inputs from this local catalogue, never odds. */
    unsigned picked=0;
    for(unsigned i=0;i<CF_LAB_CHOICES;i++) {
        int best=-1;unsigned rank=0;
        for(unsigned d=0;d<7;d++) {
            if((picked&(1u<<d)) || (int)d==last)continue;
            unsigned value=score[d]*16u + (d==4?9u:d==3?8u:d==1?7u:d==5?6u:1u);
            if(i==0 && d==0)value+=1000000u;
            if(i==2 && !score[d])value+=32u;
            if(best<0 || value>rank){best=(int)d;rank=value;}
        }
        if(best<0)break;
        picked|=1u<<best;out[i].degree=(uint8_t)best;
        out[i].mask=cf_progression_mask(h->scale,h->tonic,best,&harmony);
        out[i].reason=best==0?"Return home":i<2?"Continue":"Contrast";
    }
    return CF_LAB_CHOICES;
}
