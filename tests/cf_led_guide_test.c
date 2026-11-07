/* SPDX-License-Identifier: GPL-3.0-only
 * Actual LED matrix levels, manual gestures and accompaniment slot following. */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; if (failures <= 10) printf("FAIL %d: %s\n", __LINE__, #x); } } while(0)

static void render_blocks(unsigned n)
{
    int32_t out[CTL*2]; for(unsigned i=0;i<n;i++) audio_block(out,CTL);
    cf_snapshot();
}
static uint32_t led_keys(const uint8_t *level)
{
    uint32_t keys=0;
    for(unsigned col=0;col<FM1_NCOL;col++) for(unsigned row=1;row<5;row++) {
        int id=FM1_KEYMAP[row][col];
        if(id>=14 && (level[col]&(1u<<row))) keys|=1u<<(id-14);
    }
    return keys;
}
static uint32_t tone_keys(uint16_t tones)
{
    uint32_t keys=0;
    for(int k=0;k<CF_NKEYS;k++) {
        int idx;
        if(cf_key_role(cs.chord.algo,k,&idx)==CF_ROLE_ROOT &&
            (tones & (1u << (cf_key_root_note(&cs.chord,k)%12)))) keys|=1u<<k;
    }
    return keys;
}
static void guide_key(int key,int down)
{
    cf_post(CFE_KEY,(unsigned)key,0,down); render_blocks(4); cf_leds();
}
static void progression_lights(void)
{
    cf_set_t settings=cs; cf_harmony_t harmony=ch;
    cfp_panic(); cs_defaults(&cs); cf_harmony_defaults(&ch); ch.mode=CF_H_AUTO;
    cu.scr=SCR_HOME; cu.keys=0; cpg.explored=1; cf_scene_set(CF_SCENE_PROGRESSIONS); cu.now=750;
    cs.chord.tonic=0; cs.chord.scale=CF_SC_MAJOR; cs.lights=0; cpg.idea=0;
    cf_send_config(); render_blocks(4); cpg_follow.valid=0; cf_leds();
    const uint16_t cmaj=1u<<0, gmaj=1u<<7;
    int g=cf_progression_key(&cs.chord,4), a=cf_progression_key(&cs.chord,5), f=cf_progression_key(&cs.chord,3);
    CHECK(cpg_follow.next==0 && led_keys(fm1_led)==tone_keys(cmaj));
    CHECK(led_keys(fm1_led_dim)==0);
    /* Only C and next root G light. G being a C-chord tone cannot hide its cue. */
    guide_key(7,1); CHECK(cpg_follow.current==0 && cpg_follow.next==1);
    CHECK(led_keys(fm1_led)==tone_keys(cmaj|gmaj));
    CHECK(led_keys(fm1_led_dim)==0);
    cu.now=1000; cf_leds();
    CHECK(led_keys(fm1_led)==tone_keys(cmaj));
    CHECK((led_keys(fm1_led_faint)&tone_keys(0xFFF))==0);
    for(unsigned i=0;i<30;i++) cf_leds();
    CHECK(cpg_follow.current==0 && cpg_follow.next==1);
    guide_key(7,0); CHECK(cpg_follow.next==1 && led_keys(fm1_led)==0);
    guide_key(7,1); CHECK(cpg_follow.current==0 && cpg_follow.next==1); /* replay waits */
    guide_key(7,0);
    guide_key(g,1); CHECK(cpg_follow.current==1 && cpg_follow.next==2);
    guide_key(g,0); guide_key(a,1); CHECK(cpg_follow.current==2 && cpg_follow.next==3);
    guide_key(a,0); guide_key(f,1); CHECK(cpg_follow.current==3 && cpg_follow.next==0);
    guide_key(f,0);
    /* I-I-vi-vi: re-pressing under HOLD advances, a modifier and redraw do not. */
    cpg.idea=54; cf_send_config(); render_blocks(4); cf_leds(); CHECK(cpg_follow.next==0);
    guide_key(25,1); guide_key(25,0); guide_key(7,1);
    CHECK(cpg_follow.current==0 && cpg_follow.next==1 && cf_sn.out.hold);
    uint32_t serial=cf_sn.out.serial;
    guide_key(7,0); cf_leds(); CHECK(cpg_follow.next==1);
    guide_key(7,1); CHECK(cf_sn.out.serial==serial);
    CHECK(cpg_follow.current==1 && cpg_follow.next==2);
    guide_key(3,1); guide_key(3,0); CHECK(cpg_follow.current==1 && cpg_follow.next==2);
    guide_key(7,0);
    /* Wrong chords keep the next cue; the expected chord can resume later. */
    guide_key(g,1); CHECK(cpg_follow.next==2); guide_key(g,0);
    guide_key(a,1); CHECK(cpg_follow.current==2 && cpg_follow.next==3);
    guide_key(a,0); guide_key(a,1); CHECK(cpg_follow.current==3 && cpg_follow.next==0);
    guide_key(a,0);
    /* A new key/idea resets even if a chord is held. Other views retain their LEDs. */
    cs.chord.tonic=2; cf_send_config(); render_blocks(4); cf_leds(); CHECK(cpg_follow.next==0);
    cu.scr=SCR_GLO; cf_leds(); CHECK(!cpg_follow.valid);
    cu.scr=SCR_HOME; cs.chord.quantize=CF_Q_OFF; cf_leds(); CHECK(!cpg_follow.valid);
    /* Sequenced chords use their own gesture serial and actual voiced tones. */
    cfp_panic(); cs.chord.tonic=0; cs.chord.quantize=CF_Q_BASIC; cpg.idea=0;
    render_blocks(4); cf_leds();
    cf_sn.sq_on=1; cf_sn.sq_dn=3; cf_sn.sq_droot=60; cf_sn.sq_dmask=1u|(1u<<4)|(1u<<7);
    cf_sn.sq_dnote[0]=60; cf_sn.sq_dnote[1]=64; cf_sn.sq_dnote[2]=67; cf_sn.sq_dser++;
    cf_leds(); CHECK(cpg_follow.current==0 && cpg_follow.next==1);
    CHECK((led_keys(fm1_led)&tone_keys(0xFFF))==tone_keys(cmaj));
    cf_sn.sq_dn=0; cf_leds(); CHECK(cpg_follow.next==1);
    cs=settings; ch=harmony; cf_send_config(); render_blocks(4); cu.now=0;
}

static void accompaniment_lights(void)
{
    cpa_stop(); sqp_stop(); cfp_panic(); cf_harmony_defaults(&ch); ch.mode=CF_H_AUTO;
    cs_defaults(&cs); cs.lights=0; cs.chord.tonic=0; cs.chord.quantize=CF_Q_BASIC;
    cu.scr=SCR_HOME; cu.keys=0; cu.now=500; cpg.idea=54; cpg.explored=1;
    cf_send_config(); render_blocks(4);
    cpa_plan_t plan; CHECK(cpa_prepare(&plan,54,cs.chord,CPA_OFF,CPA_BOTH));
    cpa_publish(&plan,0,1); cf_post(CFE_ACCOMP,1,0,0); render_blocks(4); cf_leds();
    const uint16_t cpower=1u<<0, apower=1u<<9;
    CHECK(cf_sn.ac_on && cf_sn.ac_slot==0);
    CHECK(led_keys(fm1_led)==0 && led_keys(fm1_led_dim)==tone_keys(cpower));
    /* A repeated root pulses even while it is held/current. */
    cu.keys|=tone_keys(cpower);cf_leds();
    CHECK(led_keys(fm1_led)==0 && led_keys(fm1_led_dim)==tone_keys(cpower));
    cu.now=750;cf_leds();
    CHECK(led_keys(fm1_led)==tone_keys(cpower));cu.keys=0;
    for(unsigned i=0;i<384;i++)cpa_tick1(); cf_snapshot(); cf_leds();
    CHECK(cf_sn.ac_slot==1 && led_keys(fm1_led)==tone_keys(cpower|apower));
    /* A queued idea never supplies its notes before the shared-clock boundary. */
    CHECK(cpa_prepare(&plan,0,cs.chord,CPA_OFF,CPA_BOTH)); cpa_publish(&plan,0,1);
    cf_snapshot(); cf_leds(); CHECK(led_keys(fm1_led)==tone_keys(cpower|apower));
    /* Upper extensions are folded to pitch classes; shared G remains bright. */
    cf_sn.ac_root[2]=7; cf_sn.ac_mask[2]=0x204491u; cf_leds();
    uint16_t extended=1u<<7;
    CHECK(led_keys(fm1_led)==tone_keys(cpower|extended));
    cf_sn.ac_repeat_off=1; cf_sn.ac_slot=cf_sn.ac_n-1; cf_leds();
    CHECK(led_keys(fm1_led_dim)==0); /* play-once has no chord after the last */
    cf_sn.ac_waiting=1; cf_sn.ac_slot=0; cf_sn.sq_dn=0; cf_leds();
    CHECK(led_keys(fm1_led)==tone_keys(cpower) && led_keys(fm1_led_dim)==0);
    cf_sn.ac_paused=1; cf_leds(); CHECK(led_keys(fm1_led_dim)==0);
    cpa_stop(); render_blocks(4);
}
static void sixteen_slots(void)
{
    cfp_panic(); cpg.idea=0; cs.chord.tonic=0; cf_send_config(); render_blocks(4); cfd_clone(0);
    cfwd.song.n=CF_P_MAX;
    for(unsigned i=1;i<CF_P_MAX;i++)cfwd.song.slot[i]=cfwd.song.slot[0];
    cfd_view_update(); cpg.idea=CF_P_DRAFT; cf_send_config(); render_blocks(4);
    cpg_follow.valid=0; cf_leds();
    for(unsigned i=0;i<CF_P_MAX;i++) {
        guide_key(7,1);
        CHECK(cpg_follow.current==i && cpg_follow.next==(i+1)%CF_P_MAX);
        guide_key(7,0);
    }
    cfp_panic(); render_blocks(4);
}

int main(void)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0); render_blocks(64);
    progression_lights(); accompaniment_lights(); sixteen_slots();
    printf("LED guide: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
