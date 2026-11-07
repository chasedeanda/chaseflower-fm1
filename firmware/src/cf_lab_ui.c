/* SPDX-License-Identifier: GPL-3.0-only
 * Chord Lab foreground state. Reading/browsing never publishes notes or edits.
 */
enum { CFL_NOTES,CFL_INTERVALS,CFL_COMPARE,CFL_NEXT,CFL_NTAB };
static const char *const CFL_TAB[]={"Notes & Degree","Intervals","Compare","Next Chord"};
static struct {
    uint8_t target,note,comparison,side,candidate,context,insert,explicit_slot,tab;
    uint8_t have_last,now_on,now_audition,compare_valid,nchoices,fallback,plan_valid;
    cf_lab_chord_t now,last,target_chord,compare_base,compare_prev,compare[2];
    cf_lab_history_t history,next_history;
    cf_lab_choice_t choice[CF_LAB_CHOICES];
    cf_lab_info_t info,choice_info[CF_LAB_CHOICES],compare_info[2];
    uint8_t scale,tonic,source,compare_scale,compare_tonic,compare_target;
} clu;
static cpa_plan_t cfl_plan;
static struct { cf_song_t song;cf_chord_cfg_t cfg;cf_harmony_t harmony;uint16_t buttons;uint8_t idea,have; } cfl_plan_key;
static uint8_t cfl_plan_cached;
static void cfl_chord_slot(const cpa_slot_t *s,cf_lab_chord_t *out)
{ memset(out,0,sizeof *out);out->root=s->root;out->mask=s->mask;out->n=s->n;out->bass=s->nb?s->bass_pc:-1;memcpy(out->note,s->note,s->n); }
static void cfl_plan_update(void)
{
    unsigned idea=cf_sn.ac_on?cf_sn.ac_idea:cpg.idea;
    cf_chord_cfg_t cfg=cs.chord;
    if(idea==CF_P_DRAFT && cfwd.have){cfg.tonic=cfwd.song.tonic;cfg.scale=cfwd.song.scale;}
    int changed=!cfl_plan_cached || cfl_plan_key.idea!=idea || cfl_plan_key.have!=cfwd.have ||
        memcmp(&cfl_plan_key.cfg,&cfg,sizeof cfg) || memcmp(&cfl_plan_key.harmony,&ch,sizeof ch) ||
        cfl_plan_key.buttons!=cf_sn.out.buttons || (idea==CF_P_DRAFT && memcmp(&cfl_plan_key.song,&cfwd.song,sizeof cfwd.song));
    if(!changed)return;
    cfl_plan_cached=1;cfl_plan_key.idea=idea;cfl_plan_key.have=cfwd.have;cfl_plan_key.cfg=cfg;cfl_plan_key.harmony=ch;
    cfl_plan_key.buttons=cf_sn.out.buttons;if(idea==CF_P_DRAFT)cfl_plan_key.song=cfwd.song;
    clu.plan_valid=cpa_prepare_full(&cfl_plan,idea,cfg,CPA_OFF,CPA_BOTH,0,0);
    if(clu.source!=idea){clu.source=idea;clu.explicit_slot=0;clu.target=0;clu.insert=0;clu.compare_valid=0;}
    if(!clu.plan_valid){clu.target=0;clu.explicit_slot=0;}
    else if(clu.target>=cfl_plan.n+2)clu.target=0;
}
static void cfl_now(void)
{
    cf_lab_chord_t c={0};clu.now_on=clu.now_audition=0;
    if(cf_sn.out.sounding && cf_sn.out.n) {
        const cf_chord_out_t *o=&cf_sn.out;c.root=o->root_note;c.mask=o->mask;c.n=o->n;c.bass=cf_sn.nb?cf_sn.bass[cf_sn.nb-1]%12:-1;memcpy(c.note,o->note,c.n);clu.now_on=1;
    } else if(cf_sn.audition_on) {c=cf_sn.audition;clu.now_on=clu.now_audition=1;}
    else if((cf_sn.ac_on || cf_sn.sq_on) && cf_sn.sq_dn) {
        c.root=cf_sn.sq_droot;c.mask=cf_sn.sq_dmask;c.n=cf_sn.sq_dn;c.bass=cf_sn.sq_bn?cf_sn.sq_bass[cf_sn.sq_bn-1]%12:-1;
        memcpy(c.note,cf_sn.sq_dnote,c.n);clu.now_on=1;
    }
    clu.now=c;
    if(clu.now_on){clu.last=c;clu.have_last=1;}
    /* Committed live/scheduled identities only. Arp attacks and inversions
     * share root/mask; auditions have an independent owner and never enter. */
    if(!clu.now_audition && c.n && c.mask!=1)cf_lab_remember(&clu.history,cs.chord.scale,cs.chord.tonic,&c);
    if(clu.history.tonic!=cs.chord.tonic || clu.history.scale!=cs.chord.scale) {
        memset(&clu.history,0,sizeof clu.history);clu.history.tonic=cs.chord.tonic;clu.history.scale=cs.chord.scale;
    }
}
static void cfl_target_update(void)
{
    cf_lab_chord_t c={0};unsigned scale=cs.chord.scale,tonic=cs.chord.tonic;
    if(clu.target>=2 && clu.plan_valid) {cfl_chord_slot(&cfl_plan.slot[(clu.target-2)%cfl_plan.n],&c);scale=cfl_plan.scale;tonic=cfl_plan.tonic;}
    else if(clu.target==1 || !clu.now_on){if(clu.have_last)c=clu.last;}
    else c=clu.now;
    if(memcmp(&c,&clu.target_chord,sizeof c) || scale!=clu.scale || tonic!=clu.tonic) {
        clu.target_chord=c;clu.scale=scale;clu.tonic=tonic;cf_lab_describe(&c,scale,tonic,&clu.info);
    }
    if(c.n)clu.note%=c.n;else clu.note=0;
}
static void cfl_compare_init(void)
{
    clu.compare_scale=clu.scale;clu.compare_tonic=clu.tonic;clu.compare_target=clu.target;
    clu.compare_base=clu.target_chord;memset(&clu.compare_prev,0,sizeof clu.compare_prev);
    if(clu.target>=2 && clu.plan_valid)cfl_chord_slot(&cfl_plan.slot[(clu.target-2+cfl_plan.n-1)%cfl_plan.n],&clu.compare_prev);
    else if(clu.history.n>=2)clu.compare_prev=clu.history.chord[clu.history.n-2];
    clu.compare_valid=1;
}
static int cfl_comparisons(void)
{
    if(!clu.compare_valid)cfl_compare_init();
    cf_lab_chord_t base=clu.compare_base;if(!base.n)return 0;
    static cf_lab_chord_t old_base,old_prev;static uint8_t old_type,old_scale,old_tonic,valid,result;
    if(valid && old_type==clu.comparison && old_scale==clu.compare_scale && old_tonic==clu.compare_tonic &&
        !memcmp(&old_base,&base,sizeof base) && !memcmp(&old_prev,&clu.compare_prev,sizeof old_prev))return result;
    valid=1;old_base=base;old_prev=clu.compare_prev;old_type=clu.comparison;old_scale=clu.compare_scale;old_tonic=clu.compare_tonic;result=0;
    /* Suspensions and altered fifths keep their real triad identity. */
    uint32_t triad=base.mask&0xFFu; /* root through fifth, including suspensions/diminished fifth */
    if(!clu.comparison && triad==base.mask)return 0;
    for(unsigned side=0;side<2;side++) {
        cf_lab_chord_t *c=&clu.compare[side];*c=base;
        c->mask=!clu.comparison && !side?triad:base.mask;c->bass=-1;
        int root=base.root,inv=0;
        if(clu.comparison && side) {
            inv=cf_auto_inversion(root,c->mask,clu.compare_prev.note,clu.compare_prev.n);
            int8_t tones[12];int nt=cf_mask_tones(c->mask,tones);if(inv>=nt){root-=12;inv-=nt;}
        }
        c->n=cf_voice(root,c->mask,inv,c->note);cf_lab_describe(c,clu.compare_scale,clu.compare_tonic,&clu.compare_info[side]);
    }
    if(clu.comparison && (!clu.compare_prev.n || (clu.compare[0].n==clu.compare[1].n && !memcmp(clu.compare[0].note,clu.compare[1].note,clu.compare[0].n))))return 0;
    result=1;return 1;
}
static void cfl_next_update(void)
{
    if(cf_sn.audition_on)return; /* keep deliberate audition choices stable */
    cf_lab_history_t history=clu.history;
    if(clu.context) {
        memset(&history,0,sizeof history);history.tonic=clu.plan_valid?cfl_plan.tonic:cs.chord.tonic;history.scale=clu.plan_valid?cfl_plan.scale:cs.chord.scale;
        if(clu.plan_valid)for(unsigned i=0;i<clu.insert && i<cfl_plan.n;i++) {
            cf_lab_chord_t c;cfl_chord_slot(&cfl_plan.slot[i],&c);cf_lab_remember(&history,cfl_plan.scale,cfl_plan.tonic,&c);
        }
    }
    if(clu.nchoices && !memcmp(&history,&clu.next_history,sizeof history))return;
    clu.next_history=history;int fallback;clu.nchoices=cf_lab_suggest(&history,clu.choice,&fallback);clu.fallback=fallback;
    for(unsigned i=0;i<clu.nchoices;i++) {
        cf_lab_chord_t c={0};c.root=(history.tonic+cf_scale_step(history.scale,clu.choice[i].degree))%12+48;
        c.mask=clu.choice[i].mask;c.bass=-1;c.n=cf_voice(c.root,c.mask,0,c.note);
        cf_lab_describe(&c,history.scale,history.tonic,&clu.choice_info[i]);
    }
    if(clu.nchoices)clu.candidate%=clu.nchoices;
}
static void cfl_poll(void)
{
    cfl_now();
    if(cu.scr!=SCR_LAB)return;
    cfl_plan_update();cfl_target_update();if(clu.tab==CFL_NEXT)cfl_next_update();
}
static void cfl_open(void)
{
    clu.target=clu.explicit_slot=clu.compare_valid=clu.tab=0;clu.source=255;
    cfl_plan_cached=0;cu.scr=SCR_LAB;cu.pop=POP_NONE;cfl_poll();
}
static void cfl_target_turn(int d)
{
    int n=clu.plan_valid?cfl_plan.n+2:2;clu.target=wrapi(clu.target+d,n);clu.compare_valid=0;
    if(clu.target>=2){clu.explicit_slot=1;clu.insert=clu.target-2;if(clu.source==CF_P_DRAFT)cfdu.focus=clu.insert;}
    else clu.explicit_slot=0;
    cfl_target_update();
}
static void cfl_audition(void)
{
    if(cf_sn.audition_on || cla_mail.request==1){cla_request(0);return;}
    if(cf_sn.out.sounding || cf_sn.n || cf_sn.nb){cf_message("Release live chord to audition");return;}
    if(sqk.on || cf_sn.sq_rec!=255 || cf_sn.sq_on){cf_message("Finish recording/replay to audition");return;}
    if(cf_sn.ac_on && (cf_sn.ac_waiting || cf_sn.ac_paused)){cf_message("Resume clock to audition");return;}
    cf_lab_chord_t c;
    if(clu.tab==CFL_COMPARE) {
        if(!cfl_comparisons()){cf_message(clu.comparison?"Choose a target with another voicing":clu.target_chord.n?"Choose a color to compare":"Play a root or choose a slot");return;}
        c=clu.compare[clu.side];
    } else {
        cfl_next_update();if(!clu.nchoices){cf_message("Suggestions unavailable for scale");return;}
        memset(&c,0,sizeof c);c.mask=clu.choice[clu.candidate].mask;c.bass=-1;
        c.root=(clu.next_history.tonic+cf_scale_step(clu.next_history.scale,clu.choice[clu.candidate].degree))%12+48;
        c.n=cf_voice(c.root,c.mask,0,c.note);
    }
    if(c.n>6){cf_message("Audition supports six notes");return;}
    cla_request(&c);cu.pop=POP_NONE;
}
static void cfl_turn(int k,int d)
{
    if(!d)return;
    if(clu.tab==CFL_NEXT) {
        if(k==0 && clu.nchoices)clu.candidate=wrapi(clu.candidate+d,clu.nchoices);
        if(k==1){if(cf_sn.audition_on)cla_request(0);clu.context=d>0;clu.nchoices=0;}
        if(k==2 && clu.plan_valid){clu.insert=wrapi(clu.insert+d,cfl_plan.n);clu.explicit_slot=1;}
    } else if(clu.tab==CFL_COMPARE) {
        if(k==0){cla_request(0);clu.comparison=wrapi(clu.comparison+d,2);}
        if(k==1)clu.side=wrapi(clu.side+d,2);
    } else {
        if(k==0){cla_request(0);cfl_target_turn(d);}
        if(k==1 && clu.tab==CFL_INTERVALS && clu.target_chord.n)clu.note=wrapi(clu.note+d,clu.target_chord.n);
    }
    if(cf_sn.audition_on && ((clu.tab==CFL_NEXT && k==0) || (clu.tab==CFL_COMPARE && k==1))) {
        /* Switching an already playing audition is an explicit request. */
        cla_request(0);cf_lab_chord_t c;
        if(clu.tab==CFL_COMPARE){if(!cfl_comparisons())return;c=clu.compare[clu.side];}
        else {memset(&c,0,sizeof c);c.root=(clu.next_history.tonic+cf_scale_step(clu.next_history.scale,clu.choice[clu.candidate].degree))%12+48;c.mask=clu.choice[clu.candidate].mask;c.bass=-1;c.n=cf_voice(c.root,c.mask,0,c.note);}
        cla_request(&c);
    }
    cfl_poll();
}
static void cfl_select(int d)
{ cfl_turn(0,d); }
static void cfl_tab(int d)
{ cla_request(0);clu.tab=wrapi(clu.tab+d,CFL_NTAB);clu.compare_valid=0;cfl_poll(); }
static void cfl_edit(void)
{
    cla_request(0);
    if(clu.tab!=CFL_NEXT){unsigned focus=clu.target>=2?clu.target-2:0;unsigned explicit=clu.explicit_slot;cfd_open();if(cu.scr==SCR_PEDIT && !cfdu.conflict && explicit)cfdu.focus=focus%cfwd.song.n;return;}
    if(!clu.nchoices){cf_message("No suggestion to insert");return;}
    if(!clu.explicit_slot){cf_message("Choose insert slot with K3");return;}
    if(sqk.on || cf_sn.sq_rec!=255 || cf_sn.sq_on){cf_message("Finish recording/replay to insert");return;}
    if(cr_safe || cfs_restore_lock || cfds.unavailable){cf_message("Draft unavailable: restore backup");return;}
    if(clu.source!=CF_P_DRAFT && cfwd.have && cfwd.dirty){cf_message("Open existing draft before inserting");return;}
    /* Slot degrees are meaningful only in the same key/mode. Never silently
     * reinterpret a live-history candidate under a differently keyed draft. */
    if(clu.next_history.tonic!=cfl_plan.tonic || clu.next_history.scale!=cfl_plan.scale){cf_message("Choose Draft path context with K2");return;}
    if(clu.source!=CF_P_DRAFT)cfd_clone(clu.source);
    if(!cfwd.have || clu.insert>=cfwd.song.n){cf_message("Open a draft and choose a slot");return;}
    cf_song_t next=cfwd.song;cf_song_slot_t *v=&next.slot[clu.insert];v->degree=clu.choice[clu.candidate].degree;
    v->alter=0;v->quality=CF_SONG_FOLLOW;v->color=CF_MOD_NONE;v->mask=clu.choice[clu.candidate].mask;
    v->inversion=0;v->bass=-1;
    if(cfd_commit(&next)){cfdu.focus=clu.insert;cu.page[SCR_PEDIT]=CFD_HARMONY;cu.scr=SCR_PEDIT;cf_message("Inserted in selected slot / Undo: Phrase");}
}
static void cfl_knob_info(int k,cf_knob_t *o)
{
    if(clu.tab==CFL_NEXT) {
        if(k==0){kfmt(o->name,12,"CHOICE");if(clu.nchoices)kint(o->val,clu.candidate+1,"/3");else kfmt(o->val,14,"--");}
        if(k==1){kfmt(o->name,12,"CONTEXT");kfmt(o->val,14,clu.context?"Draft path":"Live history");}
        if(k==2){kfmt(o->name,12,"INSERT");if(clu.explicit_slot)kint(o->val,clu.insert+1,0);else kfmt(o->val,14,"Choose");}
    } else if(clu.tab==CFL_COMPARE) {
        if(k==0){kfmt(o->name,12,"COMPARE");kfmt(o->val,14,clu.comparison?"Root/Smooth":"Triad/Color");}
        if(k==1){kfmt(o->name,12,"SIDE");kfmt(o->val,14,clu.side?"B":"A");}
    } else {
        if(k==0){kfmt(o->name,12,"TARGET");if(clu.target>=2)kint(o->val,clu.target-1," slot");else kfmt(o->val,14,clu.target?"Last":"Now");}
        if(k==1 && clu.tab==CFL_INTERVALS){kfmt(o->name,12,"NOTE");kfmt(o->val,14,clu.info.n?clu.info.note[clu.note]:"--");}
    }
}
