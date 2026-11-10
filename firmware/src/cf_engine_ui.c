/* SPDX-License-Identifier: GPL-3.0-only
 * Foreground-only scene resolution, plan construction, staging and locks.
 * This file does not change sound programs, the catalogue, or stored formats. */
static cf_progression_t cge_view={"Heavenly Gaze","Engine / static snapshot via SAVE",2,0,CF_SC_MAJOR,4,4,8,{0,4,5,3,0,4,5,3}};
static cf_source_chord_t cge_source[8];
static void cge_view_update(void);
static uint8_t cge_sound_engine[2][3],cge_sound_factory[2][3],cge_rhythm[2],cge_fx_id[3],cge_resolved;
static cf_chord_cfg_t cge_seen_cfg;
static uint32_t cge_seen_sound,cge_seen_fx,cge_seen_groove;
static int cge_selected(void){return cge.loaded && (cpg.idea==CF_P_ENGINE || cge_active());}
static uint32_t cge_sound_hash(void)
{
    uint32_t h=0;uint8_t blob[ENG_BLOB_MAX];
    for(unsigned p=0;p<2;p++){
        h=h*16777619u^cfs_fnv(&cf_snd[p],sizeof cf_snd[p]);
        h=h*16777619u^cfs_fnv(&trk[p].p[P_E0],8*sizeof trk[p].p[0]);
        const eng_deep_t *d=ENGINES[trk[p].eng_req]->deep;
        if(d && d->blob_size<=sizeof blob){d->blob_get(&trk[p],blob);h=h*16777619u^cfs_fnv(blob,d->blob_size);}
    }return h;
}
static uint32_t cge_groove_hash(void)
{
    unsigned rhythm=cpg.rhythm,kit=0,swing=0;
    if(cge_active()){cpa_plan_t p;cpa_active_request(&p);rhythm=p.rhythm;kit=p.kit;swing=p.swing;}
    else if(rhythm<DM_N){kit=dm_kit(rhythm);swing=dm_swing(rhythm);}
    return rhythm | kit<<8 | swing<<16;
}
static void cge_remember_edits(void)
{cge_seen_cfg=cs.chord;cge_seen_sound=cge_sound_hash();cge_seen_fx=cfs_fnv(&cs.fx,sizeof cs.fx);cge_seen_groove=cge_groove_hash();}
static void cge_direct(unsigned category)
{
    if(!cge_selected())return;
    cge.locks|=category;cge.pending&=~category;
    for(unsigned i=0;i<4;i++)if(category&(1u<<i))cge.request[i]=cge.index[i];
    if(category&CGE_SOUND)for(unsigned p=0;p<2;p++)if(cf_stage[p].pat==CF_P_ENGINE)cf_stage[p].ready=0;
}
static void cge_gain_prepare(void)
{
    uint16_t next[5][3];unsigned e=cge.energy,e0=CGE_DEFAULTS[0];
    for(unsigned p=0;p<5;p++)for(unsigned k=0;k<3;k++){
        uint32_t gain=k?CGE_PROFILE[p][k+1]*e*32768u/(100u*e0):
            CGE_PROFILE[p][1]*(3u*e0+2u*e)*32768u/(500u*e0);
        next[p][k]=(uint16_t)(gain>32768?32768:gain);
    }
    fm1_irq_off();memcpy(cge.profile,next,sizeof next);fm1_irq_on();
}
static int cge_resolve_assets(void)
{
    if(cge_resolved)return 1;
    for(unsigned part=0;part<2;part++)for(unsigned j=0;j<3;j++){
        const char *name=part?CGE_BASS_SOUND[j]:CGE_CHORD_SOUND[j];unsigned found=0;
        for(unsigned e=0;e<CF_NENG;e++)for(unsigned f=0;f<ENGINES[CF_ENGINES[e]]->npresets;f++)
            if(cb_streq(ENGINES[CF_ENGINES[e]]->presets[f].name,name)){
                cge_sound_engine[part][j]=CF_ENGINES[e];cge_sound_factory[part][j]=f;found++;
            }
        if(found!=1){cf_message("Engine sound identity mismatch");return 0;}
    }
    for(unsigned j=0;j<2;j++){
        unsigned found=0;for(unsigned r=0;r<DM_N;r++)if(DM_RHY[r].id==CGE_GROOVES[j]){cge_rhythm[j]=r;found++;}
        if(found!=1 || DM_RHY[cge_rhythm[j]].beats!=4){cf_message("Engine groove ID mismatch");return 0;}
    }
    if(DM_RHY[cge_rhythm[0]].kit!=DMK_AIR){cf_message("DreamPop kit mismatch");return 0;}
    for(unsigned j=0;j<3;j++){
        unsigned found=0;for(unsigned f=0;f<FXP_FACTORY_N;f++)if(cb_streq(FXP_FACTORY[f].name,CGE_FX_NAMES[j])){cge_fx_id[j]=f;found++;}
        if(found!=1){cf_message("Engine FX identity mismatch");return 0;}
    }
    cge_resolved=1;return 1;
}
static int cge_prepare(cpa_plan_t *plan,cf_chord_cfg_t cfg,unsigned rhythm,unsigned mode)
{
    if(!cge.loaded || !cf_harmony_supported(cfg.scale,CF_Q_BASIC))return 0;
    memset(plan,0,sizeof *plan);plan->idea=CF_P_ENGINE;plan->n=8;plan->tonic=cfg.tonic;plan->scale=cfg.scale;
    plan->beats=4;plan->rhythm=cpa_compatible(rhythm,4);plan->mode=mode;
    if(plan->rhythm<DM_N){plan->kit=dm_kit(plan->rhythm);plan->swing=dm_swing(plan->rhythm);}
    int delta=((int)cfg.tonic-4+18)%12-6;
    for(unsigned i=0;i<8;i++){
        unsigned chord=i%4,variant=cge.request[0];cpa_slot_t *s=&plan->slot[i];
        unsigned degree=cge_view.degree[i];plan->degree[i]=degree;
        int root=CGE_NOTES[variant][chord].note[0]+delta+12*cfg.octave;
        if(cfg.scale==CF_SC_MAJOR){
            s->n=CGE_NOTES[variant][chord].n;
            for(unsigned j=0;j<s->n;j++){
                int note=CGE_NOTES[variant][chord].note[j]+delta+12*cfg.octave;
                if(note<0 || note>127)return 0;s->note[j]=note;s->mask|=1u<<(CGE_NOTES[variant][chord].note[j]-CGE_NOTES[variant][chord].note[0]);
            }
        }else{
            root+=cf_scale_step(cfg.scale,degree)-cf_scale_step(CF_SC_MAJOR,degree);
            cf_harmony_t h;cf_harmony_defaults(&h);uint32_t stamp[CF_H_SLOTS]={0};uint16_t used,conflict;
            s->mask=cf_harmony_mask(&h,cfg.scale,cfg.tonic,(root+120)%12,0,stamp,&used,&conflict);
            s->n=cf_voice(root,s->mask,0,s->note);
        }
        if(root<0 || root>127)return 0;
        s->root=root;s->ticks=768;s->gate=100;s->bass_pc=root%12;
        cf_chord_out_t o={0};o.sounding=1;o.root_note=root;o.root_pc=root%12;o.mask=s->mask;o.bass_pc=root%12;
        s->nb=cfp_bass_calc(&o,s->bass);plan->nb=s->nb;
    }
    return cpa_valid(plan);
}
static void cge_sound_prepare(unsigned index)
{
    for(unsigned p=0;p<2;p++){
        cf_stage_t prepared;unsigned e=cge_sound_engine[p][index],f=cge_sound_factory[p][index];
        cf_snd_build(p,e,pool_pos_factory(e,f),&prepared);
        prepared.pat=CF_P_ENGINE;prepared.own1=prepared.tw=prepared.taken=0;prepared.ready=1;
        fm1_irq_off();cf_stage[p]=prepared;fm1_irq_on();
    }
}
static int cge_sound_boundary(void)
{
    if(!cf_stage[0].ready || !cf_stage[1].ready || cf_stage[0].pat!=CF_P_ENGINE || cf_stage[1].pat!=CF_P_ENGINE)return 0;
    cf_stage_in(CF_P_ENGINE);cge.sound_taken=1;return 1;
}
static void cge_sound_poll(void)
{
    if(cge.sound_taken){
        cge.sound_taken=0;
        for(unsigned p=0;p<2;p++){
            cf_stage_t *s=&cf_stage[p];if(!s->taken)continue;
            s->taken=0;cf_snd[p]=s->meta;cf_snd_gen[p]++;cfk_own[p]=cfk_on[p]=0;cfk_gen[p]=cf_snd_gen[p];cu.edited[p]=0;
            cs.snd[p]=cf_snd_pos(p);
        }
        cge_seen_sound=cge_sound_hash();
    }
    if(cge.fx_taken){cge.fx_taken=0;cs.fx=cge.fx;cge_seen_fx=cfs_fnv(&cs.fx,sizeof cs.fx);}
}
static void cge_init(void)
{
    memset(&cge,0,sizeof cge);for(unsigned k=0;k<3;k++)cge.gain[k]=cge.target[k]=32768;
    cf_progression_engine_register(&cge_view,cge_source);
}
static void cge_load(void)
{
    if(cpa_running()){cf_message("Stop PLAY to load Engine");return;}
    if(cr_safe || cfs_restore_lock || sqk.on || cf_sn.sq_on || cf_sn.sq_rec!=255){cf_message("Stop recording/replay first");return;}
    if(!cge_resolve_assets())return;
    cge_init();cge.loaded=1;cge.energy=CGE_DEFAULTS[0];cge.movement=CGE_DEFAULTS[1];cge.variation=CGE_DEFAULTS[2];cge_gain_prepare();
    cs.chord.tonic=4;cs.chord.scale=CF_SC_MAJOR;cs.chord.quantize=CF_Q_BASIC;cs.chord.octave=0;
    cs.bpm=104;cs.beats=4;cs.bass_on=1;cpg.idea=CF_P_ENGINE;cpg.rhythm=cge_rhythm[0];cpg.mode=CPA_BOTH;cpg.repeat_off=0;
    for(unsigned p=0;p<2;p++){
        unsigned e=cge_sound_engine[p][0],f=cge_sound_factory[p][0];cf_pool_load(p,e,pool_pos_factory(e,f));cs.snd[p]=cf_snd_pos(p);
    }
    cs.fx=FXP_FACTORY[cge_fx_id[0]].fx;cf_fx_recall(&cs.fx);
    cf_send_config();cf_send_metro();cu_spell_update();cge_view_update();cge_remember_edits();cf_message("Heavenly Gaze / PLAY");
}
static void cge_request_apply(void)
{
    /* Freeze publication while foreground work prepares the complete latest request. */
    uint8_t old_pending,mask=0;for(unsigned i=0;i<4;i++)if(cge.index[i]!=cge.request[i])mask|=1u<<i;
    fm1_irq_off();old_pending=cge.pending;cge.pending=0;
    if(old_pending&CGE_HARMONY)cpa_edit.flags&=~CPA_NOTES;
    if(old_pending&CGE_GROOVE)cpa_edit.flags&=~CPA_GROOVE;
    fm1_irq_on();
    cpa_plan_t plan;cf_chord_cfg_t cfg=cs.chord;unsigned rhythm=cpg.rhythm,mode=cpg.mode;
    if(cge_active()){cpa_active_request(&plan);cfg.tonic=plan.tonic;cfg.scale=plan.scale;rhythm=plan.rhythm;mode=plan.mode;}
    if((mask|old_pending)&CGE_GROOVE)rhythm=cge_rhythm[cge.request[2]];
    if(!cge_prepare(&plan,cfg,rhythm,mode)){cf_message("Engine setup incompatible");return;}
    if(mask&CGE_SOUND)cge_sound_prepare(cge.request[1]);
    cf_fx_t fx=FXP_FACTORY[cge_fx_id[cge.request[3]]].fx;
    if(cge_active()){
        unsigned flags=0;if((mask|old_pending)&CGE_HARMONY)flags|=CPA_NOTES;if((mask|old_pending)&CGE_GROOVE)flags|=CPA_GROOVE;
        fm1_irq_off();
        if(flags){cpa_edit.plan=plan;cpa_edit.flags|=flags;}
        memcpy(cge.commit,cge.request,4);cge.fx=fx;cge.pending=mask;
        fm1_irq_on();
    }else{
        if(mask&CGE_SOUND){fm1_irq_off();cge_sound_boundary();fm1_irq_on();cge_sound_poll();}
        if(mask&CGE_FX){cs.fx=fx;cf_fx_recall(&cs.fx);}
        cpg.rhythm=rhythm;memcpy(cge.index,cge.request,4);memcpy(cge.commit,cge.request,4);cge.pending=0;
    }
    cge_seen_groove=cge_groove_hash();cge_view_update();
}

static void cge_regenerate(int undo)
{
    if(!cge_selected()){cf_message("SEL loads Heavenly Gaze");return;}
    if(cr_safe || cfs_restore_lock || sqk.on || cf_sn.sq_rec!=255){cf_message("Finish recording first");return;}
    if(undo){if(!cge.undo){cf_message("No Engine Undo");return;}memcpy(cge.request,cge.previous,4);cge.undo=0;}
    else {
        if(cge.locks==15){cf_message("Everything locked");return;}
        memcpy(cge.previous,cge.request,4);cge.undo=1;
        static const uint8_t counts[]={3,3,2,3};for(unsigned i=0;i<4;i++)if(!(cge.locks&(1u<<i)))cge.request[i]=(cge.request[i]+1u)%counts[i];
        cge.generation++;
    }
    cge_request_apply();cf_message(cge.pending?"Engine changes pending":"Engine combination ready");
}
static void cge_poll(void)
{
    cge_sound_poll();if(!cge_selected())return;
    if(cs.chord.tonic!=cge_seen_cfg.tonic || cs.chord.scale!=cge_seen_cfg.scale || cs.chord.octave!=cge_seen_cfg.octave){cge_direct(CGE_HARMONY);cge_seen_cfg=cs.chord;}
    cge_view_update();
    uint32_t sound=cge_sound_hash(),fx=cfs_fnv(&cs.fx,sizeof cs.fx),groove=cge_groove_hash();
    if(sound!=cge_seen_sound){cge_direct(CGE_SOUND);cge_seen_sound=sound;}
    if(fx!=cge_seen_fx){cge_direct(CGE_FX);cge_seen_fx=fx;}
    if(groove!=cge_seen_groove){cge_direct(CGE_GROOVE);cge_seen_groove=groove;}
}
static int cge_snapshot(cf_song_t *s)
{
    cpa_plan_t plan;if(cge_active())cpa_active_request(&plan);else if(!cge_prepare(&plan,cs.chord,cpg.rhythm,cpg.mode))return 0;
    memset(s,0,sizeof *s);str_cpy(s->name,"Heavenly Gaze snapshot",sizeof s->name);
    s->tonic=plan.tonic;s->scale=plan.scale;s->bpm=cs.bpm;s->beats=4;s->n=8;s->rhythm=plan.rhythm<DM_N?DM_RHY[plan.rhythm].id:65535;s->kit=plan.kit;s->swing=plan.swing;
    for(unsigned i=0;i<8;i++){
        cf_song_slot_t *v=&s->slot[i];const cpa_slot_t *p=&plan.slot[i];csg_slot(v);v->bars=2;v->degree=plan.degree[i];v->mask=p->mask;
        int base=60+plan.tonic-(plan.tonic>6?12:0)+cf_scale_step(plan.scale,v->degree)+12*(CF_OCT_BASE+cs.chord.octave);
        v->reg=(p->root-base)/12;
    }
    return cf_song_valid(s);
}
static void cge_knob_info(int k,cf_knob_t *o)
{
    static const char *const names[]={"ENERGY","MOVEMENT","VARIATION","REGEN/UNDO"};kfmt(o->name,12,names[k]);
    if(k<3)kint(o->val,k==0?cge.energy:k==1?cge.movement:cge.variation,0);
    else kfmt(o->val,14,cge.pending?"Pending":"+ / -");
    if(!cge.loaded && k<3)o->dis=1;
}
static void cge_knob_turn(int k,int d)
{
    if(!d)return;if(k==3){cge_regenerate(d<0);return;}if(!cge.loaded){cf_message("SEL loads Heavenly Gaze");return;}
    uint8_t *v=k==0?&cge.energy:k==1?&cge.movement:&cge.variation;*v=clampi(*v+d,0,100);
    if(k==0)cge_gain_prepare();
}
static const char *cge_stage_name(void)
{return cge.bar<4?"Intro":cge.bar<8?"Groove":cge.bar<12?"Lift":"Return";}

static void cge_view_update(void)
{
    if(!cge.loaded)return;
    cpa_plan_t plan;
    if(cge_active()){fm1_irq_off();plan=cpa.plan;fm1_irq_on();}
    else if(!cge_prepare(&plan,cs.chord,cpg.rhythm,cpg.mode))return;
    cge_view.tonic=plan.tonic;cge_view.scale=plan.scale;
    for(unsigned i=0;i<8;i++){cge_source[i].degree=plan.degree[i];cge_source[i].alter=0;cge_source[i].bass=-1;cge_source[i].mask=plan.slot[i].mask;}
}
