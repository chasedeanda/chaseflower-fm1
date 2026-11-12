/* SPDX-License-Identifier: GPL-3.0-only
 * Studio's live mixer. The keyboard follows the panel's F3–G5 layout.
 */
typedef struct {
    uint32_t keys;
    uint16_t bpm, ready;
    uint8_t view, muted, cut, solo, drums;
    uint8_t paused, playing, ext, waiting, pending, fill, dropped;
    char message[48];
} cgu_glow_model_t;
static uint8_t cgu_glow_valid;

static void cgu_glow_text_up(int baseline,int y,const char *name,uint8_t ink)
{
    const cfn_font_t *font=&CFN_11;
    for(;*name;name++){
        const cfn_glyph_t *g=&font->g[cfn_code((uint8_t)*name)-32];
        for(unsigned row=0;row<g->h;row++)
            for(unsigned col=0;col<g->w;col++){
                unsigned bit=row*g->w+col;
                unsigned coverage=(font->bits[g->off+bit/4]>>(6-2*(bit&3)))&3u;
                cfn_coverage(baseline+g->y+(int)row,y-g->x-(int)col,coverage,ink);
            }
        y-=g->adv;
    }
}

static void cgu_glow_map(const cgu_glow_model_t *m)
{
    static const char notes[]="FGABCDEFGABCDEFG";
    static const char *const actions[]={"PAUSE","RESTART","UNMUTE","TAP","FILL HOLD","FILL BAR","DROP"};
    for(int black=0;black<=1;black++){
        for(unsigned key=0;key<CF_NKEYS;key++){
            if(cf_key_is_upper(key)!=black)continue;
            unsigned p=cgu_glow_place(key,black);
            unsigned white=black?cgu_glow_place(key,0):p;
            int x=4+(int)white*232/16;
            int width=4+(int)(white+1)*232/16-x-1;
            int held=(m->keys>>key)&1u;
            int enabled=1, on=held;
            uint8_t fill=FL_BG2, accent=C_ACC(0), ink=FL_INK;
            char name[16]="";
            if(p<CGS_NPART){
                unsigned part=cgu_part(p), bit=1u<<part;
                accent=CGU_INK_BASE+part;
                if(!black)str_cpy(name,"Solo ",sizeof name);
                unsigned length=black?0:5;
                str_cpy(name+length,CGS_PART_NAME[part],sizeof name-length);
                on=black?(m->muted&bit)!=0:(m->solo&bit)!=0;
            } else if(black && p>=6u && p<10u){
                str_cpy(name,CGU_GLO_DRUMS[p-6u],sizeof name);
                accent=CGU_INK_BASE+CGS_DRUMS;
                on=(m->drums>>(p-6u))&1u;
            } else if(!black && p<=CGU_GLO_DROP){
                const char *label=actions[p-CGU_GLO_PAUSE];
                if(p==CGU_GLO_PAUSE && m->paused)label="RESUME", on=1;
                if(p==CGU_GLO_FILL_BAR && m->pending==CFPERF_FILL)on=1;
                if(p==CGU_GLO_DROP && (m->dropped || m->pending==CFPERF_DROP))label="BRING BACK", on=1;
                str_cpy(name,label,sizeof name);
                enabled=(m->ready>>p)&1u;
            }
            if(black){
                x-=5;width=10;
                fb_rect(x-1,84,12,59,FL_BG);
                fill=name[0]?FL_BG3:FL_BG1;
            } else if(!name[0])fill=FL_BG1;
            if(on){fill=black?FL_C(FL_R_ROSE,0):accent;ink=FL_BG;}
            else if(!enabled)ink=FL_INK3;
            fb_rect(x,84,width,black?58:138,fill);
            if(name[0]){
                if(!on)fb_hline(x,84,width,enabled?accent:FL_BG3);
                cgu_glow_text_up(x+width/2+4,black?137:208,name,ink);
            }
            if(!black){
                char note[2]={notes[p],0};
                cfn_center(&CFN_11,x+width/2,219,note,on?ink:FL_INK2);
            }
        }
    }
}

static const char *cgu_glow_part_state(const cgu_glow_model_t *m,unsigned part)
{
    unsigned bit=1u<<part;
    return m->muted&bit?"MUTE":m->solo&bit?"SOLO":m->cut&bit?"CUT":"ON";
}

static void cgu_glow_state(const cgu_glow_model_t *m)
{
    for(unsigned i=0;i<NELEM(CGU_GLO_DRUMS);i++){
        int x=7+i*57, muted=(m->drums>>i)&1u;
        fb_rect(x,98,54,27,muted?FL_C(FL_R_ROSE,0):FL_BG2);
        cfn_center(&CFN_11,x+27,115,CGU_GLO_DRUMS[i],muted?FL_BG:FL_INK);
    }
    cfn_text(&CFN_12,9,151,"C4 Pause / resume    D4 Restart",FL_INK2);
    cfn_text(&CFN_12,9,171,"E4 Unmute all    F4 Tap tempo",FL_INK2);
    cfn_text(&CFN_12,9,191,"G4 Fill held    A4 Fill next bar",FL_INK2);
    cfn_text(&CFN_12,9,211,"B4 Drop / bring back drums",FL_INK2);
}

static void cgu_glow_draw(void)
{
    static cgu_glow_model_t last;
    static const char *const tabs[]={"MAP","STATE"};
    cgu_glow_model_t m={0};
    m.keys=cgu_glow.keys;
    m.ext=cs.clock_in==1u;
    m.bpm=m.ext?cf_sn.ext_bpm:cs.bpm;
    m.view=cgu_glow.view;
    for(unsigned i=0;i<=CGU_GLO_DROP;i++)m.ready|=(uint16_t)(cgu_glow_ready(i)<<i);
    m.muted=cgs.muted;
    m.cut=(uint8_t)cgs_mute_mask();
    m.solo=cgs.solo|cgs.held_solo;
    m.drums=cgs.drum_mutes;
    m.paused=cf_sn.ac_paused;
    m.waiting=cf_sn.ac_waiting || cf_sn.ac_lost;
    m.playing=cf_sn.running;
    m.pending=cf_sn.perf_pending;
    m.fill=cf_sn.perf_fill || cgs.fill_held;
    m.dropped=cf_sn.perf_dropped;
    if(cu.pop==POP_MSG)str_cpy(m.message,cu.msg,sizeof m.message);
    if(cgu_glow_valid && !fb_full && !memcmp(&m,&last,sizeof m))return;
    last=m;cgu_glow_valid=1;
    fb_fill(FL_BG);
    char value[16];
    kint(value,m.bpm,m.ext?" EXT":" BPM");
    cfn_text(&CFN_14,6,17,value,C_ACC(0));
    const char *status=m.waiting?"CLOCK WAIT":m.paused?"PAUSED":m.pending==CFPERF_FILL?"FILL NEXT":
        m.pending==CFPERF_DROP?"DROP NEXT":m.pending==CFPERF_BACK?"BACK NEXT":m.dropped?"DRUMS OFF":
        m.fill?"FILL":m.playing?"PLAYING":"STOPPED";
    cfn_center(&CFN_11,120,16,status,FL_INK2);
    cfn_text(&CFN_12,174,17,"GLO LIVE",C_ACC(0));
    for(unsigned i=0;i<CGU_GLO_VIEWS;i++){
        int x=5+i*116;
        fb_rect(x,24,112,15,i==m.view?FL_BG3:FL_BG1);
        cfn_center(&CFN_11,x+56,35,tabs[i],i==m.view?C_ACC(0):FL_INK2);
    }
    for(unsigned i=0;i<CGS_NPART;i++){
        unsigned p=cgu_part(i), x=6+i*58;
        uint8_t accent=CGU_INK_BASE+p;
        fb_rect(x,47,53,31,FL_BG2);
        cfn_center(&CFN_11,x+26,59,CGS_PART_NAME[p],accent);
        cfn_center(&CFN_11,x+26,72,cgu_glow_part_state(&m,p),FL_INK2);
    }
    if(m.view==CGU_GLO_MAP)cgu_glow_map(&m);
    else cgu_glow_state(&m);
    if(m.message[0])cfn_fit(&CFN_11,5,236,m.message,209,FL_INK);
    else cfn_fit(&CFN_11,5,236,"Black Mute   ALGO View   SAVE Settings",209,FL_INK2);
    kint(value,m.view+1,"/2");
    cfn_text(&CFN_11,220,236,value,C_ACC(0));
    cf_present();
}
