/* SPDX-License-Identifier: GPL-3.0-only */
#define CF_STUDIO 1
#define CF_ENGINE_HELPERS_ONLY
#include "cf_engine_test.c"

static void studio_start_profile(uint32_t seed,unsigned genre)
{
    cpa_stop();
    sqp_stop();
    dmp_stop();
    cfp_panic();
    cf_boot_go=0;
    cfb.on=0;
    cf_intro_on=0;
    cu.scr=SCR_HOME;
    home_boot_main=1;
    cf_home_bank=CF_HB_ENGINE;
    cf_scene_set(CF_SCENE_PROGRESSIONS);
    cs.chord.tonic=seed&1u?4:9;
    cs.chord.scale=seed&1u?CF_SC_MAJOR:CF_SC_MINOR;
    cs.beats=4;
    cs.bpm=104;
    cs.clock_in=2;
    cs.midi_out=1;
    cs.ch_chord=0;
    cs.ch_bass=1;
    cs.ch_layer=2;
    cs.ch_drums=3;
    cu.arp_on=0;
    cgs_init(seed);
    cgs.genre=(uint8_t)genre;
    cgs_prepare_start();
    cf_send_config();
    cf_send_metro();
    blocks(16);
    usb.up=usb.config=1;
    mo_r=mo_w;
    cgu_home();
    ui_button(B_PLAY);
}

static void studio_start(uint32_t seed)
{
    studio_start_profile(seed,CGS_DREAM);
}

static void controls_studio(void)
{
    studio_start(101);
    blocks(1);
    CHECK(cgs_active() && cpa.t==0 && sqp.n && sqp.nb && dmp.on);
    sq_snd_poll();
    CHECK(!strcmp(cf_snd[0].name,"SOFT LENS") && !strcmp(cf_snd[1].name,"SILK PICK"));
    CHECK(!strcmp(cf_snd[2].name,"DREAM PLUCK"));
    CHECK(!cfa_sq[0].on && cs.bpm==104);
    ticks(3*CGS_TICKS+3);
    CHECK(sqp.n && sqp.ln);
    ui_algo(1);
    CHECK(cgs.focus==CGS_BASS);
    ui_button(B_SCL);
    CHECK(cgs.zoom);
    for(unsigned part=0;part<CGS_NPART;part++){
        cgs_clip_t before[CGS_NPART];
        uint32_t seeds[CGS_NPART];
        memcpy(before,cgs.active,sizeof before);
        memcpy(seeds,cgs.seed,sizeof seeds);
        unsigned grid=cf_grid.gen, chord=cpa.slot, time=cpa.q2;
        cgs.focus=(uint8_t)part;
        ui_button(B_EDIT);
        CHECK(cgs.pending==(1u<<part));
        ticks(CGS_TICKS);
        CHECK(cpa.q2==time+2*CGS_TICKS && cpa.slot==chord && cf_grid.gen==grid);
        CHECK(!cgs.pending && memcmp(&before[part],&cgs.active[part],sizeof before[part]));
        for(unsigned p=0;p<CGS_NPART;p++)if(p!=part){
            CHECK(cgs.seed[p]==seeds[p]);
            CHECK(!memcmp(&before[p],&cgs.active[p],sizeof before[p]));
        }
    }
    cgs.focus=CGS_LAYER;
    ui_button(B_GLO);
    ui_button(B_EDIT);
    ui_button(B_GLO);
    blocks(1);
    CHECK((cgs.mute_applied&(1u<<CGS_LAYER)) && !sqp.ln && sqp.n);
    ui_button(B_EDIT);
    ticks(CGS_TICKS);
    CHECK(!sqp.ln);
    ui_button(B_HOME);
    CHECK(!cgs.zoom && cgs.focus==CGS_LAYER);
    unsigned harmony=cpa.plan.idea, grid=cf_grid.gen;
    ui_button(B_EDIT);
    CHECK(cpa_setup.flags && cpa.plan.idea==harmony);
    ticks(CGS_BAR-cpa.t);
    CHECK(cgs_active() && cgs.harmony_generation==1 && cpa.plan.idea!=harmony && cf_grid.gen==grid);
    CHECK(cgs.muted==(1u<<CGS_LAYER));
    midi_event(0x90,0,60,105);
    CHECK(!sqp.n && cr_mnote[0][60]);
    ui_button(B_SCL);
    cgs.focus=CGS_CHORDS;
    ui_button(B_EDIT);
    ticks(CGS_TICKS);
    CHECK(!sqp.n && cr_mnote[0][60] && trk_voice_of(&trk[0],60));
    midi_event(0x80,0,60,0);
    ui_button(B_PLAY);
    blocks(1);
    CHECK(!cgs_active() && !sqp.n && !sqp.nb && !sqp.ln);
    cgs.mix_idle=1;
    ui_knob_turn(2,-100);
    blocks(300);
    CHECK(!cgs.gain[CGS_CHORDS] && !cpa.on);
}

static void ui_capture(const char *name)
{
    cf_snapshot();
    cf_ui_draw();
    char path[96];
    snprintf(path,sizeof path,"build/studio/ui/%s.ppm",name);
    FILE *file=fopen(path,"wb");
    CHECK(file);
    if(!file)return;
    fprintf(file,"P6\n%d %d\n255\n",FB_W,FB_H);
    for(unsigned i=0;i<FB_W*FB_H;i++){
        uint32_t color=cf_pal[fb[i]];
        uint8_t rgb[]={(uint8_t)(color>>16),(uint8_t)(color>>8),(uint8_t)color};
        fwrite(rgb,1,sizeof rgb,file);
    }
    fclose(file);
}
static void workspaces_studio(void)
{
    studio_start(207);
    blocks(3);
    cgs.focus=CGS_BASS;
    cf_motion=1;
    cf_motion_epoch=0;
    cu.now=1000;
    ui_capture("studio");
    CHECK(cf_stage_last.theory[0] && cf_stage_last.degree[0]);
    CHECK(!strcmp(cf_stage_last.label[0],"KEY"));
    ui_algo(1);
    CHECK(cgs.focus==CGS_DRUMS && cgu.band_panel==CGU_BAND);
    ui_algo(1);
    CHECK(cgs.focus==CGS_LAYER && cgu.band_panel==CGU_BAND);
    unsigned pending=cf_evq_w;
    for(unsigned panel=CGU_BAND_HARMONY;panel<CGU_BAND_PANELS;panel++){
        ui_algo(1);
        CHECK(cgu.band_panel==panel && cgs.focus==CGS_LAYER && cf_evq_w==pending);
    }
    ui_algo(1);
    CHECK(cgs.focus==CGS_CHORDS && cgu.band_panel==CGU_BAND);
    ui_algo(-1);
    CHECK(cgu.band_panel==CGU_BAND_ARP && cgs.focus==CGS_CHORDS);
    ui_algo(2);
    CHECK(cgs.focus==CGS_BASS && cgu.band_panel==CGU_BAND);
    ui_button(B_SEQ);
    CHECK(cgu.band_panel==CGU_BAND_HARMONY && !cgs.zoom);
    ui_button(B_SEQ);
    CHECK(cgu.band_panel==CGU_BAND_SHAPING);
    ui_button(B_SCL);
    cu.now+=180;
    ui_capture("studio-shaping");
    CHECK(cf_stage_last.zoom==100 && cf_stage_last.bank==CGU_BAND_SHAPING);
    CHECK(!strcmp(cf_stage_last.label[0],"SPACE") && !strcmp(cf_stage_last.label[2],"ENERGY") &&
        !strcmp(cf_stage_last.label[3],"EVOLVE"));
    unsigned space=cgs.space, energy=cgs.energy, evolve=cgs.evolution, genre=cgs.genre;
    ui_knob_turn(0,1);
    ui_knob_turn(2,1);
    ui_knob_turn(3,1);
    CHECK(cgs.space==space+1 && cgs.energy==energy+1 && cgs.evolution==evolve+1 && cgs.genre==genre);
    cgs.space=(uint8_t)space;
    cgs_energy_turn(-1);
    cgs.evolution=(uint8_t)evolve;
    ui_button(B_SCL);
    cu.now+=180;
    CHECK(cgu.band_panel==CGU_BAND_SHAPING && !cgs.zoom);
    ui_button(B_SEQ);
    CHECK(cgu.band_panel==CGU_BAND_GENRE);
    ui_button(B_SCL);
    cu.now+=180;
    ui_capture("studio-genre");
    CHECK(cf_stage_last.zoom==100 && !strcmp(cf_stage_last.label[0],"GENRE"));
    ui_button(B_SCL);
    cu.now+=180;
    ui_algo(3);
    CHECK(cgu.band_panel==CGU_BAND && cgs.focus==CGS_BASS);
    char degree[8];
    cf_lab_numeral(CF_SC_MAJOR,0,2,0x89u,degree);
    CHECK(!strcmp(degree,"ii"));
    cf_lab_numeral(CF_SC_MAJOR,0,5,0x91u,degree);
    CHECK(!strcmp(degree,"IV"));
    uint32_t header=cfs_fnv(fb,FB_W*28);
    unsigned clock=cf_grid.gen, q2=cpa.q2;
    ui_button(B_SCL);
    CHECK(cgs.zoom && cgu_zoom()==0);
    cu.now+=25;
    CHECK(cgu_zoom()>0 && cgu_zoom()<100);
    ui_capture("zoom-in");
    CHECK(cfs_fnv(fb,FB_W*28)==header);
    unsigned mid=cgu_zoom();
    ui_button(B_SCL);
    CHECK(!cgs.zoom && cgu_zoom()==mid);
    cu.now+=180;
    CHECK(cgu_zoom()==0);
    ui_button(B_SCL);
    cu.now+=180;
    ui_capture("studio-bass");
    CHECK(cf_stage_last.zoom==100 && cf_stage_last.focus==CGS_BASS);
    for(unsigned direction=0;direction<2;direction++){
        ui_button(B_SCL);
        for(unsigned frame=0;frame<6;frame++){
            char name[24];
            snprintf(name,sizeof name,"camera-%u-%u",direction,frame);
            cu.now+=30;
            ui_capture(name);
            CHECK(cfs_fnv(fb,FB_W*28)==header);
        }
    }
    ui_select(1);
    CHECK(cgu.tab==CGU_STUDIO && cgs.zoom && cgu.part_harmony);
    ui_select(-1);
    CHECK(!cgu.part_harmony);
    ui_button(B_SCL);
    cu.now+=180;
    CHECK(cgu.band_panel==CGU_BAND && !cgs.zoom);
    ui_select(1);
    CHECK(cgu.tab==CGU_SOUND && !cgs.zoom && cgs.focus==CGS_BASS);
    ui_capture("sound");
    ui_button(B_SCL);
    cu.now+=180;
    ui_capture("sound-bass");
    ui_select(1);
    CHECK(cgu.sound_page==CGU_ENVELOPE && cgs.zoom);
    ui_capture("sound-envelope");
    ui_select(1);
    CHECK(cgu.sound_page==CGU_SENDS);
    unsigned send=trk[CGS_BASS].p[P_REV];
    ui_knob_turn(3,send<127?1:-1);
    CHECK(trk[CGS_BASS].p[P_REV]!=send);
    for(int k=0;k<4;k++)ui_knob_turn(k,12);
    ui_capture("sound-fx");
    ui_select(1);
    CHECK(cgu.sound_page==CGU_PRESETS && fxpu.mode==FXP_BROWSE);
    ui_knob_turn(0,1);
    const fxp_preset_t *preset=fxp_pick();
    cf_fx_t previous=cs.fx;
    ui_button(B_EDIT);
    CHECK(cfx_equal(&cs.fx,&previous));
    ui_button(B_SCL);
    CHECK(cgs.zoom && preset && cfx_equal(&cs.fx,&preset->fx));
    ui_capture("sound-fx-presets");
    ui_select(1);
    CHECK(cgu.sound_page==CGU_TONE);
    unsigned patch=cf_snd_pos(CGS_BASS), layer=cf_snd_pos(CGS_LAYER);
    uint32_t seed=cgs.seed[CGS_BASS];
    ui_button(B_EDIT);
    CHECK(cf_snd_pos(CGS_BASS)!=patch && cf_snd_pos(CGS_LAYER)==layer && cgs.seed[CGS_BASS]==seed);
    ui_button(B_SCL);
    ui_select(1);
    CHECK(cgu.tab==CGU_RECORD && cgs.focus==CGS_BASS);
    ui_capture("record");
    CHECK(cf_stage_last.steps[CGS_DRUMS][0] && cf_stage_last.steps[CGS_BASS][0]);
    ui_button(B_SCL);
    cu.now+=180;
    ui_capture("record-bass");
    ui_select(1);
    CHECK(cgu.tab==CGU_HELP && cgs.focus==CGS_BASS);
    ui_capture("help");
    ui_algo(1);
    CHECK(cgu.topic==CGU_HARMONY && cgs.focus==CGS_BASS);
    ui_button(B_SCL);
    cu.now+=180;
    ui_capture("help-harmony");
    CHECK(cf_stage_last.learn.automatic==(ch.mode==CF_H_AUTO));
    CHECK(cpa.q2==q2 && cf_grid.gen==clock);
    unsigned mode=ch.mode;
    ui_knob_turn(0,mode==CF_H_AUTO?-1:1);
    CHECK(ch.mode!=mode);
    ui_knob_turn(0,mode==CF_H_AUTO?1:-1);
    ui_note_key_aged(7,1,0);
    blocks(2);
    CHECK(cf_learn_focus==7);
    ui_capture("help-harmony");
    ui_note_key_aged(7,0,0);
    blocks(2);
    ui_button(B_HOME);
    CHECK(cgu.tab==CGU_STUDIO && !cgs.zoom && cgs.focus==CGS_BASS);
    cf_motion=0;
    ui_button(B_SCL);
    CHECK(cgu_zoom()==100);
    int octave=cs.chord.octave;
    ui_button(B_OCTUP);
    CHECK(cs.chord.octave==octave+1);
    ui_button(B_REC);
    cf_rec_poll();
    CHECK(cu.scr==SCR_HOME && cgu.tab==CGU_RECORD && sqk.on);
    blocks(2);
    ui_capture("record-manual");
    CHECK(cf_stage_last.rec);
    sq_home_cancel();
    blocks(2);
    cpa_stop();
    ui_button(B_GLO);
    ui_button(B_SAVE);
    ui_button(B_GLO);
    CHECK(cgs_home());
    ui_button(B_HOME);
    ui_button(B_HOME);
    CHECK(cgs_home() && cgu.tab==CGU_STUDIO && !cgs.zoom);
    ui_select(-1);
    cgu.topic=CGU_HARMONY;
    ui_button(B_SCL);
    ui_button(B_EDIT);
    CHECK(cu.scr==SCR_LAB);
    ui_button(B_HOME);
    CHECK(cgs_home());
}

static void harmony_studio(void)
{
    uint32_t seed=0x2353ad17u;
    uint16_t riffs=0;
    for(unsigned i=0;i<128;i++)riffs|=(uint16_t)(1u<<(cgs_riff(i&1u,&seed)-CGS_RIFFS));
    CHECK(riffs==(1u<<(sizeof CGS_RIFFS/sizeof CGS_RIFFS[0]))-1u);
    cpa_plan_t plan;
    cgs_pitches_t pitch={0};
    cf_chord_cfg_t cfg=cs.chord;
    cfg.tonic=4;
    cfg.scale=CF_SC_MAJOR;
    int index=cge_find(133);
    CHECK(index>=0 && cpa_prepare_full(&plan,index,cfg,0,CPA_BOTH,0,0));
    cgs_voice_plan(&plan,&pitch);
    CHECK(plan.n==3 && cpa_valid(&plan));
    const cf_source_chord_t *source=cf_progression_source(cf_progression_get(index));
    for(unsigned i=0;i<plan.n;i++){
        const cpa_slot_t *s=&plan.slot[i];
        const uint8_t *notes=pitch.note[i];
        unsigned target=pitch.note[(i+1)%plan.n][0];
        CHECK(s->mask==source[i].mask && notes[0]%12==s->bass_pc);
        if(notes[0]!=target)
            CHECK(abs((int)notes[4]-(int)target)==1);
    }
    index=cge_find(79);
    CHECK(cpa_prepare_full(&plan,index,cfg,0,CPA_BOTH,0,0));
    cgs_voice_plan(&plan,&pitch);
    CHECK(plan.n==8 && cpa_valid(&plan));
}

static void arrangements_studio(void)
{
    studio_start(100);
    blocks(2);
    CHECK(cgs_active() && cpa.plan.idea==CF_P_COMPOSED);
    unsigned grid=cf_grid.gen, tonic=cpa.plan.tonic, scale=cpa.plan.scale;
    unsigned families=1u<<cgs.playing_feel;
    for(unsigned roll=0;roll<12;roll++){
        unsigned previous=cgs.playing_feel;
        cgs_clip_t drums=cgs.active[CGS_DRUMS];
        uint32_t now=cpa.q2;
        ui_button(B_EDIT);
        CHECK(cpa_setup.flags && cgs.queued_feel!=previous);
        ticks(CGS_BAR-cpa.t);
        CHECK(cpa.q2>now && cf_grid.gen==grid && cgs.playing_feel!=previous);
        CHECK(cpa.plan.tonic==tonic && cpa.plan.scale==scale);
        CHECK(memcmp(drums.event,cgs.active[CGS_DRUMS].event,sizeof drums.event));
        families|=1u<<cgs.playing_feel;
    }
    CHECK(families==15u);

    cgs.focus=CGS_BASS;
    ui_button(B_GLO);
    CHECK(cgu.mixer);
    ui_capture("mixer");
    ui_button(B_SCL);
    blocks(1);
    CHECK(cgs.solo==(1u<<CGS_BASS) && cgs.mute_applied==(15u^(1u<<CGS_BASS)));
    midi_event(0x90,0,64,96);
    CHECK(cr_mnote[0][64] && trk_voice_of(&trk[0],64));
    midi_event(0x80,0,64,0);
    ui_algo(1);
    CHECK(cgs.focus==CGS_DRUMS);
    ui_button(B_EDIT);
    ui_algo(-1);
    ui_button(B_SCL);
    blocks(1);
    CHECK(!cgs.solo && cgs.mute_applied==(1u<<CGS_DRUMS));
    ui_button(B_GLO);
    CHECK(!cgu.mixer);

    for(unsigned genre=1;genre<CGS_NGENRE;genre++){
        ui_presets(1);
        CHECK(cgs.genre==genre && cpa_setup.flags);
        ticks(CGS_BAR-cpa.t);
        CHECK(cgs.playing_genre==genre && cf_grid.gen==grid && cpa.plan.tonic==tonic);
        CHECK(cpa.plan.slot[0].ticks==(genre==CGS_AMBIENT?2u:1u)*CGS_BAR);
        for(unsigned p=0;p<CGS_DRUMS;p++)
            for(unsigned pick=0;pick<2;pick++)CHECK(cgs_palette_voice(p,pick)<VA_NPRESETS);
    }
    ui_knob_turn(3,1);
    CHECK(cgs.meter==6 && cpa.plan.beats==4);
    ticks(CGS_BAR-cpa.t);
    CHECK(cpa.plan.beats==6 && cpa.plan.slot[0].ticks==12u*SQ_PPQN && cf_grid.gen==grid);
    ui_knob_turn(3,1);
    CHECK(cgs.meter==3 && cpa.plan.beats==6);
    ticks(6u*SQ_PPQN-cpa.t);
    CHECK(cpa.plan.beats==3 && cf_grid.gen==grid);
    ui_knob_turn(0,1);
    CHECK(cpa_edit.flags&CPA_NOTES);
    ticks(cpa.plan.slot[cpa.slot].ticks-cpa.elapsed);
    CHECK(cpa.plan.tonic==(tonic+1)%12 && cf_grid.gen==grid);
    ui_knob_turn(2,1);
    CHECK(cpa_edit.flags&CPA_NOTES);
    ticks(cpa.plan.slot[cpa.slot].ticks-cpa.elapsed);
    CHECK(cpa.plan.scale!=scale && cf_grid.gen==grid);
    ui_select(1);
    cgs.focus=CGS_CHORDS;
    ui_button(B_SCL);
    ui_button(B_EDIT);
    CHECK(!strcmp(cf_snd[CGS_CHORDS].name,"SLOW ORBIT"));
    ui_button(B_EDIT);
    CHECK(!strcmp(cf_snd[CGS_CHORDS].name,"APOLLO VEIL"));
    ui_button(B_HOME);
    cpa_stop();
}

static void amp_performance_studio(void)
{
    studio_start(102);
    blocks(2);
    unsigned grid=cf_grid.gen;
    CHECK(cgs.active[CGS_CHORDS].voice==2 && sqp.n);
    unsigned count=sqp.n;
    uint8_t notes[6];
    memcpy(notes,sqp.note,count);
    for(unsigned i=0;i<CGS_STEPS;i++){
        ticks(CGS_TICKS);
        CHECK(sqp.n==count && !memcmp(sqp.note,notes,count));
        voice_t *voice=trk_voice_of(&trk[CGS_CHORDS],notes[0]);
        CHECK(voice && voice->gate && !cgs.gate[CGS_CHORDS]);
    }

    ui_button(B_SCL);
    unsigned sustain=cf_env_val(CGS_CHORDS,2);
    cu.now=2000;
    ui_knob_turn(3,-1);
    CHECK(cf_env_val(CGS_CHORDS,2)==sustain-1);
    cu.now+=35;
    ui_knob_turn(3,-1);
    CHECK(cf_env_val(CGS_CHORDS,2)==sustain-6);
    cu.now+=200;
    ui_knob_turn(3,1);
    CHECK(cf_env_val(CGS_CHORDS,2)==sustain-5);
    ui_capture("amp-chords");

    ui_note_key_aged(7,1,0);
    blocks(2);
    CHECK(cfp.n && !sqp.n);
    unsigned held=cfp.note[0];
    ticks(2u*CGS_BAR);
    voice_t *voice=trk_voice_of(&trk[CGS_CHORDS],held);
    CHECK(cfp.n && voice && voice->gate && cf_grid.gen==grid);
    ui_note_key_aged(7,0,0);
    blocks(2);
    CHECK(!cfp.n);

    cgs.focus=CGS_BASS;
    uint32_t seed=cgs.seed[CGS_BASS], phrase=cgs.active[CGS_BASS].seed;
    ui_knob_turn(1,-4);
    ticks(CGS_TICKS);
    CHECK(cgs.seed[CGS_BASS]==seed && cgs.active[CGS_BASS].seed==phrase);
    cgs.focus=CGS_DRUMS;
    ui_presets(-1);
    blocks(1);
    cf_snapshot();
    CHECK(cf_sn.perf_pending==CFPERF_FILL && !cf_sn.perf_fill);
    ui_capture("amp-drums");
    ticks(CGS_BAR-cpa.t);
    cf_snapshot();
    CHECK(cf_sn.perf_fill && !cf_sn.perf_pending);
    ui_presets(1);
    blocks(1);
    ticks(CGS_BAR-cpa.t);
    cf_snapshot();
    CHECK(cf_sn.perf_dropped && !cf_sn.perf_fill);
    ui_presets(1);
    blocks(1);
    ticks(CGS_BAR-cpa.t);
    CHECK(!cf_moment.dropped && cf_grid.gen==grid);
    cpa_stop();
}

static void play_and_mixer_studio(void)
{
    studio_start(102);
    blocks(3);
    unsigned grid=cf_grid.gen;
    ui_button(B_ARP);
    CHECK(cu.scr==SCR_HOME && cgu.band_panel==CGU_BAND_ARP && cgs.zoom && !cfa_k.n);
    ui_select(1);
    cu.now+=180;
    ui_capture("arp-timing");
    CHECK(cgu.arp_timing && !strcmp(cf_stage_last.label[0],"GATE"));
    ui_select(-1);
    CHECK(!cgu.arp_timing);
    ui_knob_turn(0,1);
    blocks(1);
    CHECK(cgu_arp_mode()==CGU_ARP_KEYS && cfa_k.on && !cfa_k.n);
    ui_note_key_aged(7,1,0);
    blocks(2);
    CHECK(cfa_k.n && cfa_k.count && !sqp.n);
    ui_note_key_aged(7,0,0);
    blocks(1);
    ui_knob_turn(0,1);
    blocks(1);
    ticks(CGS_TICKS);
    CHECK(cgu_arp_mode()==CGU_ARP_BAND && !cfa_k.on && cfa_sq[0].n && cfa_sq[0].count);
    ui_capture("arp-band");
    ui_knob_turn(0,-2);
    blocks(1);
    ticks(CGS_TICKS);
    CHECK(!cfa_sq[0].n && sqp.n);
    ui_algo(-1);
    blocks(1);
    CHECK(cgs.focus==CGS_LAYER && cfp_cp==CGS_LAYER);
    ui_note_key_aged(7,1,0);
    blocks(2);
    CHECK(cfp.n && !cfp.nb && trk_voice_of(&trk[CGS_LAYER],cfp.note[0]) && sqp.n);
    ui_note_key_aged(7,0,0);
    blocks(2);
    ui_button(B_HOME);
    ui_algo(2);
    blocks(1);
    CHECK(cgs.focus==CGS_BASS && cfp_bo);
    ui_note_key_aged(7,1,0);
    blocks(2);
    CHECK(cfp.nb && !cfp.n && trk_voice_of(&trk[CGS_BASS],cfp.bass[0]) && sqp.n);
    ui_note_key_aged(7,0,0);
    blocks(2);
    ui_algo(1);
    blocks(1);
    CHECK(cgs.focus==CGS_DRUMS && cfp_cp==CGS_DRUMS);

    unsigned keys[8], n=0;
    for(unsigned i=0;i<CF_NKEYS && n<NELEM(keys);i++)
        if(cf_key_is_upper(i))keys[n++]=i;
    CHECK(n==NELEM(keys));
    if(n!=NELEM(keys))return;
    ui_note_key_aged((int)keys[0],1,0);
    blocks(1);
    CHECK(trk_voice_of(&trk[CGS_DRUMS],36) && !cfp.n);
    ui_note_key_aged((int)keys[0],0,0);

    ui_button(B_GLO);
    ui_note_key_aged((int)keys[0],1,0);
    blocks(1);
    CHECK(cgs.mute_applied==(1u<<CGS_CHORDS) && !cgs.muted);
    ui_note_key_aged((int)keys[0],0,0);
    blocks(1);
    CHECK(!cgs.mute_applied);
    ui_note_key_aged((int)keys[5],1,0);
    blocks(1);
    CHECK(cgs.mute_applied==(15u^(1u<<CGS_BASS)) && !cgs.solo);
    ui_capture("mixer-held");
    ui_button(B_GLO);
    blocks(1);
    CHECK(!cgu.mixer && !cgs.mute_applied);
    ui_note_key_aged((int)keys[5],0,0);
    CHECK(!cgu.pad_down && cf_grid.gen==grid);
    cpa_stop();
    cgu_home();
}

static unsigned studio_led(unsigned key)
{
    unsigned pin=cu_led_pos[14u+key], bit=1u<<(pin&7u), column=pin>>3;
    return (fm1_led[column]&bit)?4u:(fm1_led_dim2[column]&bit)?3u:
        (fm1_led_dim[column]&bit)?2u:(fm1_led_faint[column]&bit)?1u:0u;
}

static void sound_and_led_studio(void)
{
    studio_start(102);
    blocks(2);
    unsigned chord=cf_panel_pitch_key(sqp.note[0]);
    cgu_key_fade.part=0;
    cf_snapshot();
    cf_leds();
    CHECK(studio_led(chord)==2u);
    cu.now+=80;
    cf_leds();
    CHECK(studio_led(chord)==4u);
    cgs_release(CGS_CHORDS);
    cf_snapshot();
    cu.now+=90;
    cf_leds();
    CHECK(studio_led(chord)==3u);
    cu.now+=180;
    cf_leds();
    CHECK(studio_led(chord)==0u);
    ui_algo(1);
    blocks(1);
    cf_snapshot();
    unsigned bass=cf_panel_pitch_key(sqp.bass[0]);
    cf_leds();
    CHECK(cgu_key_fade.part==CGS_BASS+1u && (cf_sn.studio_keys&(1u<<bass)) && studio_led(bass));
    ui_algo(1);
    blocks(1);
    unsigned key=0;
    while(key<CF_NKEYS && !cf_key_is_upper(key))key++;
    CHECK(key<CF_NKEYS);
    ui_note_key_aged((int)key,1,0);
    blocks(1);
    cf_snapshot();
    cf_leds();
    CHECK(studio_led(key)>0);
    ui_note_key_aged((int)key,0,0);
    ui_select(1);
    ui_button(B_SCL);
    ui_select(1);
    CHECK(cgu.sound_page==CGU_SENDS);
    ui_select(-1);
    CHECK(cgu.sound_page==CGU_TONE && cgu_sound_page_count(CGS_DRUMS)==3);
    ui_capture("sound-drums");
    ui_algo(1);
    ui_knob_turn(0,1);
    ui_knob_turn(0,1);
    CHECK(!strcmp(cf_snd[CGS_LAYER].name,"GLASS THREAD"));
    ui_knob_turn(0,-1);
    CHECK(!strcmp(cf_snd[CGS_LAYER].name,"DISTANT BELL"));
    cpa_stop();
    cgu_home();
}

static void curated_sounds_studio(void)
{
    static const char *const curated[][CGS_DRUMS]={
        {"CABINET PAD","ROUNDWIRE","GLASS THREAD"},
        {"JANGLE ORGAN","PICKUP BASS","GLASS THREAD"},
        {"PULSE STACK","SYNC RELAY","CRYSTAL STEP"},
        {"AMP STACK","POWER PICK","WIRE LEAD"},
        {"TAPE EP","FELT SUB","DUST TINES"},
        {"PRISM CLOUD","FLOOR SWELL","HALO BLOOM"},
    };
    static const unsigned kit[]={DMK_ROOM,DMK_ROOM,DMK_XTECH,DMK_ROOM,DMK_TAPE,DMK_GLASS};
    for(unsigned genre=0;genre<CGS_NGENRE;genre++){
        studio_start_profile(100,genre);
        /* Test the pending first PLAY as well as mid-take sound changes. */
        if(genre)blocks(2);
        unsigned grid=cf_grid.gen, slot=cpa.slot, pulse=cpa.q2;
        ui_select(1);
        CHECK(cgu.tab==CGU_SOUND && !cgs.zoom && !cgu.all_sounds);
        for(unsigned p=0;p<CGS_DRUMS;p++){
            for(unsigned pick=0;pick<cgs_palette_count(p);pick++){
                unsigned voice=cgs_palette_voice(p,pick);
                CHECK(voice<VA_NPRESETS && VA_PRESETS[voice].mono==(p==CGS_BASS));
            }
            trk[p].p[P_REV]=(int16_t)(25+p);
        }
        ui_button(B_EDIT);
        CHECK(cpa.q2==pulse && cpa.slot==slot && cgu_kit()==kit[genre]);
        blocks(2);
        if(genre){
            CHECK(cf_grid.gen==grid);
            ticks(CGS_BAR-cpa.t);
        }
        CHECK(cgs_active() && cpa.plan.kit==kit[genre]);
        for(unsigned p=0;p<CGS_DRUMS;p++){
            CHECK(!strcmp(cf_snd[p].name,curated[genre][p]) && trk[p].p[P_REV]==25+p);
            CHECK(trk[p].preset==cgs_palette_voice(p,cgs_palette_count(p)-1u));
        }
    }
    cpa_stop();
}

static void record_clear_studio(void)
{
    studio_start(100);
    blocks(2);
    unsigned grid=cf_grid.gen;
    for(unsigned i=0;i<2;i++){
        sq_fresh(&sq_st.pat[i],4);
        sq_st.pat[i].bars=1;
        sq_st.pat[i].nev=1;
    }
    ui_select(2);
    CHECK(cgu.tab==CGU_RECORD && !cgs.zoom);
    ui_knob_turn(0,1);
    CHECK(cgu.take_slot==1);
    ui_capture("record-takes");
    uint32_t save=1u<<panel.btn[B_SAVE];
    fm1_in.buttons|=save;
    ui_button(B_SAVE);
    cu.now+=1000;
    cgu_clear_poll();
    fm1_in.buttons&=~save;
    cgu_clear_poll();
    CHECK(sq_has(0) && sq_has(1) && !cgu.clear_down);
    fm1_in.buttons|=save;
    ui_button(B_SAVE);
    cu.now+=3000;
    cgu_clear_poll();
    fm1_in.buttons&=~save;
    CHECK(sq_has(0) && !sq_has(1) && cgs_active() && cf_grid.gen==grid);
    cpa_stop();
}

static void drain(FILE *trace, unsigned sample, uint8_t held[4][128], unsigned *clocks)
{
    while(mo_r!=mo_w){
        uint32_t entry=midi_out_q[mo_r++%MQ];
        unsigned status=entry>>8&255, note=entry>>16&127, velocity=entry>>24&127;
        unsigned part=status&15, type=status&0xf0;
        if(status==0xf8)(*clocks)++;
        if(part>=4 || (type!=0x90 && type!=0x80))continue;
        if(type==0x90 && velocity){CHECK(!held[part][note]);held[part][note]=1;}
        else held[part][note]=0;
        fprintf(trace,"%u,%u,%u,%u,%u\n",sample,part,type,note,velocity);
    }
}
static void render_studio(uint32_t seed, unsigned regenerate, const char *path, unsigned rate, unsigned cap, unsigned bars, unsigned genre, unsigned kit)
{
    studio_start_profile(seed,genre);
    if(kit<SQ_NKIT)cpa_setup.plan.kit=(uint8_t)kit;
    if(regenerate==6)cgs.solo=1u<<CGS_CHORDS;
    if(regenerate==7)cgs.solo=1u<<CGS_DRUMS;
    if(regenerate==8)for(unsigned p=0;p<CGS_DRUMS;p++){
        unsigned voice=cgs_palette_voice(p,cgs_palette_count(p)-1u);
        if(voice==PF_NONE)continue;
        cf_stage[p].ready=0;
        cf_pool_load(p,ENGI_VA,pool_pos_factory(ENGI_VA,voice));
    }
    ua_reset();
    ua.cap_alt=(uint8_t)cap;
    ua.cap_hold=0;
    ua_rate_set(rate);
    int32_t out[CTL*2];
    char name[512];
    snprintf(name,sizeof name,"%s.raw",path);
    FILE *raw=fopen(name,"wb");
    snprintf(name,sizeof name,"%s.csv",path);
    FILE *trace=fopen(name,"w");
    CHECK(raw && trace);
    if(!raw || !trace)return;
    fprintf(trace,"sample,part,status,note,velocity\n");
    unsigned sample=0, sent=0, clocks=0, progression=0, grid=0, kills=voice_kills, sheds=shed_count;
    uint8_t held[4][128]={{0}};
    while(cpa.q2<2u*bars*CGS_BAR){
        audio_block(out,CTL);
        if(!sample){
            CHECK(cgs_active());
            if(!cgs_active())break;
            const cf_progression_t *idea=cf_progression_get(cpa.plan.idea);
            progression=idea?idea->id:cpa.plan.idea;
            grid=cf_grid.gen;
        }
        drain(trace,sample,held,&clocks);
        CHECK(fwrite(out,sizeof *out,CTL*2,raw)==CTL*2);
        ua.cr=ua.cw;
        sample+=CTL;
        if(regenerate==6 && !sent && cgs.bar==2 && cpa.t>=CGS_TICKS){
            ui_note_key_aged(7,1,0);
            sent=1;
        }
        if(regenerate==6 && sent==1 && cgs.bar==3 && cpa.t>=CGS_BAR/2){
            ui_note_key_aged(7,0,0);
            sent=2;
        }
        if(regenerate==5 && sent<bars/8u-1u && cpa.q2>=2u*((sent*8u+7u)*CGS_BAR+13u*CGS_TICKS)){
            ui_button(B_EDIT);
            sent++;
        }
        if(regenerate>0 && regenerate<=CGS_NPART && !sent && cpa.q2>=2u*(CGS_BAR+13u*CGS_TICKS)){
            cgs_request(regenerate-1u,0);
            sent=1;
        }
        cgs_poll();
    }
    CHECK(cgs_active() && cf_grid.gen==grid);
    CHECK(!regenerate || regenerate>=5 || (sent && cgs.generation[regenerate-1u]));
    if(regenerate==5)CHECK(cgs.harmony_generation==bars/8u-1u);
    if(regenerate==6)CHECK(sent==2);
    cpa_stop();
    drain(trace,sample,held,&clocks);
    for(unsigned part=0;part<4;part++)for(unsigned n=0;n<128;n++)CHECK(!held[part][n]);
    fclose(raw);
    fclose(trace);
    printf("seed=%u progression=%u bpm=%u clocks=%u frames=%u voice_kills=%u sheds=%u\n",
           seed,progression,cs.bpm,clocks,sample,voice_kills-kills,shed_count-sheds);
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1);
    emu_hal.ready=2;
    emu_fw_init(0);
    if((argc>=8 && argc<=10) && !strcmp(argv[1],"--render"))
        render_studio((uint32_t)strtoul(argv[2],0,0),atoi(argv[3]),argv[4],atoi(argv[5]),atoi(argv[6]),atoi(argv[7]),argc>=9?atoi(argv[8]):CGS_DREAM,argc==10?(unsigned)atoi(argv[9]):SQ_NKIT);
    else {
        controls_studio();
        harmony_studio();
        workspaces_studio();
        arrangements_studio();
        amp_performance_studio();
        play_and_mixer_studio();
        sound_and_led_studio();
        curated_sounds_studio();
        record_clear_studio();
    }
    printf("CORD Studio: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
