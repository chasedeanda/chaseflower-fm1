/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t, int isr) { (void)t; (void)isr; ua_service(); }
static unsigned checks, failures, test_ms;
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n",__LINE__,#x); } } while (0)
static void advance(unsigned ms)
{
    int16_t out[HALF_FRAMES*2];
    for (unsigned t=0;t<ms;t+=3) {
        test_ms+=3;emu_fw_tick(test_ms);emu_fw_audio(out,HALF_FRAMES);
        if(test_ms%33==0)emu_fw_frame();
        cf_snapshot();cpa_ui_poll();sq_rec_poll();sq_home_poll();ua.cr=ua.cw;mo_r=mo_w;
    }
}
static void key(unsigned k, int down) { cf_post(CFE_KEY,k,0,down); advance(12); }
static void rec(void) { while(emu_stall_blocks)advance(3); ui_button(B_REC); fm1_in.buttons=0; cf_rec_poll(); advance(12); }
static void stop(void) { cf_post(CFE_PANIC,0,0,0); advance(12); }
static void empty(void)
{
    stop(); sq_defaults(); sq_unavailable=0; sq_dirty=0; sqk.on=0; sqk.last=0xFF;
    memset(emu_flash+0xE6000,0xFF,0x8000); sq_load(); cfs_restore_lock=0;
}
static void record_flow(unsigned rate)
{
    empty(); sq_pref.protect=1; ua_reset(); ua.cap_alt=1; ua.cap_hold=0; ua_rate_set(rate);
    cu.scr=SCR_HOME; home_boot_main=1; cf_home_bank=CF_HB_CORE; cf_scene_set(CF_SCENE_PROGRESSIONS);
    cpg.idea=0; cpg.rhythm=dm_nth(4,0); cpg.mode=CPA_BOTH;
    cs.bpm=120; cs.beats=4; cs.chord.tonic=0; cs.chord.scale=CF_SC_MAJOR;
    cs.chord.quantize=CF_Q_BASIC; cs.clock_in=0; ch.mode=CF_H_AUTO; cu.arp_on=0;
    cf_send_config(); cf_send_metro(); advance(12);
    ui_button(B_PLAY); advance(60); CHECK(cpa.on && sqp.n);
    unsigned rhythm=cpg.rhythm, kit=cpa.plan.kit, swing=cpa.plan.swing;
    rec(); CHECK(sqk.on && !cpa.on && !sqp.n && !sqp.nb && sqp.rec==SQ_CHORDS && sqp.cin);
    CHECK(sq_meta[0].bpm==120 && sq_meta[0].beats==4 && sq_st.pat[0].kit==kit && sq_st.pat[0].swing==swing);
    sq_pat_t expected; sq_fresh(&expected,4);dm_fill(&expected,rhythm,0);
    CHECK(!memcmp(sq_st.pat[0].hit,expected.hit,sizeof expected.hit));
    unsigned guard=0; while(sqp.cin && guard++<2000)advance(3); CHECK(!sqp.cin);
    key(7,1); uint8_t notes[6], n=cfc.out.n; memcpy(notes,cfc.out.note,sizeof notes);
    advance(400); key(7,0);
    guard=0; while(sqp.nbar<1 && guard++<2000)advance(3); CHECK(sqp.nbar==1);
    key(12,1); advance(400); key(12,0); rec();
    CHECK(!sqk.on && sqp.on && sq_st.pat[0].bars==2 && sq_st.pat[0].nev>=2);
    CHECK(sq_st.pat[0].ev[0].rootkey==0xFF && sq_st.pat[0].ev[0].n==n &&
          !memcmp(sq_st.pat[0].ev[0].note,notes,n));
    stop(); sq_save_write(); unsigned hash=cfs_fnv(&sq_st.pat[0],sizeof(sq_pat_t));
    sq_load(); CHECK(cfs_fnv(&sq_st.pat[0],sizeof(sq_pat_t))==hash && sq_meta[0].bpm==120);
    cs.bpm=77;cs.beats=3;cf_send_metro();advance(12);cu.scr=SCR_SQ;squ.pat=0;
    ui_button(B_PLAY);advance(90);CHECK(sqp.on && cfm.bpm==120 && cfm.beats==4 && sq_beats==4);
    stop();
    sq_st.pat[1]=sq_st.pat[0];sq_meta[1]=(sq_take_meta_t){90,3,0};sq_meta_known|=2;
    sqp_start(0,0);sqp_queue(1,0);CHECK(sqp.next==1);
    sqp_next_bar();cf_snapshot();sq_tempo_poll();
    CHECK(sqp.pat==1 && cfm.bpm==90 && cfm.beats==3 && cs.bpm==90 && cs.beats==3);
    stop();
    /* No history/redo: holding REC when stopped cannot change a take. */
    sq_home_cancel();CHECK(cfs_fnv(&sq_st.pat[0],sizeof(sq_pat_t))==hash);
    for(unsigned i=1;i<SQ_NPAT;i++){sq_st.pat[i]=sq_st.pat[0];sq_meta[i]=sq_meta[0];}
    sq_pref.protect=1;cu.scr=SCR_HOME; unsigned before=cfs_fnv(&sq_st,sizeof sq_st);rec();
    CHECK(!sqk.on && cfs_fnv(&sq_st,sizeof sq_st)==before && !cpa.on);
    ui_button(B_PLAY);advance(90);CHECK(cpa.on);rec();CHECK(cpa.on && !sqk.on);
    stop();
    /* The actual held-SAVE gesture clears only the selected stopped take. */
    cu.scr=SCR_SQ;squ.pat=1;ui_button(B_SAVE);fm1_in.buttons=1u<<panel.btn[B_SAVE];
    cu.now=sq_clear_down+2999;test_ms=cu.now;fm1_ms=test_ms;sq_clear_poll();CHECK(sq_has(1));
    cu.now++;test_ms=cu.now;fm1_ms=test_ms;sq_clear_poll();fm1_in.buttons=0;CHECK(!sq_has(1) && sq_has(0) && sq_has(2));
    sq_save_write();sq_load();CHECK(!sq_has(1) && sq_has(0) && sq_has(2));
    cu.scr=SCR_HOME;rec();CHECK(sqk.on && sqk.pat==1 && sqp.cin);
    sq_home_cancel();advance(12);CHECK(!sqk.on && !sq_has(1) && !sqp.on);
    sq_save_write();sq_load();CHECK(!sq_has(1));
    cu.scr=SCR_HOME;dm_view=dm_nth(3,0);rec();CHECK(sqk.on && sqk.pat==1 && sqk.rhy==rhythm+1);
    guard=0;while(sqp.cin && guard++<2000)advance(3);
    key(7,1);advance(300);sq_home_cancel();advance(30);key(7,0);advance(60);
    CHECK(!sqk.on && !sqp.on && !sq_has(1));
    sq_save_write();sq_load();CHECK(!sq_has(1));
}
static void synthetic_take(unsigned note)
{
    cu.scr=SCR_HOME;rec();CHECK(sqk.on);
    sq_pat_t *p=&sq_st.pat[sqk.pat];
    p->ev[0]=(sq_ev_t){.trk=SQ_CHORDS,.n=1,.note={note},.rootkey=0xFF,.len=24};p->nev=1;
    sqk.rolled=sqk.seen=1;sqk.nbar=0;sqk.t=24;sqk.bt=96;
    sq_home_end(0);stop();sq_save_write();
    CHECK(sq_number(sqk.pat)==1 && sq_st.pat[sq_view_slot(0)].ev[0].note[0]==note);
}
static void history(void)
{
    empty();CHECK(!sq_pref.protect);
    cpg.idea=0;cpg.rhythm=dm_nth(4,0);cs.beats=4;cs.bpm=120;
    synthetic_take(61);synthetic_take(62);synthetic_take(63);
    CHECK(sq_st.pat[sq_view_slot(0)].ev[0].note[0]==63 &&
          sq_st.pat[sq_view_slot(1)].ev[0].note[0]==62 &&
          sq_st.pat[sq_view_slot(2)].ev[0].note[0]==61);
    unsigned oldest=sq_view_slot(2),before=cfs_fnv(&sq_st,sizeof sq_st);
    cfps_set(oldest,0,&cf_snd[0]);cf_sref_t ref=cfps.r[oldest][0];
    cu.scr=SCR_HOME;rec();CHECK(sqk.on && sqk.pat==oldest && sq_number(oldest)==1);
    CHECK(crb_busy());unsigned writes=emu_flash_writes;sq_save_write();CHECK(emu_flash_writes==writes);
    sq_home_cancel();advance(90);
    CHECK(!crb_busy());CHECK(cfs_fnv(&sq_st,sizeof sq_st)==before && !memcmp(&cfps.r[oldest][0],&ref,sizeof ref));
    synthetic_take(64);sq_load();
    CHECK(sq_st.pat[sq_view_slot(0)].ev[0].note[0]==64 &&
          sq_st.pat[sq_view_slot(1)].ev[0].note[0]==63 &&
          sq_st.pat[sq_view_slot(2)].ev[0].note[0]==62);
    cu.scr=SCR_SQ;squ.pat=sq_view_slot(0);sq_algo(1);CHECK(sq_number(squ.pat)==2);
    sq_algo(1);CHECK(sq_number(squ.pat)==3);sq_algo(-2);CHECK(sq_number(squ.pat)==1);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=CF_GLO_RECORDINGS;ui_knob_turn(0,-1);
    CHECK(sq_pref.protect && sq_pref_dirty);sq_save_write();sq_pref_load();CHECK(sq_pref.protect);
    before=cfs_fnv(&sq_st,sizeof sq_st);cu.scr=SCR_HOME;rec();
    CHECK(!sqk.on && cfs_fnv(&sq_st,sizeof sq_st)==before);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=CF_GLO_RECORDINGS;ui_knob_turn(0,1);
    sq_save_write();sq_pref_load();CHECK(!sq_pref.protect);
    synthetic_take(65);sq_load();CHECK(sq_st.pat[sq_view_slot(0)].ev[0].note[0]==65);
}
static void legacy(void)
{
    empty(); static sq_legacy_store_t old;
    memset(&old,0,sizeof old);old.magic=SQ_MAGIC;old.size=sizeof old;
    old.song.n=2;old.song.pat[0]=7;old.song.pat[1]=0;old.song.mute=SQ_MUTE_OWN;
    for(unsigned i=0;i<SQ_LEGACY_NPAT;i++) {
        sq_fresh(&old.pat[i],4);old.pat[i].hit[0][0]=1u<<i;old.pat[i].dlev[0]=80+i;
        old.pat[i].ev[0]=(sq_ev_t){.trk=SQ_CHORDS,.n=3,.note={60,64,67},.rootkey=0xFF,.len=24};old.pat[i].nev=1;
    }
    old.check=cfs_fnv(&old.song,sizeof old-__builtin_offsetof(sq_legacy_store_t,song));
    for(unsigned k=0;k<SQ_NCHUNK;k++)CHECK(!st_save(OBJ_CFSEQ0+k,(uint8_t *)&old+k*SQ_CHUNK,sq_chunk_len(k)));
    unsigned archive=st_crc32(emu_flash+0xB8000,0xC000);
    sq_load();CHECK(!sq_st.song.n && sq_st.pat[2].hit[0][0]==4 && sq_st.pat[0].ev[0].note[1]==64);
    sq_save_write();CHECK(st_crc32(emu_flash+0xB8000,0xC000)==archive);
    sq_clear_take(0);sq_save_write();sq_load();CHECK(!sq_has(0) && sq_has(1));
    CHECK(st_crc32(emu_flash+0xB8000,0xC000)==archive);
    /* Malformed records cannot silently resurrect a cleared legacy take. */
    sq_take_rec_t bad=*sq_take_image(0);bad.version++;
    CHECK(!st_save(OBJ_CFTAKE0,&bad,sizeof bad));sq_load();CHECK((sq_unavailable&1u) && !sq_has(0));
    sq_clear_take(0);sq_save_write();sq_load();CHECK(!sq_unavailable && !sq_has(0));
    /* Older CFQ5 inputs migrate A..C while their untouched D..H remain exportable. */
    memset(emu_flash+0xE6000,0xFF,0x8000);
    static uint8_t v5[48+SQ_LEGACY_NPAT*SQ_PAT5];memcpy(v5,&old,48);
    for(unsigned i=0;i<SQ_LEGACY_NPAT;i++)memcpy(v5+48+i*SQ_PAT5,&old.pat[i],SQ_PAT5);
    uint32_t head[3]={SQ_MAGIC5,sizeof v5,cfs_fnv(v5+12,sizeof v5-12)};memcpy(v5,head,sizeof head);
    for(unsigned k=0;k<SQ_NCHUNK;k++){
        unsigned off=k*SQ_CHUNK,n=sizeof v5-off;if(n>SQ_CHUNK)n=SQ_CHUNK;
        CHECK(!st_save(OBJ_CFSEQ0+k,v5+off,n));
    }
    archive=st_crc32(emu_flash+0xB8000,0xC000);sq_load();
    CHECK(sq_st.pat[2].hit[0][0]==4 && sq_st.pat[2].dlev[0]==100);
    sq_save_write();CHECK(st_crc32(emu_flash+0xB8000,0xC000)==archive);
    cfk_rec_t archived;cfk_build(0,&archived);archived.edited=1;archived.pat1=8;
    archived.check=cfs_fnv(&archived,__builtin_offsetof(cfk_rec_t,check));
    CHECK(!st_save(OBJ_CFTWK7,&archived,sizeof archived));cfk_load();
    CHECK(cfk_st[CFK_NST-1].edited && cfk_st[CFK_NST-1].pat1==8 && !cfk_st[CFK_NST-1].drop);
    for(unsigned i=3;i<CFK_NST;i++){cfk_st[i].edited=1;cfk_st[i].pat1=8;}
    cfk_own[0]=1;CHECK(cfk_store_for(0)<0); /* archived tweaks cannot be reclaimed for new takes */
}
int main(void)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);advance(1600);
    CHECK(sizeof sq_st==7656 && SQ_NPAT==3 && sizeof(sq_take_rec_t)==2556);
    record_flow(44100);record_flow(48000);history();legacy();
    printf("recordings: %u checks, %u failures\n",checks,failures);return failures!=0;
}
