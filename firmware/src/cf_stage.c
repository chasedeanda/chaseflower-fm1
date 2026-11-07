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
/* Translate a freshly composed panel in place, never a second framebuffer.
 * Used only for the finite workspace arrival; controls and header stay fixed. */
static void cfn_shift_panel(int dx,int y0,int y1)
{
    if(!dx)return;
    for(int y=y0;y<y1;y++) {
        volatile uint8_t *row=fb+y*FB_W;
        if(dx>0){for(int p=FB_W-1;p>=dx;p--)row[p]=row[p-dx];for(int p=0;p<dx;p++)row[p]=FL_BG;}
        else {for(int p=0;p<FB_W+dx;p++)row[p]=row[p-dx];for(int p=FB_W+dx;p<FB_W;p++)row[p]=FL_BG;}
        if((y&15)==15)CF_PUMP();
    }
}
typedef struct {
    char chord[24],degree[12],notes[48],idea[40],sound[16],context[48],status[24],pending[48],next_setup[48];
    char rail[4][24],numeral[4][12];
    char value[4][14],label[4][12];
    uint8_t live,hold,slot,first,count,total,skin,motion,battery,halo,rec,bank,disabled[4],emphasis[4],cursor_width;
    int8_t bank_dx,chord_dy;
} cf_stage_model_t;
static cf_stage_model_t cf_stage_last;
static uint8_t cf_stage_valid;
static char cf_stage_notes[48];
static unsigned cf_stage_draws;
static uint16_t cf_stage_energy;
static uint32_t cf_stage_tick,cf_stage_chord_ms;
static char cf_stage_symbol[24];
static void cf_stage_model(cf_stage_model_t *m)
{
    memset(m,0,sizeof *m);name_update();
    m->bank=cf_home_bank;
    m->bank_dx=(int8_t)(cf_tween_offset(cf_home_bank_ms,160,10)*(cf_nav_dir<0?-1:1));
    m->live=cu_name_live;m->hold=cf_sn.out.hold;m->skin=cf_skin;m->motion=cf_motion;m->battery=(uint8_t)cf_batt();
    str_cpy(m->chord,cu_name[0]?cu_name:"--",sizeof m->chord);
    str_cpy(m->degree,cu_name_sub,sizeof m->degree);
    if(str_len(m->chord)!=str_len(cf_stage_symbol) || memcmp(m->chord,cf_stage_symbol,str_len(m->chord))) {
        str_cpy(cf_stage_symbol,m->chord,sizeof cf_stage_symbol);
        cf_stage_chord_ms=cf_motion && m->live?(cu.now?cu.now:1u):0;
    }
    m->chord_dy=(int8_t)cf_tween_offset(cf_stage_chord_ms,120,5);
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
    if(cpg.idea==CF_P_DRAFT){str_cpy(m->idea,"Draft: ",sizeof m->idea);str_cpy(m->idea+7,p->name,sizeof m->idea-7);}else str_cpy(m->idea,p->name,sizeof m->idea);str_cpy(m->sound,cf_snd[0].name,sizeof m->sound);
    char b[16],key[4];cf_pc_name(cs.chord.tonic,&cu_spell,key);
    str_cpy(m->context,key,sizeof m->context);str_cpy(m->context+str_len(m->context)," ",sizeof m->context-str_len(m->context));
    str_cpy(m->context+str_len(m->context),cf_progression_mode(cs.chord.scale),sizeof m->context-str_len(m->context));
    kint(b,cs.beats,cs.beats==6?"/8":"/4");
    str_cpy(m->context+str_len(m->context),"   ",sizeof m->context-str_len(m->context));
    str_cpy(m->context+str_len(m->context),b,sizeof m->context-str_len(m->context));
    str_cpy(m->status,cf_acc_transport(),sizeof m->status);
    /* The existing recording plaque owns count-in/capture/save truth. */
    unsigned plaque=dm_plaque(m->pending,sizeof m->pending);
    if(plaque==DMP_REC || plaque==DMP_RECIN) {
        m->rec=1;
        str_cpy(m->status,plaque==DMP_RECIN?"COUNT IN":"REC",sizeof m->status);
    } else m->pending[0]=0;
    if(!m->rec)cf_acc_pending(m->pending,sizeof m->pending,m->next_setup,sizeof m->next_setup);
    m->total=p->n;m->slot=cf_sn.ac_on && !(cf_sn.ac_waiting&&!cf_sn.ac_paused)?cf_sn.ac_slot:255;
    m->first=m->slot<p->n?(m->slot/4)*4:0;m->count=p->n-m->first;if(m->count>4)m->count=4;
    for(unsigned i=0;i<m->count;i++) {
        unsigned at=m->first+i;int root=cf_progression_root(p,at,cs.chord.scale,cs.chord.tonic);
        uint32_t mask=cf_progression_slot_mask(p,at,cs.chord.scale,cs.chord.tonic,&ch);
        const cf_source_chord_t *source=cf_progression_source(p);
        int bass=source && source[at].bass>=0?(root+source[at].bass)%12:-1;
        cf_chord_name(root,mask,bass,&cu_spell,m->rail[i]);
        cf_harmony_numeral(cs.chord.scale,cs.chord.tonic,root,m->numeral[i]);
    }
    for(int i=0;i<4;i++){cf_knob_t k;memset(&k,0,sizeof k);ui_knob_info(i,&k);str_cpy(m->label[i],k.name,sizeof m->label[i]);str_cpy(m->value[i],k.val,sizeof m->value[i]);m->disabled[i]=k.dis;}
    cf_stage_energy=cfn_motion.energy;cf_stage_tick=cu.now;
    unsigned beat=cf_motion && cf_sn.running && !cf_sn.ac_lost && !cf_sn.ac_waiting && !cf_sn.ac_paused && cf_sn.beats_done && cf_sn.now_ms-cf_sn.beat_ms<120;
    m->halo=cf_motion && cfn_motion.active?(uint8_t)(1+cfn_motion.energy/64+(cfn_motion.parts>1?1:0)+(beat?1:0)):0;
    for(unsigned k=0;k<4;k++)m->emphasis[k]=cfn_knob(k);
    m->cursor_width=cfn_cursor((1u<<16)|m->slot);
}
static void cf_stage_chord(const char *name,int y,uint8_t ink)
{
    if(cu_name_single && str_len(name)>2){cfn_center(&CFN_32,120,y,name,ink);return;}
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
    int queued=m.next_setup[0]!=0;
    if(m.pending[0])cfn_fit(&CFN_12,10,queued?55:59,m.pending,220,m.rec?FL_C(FL_R_ROSE,0):C_ACC(0));
    else cfn_text(&CFN_12,10,59,m.hold?"HOLD":m.live?"NOW":cu_name[0]?"LAST":"READY",FL_INK2);
    if(queued)cfn_fit(&CFN_12,10,68,m.next_setup,220,FL_INK2);
    char degree[24];str_cpy(degree,queued?(m.hold?"Hold ":m.live?"Now ":"Last "):"",sizeof degree);
    str_cpy(degree+str_len(degree),m.degree,sizeof degree-str_len(degree));
    cfn_center(&CFN_16,120,queued?82:76,degree,C_ACC(0));
    if(m.halo) {
        int px=120+82+m.halo,py=107;
        for(int a=16;a<=1024;a+=16) {
            int x=120+(int)(fl_fcos(a)*(82+m.halo)/16384),y=107+(int)(fl_fsin(a)*((queued?19:28)+m.halo)/16384);
            fb_line(px,py,x,y,FL_BG3);px=x;py=y;
        }
    }
    fb_clip(0,83,240,138);cf_stage_chord(m.chord,130+m.chord_dy,m.live?FL_INK:FL_INK2);fb_unclip();
    cfn_fit(&CFN_12,10,148,m.notes,165,FL_INK2);
    char window[20];kint(window,m.first+1,"-");kint(window+str_len(window),m.first+m.count,"/");kint(window+str_len(window),m.total,0);
    cfn_text(&CFN_12,230-cfn_width(&CFN_12,window),148,window,FL_INK2);
    fb_hline(10,156,220,FL_BG3);
    for(unsigned i=0;i<m.count;i++) {
        int x=30+(int)i*60;
        fb_clip((int)i*60,157,(int)(i+1)*60,195);
        cfn_center(&CFN_16,x,174,m.numeral[i],C_ACC(0));
        cfn_center(&CFN_12,x,190,m.rail[i],FL_INK);
        if(m.slot==m.first+i)fb_hline((int)i*60+9,193,42*m.cursor_width/100,C_ACC(0));
    }
    fb_unclip();fb_hline(10,198,220,FL_BG3);
    char bank[24];str_cpy(bank,CF_HOME_BANK_NAME[m.bank],sizeof bank);
    str_cpy(bank+str_len(bank)," ",sizeof bank-str_len(bank));kint(bank+str_len(bank),m.bank+1,"/4");
    fb_clip(10,199,230,211);cfn_center(&CFN_12,120+m.bank_dx,209,bank,FL_INK2);
    for(int i=0;i<4;i++) {
        int x=i*60;fb_clip(x+3,211,x+57,240);
        cfn_center(&CFN_12,x+30,223,m.label[i],FL_INK2);
        cfn_fit(cfn_width(&CFN_16,m.value[i])<=54?&CFN_16:&CFN_12,x+3,237,m.value[i],54,m.disabled[i]?FL_INK2:FL_INK);
        if(m.emphasis[i])fb_hline(x+30-m.emphasis[i],225,2*m.emphasis[i],C_ACC(0));
    }
    fb_unclip();if(!cu.pop)cf_present();
}
