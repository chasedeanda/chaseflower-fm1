/* SPDX-License-Identifier: GPL-3.0-only
 * G1 foreground selection, preparation, history and live controls. No catalog
 * walk, PRNG draw, sound preparation, allocation or flash write in the ISR. */
static cf_progression_t cge_view={"RAKT","Engine / SAVE is a static snapshot",0,0,CF_SC_MAJOR,0,4,8,{0}};
static cf_source_chord_t cge_source[8];
static uint8_t cge_sound_engine[2][2][3],cge_sound_factory[2][2][3],cge_rhythm[2][4],cge_fx_id[2][3],cge_resolved;
static cf_chord_cfg_t cge_seen_cfg;
static uint32_t cge_seen_sound,cge_seen_fx,cge_seen_groove,cge_seen_generation;
static uint32_t cge_seed_override;
static uint8_t cge_seed_supplied;
static const char *cge_notice;
static uint8_t cge_engine_focus,cge_flow_focus,cge_pins_open,cge_preview;
static void cge_view_update(void),cge_cancel(void),cge_evolve(void);
static int cge_selected(void){return cge.loaded && (cge_active() || (cpg.idea==CF_P_ENGINE && !sqp.on && !sqk.on));}
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
    unsigned r=cge_active()?cpa.plan.rhythm:cpg.rhythm;
    unsigned kit=cge_active()?cpa.plan.kit:r<DM_N?dm_kit(r):0;
    unsigned swing=cge_active()?cpa.plan.swing:r<DM_N?dm_swing(r):0;
    return r | kit<<8 | swing<<16;
}
static void cge_remember_edits(void)
{cge_seen_cfg=cs.chord;cge_seen_sound=cge_sound_hash();cge_seen_fx=cfs_fnv(&cs.fx,sizeof cs.fx);cge_seen_groove=cge_groove_hash();cge_seen_generation=cge.generation;}
static void cge_direct(unsigned category)
{
    if(!cge.loaded){cge.initial_pins|=category;cge.locks|=category;return;}
    fm1_irq_off();cge.locks|=category;cge.pending&=~category;
    if(category&CGE_HARMONY)cge.next.id=cge.current.id;
    if(category&CGE_SOUND){cge.next.chord=cge.current.chord;cge.next.bass=cge.current.bass;
        for(unsigned p=0;p<2;p++)if(cf_stage[p].pat==CF_P_ENGINE)cf_stage[p].ready=0;}
    if(category&CGE_GROOVE)cge.next.groove=cge.current.groove;
    if(category&CGE_FX){cge.next.fx=cge.current.fx;cge.fx_taken=0;}
    if(category&CGE_FEEL)cge.next.feel=cge.feel;
    fm1_irq_on();
    if(category&CGE_SOUND)cge_sound_poll();
}
static void cge_cancel(void)
{
    fm1_irq_off();cge.pending=cge.direction_request!=cge.direction?CGE_DIRECTION:0;
    for(unsigned p=0;p<2;p++)if(cf_stage[p].pat==CF_P_ENGINE)cf_stage[p].ready=0;
    fm1_irq_on();
}
static void cge_key_edit(void)
{
    cge.key_pin=1;
    if(!cge.loaded)return;
    if((cge.pending&~CGE_DIRECTION) && cge.request_kind!=CGE_EVOLVE)cge.requeue=cge.request_kind;
    cge_cancel();cge.planned_cycle=UINT32_MAX;
}
static void cge_gain_prepare(void)
{
    uint16_t next[2][4][3];unsigned e=cge.energy;
    for(unsigned v=0;v<2;v++)for(unsigned p=0;p<4;p++)for(unsigned k=0;k<3;k++){
        unsigned base=CGE_VIBES[v].gain[p][k]*32768u/100u,anchor=CGE_VIBES[v].defaults[0],gain;
        /* Linear segments retain G0's 60/40 chord bed, hit the authored gains
         * exactly at the Vibe default, and keep the upper dial responsive. */
        if(e<=anchor)gain=k?base*e/anchor:base*(3u*anchor+2u*e)/(5u*anchor);
        else gain=base+(32768u-base)*(e-anchor)/(100u-anchor);
        next[v][p][k]=(uint16_t)gain;
    }
    fm1_irq_off();memcpy(cge.profile,next,sizeof next);fm1_irq_on();
}
static int cge_resolve_assets(void)
{
    if(cge_resolved)return 1;
    for(unsigned v=0;v<2;v++){
        for(unsigned part=0;part<2;part++)for(unsigned j=0;j<3;j++){
            const char *name=part?CGE_VIBES[v].bass[j]:CGE_VIBES[v].chord[j];unsigned found=0;
            for(unsigned e=0;e<CF_NENG;e++)for(unsigned f=0;f<ENGINES[CF_ENGINES[e]]->npresets;f++)
                if(cb_streq(ENGINES[CF_ENGINES[e]]->presets[f].name,name)){
                    cge_sound_engine[v][part][j]=CF_ENGINES[e];cge_sound_factory[v][part][j]=f;found++;}
            if(found!=1){cf_message("Engine sound identity mismatch");return 0;}
        }
        for(unsigned j=0;j<4;j++){
            unsigned found=0;for(unsigned r=0;r<DM_N;r++)if(DM_RHY[r].id==CGE_VIBES[v].groove[j]){cge_rhythm[v][j]=r;found++;}
            if(found!=1 || DM_RHY[cge_rhythm[v][j]].beats!=4){cf_message("Engine groove ID mismatch");return 0;}
        }
        for(unsigned j=0;j<3;j++){
            unsigned found=0;for(unsigned f=0;f<FXP_FACTORY_N;f++)if(cb_streq(FXP_FACTORY[f].name,CGE_VIBES[v].fx[j])){cge_fx_id[v][j]=f;found++;}
            if(found!=1){cf_message("Engine FX identity mismatch");return 0;}
        }
    }
    cge_resolved=1;return 1;
}
static int cge_find(uint16_t id)
{for(unsigned i=0;i<cf_progression_count();i++)if(cf_progression_get(i)->id==id)return (int)i;return -1;}
static int cge_build(cpa_plan_t *plan,uint16_t id,cf_chord_cfg_t cfg,unsigned rhythm,unsigned mode)
{
    int i=cge_find(id);if(i<0)return 0;
    const cf_progression_t *p=cf_progression_get(i);
    if((p->n!=3 && p->n!=4) || p->beats!=4 || !cpa_prepare_full(plan,i,cfg,rhythm,mode,0,0))return 0;
    /* User-approved three-chord expansion: 1,2,3,3 in each half-cycle. */
    if(p->n==3){plan->slot[3]=plan->slot[2];plan->degree[3]=plan->degree[2];}
    for(unsigned j=0;j<8;j++){if(j>=4){plan->slot[j]=plan->slot[j-4];plan->degree[j]=plan->degree[j-4];}plan->slot[j].ticks=768;}
    plan->n=8;plan->idea=CF_P_ENGINE;return cpa_valid(plan);
}
static int cge_prepare(cpa_plan_t *plan,cf_chord_cfg_t cfg,unsigned rhythm,unsigned mode)
{return cge.loaded && cs.beats==4 && cge_build(plan,cge.current.id,cfg,rhythm,mode);}
static uint32_t cge_random(unsigned stream,unsigned bound)
{
    uint32_t x=cge.rng[stream];if(!x)x=0x9E3779B9u;
    x^=x<<13;x^=x>>17;x^=x<<5;cge.rng[stream]=x;return bound?x%bound:0;
}
static void cge_seed(uint32_t seed)
{cge_seed_override=seed?seed:0x9E3779B9u;cge_seed_supplied=1;}
static void cge_streams(uint32_t seed)
{
    static const uint32_t salts[CGE_NRNG]={0xA341316Cu,0xC8013EA4u,0xAD90777Du,0x7E95761Eu,0x9E3779B9u,0xD1B54A35u};
    cge.session_seed=seed?seed:0x9E3779B9u;
    for(unsigned i=0;i<CGE_NRNG;i++)cge.rng[i]=cge.session_seed^salts[i];
}
enum { CGE_SKIP_METER,CGE_SKIP_SHAPE,CGE_SKIP_SCALE,CGE_SKIP_WEIGHT,CGE_SKIP_PLAN,CGE_ACCEPT,CGE_NREASONS };
static unsigned cge_eligible(unsigned index,unsigned vibe,cf_chord_cfg_t cfg,unsigned *reason)
{
    const cf_progression_t *p=cf_progression_get(index);unsigned weight=0;
    if(p->beats!=4){*reason=CGE_SKIP_METER;return 0;}
    if(p->n!=3 && p->n!=4){*reason=CGE_SKIP_SHAPE;return 0;}
    if(p->scale!=cfg.scale){*reason=CGE_SKIP_SCALE;return 0;}
    for(unsigned t=0;t<8;t++)if(p->tags&(1u<<t))weight+=CGE_VIBES[vibe].weight[t];
    if(!weight){*reason=CGE_SKIP_WEIGHT;return 0;}
    cpa_plan_t plan;if(!cge_build(&plan,p->id,cfg,cge_rhythm[vibe][0],CPA_BOTH)){*reason=CGE_SKIP_PLAN;return 0;}
    *reason=CGE_ACCEPT;return weight;
}
static int cge_choose(unsigned vibe,cf_chord_cfg_t cfg,uint16_t exclude,uint16_t *id,unsigned counts[CGE_NREASONS])
{
    unsigned total=0,other=0;
    if(counts)memset(counts,0,CGE_NREASONS*sizeof *counts);
    for(unsigned i=0;i<cf_progression_count();i++){
        unsigned reason,w=cge_eligible(i,vibe,cfg,&reason);if(counts)counts[reason]++;
        total+=w;if(cf_progression_get(i)->id!=exclude)other+=w;
    }
    if(!total)return 0;
    unsigned draw=cge_random(CGE_R_HARMONY,other?other:total);
    for(unsigned i=0;i<cf_progression_count();i++){
        unsigned reason,w=cge_eligible(i,vibe,cfg,&reason);const cf_progression_t *p=cf_progression_get(i);
        if(other && p->id==exclude)continue;
        if(draw<w){*id=p->id;return 1;}draw-=w;
    }
    return 0;
}
static unsigned cge_other(unsigned stream,unsigned count,unsigned current)
{return count>1?(current+1u+cge_random(stream,count-1u))%count:current;}
static void cge_sound_prepare(const cge_take_t *take)
{
    for(unsigned p=0;p<2;p++){
        cf_stage_t prepared;unsigned j=p?take->bass:take->chord;
        unsigned e=cge_sound_engine[take->vibe][p][j],f=cge_sound_factory[take->vibe][p][j];
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

static cf_chord_cfg_t cge_config(unsigned *rhythm,unsigned *mode)
{
    cf_chord_cfg_t cfg=cs.chord;*rhythm=cpg.rhythm;*mode=cpg.mode;
    if(cge_active()){cpa_plan_t p;cpa_active_request(&p);cfg.tonic=p.tonic;cfg.scale=p.scale;*rhythm=p.rhythm;*mode=p.mode;}
    return cfg;
}
static int cge_queue(cge_take_t take,unsigned kind)
{
    cge_sound_poll();unsigned rhythm,mode;cf_chord_cfg_t cfg=cge_config(&rhythm,&mode);
    if(kind!=CGE_PICK && (cge.locks&CGE_HARMONY))take.id=cge.current.id;
    if(cge.locks&CGE_SOUND){take.chord=cge.current.chord;take.bass=cge.current.bass;}
    if(cge.locks&CGE_GROOVE)take.groove=cge.current.groove;else rhythm=cge_rhythm[take.vibe][take.groove];
    if(cge.locks&CGE_FX)take.fx=cge.current.fx;
    if(cge.locks&CGE_FEEL)take.feel=cge.feel;
    cpa_plan_t plan;
    if(!cge_build(&plan,take.id,cfg,rhythm,mode)){
        if(kind!=CGE_EVOLVE)cge_notice="No compatible ideas";
        return 0;
    }
    unsigned flags=0,vibe=take.vibe!=cge.current.vibe;
    if(take.id!=cge.current.id)flags|=CGE_HARMONY;
    if(!(cge.locks&CGE_SOUND) && (vibe || take.chord!=cge.current.chord || take.bass!=cge.current.bass))flags|=CGE_SOUND;
    if(!(cge.locks&CGE_GROOVE) && (vibe || take.groove!=cge.current.groove))flags|=CGE_GROOVE;
    if(!(cge.locks&CGE_FX) && (vibe || take.fx!=cge.current.fx))flags|=CGE_FX;
    if(take.feel!=cge.feel)flags|=CGE_FEEL;
    if(vibe)flags|=CGE_VIBE;
    if(cge.direction_request!=cge.direction)flags|=CGE_DIRECTION;
    cge_cancel();
    if(!flags){
        if(kind==CGE_BACK)cge_notice="Earlier take matches this one";
        else if(kind==CGE_PICK)cge_notice="This progression is playing";
        else if(kind==CGE_FRESH)cge_notice=(cge.locks&15)==15?"Fresh Take blocked by pins":"Same take drawn / turn K4 again";
        return 0;
    }
    if(flags&CGE_SOUND)cge_sound_prepare(&take);
    cf_fx_t fx=FXP_FACTORY[cge_fx_id[take.vibe][take.fx]].fx;
    fm1_irq_off();cge.next=take;cge_prepared_plan=plan;cge.fx=fx;cge.pending=flags;cge.request_kind=kind;
    cge.request_ms=cf_now_ms; /* telemetry only */
    fm1_irq_on();
    if(!cge_active()){
        if(flags&~CGE_DIRECTION){cge.previous=cge.current;cge.undo=1;}
        if(flags&CGE_SOUND){fm1_irq_off();cge_sound_boundary();fm1_irq_on();cge_sound_poll();}
        if(flags&CGE_FX){cs.fx=fx;cf_fx_recall(&cs.fx);}
        cge.current=take;cge.feel=take.feel;cge.direction=cge.direction_request;
        cpg.rhythm=rhythm;cge.last_change=cge_change(flags);cge.pending=0;cge.generation++;cge.commit_ms=cf_now_ms;
        cge_remember_edits();cge_view_update();
    }
    return 1;
}
static int cge_generate(unsigned vibe)
{
    if(cr_safe || cfs_restore_lock || sqk.on || cf_sn.sq_on || cf_sn.sq_rec!=255){cge_notice="Finish recording/replay first";return 0;}
    if(cpa_running() && !cge_active()){cge_notice="Press PLAY to stop first";return 0;}
    if((cge.locks&15)==15 && vibe==cge.current.vibe){cge_notice="Fresh Take blocked by pins";return 0;}
    unsigned rhythm,mode;cf_chord_cfg_t cfg=cge_config(&rhythm,&mode);cge_take_t take=cge.current;take.vibe=vibe;
    if(!(cge.locks&CGE_HARMONY) && !cge_choose(vibe,cfg,cge.current.id,&take.id,0)){cge_notice="No compatible ideas";return 0;}
    if(!(cge.locks&CGE_GROOVE))take.groove=cge_random(CGE_R_GROOVE,4);
    if(!(cge.locks&CGE_SOUND)){take.chord=cge_random(CGE_R_CHORD,3);take.bass=cge_random(CGE_R_BASS,3);}
    if(!(cge.locks&CGE_FX))take.fx=cge_random(CGE_R_FX,3);
    if(!(cge.locks&CGE_FEEL) && vibe!=cge.current.vibe)take.feel=CGE_VIBES[vibe].defaults[1];
    cge.vibe_request=vibe;return cge_queue(take,CGE_FRESH);
}
static void cge_init(void)
{
    memset(&cge,0,sizeof cge);for(unsigned k=0;k<3;k++)cge.gain[k]=cge.target[k]=32768;
    cge_notice=0;cge_pins_open=0;cge_engine_focus=cge_flow_focus=cge_preview=255;
    cge.energy=CGE_VIBES[0].defaults[0];cge.feel=CGE_VIBES[0].defaults[1];cge.space=CGE_VIBES[0].defaults[2];
    cge.direction=cge.direction_request=CGE_JAM;cge.surprise=CGE_VIBES[0].defaults[3];
    cge.planned_cycle=UINT32_MAX;cf_progression_engine_register(&cge_view,cge_source);
}
static void cge_activate(void)
{
    const cge_take_t *take=&cge.current;unsigned v=take->vibe;
    cpg.idea=CF_P_ENGINE;cpg.repeat_off=0;
    if(!(cge.locks&CGE_GROOVE))cpg.rhythm=cge_rhythm[v][take->groove];
    if(!(cge.locks&CGE_SOUND)){
        cge_sound_prepare(take);
        fm1_irq_off();cge_sound_boundary();fm1_irq_on();cge_sound_poll();
    }
    if(!(cge.locks&CGE_FX)){cs.fx=FXP_FACTORY[cge_fx_id[v][take->fx]].fx;cf_fx_recall(&cs.fx);}
    cf_send_config();cf_send_metro();cu_spell_update();cge_view_update();cge_remember_edits();
}
static int cge_load(void)
{
    cge_notice=0;
    if(cpa_running()){cge_notice="Press PLAY to stop first";return 0;}
    if(cr_safe || cfs_restore_lock || sqk.on || cf_sn.sq_on || cf_sn.sq_rec!=255){cge_notice="Finish recording/replay first";return 0;}
    if(cs.beats!=4){cge_notice="Choose 4/4 in Core";return 0;}
    if(!cge_resolve_assets())return 0;
    if(cge.loaded){
        if(cpg.idea!=CF_P_ENGINE)cge_activate();
        return cge.vibe_request==cge.current.vibe || cge_generate(cge.vibe_request);
    }
    unsigned vibe=cge.vibe_request;cf_set_t defaults;cs_defaults(&defaults);cf_chord_cfg_t cfg=cs.chord;unsigned bpm=cs.bpm;
    if(!cfs_loaded && !cge.key_pin && cfg.tonic==defaults.chord.tonic && cfg.scale==defaults.chord.scale && vibe==0){cfg.tonic=4;cfg.scale=CF_SC_MAJOR;}
    if(!cfs_loaded && !cge.tempo_pin && bpm==defaults.bpm)bpm=CGE_VIBES[vibe].bpm;
    cge_streams(cge_seed_supplied?cge_seed_override:cf_now_ms^0x434F5244u); /* establish session seed once */
    uint16_t id;if(!cge_choose(vibe,cfg,0,&id,0)){cge_notice="No compatible ideas";return 0;}
    cs.chord=cfg;cs.bpm=bpm;
    cge.loaded=1;cge.energy=CGE_VIBES[vibe].defaults[0];cge.feel=CGE_VIBES[vibe].defaults[1];
    cge.space=CGE_VIBES[vibe].defaults[2];if(!(cge.flow_manual&2))cge.surprise=CGE_VIBES[vibe].defaults[3];cge.direction=cge.direction_request;
    cge.current=(cge_take_t){id,vibe,0,0,0,0,cge.feel};cge.next=cge.current;cge.locks|=cge.initial_pins;
    if(cfs_loaded)cge.locks|=CGE_SOUND|CGE_GROOVE|CGE_FX;
    cpg.mode=CPA_BOTH;cge_gain_prepare();cge_activate();return 1;
}
static const char *cge_progression_name(unsigned id)
{
    int index=cge_find(id);
    return index<0?"Progression":cf_progression_get(index)->name;
}

static int cge_choose_progression(unsigned index)
{
    cge_notice=0;
    if(index>=cf_progression_count())return 0;
    if(cr_safe || cfs_restore_lock || sqk.on || cf_sn.sq_on || cf_sn.sq_rec!=255){
        cge_notice="Finish recording/replay first";return 0;
    }
    if(!cge_selected() && !cge_load())return 0;
    const cf_progression_t *record=cf_progression_get(index);
    unsigned rhythm,mode;
    cf_chord_cfg_t cfg=cge_config(&rhythm,&mode);
    if(record->scale!=cfg.scale){cge_notice="Choose a progression in this scale";return 0;}
    cge_take_t take=cge.current;
    take.id=record->id;
    cge.vibe_request=take.vibe;
    return cge_queue(take,CGE_PICK);
}

static void cge_preview_turn(int d)
{
    if(!d)return;
    cge_notice=0;
    unsigned rhythm,mode;
    cf_chord_cfg_t cfg=cge_config(&rhythm,&mode);
    int count=cf_progression_count(),step=d<0?-1:1;
    int index=cge_preview<count?cge_preview:cge_find(cge.current.id);
    for(int i=0;i<count;i++) {
        index=index<0?(step>0?0:count-1):wrapi(index+step,count);
        const cf_progression_t *p=cf_progression_get(index);
        if(p->scale!=cfg.scale || p->id==cge.current.id)continue;
        cpa_plan_t plan;
        if(cge_build(&plan,p->id,cfg,rhythm,mode)){cge_preview=index;return;}
    }
    cge_notice="No other compatible progressions";
}

static int cge_preview_apply(void)
{
    if(cge_preview>=cf_progression_count())return 0;
    unsigned index=cge_preview;
    cge_preview=255;
    cge_choose_progression(index);
    return 1;
}

static void cge_regenerate(int undo)
{
    cge_notice=0;cge_preview=255;
    if(!cge_selected()){cge_notice="Press PLAY to start Engine";return;}
    if(cr_safe || cfs_restore_lock || sqk.on || cf_sn.sq_on || cf_sn.sq_rec!=255){cge_notice="Finish recording/replay first";return;}
    cge_sound_poll();int ok;
    if(undo){if(!cge.undo){cge_notice="No earlier take yet";return;}ok=cge_queue(cge.previous,CGE_BACK);if(ok)cge.vibe_request=cge.next.vibe;}
    else cge_generate(cge.vibe_request);
}
static unsigned cge_jam_choices(void)
{
    unsigned choices=(CGE_GROOVE|CGE_FEEL|CGE_FX)&~cge.locks;
    if(cge.arp_request==CGE_ARP_ON)choices&=~CGE_FEEL;
    return choices;
}
static void cge_evolve(void)
{
    if(!cge_active() || cge.planned_cycle==cge.cycle)return;
    cge.planned_cycle=cge.cycle;
    unsigned next=cge.cycle+1;
    if(cge.pending || cge.direction_request!=CGE_JAM || !cge.surprise || (cge.surprise<50 && (next&1)))return;
    unsigned choices=cge_jam_choices(),legal[3],n=0;
    if(choices&CGE_GROOVE)legal[n++]=CGE_GROOVE;
    if(choices&CGE_FEEL)legal[n++]=CGE_FEEL;
    if(choices&CGE_FX)legal[n++]=CGE_FX;
    if(!n)return;
    unsigned category=legal[(next-1u)%n];cge_take_t take=cge.current;
    if(category==CGE_GROOVE)take.groove=cge_other(CGE_R_GROOVE,4,take.groove);
    else if(category==CGE_FX)take.fx=cge_other(CGE_R_FX,3,take.fx);
    else {unsigned choices[3]={0,CGE_VIBES[take.vibe].defaults[1],100},count=cge.arp_request==CGE_ARP_OFF?2:3;
        unsigned at=cge.feel<25?0:cge.feel<65?1:count==2?1:2;
        take.feel=choices[cge_other(CGE_R_FEEL,count,at)];}
    cge_queue(take,CGE_EVOLVE);
}
static void cge_poll(void)
{
    cge_sound_poll();if(!cge_selected())return;
    if(cge_seen_generation!=cge.generation)cge_remember_edits();
    if(cs.chord.tonic!=cge_seen_cfg.tonic || cs.chord.scale!=cge_seen_cfg.scale || cs.chord.octave!=cge_seen_cfg.octave)cge_seen_cfg=cs.chord;
    uint32_t sound=cge_sound_hash(),fx=cfs_fnv(&cs.fx,sizeof cs.fx),groove=cge_groove_hash();
    if(sound!=cge_seen_sound){cge_direct(CGE_SOUND);cge_seen_sound=sound;}
    if(fx!=cge_seen_fx){cge_direct(CGE_FX);cge_seen_fx=fx;}
    if(groove!=cge_seen_groove){cge_direct(CGE_GROOVE);cge_seen_groove=groove;}
    cge_view_update();
    if(cge.requeue){
        unsigned kind=cge.requeue;cge.requeue=0;
        int ok;
        if(kind==CGE_PICK){
            int index=cge_find(cge.next.id);
            ok=index>=0 && cge_choose_progression(index);
        }else if(kind==CGE_BACK)ok=cge_queue(cge.previous,CGE_BACK);
        else ok=cge_generate(cge.vibe_request);
        if(!ok)cge.vibe_request=cge.current.vibe;
    }
    cge_evolve();
}
static int cge_snapshot(cf_song_t *s)
{
    cpa_plan_t plan;if(cge_active())cpa_active_request(&plan);else if(!cge_prepare(&plan,cs.chord,cpg.rhythm,cpg.mode))return 0;
    const cf_progression_t *source=cf_progression_get(cge_find(cge.current.id));
    if(!source || !cf_song_clone(s,source))return 0;
    s->source_id=cge.current.id;s->tonic=plan.tonic;s->scale=plan.scale;s->bpm=cs.bpm;s->beats=4;s->n=8;
    s->rhythm=plan.rhythm<DM_N?DM_RHY[plan.rhythm].id:65535;s->kit=plan.kit;s->swing=plan.swing;
    for(unsigned i=0;i<8;i++){
        if(i==3 && source->n==3)s->slot[i]=s->slot[2];
        if(i>=4)s->slot[i]=s->slot[i-4];cf_song_slot_t *v=&s->slot[i];const cpa_slot_t *p=&plan.slot[i];
        v->bars=2;v->mask=p->mask;v->degree=plan.degree[i];v->bass=p->bass_pc==p->root%12?-1:(p->bass_pc-p->root%12+12)%12;
        /* Preserve source slash/inversion voicing in the existing Song fields. */
        int base=60+plan.tonic-(plan.tonic>6?12:0)+cf_scale_step(plan.scale,v->degree)+v->alter+12*(cs.chord.octave+CF_OCT_BASE),found=0;
        int8_t tones[12];unsigned count=cf_mask_tones(p->mask,tones);
        for(int reg=-2;reg<=2 && !found;reg++)for(unsigned inv=0;inv<4 && inv<count;inv++){
            uint8_t notes[CF_MAX_NOTES];int root=base+12*reg;if(root<0 || root>127)continue;
            unsigned n=cf_voice(root,p->mask,inv,notes);
            if(n==p->n && !memcmp(notes,p->note,n)){v->reg=reg;v->inversion=inv;found=1;break;}
        }
        if(!found)return 0;
    }
    return cf_song_valid(s);
}
static void cge_knob_info(int k,cf_knob_t *o)
{
    static const char *const names[]={"ENERGY","FEEL","SPACE","FRESH TAKE"};kfmt(o->name,12,names[k]);
    if(k<3)kint(o->val,k==0?cge.energy:k==1?cge.feel:cge.space,"%");
    else kfmt(o->val,14,"New / Back");
    if(!cge.loaded && k<3)o->dis=1;
}
static void cge_knob_turn(int k,int d)
{
    if(!d)return;
    cge_engine_focus=k;cge_notice=0;
    if(k==3){cge_regenerate(d<0);return;}
    if(!cge_selected()){cge_notice="Press PLAY to start Engine";return;}
    if(k==1)cge_direct(CGE_FEEL);
    uint8_t *v=k==0?&cge.energy:k==1?&cge.feel:&cge.space;*v=clampi(*v+d,0,100);
    if(k==0)cge_gain_prepare();
}
static const char *const CGE_ARP_NAMES[]={"Auto","On","Off"};
static void cge_play_it(unsigned mode)
{
    if(mode==cge.arp_request)return;
    cge.arp_request=mode;
    if(mode==CGE_ARP_ON && cge.request_kind==CGE_EVOLVE && (cge.pending&CGE_FEEL))cge_cancel();
    cge.planned_cycle=UINT32_MAX;
}
static int cge_arp_pending(void)
{return cge_active() && (cge.arp_request!=cge.arp_applied || cge.feel!=cge.feel_applied ||
    (cge.arp_request==CGE_ARP_ON && memcmp(&cs.arp,&cge.manual_arp,sizeof cs.arp)));}
static const char *cge_articulation_name(void)
{return cge.arp?"ARP":cge.feel_applied>=25?"Gated":"Held";}
static const char *cge_stage_name(void)
{return cge.bar<4?"Intro":cge.bar<8?"Foundation":cge.bar<12?"Lift":"Breathe";}
static void cge_view_update(void)
{
    if(!cge.loaded)return;int at=cge_find(cge.current.id);if(at<0)return;
    const cf_progression_t *record=cf_progression_get(at);cpa_plan_t plan;
    if(cge_active()){fm1_irq_off();plan=cpa.plan;fm1_irq_on();}
    else if(!cge_prepare(&plan,cs.chord,cpg.rhythm,cpg.mode))return;
    cge_view=*record;cge_view.n=8;cge_view.tonic=plan.tonic;cge_view.scale=plan.scale;
    const cf_source_chord_t *source=cf_progression_source(record);
    for(unsigned i=0;i<8;i++){
        unsigned slot=i%4;if(record->n==3 && slot==3)slot=2;
        cge_view.degree[i]=plan.degree[i];cge_source[i]=source?source[slot]:(cf_source_chord_t){0};
        cge_source[i].degree=plan.degree[i];cge_source[i].mask=plan.slot[i].mask;
        cge_source[i].bass=plan.slot[i].bass_pc==plan.slot[i].root%12?-1:(plan.slot[i].bass_pc-plan.slot[i].root%12+12)%12;
    }
}
static const char *const CGE_DIRECTION_NAME[]={"Repeat","Jam"};
static const char *const CGE_PIN_NAME[]={"Harmony","Sound","Groove","FX","Feel"};
static void cge_pin_move(int d)
{
    if(d)cge.pin=wrapi(cge.pin+(d>0?1:-1),5);
}
static void cge_flow_info(int k,cf_knob_t *o)
{
    static const char *const names[]={"VIBE","DIRECTION","SURPRISE","PIN"};kfmt(o->name,12,names[k]);
    if(k==0)kfmt(o->val,14,CGE_VIBES[cge.vibe_request].name);
    else if(k==1)kfmt(o->val,14,CGE_DIRECTION_NAME[cge.direction_request]);
    else if(k==2)kint(o->val,cge.surprise,"%");
    else {
        unsigned count=0;
        for(unsigned i=0;i<5;i++)if(cge.locks&(1u<<i))count++;
        kint(o->val,count," / 5");
    }
}
static void cge_flow_turn(int k,int d)
{
    if(!d)return;
    if(cge_pins_open){if(k==3)cge_pin_move(d);return;}
    cge_flow_focus=k;cge_notice=0;
    if(k==0){unsigned v=clampi(cge.vibe_request+(d>0?1:-1),0,1);if(v==cge.vibe_request)return;cge.vibe_request=v;
        if(cge.loaded){if(!cge_generate(v))cge.vibe_request=cge.current.vibe;}
        else {cge.energy=CGE_VIBES[v].defaults[0];cge.feel=CGE_VIBES[v].defaults[1];cge.space=CGE_VIBES[v].defaults[2];if(!(cge.flow_manual&2))cge.surprise=CGE_VIBES[v].defaults[3];}}
    else if(k==1){cge.flow_manual|=1;unsigned direction=d>0?CGE_JAM:CGE_REPEAT;if(direction==cge.direction_request)return;cge.direction_request=direction;
        if(cge.pending && cge.request_kind==CGE_EVOLVE)cge_cancel();
        if(cge_active()){if(cge.direction_request!=cge.direction)cge.pending|=CGE_DIRECTION;else cge.pending&=~CGE_DIRECTION;}
        else cge.direction=cge.direction_request;cge.planned_cycle=UINT32_MAX;}
    else if(k==2){cge.flow_manual|=2;unsigned surprise=clampi(cge.surprise+d,0,100);if(surprise==cge.surprise)return;cge.surprise=surprise;if(cge.pending && cge.request_kind==CGE_EVOLVE)cge_cancel();cge.planned_cycle=UINT32_MAX;}
    else cge_pins_open=1;
}
static void cge_pin_toggle(void)
{
    cge_flow_focus=3;cge_notice=0;
    unsigned bit=1u<<cge.pin;
    if(cge.locks&bit){cge.locks&=~bit;cge.initial_pins&=~bit;}else cge_direct(bit);
    cge.planned_cycle=UINT32_MAX;
}
static void cge_pin_select(void)
{
    if(cge_pins_open)cge_pin_toggle();
    else {cge_pins_open=1;cge_flow_focus=3;cge_notice=0;}
}
