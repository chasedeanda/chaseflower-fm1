/* SPDX-License-Identifier: GPL-3.0-only
 * Presentation ownership is session-only. Parameter edits reuse existing owners.
 * Kit/swing edits publish to the same accompaniment bar queue as rhythm edits.
 */
static void cf_click_cancel_pending(void)
{
    fm1_irq_off();
    if(cpg.click_generation!=cpa.click_generation){cs.metro_level=cfm.level;cpg.click_generation=cpa.click_generation;}
    cpa_edit.plan.click_start=0;cpa_setup.plan.click_start=0;fm1_irq_on();
}
static unsigned cf_home_rhythm(void)
{
    if (cpa_running()) { cpa_plan_t p; cpa_active_request(&p); return p.rhythm; }
    return cpg.rhythm;
}
/* Moments are real drum actions; availability/status comes from the audio snapshot. */
static int cf_perform_ready(void)
{
    if(sqk.on || cf_sn.sq_rec != 255 || cf_sn.sq_cin || cf_sn.dm_rin) return 0;
    if(cf_sn.ac_on) return !cf_sn.ac_paused && !cf_sn.ac_waiting && !cf_sn.ac_lost && cf_sn.dm_on;
    if(cf_sn.dm_on) return 1;
    const sq_pat_t *p=&sq_st.pat[cf_sn.sq_pat%SQ_NPAT];
    return cf_sn.sq_on && sq_own(p,SQ_DRUMS) && !((p->mute>>SQ_DRUMS)&1u);
}
static void cf_perform_turn(unsigned action)
{
    if(!cf_perform_ready()) {
        cf_message(sqk.on || cf_sn.sq_rec!=255 || cf_sn.dm_rin ? "Finish recording first" :
                   cf_sn.ac_waiting || cf_sn.ac_lost ? "Waiting for MIDI clock" : "PLAY a groove first");
        return;
    }
    if(action==CFPERF_FILL && cf_sn.perf_dropped) { cf_message("Bring drums back first"); return; }
    if(!cf_post(CFE_PERFORM,action,0,0))cf_message("Busy: turn again");
    else cu.pop=POP_NONE;
}
static int cf_home_bank_info(int k, cf_knob_t *o)
{
    if (cf_home_bank == CF_HB_CORE) return 0;
    if (cf_home_bank == CF_HB_LEARN) {
        kfmt(o->name,12,""); kfmt(o->val,14,""); o->dis=1;
        return 1;
    }
    if (cf_home_bank == CF_HB_PERFORM) {
        static const char *const label[]={"FILL","DROP","BACK","TEMPO"};
        if(k==3){cpa_tempo_info(o);return 1;}
        kfmt(o->name,12,label[k]);
        const char *value="Turn";
        if(cf_sn.perf_pending==(unsigned)k+1u)value="Queued";
        else if(k==0 && cf_sn.perf_fill)value="1 bar";
        else if(k==1 && cf_sn.perf_dropped)value="Off";
        else if(k==2 && !cf_sn.perf_dropped && !cf_sn.perf_fill)value="Groove";
        kfmt(o->val,14,value);
        o->dis=!cf_perform_ready() || (k==0 && (cf_sn.perf_dropped || cf_sn.perf_fill));
        return 1;
    }
    if (cf_home_bank == CF_HB_SOUND) {
        static const char *const label[] = {"CHORD", "BASS", "C.LEVEL", "B.LEVEL"};
        kfmt(o->name, 12, label[k]);
        if (k < 2) kfmt(o->val, 14, cf_snd[k].name);
        else {
            unsigned level = k == 2 ? cs.level_chord : cs.bass_level;
            kint(o->val, level * 100 / 127, "%"); o->bar = level * 255 / 127;
            if (k == 3 && !cs.bass_on) { kfmt(o->val,14,"Bass off"); o->dis=1; }
        }
    } else if (cf_home_bank == CF_HB_TEXTURE) {
        static const unsigned fx[] = {CFX_REVERB, CFX_DELAY, CFX_CHORUS};
        static const char *const label[] = {"REVERB", "DELAY", "CHORUS", "GLIDE"};
        kfmt(o->name, 12, label[k]);
        if (k == 3) kint(o->val, cfe_glide_ms(trk[CF_PART_CHORD].p[P_GLIDE]), "ms");
        else {
            unsigned e=fx[k]; char value[8]; kint(value,cs.fx.v[e][2],"%");
            kfmt(o->val,14,cfx_bypass?"Byp ":cfx_is_on(e)?"":"Off ");
            str_cpy(o->val+str_len(o->val),value,sizeof o->val-str_len(o->val));
            o->dis=cfx_bypass || !cfx_is_on(e); o->bar=cs.fx.v[e][2]*255/100;
        }
    } else {
        static const char *const label[] = {"KIT", "DRUM LVL", "SWING", "CLICK"};
        unsigned r=cpa_running()?cf_sn.ac_rhythm:cpg.rhythm;
        kfmt(o->name,12,label[k]);
        if ((k==0 || k==2) && r>=DM_N) { kfmt(o->val,14,"No rhythm"); o->dis=1; }
        else if (k==0) kfmt(o->val,14,SQ_KITS[(cpa_running()?cf_sn.ac_kit:dm_kit(r))%SQ_NKIT].name);
        else if (k==2) kint(o->val,cpa_running()?cf_sn.ac_swing:dm_swing(r),"%");
        else if (k==1) { kint(o->val,cs.level_drums*100/127,"%"); o->bar=cs.level_drums*255/127; }
        else if (cs.metro_level) kint(o->val,cs.metro_level,"%");
        else kfmt(o->val,14,"Off");
        if (sqk.on && (k==0 || k==2)) o->dis=1;
    }
    return 1;
}
static void cf_home_groove(int k,int d)
{
    if (sqk.on) return;
    unsigned r=cf_home_rhythm();
    if (r>=DM_N) { cf_message("Choose a rhythm in Core"); return; }
    unsigned kit=dm_kit(r),swing=dm_swing(r);
    cpa_plan_t p;
    if (cpa_running()) { cpa_active_request(&p); kit=p.kit; swing=p.swing; }
    if (k==0) kit=sq_kit_step(kit,d);
    else swing=(unsigned)clampi((int)swing+d*5,0,100);
    /* Keep the pattern's existing stable-ID tweak path, independent of selection. */
    if (k==0) dmr.kit[dm_uid(r)]=(uint8_t)(kit==DM_RHY[r].kit?0xFFu:kit);
    else dmr.swing[dm_uid(r)]=(uint8_t)(swing==DM_RHY[r].swing?0xFFu:swing);
    if (cpa_running()) { p.kit=kit; p.swing=swing; cpa_publish(&p,CPA_GROOVE,0); }
    else if (dm_plays(r)) cf_post(CFE_DRUM,2,0,dm_v(r));
}
static int cf_home_bank_turn(int k,int d)
{
    if (cf_home_bank==CF_HB_CORE) return 0;
    if (cf_home_bank==CF_HB_LEARN) return 1; /* read-only guide: all four knobs are deliberately inactive */
    if (!d) return 1;
    if(cf_home_bank==CF_HB_PERFORM) {
        if(k<3)cf_perform_turn((unsigned)k+1u);else cpa_tempo(d);
        return 1;
    }
    if (cf_home_bank==CF_HB_SOUND) {
        if (k<2) {
            if (cf_snd_step((unsigned)k,d)) {
                if (cs.env_link) cf_env_follow();
                ui_snd_heard((unsigned)k);
            }
        } else sq_level_set(k==2?SQ_CHORDS:SQ_BASS,(k==2?cs.level_chord:cs.bass_level)+d*2);
    } else if (cf_home_bank==CF_HB_TEXTURE) {
        static const unsigned fx[]={CFX_REVERB,CFX_DELAY,CFX_CHORUS};
        if (k==3) cpa_glide(d);
        else { unsigned e=fx[k]; cs.fx.v[e][2]=(uint8_t)clampi(cs.fx.v[e][2]+d,0,100); cf_fx_apply(&cs.fx); }
    } else {
        if (k==0 || k==2) cf_home_groove(k,d);
        else if (k==1) sq_level_set(SQ_DRUMS,cs.level_drums+d*2);
        else { cf_click_cancel_pending();cs.metro_level=(uint8_t)clampi(cs.metro_level+d*2,0,100);
            if(cs.metro_level && cs.metro_level!=cf_click_level) { cf_click_level=cs.metro_level; cpb_dirty=1; cpb_changed=cu.now; }
            cf_send_metro(); }
    }
    return 1;
}
