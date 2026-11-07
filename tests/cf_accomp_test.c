/* SPDX-License-Identifier: GPL-3.0-only
 * Real firmware event/ownership/clock integration, no physical key simulation
 * inside the player. Tests use keys only to exercise independent live playing.
 */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; if (failures < 20) printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)
static void flush(void) { cf_audio_block(0); cf_snapshot(); cpa_ui_poll(); }
static void ticks(unsigned n) { while (n--) { cpa_tick1(); mo_r = mo_w; } cf_snapshot(); cpa_ui_poll(); }
static void setup(unsigned idea, unsigned rhythm)
{
    cpa_stop(); sqp_stop(); dmp_stop(); cfp_panic();
    cu.scr = SCR_HOME; cf_scene_set(CF_SCENE_PROGRESSIONS);
    cpg.idea = idea; cpg.rhythm = rhythm; cpg.mode = CPA_BOTH;
    const cf_progression_t *p = cf_progression_get(idea);
    cs.chord.tonic = p->tonic; cs.chord.scale = p->scale; cs.beats = p->beats;
    cs.chord.quantize = CF_Q_BASIC; cs.bpm = 120; cs.clock_in = 0; cs.bass_on = 1;
    cf_send_config(); cf_send_metro(); flush();
}
static void catalogue(void)
{
    static const unsigned modes[] = {CF_SC_MAJOR,CF_SC_MINOR,CF_SC_DORIAN,CF_SC_PHRYGIAN,
                                    CF_SC_LYDIAN,CF_SC_MIXOLYDIAN,CF_SC_LOCRIAN};
    cpa_plan_t plan;
    for (unsigned i=0; i<cf_progression_count(); i++) {
        setup(i, CPA_OFF);
        for (unsigned m=0; m<7; m++) for (unsigned t=0; t<12; t++) {
            cf_chord_cfg_t cfg=cs.chord; cfg.scale=modes[m]; cfg.tonic=t;
            CHECK(cpa_prepare(&plan,i,cfg,CPA_OFF,CPA_BOTH));
            for (unsigned s=0;s<plan.n;s++) {
                CHECK(plan.slot[s].root%12==(t+cf_scale_step(modes[m],plan.degree[s]))%12);
                CHECK(plan.slot[s].n>=3 && plan.nb==1);
                CHECK((plan.slot[s].mask&(1u<<7)) &&
                      !!(plan.slot[s].mask&(1u<<3)) != !!(plan.slot[s].mask&(1u<<4)));
            }
        }
        ui_button(B_PLAY); flush(); CHECK(cpa.on && !sqp.on && !dmp.on && sqp.dn);
        unsigned n=cf_progression_get(i)->n, bt=cs.beats*96;
        for(unsigned s=0;s<n;s++) {
            CHECK(cpa.slot==s && sqp.droot==cpa.plan.slot[s].root);
            ticks(bt-1); CHECK(cpa.slot==s); ticks(1);
        }
        CHECK(cpa.slot==0); ui_button(B_PLAY); flush(); CHECK(!cpa.on && !sqp.n && !sqp.nb);
    }
    setup(54,CPA_OFF); ui_button(B_PLAY); flush();
    unsigned root=sqp.droot; CHECK(cf_progression_live(cf_progression_get(54),cs.chord.scale)==3);
    ticks(384); CHECK(cpa.slot==1 && cf_sn.ac_slot==1 && sqp.droot==root);
    CHECK(cf_progression_live(cf_progression_get(54),cs.chord.scale)==3);
    ui_button(B_PLAY); flush();
    CHECK(sizeof(cf_set_t)==188 && sizeof(sq_ev_t)==22 && sizeof(cf_harmony_t)==38);
    CHECK(sizeof(cpa_plan_t)<=152);
    plan.n=0; CHECK(!cpa_valid(&plan));
}
static void queues(void)
{
    setup(0, dm_nth(4,0)); ui_button(B_PLAY); flush();
    unsigned bt=384, bpm=cs.bpm; ticks(77);
    unsigned oldr=cpa.plan.rhythm; cpa_ui_rhythm(1); cpa_ui_rhythm(1); cpa_ui_rhythm(1);
    unsigned r=cpa_edit.plan.rhythm; CHECK(r!=oldr && cpa.plan.rhythm==oldr);
    unsigned origin=cpa.plan.tonic; ui_knob_turn(1,1); ui_knob_turn(1,1); ui_knob_turn(1,1);
    CHECK(cpa_edit.plan.tonic==(origin+3)%12 && cs.chord.tonic==origin);
    ui_select(11); ui_select(2); CHECK(cpa_setup.plan.idea==13 && cpg.idea==0);
    CHECK(cpa_setup.plan.beats==6 && cpa_setup.plan.tonic==7);
    ui_knob_turn(1,2); CHECK(cpa_edit.plan.idea==0 && cpa_edit.plan.tonic==(origin+5)%12);
    CHECK(cpa_setup.plan.tonic==7); /* knob edits cannot retarget the pending idea */
    cf_snapshot(); CHECK(cf_sn.ac_next==13 && cf_sn.ac_idea==0);
    ticks(bt-78); CHECK(cpa.slot==0 && cpg.idea==0 && cpa.plan.rhythm==oldr);
    ticks(1); CHECK(cpa.slot==0 && cpg.idea==13 && cs.beats==6 && cs.chord.tonic==7);
    CHECK(!cpa_setup.flags && !cpa_edit.flags && cs.bpm==bpm);
    /* Active edits coalesce independently without an idea switch. */
    cpa_ui_rhythm(1); cpa_ui_rhythm(1); r=cpa_edit.plan.rhythm;
    ui_knob_turn(1,1); ui_knob_turn(1,1); ui_knob_turn(2,1);
    unsigned tonic=cpa_edit.plan.tonic, scale=cpa_edit.plan.scale;
    CHECK(cs.chord.tonic==7 && cs.chord.scale==CF_SC_MAJOR);
    ticks(576); CHECK(cpa.slot==1 && cpa.plan.rhythm==r && cs.chord.tonic==tonic && cs.chord.scale==scale);
    CHECK(!dmp.on || DM_RHY[dmp.r].beats==6);
    /* Explicit mode and Off/Click do not silence the chord clock. */
    cpa_ui_mode(1); ticks(576); CHECK(cpa.plan.mode==CPA_DRUMS && !sqp.n && !sqp.nb && !sqp.dn);
    cpa_ui_mode(-1); ticks(576); CHECK(cpa.plan.mode==CPA_BOTH && sqp.n && sqp.nb);
    cpa_plan_t p; cpa_active_request(&p); p.rhythm=CPA_OFF; cpa_publish(&p,CPA_GROOVE,0);
    ticks(576); CHECK(!dmp.on && sqp.n && cfm.running);
    cpa_active_request(&p); p.rhythm=CPA_CLICK; cpa_publish(&p,CPA_GROOVE,0);
    ticks(576); CHECK(!dmp.on && sqp.n && cpa.plan.rhythm==CPA_CLICK);
    for(unsigned i=0;i<300;i++) ui_knob_turn(1,1);
    unsigned latest=cpa_edit.plan.tonic;
    flush(); ticks(576); CHECK(cpa.plan.tonic==latest);
    cu.scr=SCR_GLO; ui_button(B_PLAY); flush(); CHECK(!cpa.on && !cfm.running);
}
static void clocking(void)
{
    setup(0,CPA_OFF); ui_button(B_PLAY); flush();
    cpa_tick(FS/2,0); CHECK(cpa.t==96 && cpa.slot==0); /* quarter at 120 */
    uint32_t phase=cpa.acc, gen=cf_grid.gen;
    ui_knob_turn(3,120); flush(); CHECK(cpa.t==96 && cpa.acc==phase && cf_grid.gen==gen);
    cpa_tick(FS/4,0); CHECK(cpa.t==192); /* quarter at 240, no restart */
    setup(11,CPA_OFF); ui_button(B_PLAY); flush();
    cpa_tick(FS,0); CHECK(cpa.t==384 && cpa.slot==0);
    cpa_tick(FS/2,0); CHECK(cpa.t==0 && cpa.slot==1); /* 6/8 at 120 = 1.5 s */
    cpa_stop(); cs.clock_in=1; cf_send_config(); flush(); ui_button(B_PLAY); flush();
    unsigned bpm=cs.bpm; ui_knob_turn(3,40); CHECK(cs.bpm==bpm);
    cf_knob_t knob={0}; ui_knob_info(3,&knob); CHECK(knob.dis && strstr(knob.val,"EXT"));
    cpa_tick(FS,0); CHECK(cpa.t==0 && cpa.slot==0); /* no implicit clock fallback */
    cpa_tick(0,71); CHECK(cpa.t==568 && cpa.slot==0);
    cpa_tick(0,1); CHECK(cpa.slot==1 && cpa.t==0); /* 72 MIDI quarters/24 pulses per 6/8 bar */
    cpa_tick(0,7); unsigned pos=cpa.t;
    cpa_realtime(0xFC); CHECK(cpa.paused && !sqp.n && !sqp.nb && !cf_grid.on);
    cpa_tick(FS,24); CHECK(cpa.t==pos);
    cpa_realtime(0xFB); cpa_tick(0,1); CHECK(cpa.t==pos+8 && sqp.n);
    CHECK(emu_fw_midi_in(0x0000FA0Fu)); flush();
    CHECK(cpa.slot==0 && cpa.t==0 && sqp.droot==cpa.plan.slot[0].root);
    CHECK(emu_fw_midi_in(0x0000F80Fu)); flush(); CHECK(cpa.t==8);
    CHECK(emu_fw_midi_in(0x0000FC0Fu)); flush(); CHECK(cpa.paused && !sqp.n);
    CHECK(emu_fw_midi_in(0x0000F80Fu));
    CHECK(emu_fw_midi_in(0x0000FB0Fu)); flush(); CHECK(!cpa.paused && cpa.t==8 && sqp.n);
    cpa_stop(); cs.clock_in=0; cf_send_config(); flush();
    setup(0,CPA_OFF); cs.clock_in=1; cf_send_config(); flush(); ui_button(B_PLAY); flush();
    cpa_tick(0,95); CHECK(cpa.t==380);
    ui_select(11); cpa_tick(0,2); CHECK(sq_beats==6 && sq_dsteps()==12); cf_snapshot(); cpa_ui_poll();
    CHECK(cpa.plan.beats==6 && cpa.slot==0 && cpa.t==8 && cf_grid.g24==121);
    cpa_tick(0,70); CHECK(cpa.t==568);
    ui_select(-11); cpa_tick(0,2); CHECK(sq_beats==4 && sq_dsteps()==16); cf_snapshot(); cpa_ui_poll();
    CHECK(cpa.plan.beats==4 && cpa.slot==0 && cpa.t==4);
    cpa_stop(); cs.clock_in=0; cf_send_config(); flush();
}
static unsigned count_off(unsigned part,unsigned note)
{
    unsigned n=0;
    for(unsigned i=mo_r;i<mo_w;i++) { uint32_t p=midi_out_q[i%MQ];
        if(((p>>8)&255)==(0x80u|cfp_ch(part)) && ((p>>16)&127)==note) n++;
    }
    return n;
}
static void ownership(void)
{
    setup(0,CPA_OFF); ch.mode=CF_H_AUTO; ch.slot[0]=CF_MOD_7; cf_send_config(); flush();
    cf_post(CFE_KEY,1,0,1); cf_post(CFE_KEY,1,0,0); /* deliberate seventh */
    cf_post(CFE_KEY,25,0,1); cf_post(CFE_KEY,25,0,0); /* live HOLD */
    cf_post(CFE_KEY,7,0,1); cf_post(CFE_KEY,7,0,0); flush();
    CHECK(cfc.out.hold && cfp.n==4); usb.config=1; cfp_cfg.midi_out=1; mo_r=mo_w;
    ui_button(B_PLAY); flush();
    CHECK(cpa.plan.slot[0].mask==(1u|1u<<4|1u<<7|1u<<11));
    CHECK(cpa.plan.slot[1].root%12==11 && cpa.plan.slot[1].mask==(1u|1u<<4|1u<<7|1u<<11));
    /* Degree VII in E uses D# minor seventh, not diminished/dominant. */
    uint16_t used, conflict;
    CHECK(cf_harmony_mask(&ch,CF_SC_MAJOR,4,3,1,cfc.stamp,&used,&conflict)==(1u|1u<<3|1u<<7|1u<<10));
    unsigned live[12], ln=cfp.n; for(unsigned j=0;j<ln;j++)live[j]=cfp.note[j];
    ticks(384); CHECK(sqp.droot%12==11); mo_r=mo_w;
    ui_button(B_PLAY); flush(); CHECK(cfc.out.hold && cfp.n==ln && !sqp.n && !sqp.nb);
    for(unsigned j=0;j<ln;j++) CHECK(!count_off(0,live[j]) && cfp_holds(0,live[j]));
    for(unsigned i=0;i<16;i++) { ui_button(B_PLAY); flush(); ui_button(B_PLAY); flush(); CHECK(!sqp.n && cfp.n==ln); }
    ui_button(B_PLAY); flush(); cf_post(CFE_PANIC,0,0,0); flush(); CHECK(!cpa.on && !sqp.n && !cfp.n);
    usb.config=0;

}
static void rapid_transport(void)
{
    setup(0,CPA_OFF);
    ui_button(B_PLAY); cu.scr=SCR_GLO; ui_button(B_PLAY); flush();
    CHECK(!cpa.on && !cpa_commanded && !sqp.on && !dmp.on);
    cu.scr=SCR_HOME; ui_button(B_PLAY); ui_button(B_PLAY); ui_button(B_PLAY); flush();
    CHECK(cpa.on && cpa_commanded && cpa.slot==0 && sqp.n);
    ui_button(B_PLAY); ui_button(B_PLAY); flush(); CHECK(cpa.on && cpa.slot==0);
    ui_button(B_PLAY); flush(); CHECK(!cpa.on && !sqp.n && !sqp.nb);
}
static void browsing(void)
{
    cpa_stop(); cf_browser_order(&cpb,CPB_CATALOG,1); setup(0,CPA_OFF);
    cu.scr=SCR_GLO; cu.page[SCR_GLO]=CF_GLO_BROWSE;
    ui_knob_turn(0,7); ui_knob_turn(1,2); flush(); CHECK(cpg.idea==72); /* #73 in nine detents */
    cf_knob_t knob={0}; ui_knob_info(0,&knob); CHECK(!strcmp(knob.val,"73"));
    unsigned seen[CPB_MAX]={0}; cf_browser_order(&cpb,CPB_ALPHA,1);
    for(unsigned i=0;i<cpb.n;i++) {
        CHECK(cpb.order[i]<cf_progression_count() && !seen[cpb.order[i]]++);
        if(i)CHECK(cpb_compare(cpb.order[i-1],cpb.order[i])<=0);
    }
    cf_browser_order(&cpb,CPB_SHUFFLE,2); memset(seen,0,sizeof seen);
    unsigned start=cpg.idea;
    for(unsigned i=0;i<cf_progression_count();i++) { ui_select(1); flush(); CHECK(!seen[cpg.idea]++); }
    CHECK(cpg.idea==start);
    uint8_t old[CPB_MAX]; memcpy(old,cpb.order,sizeof old);
    cf_browser_order(&cpb,CPB_SHUFFLE,3); CHECK(memcmp(old,cpb.order,cpb.n));
    memset(cpb.favorites,0,sizeof cpb.favorites);
    cf_browser_star(&cpb,72,1); cf_browser_star(&cpb,4,1);
    cf_browser_order(&cpb,CPB_FAVORITES,1); CHECK(cpb.n==2);
    ui_select(1); flush(); CHECK(cpg.idea==4 || cpg.idea==72);
    unsigned first=cpg.idea; ui_select(1); flush(); CHECK(cpg.idea!=first); ui_select(1); flush(); CHECK(cpg.idea==first);
    cpb_changed_ui(); cpb_flush(); CHECK(!cpb_dirty);
    memset(&cpb,0,sizeof cpb); cpb_load(); CHECK(cpb.mode==CPB_FAVORITES && cpb.n==2 && cf_browser_favorite(&cpb,72));
    for(unsigned obj=0;obj<OBJ_COUNT;obj++)if(obj!=OBJ_CFPREFS)
        for(unsigned copy=0;copy<2;copy++)CHECK(st_sector(obj,copy)!=st_sector(OBJ_CFPREFS,copy));
    /* Short SAVE preserves sound save; long SAVE stars the visible active idea once. */
    cf_browser_order(&cpb,CPB_CATALOG,1); setup(0,CPA_OFF); cu.now=100;
    fm1_in.buttons=1u<<panel.btn[B_SAVE]; ui_button(B_SAVE); cu.now+=701; cpb_hold_poll();
    CHECK(cf_browser_favorite(&cpb,0) && cu.scr==SCR_HOME);
    cu.now+=1000; cpb_hold_poll(); CHECK(cf_browser_favorite(&cpb,0));
    fm1_in.buttons=0; cpb_hold_poll(); CHECK(cu.scr==SCR_HOME);
    fm1_in.buttons=1u<<panel.btn[B_SAVE]; ui_button(B_SAVE); cu.now+=10;
    fm1_in.buttons=0; cpb_hold_poll(); CHECK(cu.scr==SCR_SAVE);
    cu.scr=SCR_HOME; fm1_in.buttons=1u<<panel.btn[B_GLO]; ui_button(B_GLO); cu.now+=701;
    cpb_hold_poll(); CHECK(cpb_page()); fm1_in.buttons=0; cpb_hold_poll();
    /* Direct jumps and shuffled browsing retain active setup until the bar. */
    setup(0,CPA_OFF); ui_button(B_PLAY); flush(); cu.scr=SCR_GLO; cu.page[SCR_GLO]=CF_GLO_BROWSE;
    ui_knob_turn(0,7); ui_knob_turn(1,2); CHECK(cpa_setup.plan.idea==72 && cpg.idea==0);
    ticks(384); CHECK(cpg.idea==72 && cpa.slot==0);
    cf_browser_order(&cpb,CPB_SHUFFLE,4); ui_select(1);
    unsigned next=cpa_setup.plan.idea; CHECK(next!=72 && cpg.idea==72); ticks(cs.beats*96); CHECK(cpg.idea==next);
    ui_button(B_PLAY); flush(); cpb_flush(); cf_browser_order(&cpb,CPB_CATALOG,1); cpb_dirty=0;
}
static void storage(void)
{
    cpa_stop(); setup(0,CPA_OFF);
    uint32_t patterns=cfs_fnv(&sq_st,sizeof sq_st), drums=cfs_fnv(&dmr,sizeof dmr);
    cpg.explored=0; cfs_saved=cfs_seen=cfs_fnv(&cs,sizeof cs); cfs_tidy=1;
    for (unsigned i=0;i<4;i++) { cu.now+=10000; settings_poll(); }
    unsigned writes=emu_flash_writes;
    for(unsigned i=0;i<cf_progression_count();i++) { ui_select(1); flush(); cu.now+=10000; settings_poll(); }
    cf_set_t image=cs; cfs_performance_image(&image);
    CHECK(cfs_fnv(&image,sizeof image)==cfs_saved);
    CHECK(cfs_fnv(&sq_st,sizeof sq_st)==patterns && cfs_fnv(&dmr,sizeof dmr)==drums);
    CHECK(emu_flash_writes==writes);
}
static void render_audio(unsigned capture, unsigned rate, const char *path)
{
    FILE *f=fopen(path,"wb"); CHECK(f); if(!f)return;
    setup(0,dm_nth(4,0)); cf_boot_go=0; cu.pop=POP_NONE;
    ua_reset(); ua.cap_alt=capture; ua.cap_hold=0; ua_rate_set(rate);
    uint32_t patterns=cfs_fnv(&sq_st,sizeof sq_st); ui_button(B_PLAY);
    int32_t out[CTL*2]; unsigned lastslot=255;
    for(unsigned b=0;b<FS*6u/CTL;b++) {
        if (b==FS/CTL) ui_knob_turn(3,40);
        if (b==FS*2u/CTL) { ui_select(11); ui_knob_turn(1,2); }
        audio_block(out,CTL); cf_snapshot(); cpa_ui_poll();
        if(capture && lastslot!=cpa.slot) { cf_ui_draw(); lastslot=cpa.slot; }
        if(capture) ua.cr=ua.cw;
        fwrite(out,sizeof out,1,f);
    }
    CHECK(cfs_fnv(&sq_st,sizeof sq_st)==patterns && cpa.on && cpg.idea==11);
    ui_button(B_PLAY); flush(); CHECK(!sqp.n && !sqp.nb && !cpa.on);
    fclose(f);
}
static void shots(const char *dir)
{
    const unsigned ideas[]={0,78,72};
    for(unsigned i=0;i<3;i++) {
        setup(ideas[i],CPA_OFF); cf_boot_go=0; cu.pop=POP_NONE;
        ui_button(B_PLAY); flush(); ticks(384); ui_select(11); cf_snapshot();
        cu.pop=POP_NONE;
        if(i==2) { cpa_stop(); cu.scr=SCR_GLO; cu.page[SCR_GLO]=CF_GLO_BROWSE; cf_browser_star(&cpb,72,1); }
        cf_ui_draw();
        char path[512]; snprintf(path,sizeof path,"%s/playing-%u.ppm",dir,i);
        FILE *f=fopen(path,"wb"); CHECK(f); if(!f)continue;
        fprintf(f,"P6\n240 240\n255\n");
        for(unsigned j=0;j<FB_W*FB_H;j++) {
            uint32_t rgb=cf_pal[fb[j]&63]; uint8_t px[3]={rgb>>16,rgb>>8,rgb}; fwrite(px,3,1,f);
        }
        fclose(f); cpa_stop();
    }
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0); cf_intro_on=0;
    if(argc==3 && !strcmp(argv[1],"--shots")) shots(argv[2]);
    else if(argc==5 && !strcmp(argv[1],"--audio")) render_audio(atoi(argv[2]),atoi(argv[3]),argv[4]);
    else { catalogue(); queues(); clocking(); ownership(); rapid_transport(); browsing(); storage(); }
    printf("accompaniment: %u checks, %u failures; plan %zu bytes, runtime+mailboxes %zu bytes\n",
           checks,failures,sizeof(cpa_plan_t),sizeof cpa+sizeof cpa_setup+sizeof cpa_edit);
    return failures!=0;
}
