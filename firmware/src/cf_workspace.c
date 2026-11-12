/* SPDX-License-Identifier: GPL-3.0-only
 * Studio workspaces own navigation. The audio engine and stored formats are shared. */
enum { CGU_STUDIO, CGU_SOUND, CGU_RECORD, CGU_HELP, CGU_NTAB };
enum { CGU_BAND, CGU_BAND_HARMONY, CGU_BAND_SHAPING, CGU_BAND_GENRE, CGU_BAND_ARP, CGU_BAND_PANELS };
enum { CGU_TONE, CGU_ENVELOPE, CGU_SENDS, CGU_PRESETS, CGU_SOUND_PAGES };
enum { CGU_ARP_OFF, CGU_ARP_KEYS, CGU_ARP_BAND };
static const char *const CGU_SOUND_NAMES[]={"Tone","Envelope","Sends","FX Presets"};
static const char *const CGU_ARP_NAMES[]={"Off","Keys","Band"};
enum { CGU_START, CGU_HARMONY, CGU_PLAYING, CGU_CAPTURE };
static const char *const CGU_TABS[]={"Studio","Sound","Record","Help"};
static const char *const CGU_BAND_NAMES[]={"Harmony","Shaping","Genre","ARP"};
static const char *const CGU_TOPICS[]={"Start here","Harmony","Playing","Capture"};
static struct {
    uint32_t zoom_ms, turn_ms, pad_down, clear_down;
    uint8_t turn_knob, turn_dir;
    uint32_t sound_seed, action_ms;
    uint8_t tab, topic, zoom_from, action, mixer, all_sounds, band_panel, part_harmony;
    uint8_t sound_page, arp_timing, key_arp, fx_return, take_slot;
} cgu={.sound_seed=0x436f7264u};

static void cf_arp_controls_info(int k,cf_knob_t *o,int timing,int studio);
static void cf_arp_controls_turn(int k,int d,int timing,int studio);

static int cgu_play_part(void)
{
    if(!CF_STUDIO || CF_SCENES || sqk.on || cf_sn.sq_rec!=255)return -1;
    return cgs_home() || cgu.fx_return ? cgs.focus : -1;
}
static int cgu_keys_arp(void)
{ return (cgu.key_arp>>cgs.focus)&1u; }
static unsigned cgu_arp_mode(void)
{
    unsigned bit=1u<<cgs.focus;
    return cgs.arp&bit?CGU_ARP_BAND:cgu.key_arp&bit?CGU_ARP_KEYS:CGU_ARP_OFF;
}
static void cgu_arp_turn(int d)
{
    unsigned p=cgs.focus;
    if(p!=CGS_CHORDS && p!=CGS_LAYER)return;
    unsigned bit=1u<<p;
    unsigned mode=(unsigned)clampi((int)cgu_arp_mode()+d,0,CGU_ARP_BAND);
    cgu.key_arp=(uint8_t)((cgu.key_arp&~bit)|(mode==CGU_ARP_KEYS?bit:0));
    cgs.arp=(uint8_t)((cgs.arp&~bit)|(mode==CGU_ARP_BAND?bit:0));
}
static void cgu_sound_page(unsigned page)
{
    cgu.sound_page=(uint8_t)(cgs.focus==CGS_DRUMS && page==CGU_ENVELOPE?CGU_SENDS:page);
    if(cgu.sound_page==CGU_PRESETS){
        cfx_sync();fxpu.mode=FXP_BROWSE;cfxu.view=CFX_MACROS;
    }
    cgu.turn_ms=0;
}

static unsigned cgu_sound_page_count(unsigned part)
{ return part==CGS_DRUMS?CGU_SOUND_PAGES-1u:CGU_SOUND_PAGES; }

static unsigned cgu_sound_page_rank(unsigned part,unsigned page)
{ return page-(part==CGS_DRUMS && page>CGU_ENVELOPE); }

static void cgu_sound_page_turn(int d)
{
    int count=(int)cgu_sound_page_count(cgs.focus);
    int rank=(int)cgu_sound_page_rank(cgs.focus,cgu.sound_page);
    unsigned next=(unsigned)wrapi(rank+d%count,count);
    cgu_sound_page(next+(cgs.focus==CGS_DRUMS && next>=CGU_ENVELOPE));
}

static unsigned cgu_zoom(void)
{
    unsigned target=cgs.zoom?100u:0u;
    unsigned from=cgu.zoom_from;
    unsigned distance=target>from?target-from:from-target;
    int remain=cf_tween_offset(cgu.zoom_ms,180,distance);
    return cgs.zoom?100u-(unsigned)remain:(unsigned)remain;
}

static void cgu_set_zoom(int detail)
{
    if(cgs.zoom==detail)return;
    cgu.turn_ms=0;
    if(cgu.tab==CGU_STUDIO)cgu.part_harmony=0;
    cgu.zoom_from=(uint8_t)cgu_zoom();
    cgu.zoom_ms=cf_motion?cu.now:0;
    cgs.zoom=(uint8_t)detail;
}

static void cgu_open_arp(void)
{
    if(cgu.tab==CGU_STUDIO && cgu.band_panel==CGU_BAND_ARP && cgs.zoom){cgu_set_zoom(0);return;}
    cgu.tab=CGU_STUDIO;
    cgu.band_panel=CGU_BAND_ARP;
    cgu.arp_timing=0;
    cgs.zoom=0;
    cgu.zoom_ms=0;
    cgu_set_zoom(1);
}

static void cgu_mixer_open(int on)
{
    cgu.mixer=(uint8_t)on;
    cgs.held_mute=cgs.held_solo=0;
    cgu.turn_ms=0;
    cu.pop=POP_NONE;
}
static void cgu_home(void)
{
    cu.pop=POP_NONE;
    cgu_mixer_open(0);
    cgu.action=cgu.band_panel=cgu.part_harmony=0;
    cgu.turn_ms=0;
    cgu.sound_page=CGU_TONE;
    if(cgu.tab==CGU_STUDIO)cgu_set_zoom(0);
    else {
        cgu.tab=CGU_STUDIO;
        cgs.zoom=0;
        cgu.zoom_ms=0;
    }
}

static void cgu_tab_turn(int d)
{
    unsigned tab=(unsigned)wrapi(cgu.tab+d%CGU_NTAB,CGU_NTAB);
    if(tab==cgu.tab || cgu.mixer)return;
    unsigned previous=cgu.tab;
    cgu.tab=(uint8_t)tab;
    cgu.turn_ms=0;
    cgu.sound_page=CGU_TONE;
    cgu.part_harmony=0;
    if(cgu.band_panel)cgs.zoom=0;
    cgu.band_panel=0;
    cgu.clear_down=0;
    if(previous>CGU_SOUND || tab>CGU_SOUND)cgs.zoom=0;
    cgu.zoom_ms=0;
    cgu.action=0;
    cu.pop=POP_NONE;
}

static void cgu_studio_page_turn(int d)
{
    if(!cgs.zoom)cgu.band_panel=(uint8_t)wrapi(cgu.band_panel+d%CGU_BAND_PANELS,CGU_BAND_PANELS);
    else if(cgu.band_panel==CGU_BAND_ARP)cgu.arp_timing^=(uint8_t)(d%2!=0);
    else if(!cgu.band_panel)cgu.part_harmony^=(uint8_t)(d%2!=0);
    cgu.turn_ms=0;
}

static int cgu_on_harmony(void)
{ return cgu.band_panel?cgu.band_panel==CGU_BAND_HARMONY:!cgs.zoom || cgu.part_harmony; }

static void cgu_select(int d)
{
    if(cgu.mixer || !d)return;
    if(cgs.zoom && cgu.tab==CGU_SOUND)cgu_sound_page_turn(d);
    else if(cgs.zoom && cgu.tab==CGU_STUDIO)cgu_studio_page_turn(d);
    else cgu_tab_turn(d);
}

/* The on-screen order swaps the last two audio parts. The mapping is its own inverse. */
static unsigned cgu_part(unsigned tile)
{ return tile<2?tile:tile^1u; }

static unsigned cgu_studio_tile(unsigned part,unsigned band_panel)
{ return band_panel?CGS_NPART+band_panel-1u:cgu_part(part); }

static void cgu_focus(int d)
{
    if(!d)return;
    cgu.turn_ms=0;
    if(!cgs.zoom)cgu.zoom_ms=0;
    if(cgu.tab==CGU_HELP && !cgu.mixer)cgu.topic=(uint8_t)wrapi(cgu.topic+d%4,4);
    else {
        unsigned tile=cgu_part(cgs.focus);
        if(cgu.tab==CGU_STUDIO && !cgs.zoom && !cgu.mixer){
            int count=CGS_NPART+CGU_BAND_PANELS-1;
            tile=(unsigned)wrapi((int)cgu_studio_tile(cgs.focus,cgu.band_panel)+d%count,count);
            cgu.band_panel=tile>=CGS_NPART?(uint8_t)(tile-CGS_NPART+1u):CGU_BAND;
            if(cgu.band_panel)return;
        } else tile=(unsigned)wrapi((int)tile+d%CGS_NPART,CGS_NPART);
        unsigned part=cgu_part(tile);
        if(part==cgs.focus)return;
        cgs.focus=(uint8_t)part;
        if(cgs.focus==CGS_DRUMS && cgu.sound_page==CGU_ENVELOPE)cgu_sound_page(CGU_SENDS);
        cf_keys_changed();
    }
}

static int cgu_recording_here(void)
{
    return cgs_home() && cgu.tab==CGU_RECORD;
}

static void cgu_clear_poll(void)
{
    if(!cgu.clear_down)return;
    if(!cgu_recording_here() || cgu.mixer || cgs.zoom ||
        !((fm1_in.buttons>>panel.btn[B_SAVE])&1u)){
        cgu.clear_down=0;
        return;
    }
    if(cu.now-cgu.clear_down>=3000u){
        cgu.clear_down=0;
        sq_clear_take(cgu.take_slot);
    }
}

static unsigned cgu_kit_step(unsigned kit,int d)
{
    unsigned rank=0;
    while(rank+1<NELEM(SQ_KIT_ORDER) && SQ_KIT_ORDER[rank]!=kit)rank++;
    return SQ_KIT_ORDER[wrapi((int)rank+d%(int)NELEM(SQ_KIT_ORDER),NELEM(SQ_KIT_ORDER))];
}

static unsigned cgu_kit(void)
{
    if(cgs_active())return cpa_edit.flags&CPA_GROOVE?cpa_edit.plan.kit:cpa.plan.kit;
    return cpa_setup.plan.kit;
}

static int cgu_sound_busy(unsigned p)
{
    return sqp.on || sqk.on || (p<CGS_DRUMS && cgs_manual(p));
}

static void cgu_set_kit(unsigned kit)
{
    cgs.kit_pinned=1;
    fm1_irq_off();
    if(!cgs_active() || cpa_setup.flags)cpa_setup.plan.kit=(uint8_t)kit;
    fm1_irq_on();
    if(cgs_active()){
        cpa_plan_t plan;
        cpa_active_request(&plan);
        plan.kit=(uint8_t)kit;
        cpa_publish(&plan,CPA_GROOVE,0);
    }
}

#if CHASE_LIBRARY && FELUCCA_VA
static unsigned cgu_palette_rank(unsigned part,unsigned *count)
{
    const char *const *tone=CGS_PROFILES[cgs.genre].tone[part];
    *count=cgs_palette_count(part);
    unsigned rank=0;
    if(cf_snd[part].engine!=ENGI_VA || cf_snd[part].kind!=PK_FACTORY)return *count;
    while(rank<*count && !cb_streq(cf_snd[part].name,tone[rank]))rank++;
    return rank;
}
#endif

static void cgu_load_sound(unsigned part,unsigned engine,unsigned pos)
{
    fm1_irq_off();
    if(cf_stage[part].pat==CGS_PATCH_STAGE)cf_stage[part].ready=0;
    if(cgs_active())cgs_release(part);
    fm1_irq_on();
    cf_pool_load(part,engine,pos);
    if(part==CGS_LAYER)cs.snd_layer=(uint16_t)cf_snd_pos(part);
    ui_snd_heard(part);
    if(part==CGS_CHORDS && cs.env_link)cf_env_follow();
}

static void cgu_sound_step(unsigned p,int d)
{
    if(!d)return;
    if(cgu_sound_busy(p)){
        cf_message("Release notes to choose sounds");
        return;
    }
    const cgs_profile_t *profile=&CGS_PROFILES[cgs.genre];
    if(p==CGS_DRUMS){
        unsigned kit=cgu_kit();
        unsigned pick=kit==profile->kit[0] || (kit!=profile->kit[1] && d<0);
        cgu_set_kit(cgu.all_sounds?cgu_kit_step(kit,d):profile->kit[pick]%SQ_NKIT);
        return;
    }
    unsigned engine, pos;
#if CHASE_LIBRARY && FELUCCA_VA
    if(!cgu.all_sounds){
        unsigned count;
        unsigned rank=cgu_palette_rank(p,&count);
        int from=rank==count?(d>0?-1:0):(int)rank;
        unsigned pick=(unsigned)wrapi(from+d%(int)count,count);
        unsigned voice=cgs_palette_voice(p,pick);
        if(voice==PF_NONE || pick==rank)return;
        engine=ENGI_VA;
        pos=pool_pos_factory(engine,voice);
    } else
#endif
    {
        unsigned count=cf_snd_count(p);
        if(!count)return;
        unsigned rank=(unsigned)wrapi((int)cf_snd_browse_rank(p,cf_snd_pos(p))+d%(int)count,count);
        unsigned index=cf_snd_browse_index(p,rank);
        if(index==cf_snd_pos(p))return;
        cf_snd_at(p,index,&engine,&pos);
    }
    cgu_load_sound(p,engine,pos);
}

#if CHASE_LIBRARY && FELUCCA_VA
static void cgu_load_palette(void)
{
    unsigned pos[CGS_DRUMS];
    for(unsigned p=0;p<CGS_DRUMS;p++){
        unsigned voice=cgs_palette_voice(p,cgs_palette_count(p)-1u);
        if(voice==PF_NONE)return;
        pos[p]=pool_pos_factory(ENGI_VA,voice);
    }
    for(unsigned p=0;p<CGS_DRUMS;p++)cgu_load_sound(p,ENGI_VA,pos[p]);
    cgu_set_kit(CGS_PROFILES[cgs.genre].kit[1]%SQ_NKIT);
}
#endif

static void cgu_perform_turn(int d)
{
    if(!d)return;
    if(!cgs_active() || cpa.clock_wait || cpa.paused || sqk.on || cpa.plan.rhythm>=DM_N){
        cf_message("PLAY a Studio groove first");
        return;
    }
    unsigned action=d<0?CFPERF_FILL:(cf_sn.perf_dropped || cf_sn.perf_pending==CFPERF_DROP)?CFPERF_BACK:CFPERF_DROP;
    if(!cf_post(CFE_PERFORM,action,0,0))cf_message("Busy: try again");
}

static int cgu_mixer_key(unsigned key,int down)
{
    if(key>=CF_NKEYS)return 0;
    uint32_t key_bit=1u<<key;
    if(!down && !(cgu.pad_down&key_bit))return 0;
    if(down && (!cgu.mixer || !cgs_home() || !cf_key_is_upper(key)))return 0;
    unsigned row=0;
    for(unsigned k=0;k<key;k++)row+=cf_key_is_upper(k)!=0;
    if(row>=8u)return 0;
    unsigned bit=1u<<cgu_part(row&3u);
    uint8_t *held=row<4u?&cgs.held_mute:&cgs.held_solo;
    if(down){cgu.pad_down|=key_bit;*held|=(uint8_t)bit;}
    else {cgu.pad_down&=~key_bit;*held&=(uint8_t)~bit;}
    return 1;
}

static void cgu_edit(void)
{
    if(cgu.tab==CGU_STUDIO){
        if(cgs.zoom && !cgu.band_panel){
            cgs_request(cgs.focus,CGS_REROLL);
            cgu.action=1;
        } else {
            if(cpa_commanded!=cpa_running() || !cgs_new_take(0))return;
            cgu.action=2;
        }
        cgu.action_ms=cu.now;
    } else if(cgu.tab==CGU_SOUND){
        if(cgs.zoom && cgu.sound_page!=CGU_TONE)return;
        unsigned first=cgs.zoom?cgs.focus:0;
        unsigned end=cgs.zoom?first+1:CGS_NPART;
        for(unsigned p=first;p<end;p++)
            if(cgu_sound_busy(p)){
                cf_message("Release notes to choose sounds");
                return;
            }
#if CHASE_LIBRARY && FELUCCA_VA
        if(!cgs.zoom && !cgu.all_sounds){cgu_load_palette();return;}
#endif
        for(unsigned p=first;p<end;p++){
            if(!cgu.all_sounds){cgu_sound_step(p,1);continue;}
            int step=(int)cgs_rand(&cgu.sound_seed,7)+1;
            cgu_sound_step(p,cgs_rand(&cgu.sound_seed,2)?step:-step);
        }
    } else if(cgu.tab==CGU_HELP && cgu.topic==CGU_HARMONY && cgs.zoom){
        cfl_open();
        cf_keys_changed();
    }
}

static void cf_harmony_turn(int k,int d);
static const uint8_t CGU_SEND_PARAM[]={P_DIST,P_CHOR,P_DLY,P_REV};

static int cgu_sweep(int k,int d)
{
    unsigned dir=d>0;
    int fast=d>1 || d<-1 || (cgu.turn_ms && cgu.turn_knob==k && cgu.turn_dir==dir && cu.now-cgu.turn_ms<=90u);
    cgu.turn_ms=cu.now?cu.now:1u;
    cgu.turn_knob=(uint8_t)k;
    cgu.turn_dir=(uint8_t)dir;
    return fast?d*5:d;
}

static void cgu_knob_turn(int k,int d)
{
    if(!d)return;
    unsigned p=cgs.focus;
    if(cgu.mixer){
        d=cgu_sweep(k,d);
        p=cgu_part((unsigned)k);
        cgs.level[p]=(uint8_t)clampi(cgs.level[p]+d,0,100);
    } else if(cgu.tab==CGU_STUDIO){
        int part_detail=cgs.zoom && !cgu.band_panel;
        if(cgu.band_panel==CGU_BAND_ARP){if(cgs.zoom)cf_arp_controls_turn(k,d,cgu.arp_timing,1);}
        else if(cgu.band_panel==CGU_BAND_GENRE){if(k==0 && cgs.zoom)cgs_genre_turn(d);}
        else if(cgu_on_harmony())cgs_harmony_turn(k,k==1?cgu_sweep(k,d):d);
        else if(cgs.zoom)cgs_knob_turn(k,!part_detail || k!=0?cgu_sweep(k,d):d,part_detail);
    } else if(cgu.tab==CGU_SOUND && !cgs.zoom){
        if(k==0)cgu.all_sounds=(uint8_t)(d>0);
        if(k==1)cgu_sound_step(p,d);
    } else if(cgu.tab==CGU_SOUND){
        if(cgu.sound_page==CGU_PRESETS){if(k==0)cfx_select(d);return;}
        if(k || cgu.sound_page!=CGU_TONE)d=cgu_sweep(k,d);
        if(cgu.sound_page==CGU_SENDS){
            unsigned param=CGU_SEND_PARAM[k];
            trk[p].p[param]=(int16_t)clampi(trk[p].p[param]+d,0,127);
            cf_edited(p);
        } else if(cgu.sound_page==CGU_ENVELOPE){
            if(p!=CGS_DRUMS){
                cf_env_step(p,k,d);cf_edited(p);
                if(p==CGS_CHORDS && cs.env_link)cf_env_follow();
            }
        } else if(k==0)cgu_sound_step(p,d);
        else if(k==1)cgs.level[p]=(uint8_t)clampi(cgs.level[p]+d,0,100);
        else if(k==2 && p!=CGS_DRUMS){
            const engine_t *e=ENGINES[cf_snd[p].engine];
            const param_desc_t *macro=&e->edit[0];
            if(macro->label && macro->label[0]){
                trk[p].p[P_E0]=(int16_t)clampi(trk[p].p[P_E0]+d,macro->min,macro->max);
                cf_edited(p);
            }
        } else if(k==3){
            trk[p].p[P_PAN]=(int16_t)clampi(trk[p].p[P_PAN]+d,-64,63);
            cf_edited(p);
        }
    } else if(cgu.tab==CGU_RECORD && !cgs.zoom && k==0){
        cgu.take_slot=(uint8_t)wrapi(cgu.take_slot+d%SQ_NPAT,SQ_NPAT);
        cgu.clear_down=0;
    } else if(cgu.tab==CGU_HELP && cgu.topic==CGU_HARMONY && cgs.zoom && k==0)cf_harmony_turn(0,d);
}

static void cgu_sound_info(int k,cf_knob_t *o,unsigned p)
{
    if(cgu.sound_page==CGU_PRESETS){
        if(k==0){kfmt(o->name,12,"BROWSE");kfmt(o->val,14,fxp_pick_name());
            o->bar=(int16_t)(fxpu.pick*255u/(FXP_FACTORY_N+FXP_SLOTS-1u));}
    } else if(cgu.sound_page==CGU_SENDS){
        static const char *const names[]={"DRIVE","CHORUS","DELAY","REVERB"};
        unsigned value=(unsigned)trk[p].p[CGU_SEND_PARAM[k]];
        kfmt(o->name,12,names[k]);kint(o->val,value*100u/127u,"%");
        o->bar=(int16_t)(value*255u/127u);
    } else if(cgu.sound_page==CGU_ENVELOPE && p!=CGS_DRUMS){
        static const char *const names[]={"ATTACK","DECAY","SUSTAIN","RELEASE"};
        kfmt(o->name,12,names[k]);cf_env_fmt(o->val,k,cf_env_val(p,k));
        o->bar=(int16_t)(cf_env_get(p,k)*255/99);
    } else if(k==0){
        kfmt(o->name,12,p==CGS_DRUMS?"KIT":"PRESET");
        kfmt(o->val,14,p==CGS_DRUMS?SQ_KITS[cgu_kit()%SQ_NKIT].name:cf_snd[p].name);
        if(!cgu.all_sounds){
            if(p==CGS_DRUMS)o->bar=cgu_kit()==CGS_PROFILES[cgs.genre].kit[1]?255:0;
#if CHASE_LIBRARY && FELUCCA_VA
            else {
                unsigned count;
                unsigned rank=cgu_palette_rank(p,&count);
                o->bar=(int16_t)(rank<count?rank*255u/(count-1u):0);
            }
#endif
        }
    } else if(k==1){
        kfmt(o->name,12,"LEVEL");kint(o->val,cgs.level[p],"%");
        o->bar=(int16_t)(cgs.level[p]*255u/100u);
    } else if(k==2 && p!=CGS_DRUMS){
        const param_desc_t *d=&ENGINES[cf_snd[p].engine]->edit[0];
        if(d->label && d->label[0]){
            char val[12];const char *unit;
            int value=trk[p].p[P_E0];
            param_format(d,value,val,&unit);
            kfmt(o->name,12,d->label);kfmt(o->val,14,val);
            size_t n=str_len(o->val);
            str_cpy(o->val+n,unit,14-n);
            if(d->max>d->min)o->bar=(int16_t)((value-d->min)*255/(d->max-d->min));
        }
    } else if(k==3){
        kfmt(o->name,12,"PAN");kint(o->val,trk[p].p[P_PAN],0);
        o->bar=(int16_t)((trk[p].p[P_PAN]+64)*255/127);
    }
}

static void cgu_knob_info(int k,cf_knob_t *o)
{
    unsigned p=cgs.focus;
    if(cgu.mixer){
        p=cgu_part((unsigned)k);
        kfmt(o->name,12,CGS_PART_NAME[p]);
        kint(o->val,cgs.level[p],"%");
    } else if(cgu.tab==CGU_STUDIO){
        if(cgu.band_panel==CGU_BAND_ARP)cf_arp_controls_info(k,o,cgu.arp_timing,1);
        else if(cgu.band_panel==CGU_BAND_GENRE){
            if(k==0){kfmt(o->name,12,"GENRE");kfmt(o->val,14,CGS_PROFILES[cgs.genre].name);}
        } else if(cgu_on_harmony()){
            static const char *const names[]={"KEY","TEMPO","SCALE","METER"};
            unsigned notes=cgs_active() && (cpa_edit.flags&CPA_NOTES);
            kfmt(o->name,12,names[k]);
            if(k==0)kpc(o->val,notes?cpa_edit.plan.tonic:cs.chord.tonic);
            else if(k==1)cpa_tempo_info(o);
            else if(k==2)kfmt(o->val,14,CF_SCALES[notes?cpa_edit.plan.scale:cs.chord.scale].shrt);
            else kint(o->val,cgs.meter,cgs.meter==6u?"/8":"/4");
        } else cgs_knob_info(k,o,cgs.zoom && !cgu.band_panel);
    } else if(cgu.tab==CGU_SOUND && !cgs.zoom){
        if(k==0){
            kfmt(o->name,12,"LIBRARY");
            kfmt(o->val,14,cgu.all_sounds?"All sounds":"Genre");
        }
        if(k==1){
            kfmt(o->name,12,"PRESET");
            kfmt(o->val,14,p==CGS_DRUMS?SQ_KITS[cgu_kit()%SQ_NKIT].name:cf_snd[p].name);
        }
    } else if(cgu.tab==CGU_SOUND)cgu_sound_info(k,o,p);
    else if(cgu.tab==CGU_HELP && cgu.topic==CGU_HARMONY && cgs.zoom && k==0){
        kfmt(o->name,12,"HARMONY");
        kfmt(o->val,14,ch.mode==CF_H_AUTO?"Auto":"Manual");
    }
}
