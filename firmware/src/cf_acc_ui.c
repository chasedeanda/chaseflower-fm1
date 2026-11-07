/* SPDX-License-Identifier: GPL-3.0-only
 * Shared-clock accompaniment controls. No screen creates another transport.
 */
static unsigned cf_acc_tab(void) { return cu.page[SCR_ACC]%CF_ACC_N; }
static void cf_acc_repeat(int d)
{
    cpg.repeat_off=(uint8_t)(d<0);
    if(cpa_running()) { cpa_plan_t p;cpa_active_request(&p);p.repeat_off=cpg.repeat_off;cpa_publish(&p,CPA_GROOVE,0); }
}
static void cf_acc_click_remember(unsigned level)
{
    level=level<1?1:level>100?100:level;
    if(cf_click_level!=level){cf_click_level=(uint8_t)level;cpb_dirty=1;cpb_changed=cu.now;}
}
static void cf_acc_knob_info(int k,cf_knob_t *o)
{
    unsigned tab=cf_acc_tab(),n=cf_progression_current()->n;
    if(tab==CF_ACC_GROOVE) {
        if(k==0){kfmt(o->name,12,"RHYTHM");kfmt(o->val,14,cpa_rhythm_name(cpa_running()?cf_sn.ac_rhythm:cpg.rhythm));o->dis=sqk.on;}
        else { unsigned old=cf_home_bank;cf_home_bank=CF_HB_GROOVE;cf_home_bank_info(k==1?0:k==2?1:2,o);cf_home_bank=old; }
    } else if(tab==CF_ACC_LOOP) {
        if(k==0){kfmt(o->name,12,"MODE");kfmt(o->val,14,(cpa_running()?cf_sn.ac_mode:cpg.mode)==CPA_DRUMS?"Drums only":"Chord+Drum");}
        if(k==1){kfmt(o->name,12,"FIRST");kint(o->val,1,0);o->dis=1;}
        if(k==2){kfmt(o->name,12,"LAST");kint(o->val,n,0);o->dis=1;}
        if(k==3){kfmt(o->name,12,"REPEAT");kfmt(o->val,14,(cpa_running()?cf_sn.ac_repeat_off:cpg.repeat_off)?"Off":"On");}
    } else if(tab==CF_ACC_MIX) {
        static const char *const label[]={"CHORD","BASS","DRUMS","BASS LINK"};kfmt(o->name,12,label[k]);
        if(k<3){unsigned v=k==0?cs.level_chord:k==1?cs.bass_level:cs.level_drums;kint(o->val,v*100/127,"%");o->bar=v*255/127;}
        else kfmt(o->val,14,cs.bass_on?"On":"Off");
    } else {
        if(k==0){kfmt(o->name,12,"CLICK");kfmt(o->val,14,cs.metro_level?"On":"Off");}
        if(k==1){kfmt(o->name,12,"LEVEL");kint(o->val,cs.metro_level?cs.metro_level:cf_click_level,"%");}
        if(k==2){kfmt(o->name,12,"SOUND");kfmt(o->val,14,CF_CLICKS[cs.click%CF_NCLICKS].name);}
        if(k==3)cpa_tempo_info(o);
    }
}
static void cf_acc_knob_turn(int k,int d)
{
    if(!d)return;
    switch(cf_acc_tab()) {
    case CF_ACC_GROOVE:
        if(k==0)cf_progression_rhythm(d);
        else if(k==1 || k==3)cf_home_groove(k==1?0:2,d);
        else sq_level_set(SQ_DRUMS,cs.level_drums+d*2);
        break;
    case CF_ACC_LOOP:
        if(k==0)cpa_ui_mode(d);
        if(k==3)cf_acc_repeat(d);
        break; /* full phrase endpoints are deliberately read-only in phase 1 */
    case CF_ACC_MIX:
        if(k<3)sq_level_set(k==0?SQ_CHORDS:k==1?SQ_BASS:SQ_DRUMS,(k==0?cs.level_chord:k==1?cs.bass_level:cs.level_drums)+d*2);
        else {cs.bass_on=(uint8_t)(d>0);cf_send_config();}
        break;
    case CF_ACC_CLICK:
        if(k==0){cf_click_cancel_pending();if(cs.metro_level)cf_acc_click_remember(cs.metro_level);cs.metro_level=d>0?cf_click_level:0;cf_send_metro();}
        if(k==1){cf_click_cancel_pending();cf_acc_click_remember((unsigned)clampi((int)(cs.metro_level?cs.metro_level:cf_click_level)+d,1,100));if(cs.metro_level){cs.metro_level=cf_click_level;cf_send_metro();}}
        if(k==2){cs.click=(uint8_t)wrapi(cs.click+d,CF_NCLICKS);cf_click_sound=cs.click;}
        if(k==3)cpa_tempo(d);
        break;
    }
}
static void cf_acc_select(int d)
{
    if(cf_acc_tab()==CF_ACC_GROOVE)cf_progression_rhythm(d);
    else if(cf_acc_tab()==CF_ACC_LOOP || cf_acc_tab()==CF_ACC_MIX) {
        unsigned n=cf_acc_tab()==CF_ACC_LOOP?cf_progression_current()->n:3;
        cf_acc_focus=(uint8_t)wrapi(cf_acc_focus+d,n);
    }
}
static void cf_acc_restore(void)
{
    unsigned r=cf_home_rhythm();if(r>=DM_N || sqk.on)return;
    dmr.kit[dm_uid(r)]=dmr.swing[dm_uid(r)]=0xFF;
    if(cpa_running()){cpa_plan_t p;cpa_active_request(&p);p.kit=dm_kit(r);p.swing=dm_swing(r);cpa_publish(&p,CPA_GROOVE,0);}
    else if(dm_plays(r))cf_post(CFE_DRUM,2,0,dm_v(r));
}
/* Two bounded lines, always distinct from the currently applied setup. */
static void cf_acc_pending(char *name,unsigned nn,char *setup,unsigned ns)
{
    name[0]=setup[0]=0;if(!cf_sn.ac_on || (cf_sn.ac_next==CF_P_NONE && !cf_sn.ac_edits))return;
    if(cf_sn.ac_next!=CF_P_NONE){str_cpy(name,"Next: ",nn);str_cpy(name+6,cf_progression_get(cf_sn.ac_next)->name,nn-6);}
    else str_cpy(name,"Next bar: active setup",nn);
    cf_spell_t spell;char key[4],b[16];
    cf_key_spell(cf_sn.ac_next_tonic,cf_sn.ac_next_scale,&spell);cf_pc_name(cf_sn.ac_next_tonic,&spell,key);
    str_cpy(setup,key,ns);str_cpy(setup+str_len(setup)," ",ns-str_len(setup));
    str_cpy(setup+str_len(setup),CF_SCALES[cf_sn.ac_next_scale].shrt,ns-str_len(setup));
    str_cpy(setup+str_len(setup)," ",ns-str_len(setup));kint(b,cf_sn.ac_next_beats,cf_sn.ac_next_beats==6?"/8 ":"/4 ");
    str_cpy(setup+str_len(setup),b,ns-str_len(setup));
    str_cpy(setup+str_len(setup),cpa_rhythm_name(cf_sn.ac_next_rhythm),ns-str_len(setup));
    if(cf_sn.ac_next==CF_P_NONE) {
        const char *prefix=(cf_sn.ac_edits&(CPA_NOTES|CPA_PHRASE))?"Next chord: ":"Next bar: ";unsigned at=str_len(prefix);
        str_cpy(name,prefix,nn);str_cpy(name+at,cf_sn.ac_next_mode==CPA_DRUMS?"Drums":"Chords+Drums",nn-at);
        str_cpy(name+str_len(name),cf_sn.ac_next_repeat_off?" / once":" / loop",nn-str_len(name));
        if(cf_sn.ac_next_rhythm<DM_N) {
            str_cpy(setup+str_len(setup)," / ",ns-str_len(setup));
            str_cpy(setup+str_len(setup),SQ_KITS[cf_sn.ac_next_kit%SQ_NKIT].name,ns-str_len(setup));
            kint(b,cf_sn.ac_next_swing,"%");str_cpy(setup+str_len(setup)," ",ns-str_len(setup));str_cpy(setup+str_len(setup),b,ns-str_len(setup));
        }
    }
}
static const char *cf_acc_transport(void)
{
    if(cf_sn.ac_lost)return "EXT LOST";
    if(cf_sn.ac_waiting || (cs.clock_in==1 && !cf_sn.ext_clock))return "EXT WAIT";
    if(cf_sn.ac_on)return cf_sn.ac_paused?"PAUSE":"PLAY";
    if(cf_sn.sq_rec!=255)return cf_sn.sq_cin?"COUNT IN":"REC";
    if(cf_sn.sq_on)return "REPLAY";
    if(cf_sn.dm_on)return "DRUMS";
    return cf_sn.running||cf_sn.metro_on?"CLOCK":"STOP";
}
