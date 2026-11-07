/* SPDX-License-Identifier: GPL-3.0-only
 * Numbered semantic editor, foreground only. PLAY shares the accompaniment clock.
 */
enum { CFD_HARMONY,CFD_TIMING,CFD_VOICING,CFD_BASS,CFD_PHRASE,CFD_NTAB };
static const char *const CFD_TAB[]={"Edit Chord","Timing","Voicing","Bass","Phrase"};
static const char *const CFD_QUALITY[]={"Follow","Major","Minor","Dim","Aug","Power"};
static const char *const CFD_ACTION[]={"Insert","Duplicate","Remove","Undo"};
static struct { uint8_t focus,action,confirm,conflict,choice;unsigned requested; } cfdu;
static int cfd_prepare(cpa_plan_t *plan,cf_chord_cfg_t cfg,unsigned rhythm,unsigned mode,unsigned repeat_off,unsigned click_start)
{
    if(!cfwd.have || !cf_song_valid(&cfwd.song))return 0;
    memset(plan,0,sizeof *plan);plan->idea=CF_P_DRAFT;plan->n=cfwd.song.n;plan->tonic=cfg.tonic;plan->scale=cfg.scale;
    plan->beats=cfwd.song.beats;plan->rhythm=cpa_compatible(rhythm,plan->beats);plan->mode=mode;plan->repeat_off=repeat_off;plan->click_start=click_start;
    if(plan->rhythm<DM_N){plan->kit=dm_kit(plan->rhythm);plan->swing=dm_swing(plan->rhythm);}
    for(unsigned i=0;i<plan->n;i++) {
        cf_song_chord_t o;cpa_slot_t *s=&plan->slot[i];const cpa_slot_t *prev=i?&plan->slot[i-1]:0;
        if(!cf_song_resolve(&cfwd.song,i,&cfg,prev?prev->note:0,prev?prev->n:0,&o))return 0;
        s->root=o.root;s->mask=o.mask;s->n=o.n;s->ticks=o.ticks;s->gate=o.gate;s->tie=o.tie;s->bass_pc=o.bass_pc;
        memcpy(s->note,o.note,o.n);plan->degree[i]=cfwd.song.slot[i].degree;
        cf_chord_out_t bass={0};bass.sounding=1;bass.root_note=o.root;bass.root_pc=o.root%12;bass.mask=o.mask;bass.bass_pc=o.bass_pc;
        s->nb=o.bass_off?0:cfp_bass_notes_of(&bass,s->bass);
        for(unsigned j=0;j<s->nb;j++)s->bass[j]=(uint8_t)clampi(s->bass[j]+12*o.bass_oct,0,127);
        if(s->nb==2 && s->bass[0]==s->bass[1])s->nb=1;
        plan->nb=s->nb;
    }
    return cpa_valid(plan);
}
static void cfd_publish(void)
{
    cfd_changed();
    if(!cpa.on){cpg.idea=CF_P_DRAFT;cs.beats=cfwd.song.beats;cpg.rhythm=cpa_compatible(cpg.rhythm,cs.beats);cf_send_metro();return;}
    cpa_plan_t active,plan;cpa_active_request(&active);cf_chord_cfg_t cfg=cs.chord;cfg.tonic=active.tonic;cfg.scale=active.scale;
    if(cfd_prepare(&plan,cfg,active.rhythm,active.mode,active.repeat_off,active.click_start))
        cpa_publish(&plan,CPA_PHRASE,cfwd.song.beats!=active.beats);
}
static int cfd_commit(const cf_song_t *s)
{
    cf_song_t next=*s;
    if(next.rhythm<65534)for(unsigned i=0;i<DM_N;i++)if(DM_RHY[i].id==next.rhythm && DM_RHY[i].beats!=next.beats)next.rhythm=65535;
    if(!cf_song_draft_commit(&cfwd,&next))return 0;cfd_publish();return 1;
}
static void cfd_clone(unsigned idea)
{
    cf_song_t s;if(!cf_song_clone(&s,cf_progression_get(idea)))return;
    s.tonic=cs.chord.tonic;s.scale=cf_harmony_supported(cs.chord.scale,CF_Q_BASIC)?cs.chord.scale:s.scale;s.bpm=cs.bpm;
    s.rhythm=cpg.rhythm<DM_N?DM_RHY[cpg.rhythm].id:cpg.rhythm==CPA_CLICK?65534:65535;
    s.kit=cpg.rhythm<DM_N?dm_kit(cpg.rhythm):0;s.swing=cpg.rhythm<DM_N?dm_swing(cpg.rhythm):0;
    s.drums_only=cpg.mode;s.repeat_off=cpg.repeat_off;
    cfwd.dirty=0;cf_song_draft_begin(&cfwd,&s);cfdu.focus=0;cfdu.confirm=cfdu.conflict=0;cfd_view_update();
}
static void cfd_open(void)
{
    if(cr_safe || cfs_restore_lock || cfds.unavailable){cf_message("Draft unavailable: restore backup");return;}
    if(cf_sn.sq_rec!=255 || sqk.on){cf_message("Finish recording to edit");return;}
    unsigned idea=cpg.idea;
    if(cpa.on)idea=cf_sn.ac_idea;
    if(idea!=CF_P_DRAFT)idea=cf_progression_pick(idea,0);
    if(idea!=CF_P_DRAFT && cfwd.have && cfwd.dirty) {
        cfdu.conflict=1;cfdu.choice=0;cfdu.requested=idea;
    } else if(!cfwd.have || (!cfwd.dirty && idea!=CF_P_DRAFT))cfd_clone(idea);
    if(!cfwd.have){cf_message("Draft could not open");return;}
    cu.scr=SCR_PEDIT;cu.pop=POP_NONE;cfdu.confirm=0;
}
static void cfd_select_source(void)
{
    cfd_view_update();cf_progression_choose(CF_P_DRAFT);
    if(!cpa.on){cs.bpm=cfwd.song.bpm;cpg.mode=cfwd.song.drums_only;cpg.repeat_off=cfwd.song.repeat_off;
        unsigned r=CPA_OFF;if(cfwd.song.rhythm==65534)r=CPA_CLICK;else for(unsigned i=0;i<DM_N;i++)if(DM_RHY[i].id==cfwd.song.rhythm)r=i;
        cpg.rhythm=cpa_compatible(r,cfwd.song.beats);
        if(cpg.rhythm<DM_N){dmr.kit[dm_uid(cpg.rhythm)]=cfwd.song.kit;dmr.swing[dm_uid(cpg.rhythm)]=cfwd.song.swing;}
        cf_send_metro();}
}
static void cfd_knob_info(int k,cf_knob_t *o)
{
    if(!cfwd.have){o->dis=1;return;}
    if(cfdu.conflict){if(!k){kfmt(o->name,12,"ACTION");kfmt(o->val,14,cfdu.choice?"Discard old":"Return draft");}return;}
    if(cfdu.confirm){if(!k){kfmt(o->name,12,"REMOVE?");kfmt(o->val,14,cfdu.choice?"Remove":"Cancel");}return;}
    const cf_song_slot_t *v=&cfwd.song.slot[cfdu.focus%cfwd.song.n];unsigned tab=cu.page[SCR_PEDIT];
    if(tab==CFD_HARMONY) {
        static const char *const name[]={"DEGREE","QUALITY","COLOR","BARS"};kfmt(o->name,12,name[k]);
        if(k==0){if(v->alter){o->val[0]=v->alter<0?'b':'#';kint(o->val+1,v->degree+1,0);}else kint(o->val,v->degree+1,0);}if(k==1)kfmt(o->val,14,!v->quality&&v->mask?"Source":CFD_QUALITY[v->quality]);
        if(k==2)kfmt(o->val,14,cf_modifier_label(v->color));if(k==3){if(v->unit){kfmt(o->val,14,"Timing tab");o->dis=1;}else kint(o->val,v->bars,0);}
    } else if(tab==CFD_TIMING) {
        static const char *const name[]={"UNIT","LENGTH","GATE","TIE"};kfmt(o->name,12,name[k]);
        if(k==0)kfmt(o->val,14,v->unit?"Quarters":"Bars");if(k==1){if(!v->unit)kint(o->val,v->bars," bar");else{fmt_fix(o->val,v->quarters*25,2);str_cpy(o->val+str_len(o->val)," q",14-str_len(o->val));}}
        if(k==2)kint(o->val,v->gate,"%");if(k==3)kfmt(o->val,14,v->tie?"On":"Off");
    } else if(tab==CFD_VOICING) {
        if(k==0){kfmt(o->name,12,"INVERSION");if(v->inversion<0)kfmt(o->val,14,"Smooth");else kint(o->val,v->inversion,0);}
        if(k==1){kfmt(o->name,12,"REGISTER");kint(o->val,v->reg," oct");}
    } else if(tab==CFD_BASS) {
        if(k==0){kfmt(o->name,12,"BASS TONE");if(v->bass<0)kfmt(o->val,14,"Root");else{cf_spell_t sp;cf_key_spell(cfwd.song.tonic,cfwd.song.scale,&sp);cf_pc_name((cfwd.song.tonic+cf_scale_step(cfwd.song.scale,v->degree)+v->alter+v->bass+12)%12,&sp,o->val);}}
        if(k==1){kfmt(o->name,12,"REGISTER");kint(o->val,v->bass_oct," oct");}
        if(k==2){kfmt(o->name,12,"SLOT BASS");kfmt(o->val,14,v->bass_off?"Off":"Follow");}
    } else {
        if(k==0){kfmt(o->name,12,"ACTION");kfmt(o->val,14,CFD_ACTION[cfdu.action]);}
        if(k==1){kfmt(o->name,12,"POSITION");kint(o->val,cfdu.focus+1,0);}
        if(k==2){kfmt(o->name,12,"METRE");kint(o->val,cfwd.song.beats,cfwd.song.beats==6?"/8":"/4");}
        if(k==3){kfmt(o->name,12,"SECTION");kfmt(o->val,14,"A");o->dis=1;}
    }
}
static void cfd_turn(int k,int d)
{
    if(!d || !cfwd.have)return;
    if(cfdu.conflict || cfdu.confirm){if(!k)cfdu.choice=d>0;return;}
    cf_song_t next=cfwd.song;cf_song_slot_t *v=&next.slot[cfdu.focus%next.n];unsigned tab=cu.page[SCR_PEDIT];
    if(tab==CFD_HARMONY){if(k==0)v->degree=wrapi(v->degree+d,7);if(k==1){v->quality=wrapi(v->quality+d,CF_SONG_NQUALITY);v->mask=0;}if(k==2)v->color=wrapi(v->color+d,CF_MOD_MAJ);if(k==3&&!v->unit)v->bars=clampi(v->bars+d,1,8);}
    if(tab==CFD_TIMING){if(k==0){unsigned unit=d>0;if(unit!=v->unit){unsigned per=next.beats*(next.beats==6?2:4);if(unit)v->quarters=clampi(v->bars*per,1,128);else v->bars=clampi((v->quarters+per-1)/per,1,8);v->unit=unit;}}if(k==1){if(v->unit)v->quarters=clampi(v->quarters+d,1,128);else v->bars=clampi(v->bars+d,1,8);}if(k==2)v->gate=clampi(v->gate+d,20,100);if(k==3)v->tie=d>0;}
    if(tab==CFD_VOICING){if(k==0)v->inversion=clampi(v->inversion+d,-1,3);if(k==1)v->reg=clampi(v->reg+d,-2,2);}
    if(tab==CFD_BASS){if(k==0){int8_t tones[12];int n=cf_mask_tones(cf_song_mask(&next,cfdu.focus,next.scale),tones),at=0;for(int i=0;i<n;i++)if(v->bass==tones[i]%12)at=i;at=wrapi(at+d,n);v->bass=at?tones[at]%12:-1;}if(k==1)v->bass_oct=clampi(v->bass_oct+d,-2,1);if(k==2)v->bass_off=d<0;}
    if(tab==CFD_PHRASE){if(k==0){cfdu.action=wrapi(cfdu.action+d,4);return;}if(k==1){cfdu.focus=wrapi(cfdu.focus+d,next.n);return;}if(k==2)next.beats=clampi(next.beats+d,2,7);}
    (void)cfd_commit(&next);
}
static void cfd_execute(void)
{
    if(cfdu.conflict){if(cfdu.choice){cfd_clone(cfdu.requested);cfd_changed();}else {cfdu.conflict=0;cfd_select_source();}return;}
    if(cfdu.confirm){if(cfdu.choice){cf_song_t next=cfwd.song;if(cf_song_remove(&next,cfdu.focus)){cfdu.focus%=next.n;cfd_commit(&next);}}cfdu.confirm=0;return;}
    if(cu.page[SCR_PEDIT]!=CFD_PHRASE)return;
    if(cfdu.action==3){if(cf_song_draft_undo(&cfwd)){cfdu.focus%=cfwd.song.n;cfd_publish();}return;}
    if(cfdu.action==2){if(cfwd.song.n>1){cfdu.confirm=1;cfdu.choice=0;}else cf_message("Keep at least one slot");return;}
    cf_song_t next=cfwd.song;if(cf_song_insert(&next,cfdu.focus,cfdu.action==1)){if(cfdu.action==1)cfdu.focus++;cfd_commit(&next);}else cf_message("16-slot limit");
}

/* Setup performance belongs to the selected draft; browsing never overwrites it. */
static void cfd_ui_poll(void)
{
    if(!cfwd.have || cpg.idea!=CF_P_DRAFT || cfds.unavailable || cpa_setup.flags)return;
    cf_song_t next=cfwd.song;next.tonic=cs.chord.tonic;next.scale=cs.chord.scale;next.bpm=cs.bpm;
    unsigned r=cpg.rhythm;
    if(cf_sn.ac_on && cf_sn.ac_idea==CF_P_DRAFT){r=cf_sn.ac_rhythm;next.kit=cf_sn.ac_kit;next.swing=cf_sn.ac_swing;next.drums_only=cf_sn.ac_mode;next.repeat_off=cf_sn.ac_repeat_off;}
    else {if(r<DM_N){next.kit=dm_kit(r);next.swing=dm_swing(r);}next.drums_only=cpg.mode;next.repeat_off=cpg.repeat_off;}
    next.rhythm=r<DM_N?DM_RHY[r].id:r==CPA_CLICK?65534:65535;
    if(cfd_device_valid(&next) && memcmp(&next,&cfwd.song,sizeof next)) {
        /* Native phrase undo keeps the latest independent performance setup. */
        if(cfwd.can_undo){cfwd.previous.tonic=next.tonic;cfwd.previous.scale=next.scale;cfwd.previous.bpm=next.bpm;
            cfwd.previous.rhythm=next.rhythm;cfwd.previous.kit=next.kit;cfwd.previous.swing=next.swing;
            cfwd.previous.drums_only=next.drums_only;cfwd.previous.repeat_off=next.repeat_off;}
        cfwd.song=next;cfwd.dirty=1;cfd_changed();
    }
}
