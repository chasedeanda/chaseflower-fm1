/* SPDX-License-Identifier: GPL-3.0-only
 * Real renderer, event handoff, clock, draft and MIDI ownership checks.
 */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0,int isr){(void)t0;(void)isr;ua_service();}
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;if(failures<40)printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void flush(void){cf_audio_block(0);cf_snapshot();cpa_ui_poll();cfl_poll();}
static void ticks(unsigned n){while(n--)cpa_tick1();cf_snapshot();cpa_ui_poll();cfl_poll();}
static void blocks(unsigned n){int32_t out[CTL*2];while(n--){audio_block(out,CTL);mo_r=mo_w;}cf_snapshot();cpa_ui_poll();cfl_poll();}
static void setup(unsigned idea)
{
    cpa_stop();sqp_stop();dmp_stop();cfp_panic();sqk.on=0;cu.keys=0;cfds.pending=cfds.unavailable=0;memset(&cfwd,0,sizeof cfwd);cfd_view_update();memset(&cfdu,0,sizeof cfdu);memset(&clu,0,sizeof clu);cfl_plan_cached=0;
    cu.scr=SCR_HOME;cu.page[SCR_PEDIT]=CFD_HARMONY;home_boot_main=1;cu.pop=POP_NONE;cpg.idea=idea;cpg.rhythm=CPA_OFF;cpg.mode=CPA_BOTH;cpg.repeat_off=0;
    const cf_progression_t *p=cf_progression_get(idea);cs.chord.tonic=p->tonic;cs.chord.scale=p->scale;cs.chord.quantize=CF_Q_BASIC;cs.clock_in=0;cs.beats=p->beats;cs.bpm=120;cs.bass_on=1;
    cf_harmony_defaults(&ch);ch.mode=CF_H_AUTO;cf_send_config();cf_send_metro();flush();cf_motion=0;cf_intro_on=0;cu.now=2000;cu.frame_dt=20;
}
static void screenshot(const char *name)
{
    char path[160];snprintf(path,sizeof path,"build/lab-tests/%s.ppm",name);cu.pop=POP_NONE;cf_ui_draw();FILE *f=fopen(path,"wb");CHECK(f!=0);if(!f)return;fprintf(f,"P6\n240 240\n255\n");
    for(unsigned i=0;i<240*240;i++){uint32_t c=cf_pal[fb[i]];uint8_t rgb[3]={c>>16,c>>8,c};fwrite(rgb,1,3,f);}fclose(f);
}
static void inspection(void)
{
    setup(0);unsigned writes=emu_flash_writes;cf_harmony_t harmony=ch;ui_button(B_SCL);CHECK(cu.scr==SCR_LAB && !clu.info.n);
    cf_post(CFE_KEY,7,0,1);flush();CHECK(clu.now_on && clu.info.n>=3);unsigned history=clu.history.n;
    cf_post(CFE_KEY,7,0,0);flush();CHECK(!clu.now_on && clu.have_last && clu.info.n && clu.history.n==history);
    cfl_model_t model;cfl_model(&model);CHECK(!strcmp(model.target,"Last"));
    ui_select(2);CHECK(clu.target==2 && clu.explicit_slot && clu.target_chord.n && !cpa.on && !cla.on && emu_flash_writes==writes && !memcmp(&harmony,&ch,sizeof ch));
    cf_lab_chord_t planned=clu.target_chord;ui_button(B_PLAY);flush();CHECK(cpa.on);ticks(cpa.plan.slot[0].ticks);CHECK(clu.target==2 && !memcmp(&planned,&clu.target_chord,sizeof planned));
    ui_button(B_PLAY);flush();
    /* Exact Bmaj7 spelling and factual outside-scale label from resolved notes. */
    clu.target=0;cf_lab_chord_t b={.root=59,.mask=0x891,.bass=-1};b.n=cf_voice(b.root,b.mask,0,b.note);
    cs.chord.tonic=4;cs.chord.scale=CF_SC_MAJOR;cf_sn.out.sounding=1;cf_sn.out.n=b.n;cf_sn.out.root_pc=11;cf_sn.out.root_note=59;cf_sn.out.mask=b.mask;cf_sn.out.bass_pc=-1;memcpy(cf_sn.out.note,b.note,b.n);cfl_poll();
    CHECK(!strcmp(clu.info.name,"Bmaj7") && clu.info.outside[3] && !strcmp(clu.info.note[3],"A#"));screenshot("notes");
    ui_algo(1);ui_knob_turn(1,3);CHECK(clu.tab==CFL_INTERVALS && clu.note==3);screenshot("intervals");
    /* Retargeting only voicing never adds committed history. */
    history=clu.history.n;cf_voice(b.root,b.mask,1,cf_sn.out.note);cfl_poll();CHECK(clu.history.n==history);
    cf_sn.out.sounding=0;cf_sn.out.n=0;cfl_poll();ui_algo(1);CHECK(clu.tab==CFL_COMPARE && cfl_comparisons());screenshot("compare");
}
static void audition(void)
{
    setup(0);ui_button(B_PLAY);flush();ui_button(B_SCL);ui_algo(3);ui_knob_turn(2,1);CHECK(clu.nchoices==3 && clu.explicit_slot);
    cf_song_draft_t draft=cfwd;cf_harmony_t harmony=ch;unsigned writes=emu_flash_writes;uint32_t t=cpa.t;
    ui_select(1);flush();CHECK(!cla.on && !memcmp(&draft,&cfwd,sizeof draft));ui_button(B_SCL);flush();CHECK(cla.on && cpa.on && cpa.t==t && !sqp.n && !sqp.nb);
    CHECK(cfl_last.playing==0);screenshot("next-audition");
    unsigned history=clu.history.n;ui_select(1);flush();CHECK(cla.on && clu.history.n==history && !memcmp(&ch,&harmony,sizeof ch));
    /* Cross a chord boundary before expiry: resume current slot, never old. */
    ticks(cpa.plan.beats*SQ_PPQN);CHECK(!cla.on && cpa.slot==1 && sqp.n && sqp.droot==cpa.plan.slot[1].root);
    CHECK(!memcmp(&draft,&cfwd,sizeof draft) && emu_flash_writes==writes);
    ui_button(B_SCL);flush();CHECK(cla.on);ticks(17);ui_button(B_HOME);flush();CHECK(!cla.on && cpa.on && cpa.t==17 && cu.scr==SCR_HOME);
    /* Live HOLD blocks audition and remains independent of stop/panic. */
    cf_post(CFE_KEY,7,0,1);cf_post(CFE_KEY,25,0,1);cf_post(CFE_KEY,25,0,0);cf_post(CFE_KEY,7,0,0);flush();CHECK(cfc.out.hold && cfp.n);
    uint8_t held[12];unsigned n=cfp.n;memcpy(held,cfp.note,n);ui_button(B_SCL);ui_algo(3);ui_button(B_SCL);flush();CHECK(!cla.on && cfc.out.hold && cfp.n==n && !memcmp(held,cfp.note,n));
    ui_button(B_PLAY);flush();CHECK(!cpa.on && cfp.n==n && cfc.out.hold);cfp_panic();flush();
    /* A root arriving after request publication wins on the audio side. */
    ui_button(B_SCL);cf_post(CFE_KEY,7,0,1);flush();CHECK(!cla.on && cfp.n);cf_post(CFE_KEY,7,0,0);flush();
    ui_button(B_SCL);flush();CHECK(cla.on);ui_button(B_PLAY);flush();CHECK(!cla.on);
    ui_button(B_PLAY);flush();ui_button(B_SCL);flush();CHECK(cla.on);cf_post(CFE_PANIC,0,0,0);flush();CHECK(!cla.on && !cpa.on && !cfp.n && !sqp.n);
}
static void insertion(void)
{
    setup(0);ui_button(B_SCL);ui_algo(3);cf_song_draft_t before=cfwd;ui_button(B_EDIT);CHECK(cu.scr==SCR_LAB && !memcmp(&before,&cfwd,sizeof before));
    ui_knob_turn(2,2);unsigned selected=clu.insert;uint32_t mask=clu.choice[clu.candidate].mask;unsigned degree=clu.choice[clu.candidate].degree;ui_button(B_EDIT);CHECK(cu.scr==SCR_PEDIT && cfwd.dirty && cfwd.can_undo && cfdu.focus==selected);
    CHECK(cfwd.song.slot[selected].mask==mask && cfwd.song.slot[selected].degree==degree);
    cf_song_t original=cfwd.previous;cu.page[SCR_PEDIT]=CFD_PHRASE;cfdu.action=3;ui_button(B_SCL);CHECK(!memcmp(&original,&cfwd.song,sizeof original));
    /* A different source cannot silently replace a protected dirty draft. */
    ui_button(B_HOME);cf_progression_choose(1);flush();ui_button(B_SCL);ui_algo(3);ui_knob_turn(2,1);before=cfwd;ui_button(B_EDIT);CHECK(cu.scr==SCR_LAB && !memcmp(&before,&cfwd,sizeof before));
    setup(0);ui_button(B_EDIT);cf_song_t next=cfwd.song;while(next.n<16)cf_song_insert(&next,next.n,0);CHECK(cfd_commit(&next));cfd_select_source();flush();ui_button(B_HOME);ui_button(B_SCL);ui_select(17);CHECK(clu.target==17 && clu.insert==15 && clu.explicit_slot);ui_algo(3);ui_knob_turn(1,1);screenshot("next");ui_button(B_EDIT);CHECK(cu.scr==SCR_PEDIT && cfdu.focus==15 && cfwd.song.n==16);
}
static void comparison_and_clock(void)
{
    setup(0);ui_button(B_SCL);ui_select(2);ui_algo(2);CHECK(!cfl_comparisons());ui_button(B_SCL);flush();CHECK(!cla.on);
    ui_knob_turn(0,1);CHECK(cfl_comparisons());ui_knob_turn(1,1);ui_button(B_SCL);flush();CHECK(cla.on && !cpa.on);unsigned ticks=cla.ticks;blocks(FS/CTL*3);CHECK(!cla.on && ticks==cs.beats*SQ_PPQN);
    for(unsigned beats=2;beats<=7;beats++) {
        setup(0);cs.beats=beats;cf_send_metro();flush();cf_lab_chord_t c={.root=48,.mask=0x91,.bass=-1};c.n=cf_voice(c.root,c.mask,0,c.note);cla_request(&c);flush();CHECK(cla.on);
        unsigned duration=FS*60u*(beats==6?3u:beats)/cs.bpm;blocks(duration/CTL-2);CHECK(cla.on);blocks(5);CHECK(!cla.on);
    }
    setup(0);cs.clock_in=1;cf_send_config();flush();ui_button(B_PLAY);flush();ui_button(B_SCL);ui_algo(3);ui_button(B_SCL);flush();CHECK(!cla.on && cpa.clock_wait);
    cpa_realtime(0xFA);flush();ui_button(B_SCL);flush();CHECK(cla.on);cpa_realtime(0xFC);flush();CHECK(!cla.on && cpa.paused);
    cpa_realtime(0xFB);flush();ui_button(B_SCL);flush();CHECK(cla.on);cf_now_ms+=301;cpa_tick(0,0);flush();CHECK(!cla.on && cpa.clock_lost);
}
static void settled_and_unsupported(void)
{
    setup(0);ui_button(B_SCL);ui_select(2);
    for(unsigned tab=0;tab<4;tab++) {
        clu.tab=tab;cu.pop=POP_NONE;cf_ui_draw();unsigned hash=cfs_fnv(fb,FB_W*FB_H),draws=cfl_draws,tiles=fb_tiles_sent;
        for(unsigned i=0;i<10;i++){cu.now+=100;cf_ui_draw();}
        CHECK(hash==cfs_fnv(fb,FB_W*FB_H) && draws==cfl_draws && tiles==fb_tiles_sent);
    }
    cs.chord.scale=CF_SC_WHOLE;cf_send_config();flush();clu.context=0;clu.nchoices=0;cfl_poll();CHECK(!clu.nchoices);screenshot("unsupported");
    ui_button(B_SCL);flush();CHECK(!cla.on);cf_song_draft_t draft=cfwd;ui_button(B_EDIT);CHECK(!memcmp(&draft,&cfwd,sizeof draft));
    /* Old explicit chord-button editor remains reachable through Harmony. */
    ui_button(B_GLO);cu.page[SCR_GLO]=CF_GLO_HARMONY;ui_button(B_SCL);CHECK(cu.scr==SCR_SEL);
}
static void render_audio(unsigned browse,const char *path)
{
    setup(0);cpg.rhythm=dm_nth(cs.beats,0);cf_boot_go=0;cu.pop=POP_NONE;
    FILE *f=fopen(path,"wb");CHECK(f!=0);if(!f)return;ui_button(B_PLAY);flush();unsigned history=0;
    int32_t out[CTL*2];for(unsigned b=0;b<FS*5u/CTL;b++) {
        if(browse && b==FS/CTL){ui_button(B_SCL);ui_select(2);}
        if(browse && b==FS*2u/CTL)ui_algo(1);
        if(browse && b==FS*3u/CTL)ui_algo(1);
        if(browse && b==FS*4u/CTL)ui_algo(1);
        audio_block(out,CTL);cf_snapshot();cpa_ui_poll();cfl_poll();
        if(browse && cu.scr==SCR_LAB && b%100==0){ui_select(1);cf_ui_draw();}
        CHECK(!cla.on && !cfwd.have);ua.cr=ua.cw;mo_r=mo_w;fwrite(out,sizeof out,1,f);history=clu.history.n;
    }
    CHECK(history && cpa.on);fclose(f);ui_button(B_PLAY);flush();
}
int main(int argc,char **argv)
{
    emu_fw_options(0,1,0,1);emu_hal.ready=2;emu_fw_init(0);if(argc>1){ua_reset();ua.cap_alt=1;ua.cap_hold=0;ua_rate_set(atoi(argv[1]));}
    if(argc==4)render_audio(atoi(argv[2]),argv[3]);
    else {inspection();audition();insertion();comparison_and_clock();settled_and_unsupported();}
    printf("Chord Lab UI/ownership USB %u: %u checks, %u failures\n",argc>1?atoi(argv[1]):44100,checks,failures);return failures!=0;
}
