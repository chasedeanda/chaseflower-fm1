/* SPDX-License-Identifier: GPL-3.0-only
 * Native HOME: copied musical state only. No audio arrays, clock or flash writes.
 * One complete bounded model is compared before any raster/hash/transfer work.
 */
#include "cf_native_fonts.h"
/* Shared knob labels encode accidentals as pixel-font glyph bytes. */
static unsigned cfn_code(unsigned c)
{ return c==0x80?'#':c==0x81?'b':c<32 || c>126?'?':c; }
static int cfn_width(const cfn_font_t *f, const char *s)
{
    int w=0; for (;*s;s++) { unsigned c=cfn_code((uint8_t)*s); w+=f->g[c-32].adv; } return w;
}
static void cfn_text(const cfn_font_t *f,int x,int baseline,const char *s,uint8_t ink)
{
    for(;*s;s++) {
        unsigned c=cfn_code((uint8_t)*s);
        const cfn_glyph_t *g=&f->g[c-32];
        for(unsigned j=0;j<g->h;j++) {
            for(unsigned i=0;i<g->w;i++) {
                unsigned bit=j*g->w+i;
                if(f->bits[g->off+bit/8]&(128u>>(bit&7)))fb_pset(x+g->x+(int)i,baseline+g->y+(int)j,ink);
            }
            if(g->h>24 && (j&7u)==7u)CF_PUMP(); /* releases/stop input between bounded glyph strips */
        }
        x+=g->adv;
    }
}
static void cfn_center(const cfn_font_t *f,int x,int baseline,const char *s,uint8_t ink)
{ cfn_text(f,x-cfn_width(f,s)/2,baseline,s,ink); }
/* Window text deliberately; essential chord identity uses composition below. */
static void cfn_fit(const cfn_font_t *f,int x,int baseline,const char *s,int width,uint8_t ink)
{
    char b[64];str_cpy(b,s,sizeof b);unsigned n=str_len(b);
    if(cfn_width(f,b)>width) {
        while(n && cfn_width(f,b)+cfn_width(f,"...")>width)b[--n]=0;
        str_cpy(b+n,"...",sizeof b-n);
    }
    cfn_text(f,x,baseline,b,ink);
}
typedef struct {
    char chord[24],degree[12],notes[48],idea[40],sound[16],context[48],status[24],pending[48];
    char rail[4][24],numeral[4][12];
    char value[4][14],label[4][12];
    uint8_t live,hold,slot,first,count,total,skin,motion,battery,halo,rec;
} cf_stage_model_t;
static cf_stage_model_t cf_stage_last;
static uint8_t cf_stage_valid;
static char cf_stage_notes[48];
static unsigned cf_stage_draws;
static uint16_t cf_stage_energy;
static uint32_t cf_stage_tick;
static void cf_stage_model(cf_stage_model_t *m)
{
    memset(m,0,sizeof *m);name_update();
    m->live=cu_name_live;m->hold=cf_sn.out.hold;m->skin=cf_skin;m->motion=cf_motion;m->battery=(uint8_t)cf_batt();
    str_cpy(m->chord,cu_name[0]?cu_name:"--",sizeof m->chord);
    str_cpy(m->degree,cu_name_sub,sizeof m->degree);
    unsigned n=0;const uint8_t *note=0;
    if(cf_sn.out.sounding && cf_sn.out.n){n=cf_sn.out.n;note=cf_sn.out.note;}
    else if((cf_sn.ac_on || cf_sn.sq_on) && cf_sn.sq_dn){n=cf_sn.sq_dn;note=cf_sn.sq_dnote;}
    if(n) {
        cf_stage_notes[0]=0;
        for(unsigned i=0;i<n && i<6;i++) {
            char b[4];cf_pc_name(note[i]%12,&cu_spell,b);
            if(i)str_cpy(cf_stage_notes+str_len(cf_stage_notes),"  ",sizeof cf_stage_notes-str_len(cf_stage_notes));
            str_cpy(cf_stage_notes+str_len(cf_stage_notes),b,sizeof cf_stage_notes-str_len(cf_stage_notes));
        }
    }
    if(m->live && !m->degree[0] && n) {
        int root=cf_sn.out.sounding?cf_sn.out.root_pc:cf_sn.sq_droot%12;
        cf_harmony_numeral(cs.chord.scale,cs.chord.tonic,root,m->degree);
    }
    str_cpy(m->notes,cf_stage_notes,sizeof m->notes);
    const cf_progression_t *p=cf_progression_current();
    str_cpy(m->idea,p->name,sizeof m->idea);str_cpy(m->sound,cf_snd[0].name,sizeof m->sound);
    char b[16],key[4];cf_pc_name(cs.chord.tonic,&cu_spell,key);
    str_cpy(m->context,key,sizeof m->context);str_cpy(m->context+str_len(m->context)," ",sizeof m->context-str_len(m->context));
    str_cpy(m->context+str_len(m->context),cf_progression_mode(cs.chord.scale),sizeof m->context-str_len(m->context));
    kint(b,cs.beats,cs.beats==6?"/8":"/4");
    str_cpy(m->context+str_len(m->context),"   ",sizeof m->context-str_len(m->context));
    str_cpy(m->context+str_len(m->context),b,sizeof m->context-str_len(m->context));
    str_cpy(m->status,cf_sn.ac_on?(cf_sn.ac_paused?"PAUSE":"PLAY"):"STOP",sizeof m->status);
    if(cs.clock_in==1 && !cf_sn.ext_clock)str_cpy(m->status,"EXT WAIT",sizeof m->status);
    /* The existing recording plaque owns count-in/capture/save truth. */
    unsigned plaque=dm_plaque(m->pending,sizeof m->pending);
    if(plaque==DMP_REC || plaque==DMP_RECIN) {
        m->rec=1;
    } else m->pending[0]=0;
    if(!m->rec && cf_sn.ac_next!=CF_P_NONE && cf_sn.ac_on) {
        str_cpy(m->pending,"Next: ",sizeof m->pending);
        str_cpy(m->pending+6,cf_progression_get(cf_sn.ac_next)->name,sizeof m->pending-6);
    } else if(!m->rec && cf_sn.ac_edits)str_cpy(m->pending,"Next setup: at bar",sizeof m->pending);
    m->total=p->n;m->slot=cf_sn.ac_on?cf_sn.ac_slot:255;
    m->first=m->slot<p->n?(m->slot/4)*4:0;m->count=p->n-m->first;if(m->count>4)m->count=4;
    for(unsigned i=0;i<m->count;i++) {
        unsigned at=m->first+i;
        int root=cf_progression_root(p,at,cs.chord.scale,cs.chord.tonic);
        uint32_t mask=cf_progression_slot_mask(p,at,cs.chord.scale,cs.chord.tonic,&ch);
        const cf_source_chord_t *source=cf_progression_source(p);
        int bass=source && source[at].bass>=0 ? (root+source[at].bass)%12 : -1;
        cf_chord_name(root,mask,bass,&cu_spell,m->rail[i]);
        cf_harmony_numeral(cs.chord.scale,cs.chord.tonic,root,m->numeral[i]);
    }
    for(int i=0;i<4;i++){cf_knob_t k;memset(&k,0,sizeof k);ui_knob_info(i,&k);str_cpy(m->label[i],k.name,sizeof m->label[i]);str_cpy(m->value[i],k.val,sizeof m->value[i]);}
    /* Documented volume/activity fallback, not a measured audio envelope.
     * Real hits only: a selected/muted drum pattern alone cannot animate. */
    unsigned parts=(n && cs.level_chord?1:0)+(cf_sn.nb && cs.bass_level?1:0);
    if(cs.level_drums && cf_sn.sq_lit_t[SQ_DRUMS] && cf_sn.now_ms-cf_sn.sq_lit_t[SQ_DRUMS]<150)parts++;
    unsigned v=song.master_q12>4096?4096:song.master_q12;
    unsigned raw=parts?v*(204+17*parts)/4096:0;if(raw>255)raw=255;
    unsigned dt=cu.now-cf_stage_tick;cf_stage_tick=cu.now;if(dt>100)dt=100;
    if(!cf_motion){cf_stage_energy=0;m->halo=0;}
    else {
        int delta=(int)raw-cf_stage_energy;unsigned tau=delta>0?60:450;
        int step=delta*(int)dt/(int)(tau+dt);if(!step && dt && delta)step=delta>0?1:-1;
        cf_stage_energy=(uint16_t)((int)cf_stage_energy+step);
        unsigned beat=cf_sn.beats_done && cf_sn.now_ms-cf_sn.beat_ms<120?1:0;
        m->halo=(uint8_t)(cf_stage_energy/64+(beat?cf_stage_energy/64:0));
    }
}
static void cf_stage_chord(const char *name,int y,uint8_t ink)
{
    if(cfn_width(&CFN_64,name)<=220){cfn_center(&CFN_64,120,y,name,ink);return;}
    char root[3]={name[0],0,0};unsigned n=1;if(name[1]=='b'||name[1]=='#'){root[1]=name[1];n=2;}
    int w=cfn_width(&CFN_64,root)+cfn_width(&CFN_32,name+n),x=120-w/2;
    if(w>220){cfn_center(&CFN_32,120,y,name,ink);return;}
    cfn_text(&CFN_64,x,y,root,ink);cfn_text(&CFN_32,x+cfn_width(&CFN_64,root),y-15,name+n,ink);
}
static void cf_stage_draw(void)
{
    cf_stage_model_t m;cf_stage_model(&m);
    if(cf_stage_valid && !fb_full && !memcmp(&m,&cf_stage_last,sizeof m))return;
    cf_stage_last=m;cf_stage_valid=1;cf_stage_draws++;
    fb_unclip();fb_fill(FL_BG);
    cfn_fit(&CFN_12,10,17,m.idea,155,FL_INK2);
    cfn_text(&CFN_12,230-cfn_width(&CFN_12,m.status),17,m.status,C_ACC(0));
    fb_hline(10,25,220,FL_BG3);
    cfn_fit(&CFN_12,10,43,m.context,124,FL_INK2);
    cfn_fit(&CFN_12,140,43,m.sound,90,FL_INK2);
    if(m.pending[0])cfn_fit(&CFN_12,10,59,m.pending,178,m.rec?FL_C(FL_R_ROSE,0):C_ACC(0));
    else cfn_text(&CFN_12,10,59,m.hold?"HOLD":m.live?"NOW":cu_name[0]?"LAST":"READY",FL_INK2);
    cfn_text(&CFN_12,230-cfn_width(&CFN_12,"CORE"),59,"CORE",FL_INK2);
    cfn_center(&CFN_16,120,76,m.degree,C_ACC(0));
    if(m.halo) {
        int px=120+82+m.halo,py=107;
        for(int a=16;a<=1024;a+=16) {
            int x=120+(int)(fl_fcos(a)*(82+m.halo)/16384),y=107+(int)(fl_fsin(a)*(28+m.halo)/16384);
            fb_line(px,py,x,y,FL_BG3);px=x;py=y;
        }
    }
    cf_stage_chord(m.chord,130,m.live?FL_INK:FL_INK2);
    cfn_fit(&CFN_12,10,148,m.notes,165,FL_INK2);
    char window[20];kint(window,m.first+1,"-");kint(window+str_len(window),m.first+m.count,"/");kint(window+str_len(window),m.total,0);
    cfn_text(&CFN_12,230-cfn_width(&CFN_12,window),148,window,FL_INK2);
    fb_hline(10,156,220,FL_BG3);
    for(unsigned i=0;i<m.count;i++) {
        int x=30+(int)i*60;
        fb_clip((int)i*60,157,(int)(i+1)*60,195);
        cfn_center(&CFN_16,x,174,m.numeral[i],C_ACC(0));
        cfn_center(&CFN_12,x,190,m.rail[i],FL_INK);
        if(m.slot==m.first+i)fb_hline((int)i*60+9,193,42,C_ACC(0));
    }
    fb_unclip();fb_hline(10,198,220,FL_BG3);
    for(int i=0;i<4;i++) {
        int x=i*60;fb_clip(x+3,199,x+57,240);
        cfn_center(&CFN_12,x+30,214,m.label[i],FL_INK2);
        cfn_fit(cfn_width(&CFN_16,m.value[i])<=54?&CFN_16:&CFN_12,x+3,234,m.value[i],54,FL_INK);
    }
    fb_unclip();cf_present();
}
