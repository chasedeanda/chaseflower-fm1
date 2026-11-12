/* SPDX-License-Identifier: GPL-3.0-only */
#define CF_STUDIO 0
#include <stdio.h>
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void blocks(unsigned n){int32_t out[CTL*2];while(n--)audio_block(out,CTL);cf_snapshot();cpa_ui_poll();}
static void setup(unsigned idea)
{
    cpa_stop();sqp_stop();dmp_stop();cfp_panic();cu.scr=SCR_HOME;home_boot_main=1;cf_home_bank=0;cu.pop=POP_NONE;
    cpg.idea=idea;cpg.rhythm=CPA_OFF;cpg.mode=CPA_BOTH;cpg.repeat_off=0;
    const cf_progression_t *p=cf_progression_get(idea);cs.chord.tonic=p->tonic;cs.chord.scale=p->scale;cs.chord.quantize=CF_Q_BASIC;cs.beats=p->beats;cs.bpm=120;cs.clock_in=0;cs.metro_level=0;cs.level_chord=104;cs.bass_level=96;cs.bass_on=1;
    cf_send_config();cf_send_metro();blocks(2);cf_motion=0;
}
static void navigation(void)
{
    setup(0);cf_set_t saved=cs;unsigned q=cf_evq_w;ui_button(B_SEQ);CHECK(cu.scr==SCR_ACC && cf_acc_tab()==CF_ACC_GROOVE);
    for(unsigned i=0;i<12;i++){CHECK(cf_acc_tab()==i%4);cf_ui_draw();ui_algo(1);CHECK(cf_evq_w==q && !memcmp(&cs,&saved,sizeof cs));}
    ui_button(B_SEQ);CHECK(home_stage() && cf_home_bank==0);ui_button(B_SEQ);CHECK(cf_acc_tab()==0);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=CF_GLO_LEGACY;
    static const unsigned scr[]={SCR_SQ,SCR_SEQ,SCR_DRUM};
    for(unsigned i=0;i<3;i++){cf_legacy_pick=i;cf_ui_draw();ui_button(B_SCL);CHECK(cu.scr==scr[i]);ui_button(B_HOME);CHECK(home_stage());cu.scr=SCR_GLO;cu.page[SCR_GLO]=CF_GLO_LEGACY;}
    ui_button(B_HOME);ui_button(B_SEQ);ui_algo(1);q=cf_evq_w;ui_select(2);CHECK(cf_acc_focus==2 && cf_evq_w==q);cf_knob_t k={0};ui_knob_info(1,&k);CHECK(k.dis && !strcmp(k.val,"1"));ui_knob_turn(1,3);CHECK(cf_evq_w==q);
}
static void one_shot(void)
{
    for(unsigned idea=0;idea<cf_progression_count();idea++) {
        setup(idea);cu.scr=SCR_ACC;cu.page[SCR_ACC]=CF_ACC_LOOP;ui_knob_turn(3,-1);CHECK(cpg.repeat_off);
        ui_button(B_PLAY);blocks(2);CHECK(cpa.on && cpa.plan.repeat_off);
        unsigned total=0;for(unsigned i=0;i<cpa.plan.n;i++)total+=cpa.plan.slot[i].ticks;
        for(unsigned i=0;i+1<total;i++)cpa_tick1();CHECK(cpa.on && cpa.slot+1u==cpa.plan.n);
        cpa_tick1();CHECK(!cpa.on && !sqp.n && !sqp.nb && !cf_grid.on);
        cpa_tick(FS*10u,1000);CHECK(!sqp.n && !dmp.on);
    }
    setup(0);cf_post(CFE_KEY,7,0,1);cf_post(CFE_KEY,25,0,1);cf_post(CFE_KEY,25,0,0);cf_post(CFE_KEY,7,0,0);blocks(2);unsigned held=cfp.n;CHECK(cfc.out.hold && held);
    cu.scr=SCR_ACC;cu.page[SCR_ACC]=CF_ACC_LOOP;ui_knob_turn(3,-1);ui_button(B_PLAY);blocks(2);
    for(unsigned i=0;i<4u*384u;i++)cpa_tick1();CHECK(!cpa.on && cfc.out.hold && cfp.n==held);
    /* A complete queued idea wins over ending the previous play-once phrase. */
    setup(0);cpg.repeat_off=1;ui_button(B_PLAY);blocks(2);for(unsigned i=0;i<3u*384u;i++)cpa_tick1();
    cf_progression_choose(1);for(unsigned i=0;i<384;i++)cpa_tick1();CHECK(cpa.on && cpa.plan.idea==1 && cpa.slot==0 && cpa.plan.repeat_off);
    cpa_stop();
}
static void clock_loss(void)
{
    setup(0);cf_post(CFE_KEY,7,0,1);cf_post(CFE_KEY,25,0,1);cf_post(CFE_KEY,25,0,0);cf_post(CFE_KEY,7,0,0);blocks(2);unsigned held=cfp.n;
    cs.clock_in=1;cf_send_config();blocks(2);ui_button(B_PLAY);blocks(2);
    CHECK(cpa.on && cpa.clock_wait && !sqp.n && !cf_grid.on && cfp.n==held);
    cpa_tick(0,1);CHECK(!cpa.clock_wait && sqp.n && cpa.t==4);
    cf_progression_choose(1);unsigned slot=cpa.slot,t=cpa.t;
    blocks(FS/CTL);CHECK(cpa.paused && cpa.clock_lost && !sqp.n && !sqp.nb && !cf_grid.on && cpa.slot==slot && cpa.t==t && cpa_setup.flags && cfp.n==held);
    cf_acc_model_t model;cu.scr=SCR_ACC;cf_acc_model(&model);CHECK(!strcmp(model.status,"EXT LOST"));
    cpa_tick(0,200);CHECK(cpa.paused && cpa.slot==slot && !sqp.n);
    cpa_realtime(0xFB);cpa_tick(0,1);CHECK(!cpa.paused && !cpa.clock_lost && cpa.t==t+4 && sqp.n && cpa_setup.flags);
    cpa_realtime(0xFA);CHECK(cpa.slot==0 && cpa.t==0 && sqp.n);
    cpa_stop();CHECK(cfp.n==held && cfc.out.hold);
    /* Switching Receive on cannot keep a scheduled chord hanging; explicit resume is required. */
    setup(0);ui_button(B_PLAY);blocks(2);cs.clock_in=1;cf_send_config();blocks(2);CHECK(cpa.clock_wait && cpa.paused && !sqp.n);
    cpa_tick(0,100);CHECK(cpa.paused && !sqp.n);cpa_realtime(0xFB);CHECK(!cpa.paused && sqp.n);
    cpa_stop();setup(0);cpg.rhythm=dm_nth(4,0);cs.clock_in=1;cf_send_config();blocks(2);ui_button(B_PLAY);blocks(2);
    CHECK(cpa.clock_wait && !dmp.on);cpa_realtime(0xFB);CHECK(!cpa.clock_wait && !cpa.paused && dmp.on && cf_grid.on && sqp.n);
    cpa_stop();cs.clock_in=0;cf_send_config();blocks(2);
}
static void click_and_preferences(void)
{
    setup(0);cu.scr=SCR_ACC;cu.page[SCR_ACC]=CF_ACC_CLICK;unsigned run=cfm.running,writes=emu_flash_writes;
    ui_knob_turn(1,19);unsigned level=cf_click_level;CHECK(!cs.metro_level && level>=1 && level<=100);
    ui_knob_turn(0,1);blocks(2);CHECK(cs.metro_level==level && cfm.level==level && cfm.running==run && emu_flash_writes==writes);
    cpb_flush();unsigned size=sizeof(cpb_rec_t);cpb_load();CHECK(cf_click_level==level);cpb_rec_t rec;CHECK(st_load(OBJ_CFPREFS,&rec,size)==size && rec.version==5 && cpb_valid(&rec));
    rec.version=3;rec.reserved[0]=CF_SKIN_EDITORIAL;rec.reserved[1]=0;rec.check=cfs_fnv(&rec,__builtin_offsetof(cpb_rec_t,check));CHECK(!st_save(OBJ_CFPREFS,&rec,size));cpb_load();CHECK(cf_skin==CF_SKIN_EMBER && !cf_motion && cf_click_level==cs.metro_level && !cpb_dirty);
    /* Click sounds with a real drum groove, and Off really mutes its future attacks. */
    setup(0);cpg.rhythm=dm_nth(4,0);cs.metro_level=35;cf_send_metro();blocks(2);ui_button(B_PLAY);blocks(2);
    cfm.click=2;int32_t out[CTL*2]={0};cf_click_mix(out,CTL);unsigned nonzero=0;for(unsigned i=0;i<CTL*2;i++)nonzero|=out[i]!=0;CHECK(nonzero && cpa.plan.rhythm<DM_N);
    cs.metro_level=0;cf_send_metro();blocks(2);cfm.click=2;memset(out,0,sizeof out);cf_click_mix(out,CTL);CHECK(cf_ck_amp==0);
    cpa_stop();
    setup(0);cpg.rhythm=CPA_OFF;cf_acc_click_remember(43);cpa_ui_rhythm(1);blocks(2);CHECK(cpg.rhythm==CPA_CLICK && cs.metro_level==43 && cfm.level==43);
    cs.metro_level=0;cf_send_metro();ui_button(B_PLAY);blocks(2);CHECK(cfm.level==0); /* explicit Off wins over source */
    cpa_stop();setup(0);cpg.rhythm=CPA_OFF;ui_button(B_PLAY);blocks(2);cpa_ui_rhythm(1);CHECK(cpa_edit.plan.click_start==cf_click_level && cfm.level==0);
    for(unsigned i=0;i<384;i++)cpa_tick1();cf_snapshot();cpa_ui_poll();CHECK(cpa.plan.rhythm==CPA_CLICK && cfm.level==cf_click_level && cs.metro_level==cf_click_level);
    cpa_stop();setup(0);ui_button(B_PLAY);blocks(2);cpa_ui_rhythm(1);cpa_tempo(1);
    for(unsigned i=0;i<384;i++)cpa_tick1();blocks(2);CHECK(cfm.level==cf_click_level && cs.metro_level==cf_click_level);
    cpa_stop();setup(0);ui_button(B_PLAY);blocks(2);cpa_ui_rhythm(1);
    for(unsigned i=0;i<384;i++)cpa_tick1();cu.scr=SCR_ACC;cu.page[SCR_ACC]=CF_ACC_CLICK;ui_knob_turn(0,-1);blocks(2);CHECK(!cfm.level && !cs.metro_level);
    cpa_stop();setup(0);ui_button(B_PLAY);blocks(2);cpa_ui_rhythm(1);cu.scr=SCR_ACC;cu.page[SCR_ACC]=CF_ACC_CLICK;ui_knob_turn(0,-1);blocks(2);CHECK(!cpa_edit.plan.click_start);
    for(unsigned i=0;i<384;i++)cpa_tick1();CHECK(cfm.level==0);cpa_stop();
}
static void display(void)
{
    setup(0);cu.scr=SCR_ACC;cf_acc_valid=0;cu.pop=POP_NONE;cf_ui_draw();unsigned draws=cf_acc_draws,tiles=fb_tiles_sent;
    for(unsigned i=0;i<20;i++){cu.now+=100;cf_sn.now_ms=cu.now;cf_ui_draw();}CHECK(cf_acc_draws==draws && fb_tiles_sent==tiles);
    for(unsigned tab=0;tab<4;tab++){cu.page[SCR_ACC]=tab;cf_ui_draw();unsigned hash=cfs_fnv(fb,FB_W*FB_H);cu.now+=1000;cf_ui_draw();CHECK(hash==cfs_fnv(fb,FB_W*FB_H));}
    cpg.rhythm=dm_nth(4,0);ui_button(B_PLAY);blocks(2);cf_progression_choose(11);cf_snapshot();cf_acc_model_t m;cf_acc_model(&m);
    CHECK(m.pending[0] && m.next[0] && cf_sn.ac_next_beats==6 && cf_sn.ac_next==11);
    cf_stage_model_t stage;cf_stage_model(&stage);CHECK(stage.next_setup[0] && strstr(stage.next_setup,"6/8"));
    cpa_stop();cf_motion=1;cu.scr=SCR_ACC;cu.now+=100;ui_algo(1);cf_acc_model(&m);CHECK(m.dx==8);
    cf_motion=0;cf_acc_model(&m);CHECK(!m.dx);
    cf_motion=1;cu.scr=SCR_HOME;cu.now+=100;cf_post(CFE_KEY,9,0,1);blocks(2);cf_stage_symbol[0]=0;cf_stage_model(&stage);CHECK(stage.chord_dy==5);
    uint32_t start=cf_stage_chord_ms;cu.now+=30;cf_stage_model(&stage);CHECK(cf_stage_chord_ms==start && stage.chord_dy<5);
    cf_motion=0;cf_stage_model(&stage);CHECK(!stage.chord_dy && !stage.halo);
}
int main(void)
{
    emu_fw_options(0,1,0,1);emu_hal.ready=2;emu_fw_init(0);navigation();one_shot();clock_loss();click_and_preferences();display();
    printf("accompaniment workspace: %u checks, %u failures\n",checks,failures);return failures!=0;
}
