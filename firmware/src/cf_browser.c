/* SPDX-License-Identifier: GPL-3.0-only
 * Foreground-only navigation. Stable catalogue IDs own favorites; order is a
 * compact permutation, independent of playing/queued setup and musical data.
 */
#include "cf_browser.h"
static int cpb_compare(unsigned a, unsigned b)
{
    const char *x=cf_progression_get(a)->name, *y=cf_progression_get(b)->name;
    while (*x && *y) {
        int c=*x++, d=*y++;
        if(c>='A' && c<='Z')c+='a'-'A';
        if(d>='A' && d<='Z')d+='a'-'A';
        if(c!=d)return c-d;
    }
    return *x ? 1 : *y ? -1 : (int)a-(int)b;
}
int cf_browser_favorite(const cf_browser_t *b, unsigned index)
{
    const cf_progression_t *p=cf_progression_get(index);
    return p && p->id && p->id<=CPB_MAX && (b->favorites[(p->id-1)/8]>>((p->id-1)%8)&1u);
}
void cf_browser_star(cf_browser_t *b, unsigned index, int on)
{
    const cf_progression_t *p=cf_progression_get(index);
    if(!p || !p->id || p->id>CPB_MAX)return;
    unsigned bit=p->id-1;
    if(on)b->favorites[bit/8]|=1u<<(bit%8);
    else b->favorites[bit/8]&=~(1u<<(bit%8));
    if(b->mode==CPB_FAVORITES)cf_browser_order(b,b->mode,0);
}
static uint32_t cpb_random(cf_browser_t *b)
{
    uint32_t r=b->rng; r^=r<<13; r^=r>>17; r^=r<<5; return b->rng=r;
}
void cf_browser_order(cf_browser_t *b, unsigned mode, uint32_t seed)
{
    unsigned count=cf_progression_count(); if(count>CPB_MAX)count=CPB_MAX;
    b->n=0; b->mode=mode<CPB_MODES?mode:CPB_CATALOG;
    for(unsigned i=0;i<count;i++)
        if(b->mode!=CPB_FAVORITES || cf_browser_favorite(b,i))b->order[b->n++]=i;
    if(b->mode==CPB_ALPHA) {
        for(unsigned i=1;i<b->n;i++) {
            unsigned x=b->order[i],j=i;
            while(j && cpb_compare(x,b->order[j-1])<0) { b->order[j]=b->order[j-1]; j--; }
            b->order[j]=x;
        }
    }
    if(b->mode==CPB_SHUFFLE) {
        b->rng^=seed; if(!b->rng)b->rng=0xA73C9B21u;
        for(unsigned i=b->n;i>1;i--) {
            unsigned j=cpb_random(b)%i; uint8_t t=b->order[i-1];
            b->order[i-1]=b->order[j]; b->order[j]=t;
        }
    }
}
uint8_t cf_browser_pick(const cf_browser_t *b, unsigned current, int turn)
{
    if(!b->n || !turn)return cf_progression_pick(current,0);
    int at=-1;
    for(unsigned i=0;i<b->n;i++)if(b->order[i]==current)at=i;
    if(at<0)at=turn>0?-1:0;
    int next=(at+turn%(int)b->n)%(int)b->n; if(next<0)next+=b->n;
    return b->order[next];
}
