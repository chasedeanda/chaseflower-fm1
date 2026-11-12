/* SPDX-License-Identifier: GPL-3.0-only
 * Studio workspaces own navigation. The audio engine and stored formats are shared. */
enum { CGU_STUDIO, CGU_SOUND, CGU_RECORD, CGU_HELP, CGU_NTAB };
enum { CGU_BAND, CGU_BAND_HARMONY, CGU_BAND_SHAPING, CGU_BAND_GENRE, CGU_BAND_ARP, CGU_BAND_PROGRESSION, CGU_BAND_PANELS };
enum { CGU_TONE, CGU_ENVELOPE, CGU_SENDS, CGU_PRESETS, CGU_SOUND_PAGES };
enum { CGU_ARP_OFF, CGU_ARP_KEYS, CGU_ARP_BAND };
static const char *const CGU_SOUND_NAMES[]={"Tone","Envelope","Sends","FX Presets"};
static const char *const CGU_ARP_NAMES[]={"Off","Keys","Band"};
enum { CGU_START, CGU_HARMONY, CGU_PLAYING, CGU_CAPTURE };
enum { CGU_RACK_LIBRARY=1, CGU_RACK_MIX };
enum { CGU_KNOB_NONE, CGU_KNOB_PART, CGU_KNOB_HARMONY, CGU_KNOB_SHAPING,
    CGU_KNOB_GENRE, CGU_KNOB_ARP, CGU_KNOB_PROGRESSION, CGU_KNOB_LIBRARY, CGU_KNOB_SOUND,
    CGU_KNOB_TAKES, CGU_KNOB_LESSON, CGU_KNOB_MIX };
enum { CGU_RACK_COUNT=CGS_NPART+CGU_BAND_PANELS-1 };
static const char *const CGU_TABS[]={"Studio","Sound","Record","Help"};
static const char *const CGU_BAND_NAMES[]={"Harmony","Shaping","Genre","ARP","Progression"};
static const char *const CGU_TOPICS[]={"Start here","Harmony","Playing","Capture"};
static struct {
    uint32_t zoom_ms, turn_ms, clear_down, tab_ms;
    uint8_t turn_knob, turn_dir;
    uint32_t sound_seed, action_ms;
    uint8_t tab, topic, zoom_from, action, all_sounds, band_panel, part_harmony;
    uint8_t sound_page, arp_timing, key_arp, fx_return, take_slot, progression_pick, rack_controls;
    int8_t tab_dir;
} cgu={.sound_seed=0x436f7264u};

enum { CGU_GLO_MAP, CGU_GLO_STATE, CGU_GLO_VIEWS };
enum {
    CGU_GLO_PAUSE=4, CGU_GLO_RESTART, CGU_GLO_UNMUTE, CGU_GLO_TAP,
    CGU_GLO_FILL, CGU_GLO_FILL_BAR, CGU_GLO_DROP
};
static const char *const CGU_GLO_DRUMS[]={"KICK","SNARE","HATS","PERC"};
static struct {
    uint32_t keys, taps[4];
    uint8_t active, view, ntap;
} cgu_glow;

static void cf_arp_controls_info(int k,cf_knob_t *o,int timing,int studio);
static void cf_arp_controls_turn(int k,int d,int timing,int studio);
static void ui_open(uint32_t scr);

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
    if(detail){
        if(cgu.tab==CGU_STUDIO){cgu.part_harmony=0;cgu.arp_timing=0;}
        if(cgu.tab==CGU_SOUND)cgu_sound_page(CGU_TONE);
    }
    cgu.zoom_from=(uint8_t)cgu_zoom();
    cgu.zoom_ms=cf_motion?cu.now:0;
    cgs.zoom=(uint8_t)detail;
}

static void cgu_open_arp(void)
{
    if(cgu.tab==CGU_STUDIO && cgu.band_panel==CGU_BAND_ARP && cgs.zoom){cgu_set_zoom(0);return;}
    cgu.tab=CGU_STUDIO;
    cgu.rack_controls=0;
    cgu.band_panel=CGU_BAND_ARP;
    cgu.arp_timing=0;
    cgs.zoom=0;
    cgu.zoom_ms=0;
    cgu_set_zoom(1);
}

static void cgu_home(void)
{
    cu.pop=POP_NONE;
    cgu.action=cgu.band_panel=cgu.part_harmony=0;
    cgu.rack_controls=0;
    cgu.tab_ms=0;
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
    if(tab==cgu.tab)return;
    unsigned previous=cgu.tab;
    cgu.tab=(uint8_t)tab;
    cgu.tab_ms=cf_motion?(cu.now?cu.now:1u):0;
    cgu.tab_dir=d<0?-1:1;
    cgu.turn_ms=0;
    cgu.sound_page=CGU_TONE;
    cgu.part_harmony=0;
    if(cgu.band_panel || cgu.rack_controls)cgs.zoom=0;
    cgu.band_panel=0;
    cgu.rack_controls=0;
    cgu.clear_down=0;
    if(previous>CGU_SOUND || tab>CGU_SOUND)cgs.zoom=0;
    cgu.zoom_ms=0;
    cgu.action=0;
    cu.pop=POP_NONE;
}

static void cgu_band_focus(unsigned panel)
{
    if(panel==cgu.band_panel)return;
    cgu.band_panel=(uint8_t)panel;
    cgu.zoom_ms=0;
    cgu.turn_ms=0;
    if(panel==CGU_BAND_PROGRESSION)cgu.progression_pick=cgs.custom;
    if(panel==CGU_BAND_ARP)cgu.arp_timing=0;
}

static void cgu_studio_page_turn(int d)
{
    if(!cgs.zoom)cgu_band_focus((unsigned)wrapi(cgu.band_panel+d%CGU_BAND_PANELS,CGU_BAND_PANELS));
    else if(cgu.band_panel==CGU_BAND_ARP)cgu.arp_timing^=(uint8_t)(d%2!=0);
    else if(!cgu.band_panel)cgu.part_harmony^=(uint8_t)(d%2!=0);
    cgu.turn_ms=0;
}

static unsigned cgu_knob_panel(int detail)
{
    if(cgu.tab==CGU_STUDIO){
        static const uint8_t panel[]={CGU_KNOB_PART,CGU_KNOB_HARMONY,CGU_KNOB_SHAPING,
            CGU_KNOB_GENRE,CGU_KNOB_ARP,CGU_KNOB_PROGRESSION};
        if(!cgu.band_panel && detail && cgu.part_harmony)return CGU_KNOB_HARMONY;
        return panel[cgu.band_panel];
    }
    if(cgu.tab==CGU_SOUND){
        if(cgu.rack_controls==CGU_RACK_MIX)return CGU_KNOB_MIX;
        return cgu.rack_controls?CGU_KNOB_LIBRARY:CGU_KNOB_SOUND;
    }
    if(cgu.tab==CGU_RECORD && cgu.rack_controls)return CGU_KNOB_TAKES;
    if(cgu.tab==CGU_HELP && cgu.topic==CGU_HARMONY)return CGU_KNOB_LESSON;
    return CGU_KNOB_NONE;
}

static unsigned cgu_tone_page(int detail)
{ return detail?cgu.sound_page:CGU_TONE; }

static unsigned cgu_arp_page(int detail)
{ return detail?cgu.arp_timing:0; }

static int cgu_progression_panel(void)
{ return cgu.tab==CGU_STUDIO && cgs.zoom && cgu.band_panel==CGU_BAND_PROGRESSION; }

static void cgu_select(int d)
{
    if(!d)return;
    if(cgs.zoom && cgu.tab==CGU_SOUND){if(!cgu.rack_controls)cgu_sound_page_turn(d);}
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
    if(cgu.tab==CGU_HELP)cgu.topic=(uint8_t)wrapi(cgu.topic+d%4,4);
    else {
        unsigned tile=cgu_part(cgs.focus);
        if(cgu.tab==CGU_STUDIO && !cgs.zoom){
            tile=(unsigned)wrapi((int)cgu_studio_tile(cgs.focus,cgu.band_panel)+d%CGU_RACK_COUNT,CGU_RACK_COUNT);
            cgu_band_focus(tile>=CGS_NPART?tile-CGS_NPART+1u:CGU_BAND);
            if(cgu.band_panel)return;
        } else if(!cgs.zoom && (cgu.tab==CGU_SOUND || cgu.tab==CGU_RECORD)){
            int count=CGS_NPART+(cgu.tab==CGU_SOUND?2:1);
            if(cgu.rack_controls)tile=CGS_NPART+cgu.rack_controls-1u;
            tile=(unsigned)wrapi((int)tile+d%count,count);
            cgu.rack_controls=(uint8_t)(tile>=CGS_NPART?tile-CGS_NPART+1u:0);
            if(cgu.rack_controls)return;
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
    if(!cgu_recording_here() || !cgu.rack_controls ||
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

static void cgu_set_kit(unsigned kit,int pin)
{
    if(pin)cgs.kit_pinned=1;
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
static unsigned cgu_palette_rank(unsigned part,const cf_snd_t *sound,unsigned count)
{
    const char *const *tone=CGS_PROFILES[cgs.genre].tone[part];
    unsigned rank=0;
    if(sound->engine!=ENGI_VA || sound->kind!=PK_FACTORY)return count;
    while(rank<count && !cb_streq(sound->name,tone[rank]))rank++;
    return rank;
}
#endif

static void cgu_load_sound(unsigned part,unsigned engine,unsigned pos)
{
    fm1_irq_off();
    if(cf_stage[part].pat==CGS_PATCH_STAGE || cf_stage[part].pat==CGS_PATCH_ROLL)
        cf_stage[part].ready=cf_stage[part].taken=0;
    if(cgs_active())cgs_release(part);
    fm1_irq_on();
    cf_pool_load(part,engine,pos);
    if(part==CGS_LAYER)cs.snd_layer=(uint16_t)cf_snd_pos(part);
    cu.edited[part]=0;
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
        cgu_set_kit(cgu.all_sounds?cgu_kit_step(kit,d):profile->kit[pick]%SQ_NKIT,1);
        return;
    }
    unsigned engine, pos;
#if CHASE_LIBRARY && FELUCCA_VA
    if(!cgu.all_sounds){
        unsigned count=cgs_palette_count(p);
        if(!count)return;
        unsigned rank=cgu_palette_rank(p,&cf_snd[p],count);
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
static void cgu_apply_palette(void)
{
    for(unsigned p=0;p<CGS_NPART;p++){
        if(!cgu_sound_busy(p))continue;
        cf_message("Release notes to choose sounds");
        return;
    }
    unsigned pos[CGS_DRUMS];
    for(unsigned p=0;p<CGS_DRUMS;p++){
        unsigned voice=cgs_palette_voice(p,cgs_palette_count(p)-1u);
        if(voice==PF_NONE)return;
        pos[p]=pool_pos_factory(ENGI_VA,voice);
    }
    for(unsigned p=0;p<CGS_DRUMS;p++)cgu_load_sound(p,ENGI_VA,pos[p]);
    cgu_set_kit(CGS_PROFILES[cgs.genre].kit[1]%SQ_NKIT,1);
}
#endif

static void cgu_roll_studio_sounds(unsigned first,unsigned end)
{
    for(unsigned p=first;p<end;p++){
        if(cgu_sound_busy(p))continue;
        if(p==CGS_DRUMS){
            const uint8_t *kit=CGS_PROFILES[cgs.genre].kit;
            unsigned current=cgu_kit();
            if(!cgs.kit_pinned || current==kit[0] || current==kit[1])
                cgu_set_kit(kit[current==kit[0]]%SQ_NKIT,0);
            continue;
        }
#if CHASE_LIBRARY && FELUCCA_VA
        if(cu.edited[p] || cfk_edited(p) || cf_snd[p].kind==PK_USER)continue;
        unsigned count=cgs_palette_count(p);
        if(!count)continue;
        const cf_stage_t *stage=&cf_stage[p];
        fm1_irq_off();
        cf_snd_t sound=stage->pat==CGS_PATCH_ROLL && (stage->ready || stage->taken)?
            stage->meta:cf_snd[p];
        fm1_irq_on();
        unsigned rank=cgu_palette_rank(p,&sound,count);
        for(unsigned i=1;i<=count;i++){
            unsigned pick=rank==count?(count-i):(rank+i)%count;
            unsigned voice=cgs_palette_voice(p,pick);
            if(voice==PF_NONE || pick==rank)continue;
            unsigned pos=pool_pos_factory(ENGI_VA,voice);
            if(pool_entry(ENGI_VA,pos).kind!=PK_FACTORY)continue;
            if(!cgs_active() && !cpa_commanded)cgu_load_sound(p,ENGI_VA,pos);
            else {
                cf_stage_t prepared;
                cf_snd_build(p,ENGI_VA,pos,&prepared);
                prepared.pat=CGS_PATCH_ROLL;
                prepared.own1=prepared.tw=prepared.taken=0;
                prepared.ready=1;
                fm1_irq_off();
                cf_stage[p]=prepared;
                fm1_irq_on();
            }
            break;
        }
#endif
    }
}

static int cgu_perform_ready(void)
{
    return cgs_active() && !cpa.clock_wait && !cpa.paused && !sqk.on && cpa.plan.rhythm<DM_N;
}

static void cgu_perform(unsigned action)
{
    if(!cgu_perform_ready())cf_message("PLAY a Studio groove first");
    else if(!cf_post(CFE_PERFORM,action,0,0))cf_message("Busy: try again");
}

static void cgu_perform_turn(int d)
{
    if(!d)return;
    unsigned action=d<0?CFPERF_FILL:(cf_sn.perf_dropped || cf_sn.perf_pending==CFPERF_DROP)?CFPERF_BACK:CFPERF_DROP;
    cgu_perform(action);
}

static void cgu_glow_open(int on)
{
    cgu_glow.active=(uint8_t)on;
    cgs.held_solo=cgs.fill_held=0;
    cgu.turn_ms=0;
    cu.pop=POP_NONE;
    if(on)cgu_glow.view=CGU_GLO_MAP;
    else fb_full=1; /* repaint the page that the overlay covered */
}

static unsigned cgu_glow_place(unsigned key,int black)
{
    unsigned place=0;
    for(unsigned k=0;k<key;k++)place+=cf_key_is_upper(k)==black;
    return place;
}

static void cgu_glow_tap(void)
{
    if(cs.clock_in==1u){cf_message("Tempo follows MIDI clock");return;}
    unsigned n=cgu_glow.ntap, now=fm1_ms;
    if(n && now-cgu_glow.taps[n-1]>2000u)n=0;
    if(n==NELEM(cgu_glow.taps)){
        for(unsigned i=1;i<n;i++)cgu_glow.taps[i-1]=cgu_glow.taps[i];
        n--;
    }
    cgu_glow.taps[n++]=now;
    cgu_glow.ntap=(uint8_t)n;
    if(n>=3 && now!=cgu_glow.taps[0])cpa_tempo((int)(60000u*(n-1u)/(now-cgu_glow.taps[0]))-cs.bpm);
}

static void cgu_glow_transport(unsigned action)
{
    if(cs.clock_in==1u){cf_message("Transport follows MIDI clock");return;}
    if(!cgs_active()){cf_message("PLAY a Studio groove first");return;}
    if(!cf_post(CFE_ACCOMP,action,0,0))cf_message("Busy: try again");
}

static int cgu_glow_ready(unsigned place)
{
    if(place<CGS_NPART || place==CGU_GLO_UNMUTE)return 1;
    if(place==CGU_GLO_TAP)return cs.clock_in!=1u;
    if(place==CGU_GLO_PAUSE || place==CGU_GLO_RESTART)return cgs_active() && cs.clock_in!=1u;
    return place<=CGU_GLO_DROP && cgu_perform_ready() &&
        (place==CGU_GLO_DROP || !cf_sn.perf_dropped);
}

static int cgu_glow_key(unsigned key,int down)
{
    if(key>=CF_NKEYS)return 0;
    uint32_t bit=1u<<key;
    if(!down && !(cgu_glow.keys&bit))return 0;
    if(down && !cgu_glow.active)return 0;
    int black=cf_key_is_upper(key);
    unsigned place=cgu_glow_place(key,black);
    if(!down){
        cgu_glow.keys&=~bit;
        if(!black && place<CGS_NPART)cgs.held_solo&=(uint8_t)~(1u<<cgu_part(place));
        if(!black && place==CGU_GLO_FILL)cgs.fill_held=0;
        return 1;
    }
    cgu_glow.keys|=bit;
    if(black){
        if(place<CGS_NPART)cgs.muted^=(uint8_t)(1u<<cgu_part(place));
        else if(place>=6u && place<10u)cgs.drum_mutes^=(uint8_t)(1u<<(place-6u));
    } else if(place<CGS_NPART)cgs.held_solo|=(uint8_t)(1u<<cgu_part(place));
    else switch(place){
    case CGU_GLO_PAUSE:cgu_glow_transport(CGS_PLAY_PAUSE);break;
    case CGU_GLO_RESTART:cgu_glow_transport(CGS_PLAY_RESTART);break;
    case CGU_GLO_UNMUTE:cgs.muted=cgs.solo=cgs.drum_mutes=0;break;
    case CGU_GLO_TAP:cgu_glow_tap();break;
    case CGU_GLO_FILL:
        if(cgu_perform_ready() && !cf_sn.perf_dropped)cgs.fill_held=1;
        break;
    case CGU_GLO_FILL_BAR:cgu_perform(CFPERF_FILL);break;
    case CGU_GLO_DROP:cgu_perform_turn(1);break;
    }
    return 1;
}

static void cgu_edit(void)
{
    if(cgu.tab==CGU_STUDIO){
        if(cgu_progression_panel()){
            if(cfd_open_studio(cgs.meter))ui_open(SCR_PEDIT);
            return;
        }
        if(cgs.zoom && !cgu.band_panel){
            cgs_request(cgs.focus,CGS_REROLL);
            cgu_roll_studio_sounds(cgs.focus,cgs.focus+1u);
            cgu.action=1;
        } else {
            if(cpa_commanded!=cpa_running() || !cgs_new_take(0))return;
            cgu_roll_studio_sounds(0,CGS_NPART);
            cgu.action=2;
        }
        cgu.action_ms=cu.now;
    } else if(cgu.tab==CGU_SOUND){
        if(cgu.rack_controls==CGU_RACK_MIX)return;
        int one_part=cgs.zoom && !cgu.rack_controls;
        if(one_part && cgu.sound_page!=CGU_TONE)return;
        unsigned first=one_part?cgs.focus:0;
        unsigned end=one_part?first+1:CGS_NPART;
        for(unsigned p=first;p<end;p++)
            if(cgu_sound_busy(p)){
                cf_message("Release notes to choose sounds");
                return;
            }
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

static void cgu_knob_view(int k,cf_knob_t *o,int detail);

static int cgu_sweep(int k,int d)
{
    int fast=d>1 || d<-1 || (cgu.turn_ms && cgu.turn_knob==k &&
        cgu.turn_dir==(d>0) && cu.now-cgu.turn_ms<=90u);
    return fast?d*5:d;
}

static void cgu_sound_turn(unsigned p,unsigned page,int k,int d)
{
    if(page==CGU_PRESETS){cfx_select(d);return;}
    if(page==CGU_SENDS){
        unsigned param=CGU_SEND_PARAM[k];
        trk[p].p[param]=(int16_t)clampi(trk[p].p[param]+d,0,127);
        cf_edited(p);
    } else if(page==CGU_ENVELOPE){
        cf_env_step(p,k,d);cf_edited(p);
        if(p==CGS_CHORDS && cs.env_link)cf_env_follow();
    } else if(k==0)cgu_sound_step(p,d);
    else if(k==1)cgs.level[p]=(uint8_t)clampi(cgs.level[p]+d,0,100);
    else if(k==2){
        const param_desc_t *macro=&ENGINES[cf_snd[p].engine]->edit[0];
        trk[p].p[P_E0]=(int16_t)clampi(trk[p].p[P_E0]+d,macro->min,macro->max);
        cf_edited(p);
    } else {
        trk[p].p[P_PAN]=(int16_t)clampi(trk[p].p[P_PAN]+d,-64,63);
        cf_edited(p);
    }
}

static void cgu_knob_turn(int k,int d)
{
    if(!d)return;
    cf_knob_t knob={.bar=-1};
    cgu_knob_view(k,&knob,cgs.zoom);
    if(!knob.name[0])return;

    unsigned panel=cgu_knob_panel(cgs.zoom), page=cgu_tone_page(cgs.zoom);
    int continuous=panel==CGU_KNOB_SHAPING || panel==CGU_KNOB_MIX ||
        (panel==CGU_KNOB_HARMONY && k==1) || (panel==CGU_KNOB_PART && k!=0) ||
        (panel==CGU_KNOB_SOUND && (page==CGU_SENDS || page==CGU_ENVELOPE || (page==CGU_TONE && k!=0)));
    if(continuous)d=cgu_sweep(k,d);
    cgu.turn_ms=cu.now?cu.now:1u;
    cgu.turn_knob=(uint8_t)k;
    cgu.turn_dir=(uint8_t)(d>0);

    switch(panel){
    case CGU_KNOB_MIX: {
        unsigned p=cgu_part((unsigned)k);
        cgs.level[p]=(uint8_t)clampi(cgs.level[p]+d,0,100);
        break;
    }
    case CGU_KNOB_PART: cgs_knob_turn(k,d,1);break;
    case CGU_KNOB_HARMONY: cgs_harmony_turn(k,d);break;
    case CGU_KNOB_SHAPING: cgs_knob_turn(k,d,0);break;
    case CGU_KNOB_GENRE: cgs_genre_turn(d);break;
    case CGU_KNOB_ARP: cf_arp_controls_turn(k,d,cgu_arp_page(cgs.zoom),1);break;
    case CGU_KNOB_PROGRESSION: cgu.progression_pick=(uint8_t)(d>0);break;
    case CGU_KNOB_LIBRARY: cgu.all_sounds=(uint8_t)(d>0);break;
    case CGU_KNOB_SOUND: cgu_sound_turn(cgs.focus,page,k,d);break;
    case CGU_KNOB_TAKES:
        cgu.take_slot=(uint8_t)wrapi(cgu.take_slot+d%SQ_NPAT,SQ_NPAT);
        cgu.clear_down=0;
        break;
    case CGU_KNOB_LESSON: cf_harmony_turn(0,d);break;
    }
}

static void cgu_sound_info(int k,cf_knob_t *o,unsigned p,unsigned page)
{
    if(page==CGU_PRESETS){
        if(k==0){kfmt(o->name,12,"BROWSE");kfmt(o->val,14,fxp_pick_name());
            o->bar=(int16_t)(fxpu.pick*255u/(FXP_FACTORY_N+FXP_SLOTS-1u));}
    } else if(page==CGU_SENDS){
        static const char *const names[]={"DRIVE","CHORUS","DELAY","REVERB"};
        unsigned value=(unsigned)trk[p].p[CGU_SEND_PARAM[k]];
        kfmt(o->name,12,names[k]);kint(o->val,value*100u/127u,"%");
        o->bar=(int16_t)(value*255u/127u);
    } else if(page==CGU_ENVELOPE && p!=CGS_DRUMS){
        static const char *const names[]={"ATTACK","DECAY","SUSTAIN","RELEASE"};
        kfmt(o->name,12,names[k]);cf_env_fmt(o->val,k,cf_env_val(p,k));
        o->bar=(int16_t)(cf_env_get(p,k)*255/99);
    } else if(k==0){
        kfmt(o->name,12,p==CGS_DRUMS?"KIT":"PRESET");
        kfmt(o->val,14,p==CGS_DRUMS?SQ_KITS[cgu_kit()%SQ_NKIT].name:cf_snd[p].name);
        unsigned count=p==CGS_DRUMS?SQ_NKIT:cf_snd_count(p);
        unsigned pos=p==CGS_DRUMS?cgu_kit():cf_snd_pos(p);
        o->bar=(int16_t)(count>1?pos*255u/(count-1u):0);
        if(!cgu.all_sounds){
            if(p==CGS_DRUMS)o->bar=cgu_kit()==CGS_PROFILES[cgs.genre].kit[1]?255:0;
#if CHASE_LIBRARY && FELUCCA_VA
            else {
                unsigned voices=cgs_palette_count(p);
                unsigned rank=cgu_palette_rank(p,&cf_snd[p],voices);
                o->bar=(int16_t)(voices>1 && rank<voices?rank*255u/(voices-1u):0);
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

static void cgu_knob_view(int k,cf_knob_t *o,int detail)
{
    switch(cgu_knob_panel(detail)){
    case CGU_KNOB_MIX: {
        unsigned p=cgu_part((unsigned)k);
        kfmt(o->name,12,CGS_PART_NAME[p]);
        kint(o->val,cgs.level[p],"%");
        break;
    }
    case CGU_KNOB_PART: cgs_knob_info(k,o,1);break;
    case CGU_KNOB_SHAPING: cgs_knob_info(k,o,0);break;
    case CGU_KNOB_HARMONY: {
        unsigned notes=cgs_active() && (cpa_edit.flags&CPA_NOTES);
        if(k==0){
            unsigned tonic=notes?cpa_edit.plan.tonic:cs.chord.tonic;
            kfmt(o->name,12,"KEY");kpc(o->val,tonic);
            o->bar=(int16_t)(tonic*255u/11u);
        } else if(k==1)cgs_tempo_info(o);
        else if(k==2){
            unsigned scale=notes?cpa_edit.plan.scale:cs.chord.scale;
            kfmt(o->name,12,"SCALE");kfmt(o->val,14,CF_SCALES[scale].shrt);
            o->bar=(int16_t)(scale*255u/(CF_NSCALES-1u));
        } else {
            kfmt(o->name,12,"METER");kint(o->val,cgs.meter,cgs.meter==6u?"/8":"/4");
            o->bar=cgs.meter==3?0:cgs.meter==6?255:127;
            o->dis=cgs.custom;
        }
        break;
    }
    case CGU_KNOB_GENRE:
        if(k==0){
            kfmt(o->name,12,"GENRE");kfmt(o->val,14,CGS_PROFILES[cgs.genre].name);
            o->bar=(int16_t)(cgs.genre*255u/(CGS_NGENRE-1u));
        }
        break;
    case CGU_KNOB_ARP: cf_arp_controls_info(k,o,cgu_arp_page(detail),1);break;
    case CGU_KNOB_PROGRESSION:
        if(k==0){
            kfmt(o->name,12,"SOURCE");kfmt(o->val,14,cgu.progression_pick?"My draft":"Bank");
            o->bar=cgu.progression_pick?255:0;
        }
        break;
    case CGU_KNOB_LIBRARY:
        if(k==0){
            kfmt(o->name,12,"LIBRARY");kfmt(o->val,14,cgu.all_sounds?"All sounds":"Genre");
            o->bar=cgu.all_sounds?255:0;
        }
        break;
    case CGU_KNOB_SOUND: cgu_sound_info(k,o,cgs.focus,cgu_tone_page(detail));break;
    case CGU_KNOB_TAKES:
        if(k==0){
            unsigned slot=cgu.take_slot;
            kfmt(o->name,12,"TAKE");
            kint(o->val,slot+1u,(sq_unavailable&(1u<<slot))?" Unread":sq_has(slot)?" Saved":" Empty");
            o->bar=(int16_t)(slot*255u/(SQ_NPAT-1u));
        }
        break;
    case CGU_KNOB_LESSON:
        if(k==0){
            kfmt(o->name,12,"HARMONY");kfmt(o->val,14,ch.mode==CF_H_AUTO?"Auto":"Manual");
            o->bar=ch.mode==CF_H_AUTO?255:0;
        }
        break;
    }
}

static void cgu_knob_info(int k,cf_knob_t *o)
{ cgu_knob_view(k,o,cgs.zoom); }
