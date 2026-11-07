/* SPDX-License-Identifier: GPL-3.0-only
 * Native shared-clock workspace. Fixed-size copied model, no pattern allocation,
 * mutable audio buffers, independent transport or display-held audio lock.
 */
typedef struct {
    char title[20],status[24],sub[48],main[40],detail[48],pending[48],next[48],message[64];
    char label[4][12],value[4][14],rail[4][24],degree[4][12];
    uint32_t lane[3];
    uint8_t tab,disabled[4],level[3],bass_on,steps,playhead,bar,bars,focus,first,n,slot,skin;
    int8_t dx;
    uint8_t emphasis[4],cursor_width,focus_weight;
} cf_acc_model_t;
static cf_acc_model_t cf_acc_last;
static uint8_t cf_acc_valid;
static unsigned cf_acc_draws;
static void cf_acc_model(cf_acc_model_t *m)
{
    memset(m,0,sizeof *m);m->tab=cu.scr==SCR_ACC?cf_acc_tab():CF_ACC_N;m->skin=cf_skin;
    str_cpy(m->title,m->tab<CF_ACC_N?CF_ACC_NAME[m->tab]:"Legacy tools",sizeof m->title);
    str_cpy(m->status,cf_acc_transport(),sizeof m->status);
    if(cu.pop==POP_MSG)str_cpy(m->message,cu.msg,sizeof m->message);
    else if(cu.pop==POP_SOUND_CHORD || cu.pop==POP_SOUND_BASS)str_cpy(m->message,cf_snd[cu.pop==POP_SOUND_BASS?1:0].name,sizeof m->message);
    if(m->tab==CF_ACC_N){
        m->focus=cf_legacy_pick;m->focus_weight=cfn_focus(CFN_FOCUS_LEGACY|m->focus);
        for(int k=0;k<4;k++){cf_knob_t o={0};ui_knob_info(k,&o);str_cpy(m->label[k],o.name,sizeof m->label[k]);str_cpy(m->value[k],o.val,sizeof m->value[k]);m->disabled[k]=o.dis;m->emphasis[k]=cfn_knob(k);}
        return;
    }
    uint32_t arrival=(int32_t)(cf_acc_page_ms-cfn_motion.page_ms)>0?cf_acc_page_ms:cfn_motion.page_ms;
    m->dx=(int8_t)(cf_tween_offset(arrival,170,8)*(cf_nav_dir<0?-1:1));
    const cf_progression_t *p=cf_progression_current();m->n=p->n;m->focus=cf_acc_focus%p->n;
    m->slot=cf_sn.ac_on && !(cf_sn.ac_waiting&&!cf_sn.ac_paused)?cf_sn.ac_slot:255;m->bass_on=cs.bass_on;
    m->level[0]=cs.level_chord;m->level[1]=cs.bass_level;m->level[2]=cs.level_drums;
    char b[24],key[4];cf_pc_name(cs.chord.tonic,&cu_spell,key);str_cpy(m->sub,key,sizeof m->sub);
    str_cpy(m->sub+str_len(m->sub)," ",sizeof m->sub-str_len(m->sub));str_cpy(m->sub+str_len(m->sub),CF_SCALES[cs.chord.scale].shrt,sizeof m->sub-str_len(m->sub));
    kint(b,cs.beats,cs.beats==6?"/8":"/4");str_cpy(m->sub+str_len(m->sub),"  ",sizeof m->sub-str_len(m->sub));str_cpy(m->sub+str_len(m->sub),b,sizeof m->sub-str_len(m->sub));
    kint(b,cs.clock_in==1?cf_sn.ext_bpm:cs.bpm,cs.clock_in==1?" EXT":" BPM");str_cpy(m->sub+str_len(m->sub),"  ",sizeof m->sub-str_len(m->sub));str_cpy(m->sub+str_len(m->sub),b,sizeof m->sub-str_len(m->sub));
    for(int k=0;k<4;k++){cf_knob_t o;memset(&o,0,sizeof o);cf_acc_knob_info(k,&o);str_cpy(m->label[k],o.name,sizeof m->label[k]);str_cpy(m->value[k],o.val,sizeof m->value[k]);m->disabled[k]=o.dis;}
    cf_acc_pending(m->pending,sizeof m->pending,m->next,sizeof m->next);
    for(unsigned k=0;k<4;k++)m->emphasis[k]=cfn_knob(k);
    m->cursor_width=cfn_cursor(0x20000u|m->slot);m->focus_weight=cfn_focus(0x20000u|m->focus);
    unsigned r=cf_sn.ac_on?cf_sn.ac_rhythm:cpg.rhythm;
    if(m->tab==CF_ACC_GROOVE) {
        str_cpy(m->main,cpa_rhythm_name(r),sizeof m->main);
        if(r<DM_N){
            m->steps=dm_steps(cs.beats);m->bars=DM_RHY[r].bars;
            int active=cf_sn.ac_on && !cf_sn.ac_paused && !cf_sn.ac_waiting && cf_sn.dm_on && cf_sn.dm_r==r;
            m->bar=active?cf_sn.dm_bar:0;m->playhead=active?cf_sn.dm_step:255;
            static const unsigned lane[]={DM_K,DM_S,DM_H};for(unsigned i=0;i<3;i++)m->lane[i]=dm_mask(r,lane[i],m->bar);
            kint(m->detail,m->bar+1,"/");kint(m->detail+str_len(m->detail),m->bars," bars / suggested ");kint(m->detail+str_len(m->detail),DM_RHY[r].bpm," BPM");
        } else str_cpy(m->detail,dm_count(cs.beats)?"Chords keep the shared clock":"No matching groove: Off or Click",sizeof m->detail);
    } else if(m->tab==CF_ACC_LOOP) {
        str_cpy(m->main,(cf_sn.ac_on?cf_sn.ac_mode:cpg.mode)==CPA_DRUMS?"Drums only":"Chord + Drum",sizeof m->main);
        kint(m->detail,1,"-");kint(m->detail+str_len(m->detail),p->n," / full phrase / ");str_cpy(m->detail+str_len(m->detail),(cf_sn.ac_on?cf_sn.ac_repeat_off:cpg.repeat_off)?"Play once":"Repeat",sizeof m->detail-str_len(m->detail));
        m->first=(m->focus/4)*4;
        for(unsigned i=0;i<4 && m->first+i<p->n;i++) {
            unsigned at=m->first+i;int root=cf_progression_root(p,at,cs.chord.scale,cs.chord.tonic);
            uint32_t mask=cf_progression_slot_mask(p,at,cs.chord.scale,cs.chord.tonic,&ch);
            const cf_source_chord_t *src=cf_progression_source(p);int bass=src && src[at].bass>=0?(root+src[at].bass)%12:-1;
            cf_chord_name(root,mask,bass,&cu_spell,m->rail[i]);cf_harmony_numeral(cs.chord.scale,cs.chord.tonic,root,m->degree[i]);
        }
    } else if(m->tab==CF_ACC_CLICK) {
        str_cpy(m->main,cs.metro_level?"On":"Off",sizeof m->main);str_cpy(m->detail,"Same clock / quarter-note BPM",sizeof m->detail);
    }
}
static void cf_acc_draw(void)
{
    cf_acc_model_t m;cf_acc_model(&m);cf_stage_valid=0;
    if(cf_acc_valid && !fb_full && !memcmp(&m,&cf_acc_last,sizeof m))return;
    cf_acc_last=m;cf_acc_valid=1;cf_acc_draws++;fb_unclip();fb_fill(FL_BG);
    cfn_text(&CFN_16,10,20,m.title,FL_INK);cfn_text(&CFN_12,230-cfn_width(&CFN_12,m.status),20,m.status,C_ACC(0));fb_hline(10,28,220,FL_BG3);
    if(m.tab==CF_ACC_N) {
        cfn_text(&CFN_12,10,49,"K1 selects / SEL opens",FL_INK2);
        for(unsigned i=0;i<3;i++){if(m.focus==i){fb_rect(10,64+i*38,220,32,FL_BG1);fb_rect(10,64+i*38,2+m.focus_weight,32,C_ACC(0));}cfn_text(&CFN_16,20,86+i*38,CF_LEGACY_NAME[i],m.focus==i?C_ACC(0):FL_INK);}
        cfn_hint("HOME: Core / SELECT: system page",FL_INK2);
        cfn_controls(m.label,m.value,m.disabled,m.emphasis);
    } else {
        cfn_fit(&CFN_12,10,45,m.sub,220,FL_INK2);
        fb_clip(0,49,240,174);
        int x=m.dx;
        if(m.tab==CF_ACC_MIX) {
            static const char *const label[]={"Chord","Bass","Drums"};
            for(unsigned i=0;i<3;i++){int y=76+i*32;cfn_text(&CFN_16,10+x,y,label[i],i==1&&!m.bass_on?FL_INK2:FL_INK);fb_rect(80+x,y-9,150,6,FL_BG1);fb_rect(80+x,y-9,150*m.level[i]/127,6,C_ACC(0));if(m.focus==i)fb_hline(10+x,y+5,220,FL_BG3);}
            cfn_text(&CFN_11,10+x,172,m.bass_on?"Linked bass On":"Linked bass Off",FL_INK2);
        } else {
            cfn_fit(m.tab==CF_ACC_CLICK?&CFN_64:&CFN_32,10+x,m.tab==CF_ACC_CLICK?106:83,m.main,220,FL_INK);
            cfn_fit(&CFN_12,10+x,104+(m.tab==CF_ACC_CLICK?19:0),m.detail,220,FL_INK2);
            if(m.tab==CF_ACC_GROOVE && m.steps) {
                static const char *const label[]={"K","S","H"};
                for(unsigned l=0;l<3;l++){int y=119+l*17;cfn_text(&CFN_12,10+x,y+8,label[l],FL_INK2);for(unsigned i=0;i<m.steps;i++){int gx=30+x+i*200/m.steps,w=200/m.steps-2;fb_rect(gx,y,w,7,(m.lane[l]>>i)&1?C_ACC(0):FL_BG1);if(m.playhead==i)fb_hline(gx,y+9,w,FL_INK);}}
            }
            if(m.tab==CF_ACC_LOOP) {
                for(unsigned i=0;i<4 && m.first+i<m.n;i++){int cx=30+i*60+x;fb_clip(i*60+x,112,(i+1)*60+x,168);if(m.focus==m.first+i){fb_frame(cx-27,114,54,41,FL_BG3);if(m.focus_weight)fb_hline(cx-26,115,52,C_ACC(0));}cfn_center(&CFN_16,cx,131,m.degree[i],C_ACC(0));cfn_center(&CFN_12,cx,148,m.rail[i],FL_INK);if(m.slot==m.first+i)fb_hline(cx-22,158,44*m.cursor_width/100,C_ACC(0));}
                fb_clip(0,49,240,174);char b[24];kint(b,m.focus+1," focus / ");if(m.slot!=255)kint(b+str_len(b),m.slot+1," playing");else str_cpy(b+str_len(b),"stopped",sizeof b-str_len(b));cfn_text(&CFN_11,10+x,172,b,FL_INK2);
            }
        }
        fb_unclip();
        cfn_fit(&CFN_11,10,183,m.pending[0]?m.pending:"ALGORITHM: Groove / Loop / Mix / Click",220,m.pending[0]?C_ACC(0):FL_INK2);
        cfn_hint(m.next,FL_INK2);
        cfn_controls(m.label,m.value,m.disabled,m.emphasis);
        fb_unclip();
    }
    if(m.message[0]) {
        fb_rect(8,75,224,80,FL_BG);fb_frame(8,75,224,80,FL_BG3);
        char b[64];str_cpy(b,m.message,sizeof b);unsigned n=0;while(b[n] && b[n]!='\n')n++;
        char *next=b[n]?b+n+1:0;b[n]=0;cfn_fit(&CFN_12,16,109,b,208,FL_INK);if(next)cfn_fit(&CFN_12,16,130,next,208,FL_INK);
    }
    if(!cu.pop)cf_present();
}
