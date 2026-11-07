/* SPDX-License-Identifier: GPL-3.0-only
 * Actual UI -> ISR player -> MIDI / DSP / recording / flash on host HAL. */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures, on_count[16];
static uint8_t sounding[16][128];
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n",__LINE__,#x); } } while(0)
static void midi_drain(void)
{
    uint32_t p;
    while(emu_fw_midi_out_take(&p)) {
        unsigned st=(p>>8)&255, note=(p>>16)&127, ch=st&15;
        if((st&240)==144 && (p>>24)) { sounding[ch][note]=1; on_count[ch]++; }
        if((st&240)==128 || ((st&240)==144 && !(p>>24))) sounding[ch][note]=0;
    }
}
static void audio_ms(unsigned ms)
{
    int16_t audio[HALF_FRAMES*2];
    for(unsigned i=0;i<ms;i+=3) { emu_fw_tick(3); emu_fw_audio(audio,HALF_FRAMES); midi_drain(); }
}
static void config(void) { cf_send_config(); audio_ms(6); }
static void key(int k,int down) { cf_post(CFE_KEY,(uint32_t)k,0,down); audio_ms(6); }
static void released(void)
{
    midi_drain();
    for(unsigned c=0;c<16;c++) for(unsigned n=0;n<128;n++) CHECK(!sounding[c][n]);
    CHECK(!cfp.n && !cfp.nb && !cfc.out.sounding);
}
static void ui_and_store(void)
{
    cf_chord_cfg_t manual=cs.chord;
    CHECK(sizeof(cf_set_t)==188 && sizeof(sq_ev_t)==22);
    CHECK(st_sector(OBJ_CFHARM,0)==0xB4000 && st_sector(OBJ_CFHARM,1)==0xB5000);
    for(unsigned o=0;o<OBJ_COUNT;o++) if(o!=OBJ_CFHARM)
        for(unsigned a=0;a<2;a++) for(unsigned b=0;b<2;b++) CHECK(st_sector(o,a)!=st_sector(OBJ_CFHARM,b));
    cu.scr=SCR_GLO; cu.page[SCR_GLO]=CF_GLO_HARMONY;
    ui_knob_turn(0,1); audio_ms(6); CHECK(ch.mode==CF_H_AUTO && cfc.harmony.mode==CF_H_AUTO);
    ui_knob_turn(1,1); audio_ms(6); CHECK(ch.style==CF_H_RICH);
    cu.scr=SCR_SEL; cu.sel_btn=0; ui_knob_turn(0,CF_MOD_DIM-ch.slot[0]); audio_ms(6); CHECK(ch.slot[0]==CF_MOD_DIM);
    CHECK(!memcmp(&manual,&cs.chord,sizeof manual));
    CHECK(cfh_write(0)==0); cf_harmony_t saved=ch;
    cf_harmony_defaults(&ch); cfh_load(); CHECK(!memcmp(&ch,&saved,sizeof ch));
    CHECK(crb_obj(CFB_HARM)==OBJ_CFHARM && crb_size_ok(CFB_HARM,sizeof(cfh_rec_t)) && !crb_size_ok(CFB_HARM,5));
    CHECK(cfh_valid(&cfh_rec)); cfh_rec.version=99; CHECK(!cfh_valid(&cfh_rec));
    cfh_rec.version=1; cfh_rec.h.slot[0]^=1; CHECK(!cfh_valid(&cfh_rec));
    cfh_load(); cr_safe=1; cfh_load(); CHECK(ch.mode==CF_H_MANUAL); cr_safe=0;
    cfh_load(); CHECK(ch.mode==saved.mode);
    CHECK(cf_settings_write(0)==1); cs.chord.top[0].type=CF_T_POWER; CHECK(cf_settings_load());
    CHECK(!memcmp(manual.top,cs.chord.top,sizeof manual.top));
    cf_harmony_defaults(&ch); ch.mode=CF_H_AUTO; config();
}
static void performance(void)
{
    for(int retrig=0;retrig<CF_NRT;retrig++) for(int moment=0;moment<2;moment++) {
        cfp_panic(); midi_drain();
        cs.chord.algo=CF_ALG_TOP_LOCK; cs.chord.quantize=CF_Q_BASIC; cs.chord.tonic=0; cs.chord.scale=CF_SC_MAJOR;
        cs.retrig=(uint8_t)retrig; ch.momentary=(uint8_t)moment; ch.style=CF_H_BASIC;
        trk[0].p[P_GLIDE]=retrig?20:0; config();
        key(7,1); CHECK(cfc.out.n==3 && cfp.n==3 && cfp.nb==1);
        audio_ms(60); key(3,1); /* diatonic seventh */
        CHECK(cfc.out.n==(retrig==CF_RT_WAIT?3:4));
        key(9,1); CHECK(cfc.out.n==4 && cfc.out.root_pc==2);
        key(3,0); key(7,0); key(9,0); audio_ms(60); released();
        key(25,1); key(25,0); key(7,1); key(7,0); CHECK(cfc.out.sounding && cfc.out.hold);
        key(14,1); key(14,0); CHECK(cfc.nroots==1 && cfc.out.root_pc==7);
        key(25,1); key(25,0); released();
    }
    CHECK(on_count[cs.ch_chord]>0 && on_count[cs.ch_bass]>0);
    cfp_panic(); midi_drain(); cs.notes_in=CF_NI_ROOTS; cs.retrig=CF_RT_MOVE; config();
    CHECK(emu_fw_midi_in(0x643C9009u)); audio_ms(6); cf_xq_drain(); audio_ms(6);
    CHECK(cfc.out.n==3 && cfc.out.root_pc==0);
    CHECK(emu_fw_midi_in(0x003C8008u)); audio_ms(6); cf_xq_drain(); audio_ms(6); released();
    cs.notes_in=CF_NI_KEYS; cu.arp_on=1; config(); key(7,1); audio_ms(600);
    CHECK(cfa_k.on && cfa_k.n==3); key(7,0); cu.arp_on=0; config(); audio_ms(60); released();
}
static void recording(void)
{
    cs.retrig=CF_RT_MOVE; ch.style=CF_H_RICH; ch.momentary=0; ch.slot[0]=CF_MOD_13; config();
    key(1,1); key(1,0); key(7,1); CHECK(cfc.out.n==6);
    sqr_ev_t take={0}; sqp_rec_chord(&take);
    CHECK(take.rkey==0xFF && (take.btns&CF_H_EVENT) && take.n==6);
    CHECK(cf_harmony_event_mask(take.mask,take.btns)==cfc.out.mask);
    sq_ev_t event={0}; event.trk=SQ_CHORDS; event.n=take.n; event.mask=take.mask; event.btns=take.btns;
    event.rootkey=take.rkey; event.root=take.root; event.len=24; event.nb=take.nb;
    memcpy(event.note,take.note,sizeof event.note); memcpy(event.bass,take.bass,sizeof event.bass);
    key(7,0); released();
    sq_pat_t *p=&sq_st.pat[0]; sq_fresh(p,4); p->follow=1; CHECK(sq_put(p,&event)>=0);
    ch.mode=CF_H_MANUAL; cs.chord.tonic=7; config();
    sq_ev_t resolved; CHECK(!sq_resolve(&sqp.cm,&p->ev[0],&cs.chord,&resolved));
    sqp_start(0,0); audio_ms(30); CHECK(sqp.dn==6 && sqp.dmask==cf_harmony_event_mask(event.mask,event.btns));
    CHECK(!memcmp(sqp.dnote,event.note,6)); sqp_stop(); audio_ms(30); released();
    char expected[16]; cf_spell_t spell; cf_key_spell(0,0,&spell);
    cf_chord_name(0,sqp.dmask,-1,&spell,expected); CHECK(!strcmp(expected,"Cmaj13"));
}
int main(void)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0); audio_ms(1600); midi_drain();
    ui_and_store(); performance(); recording();
    printf("cf_auto: %u checks, %u failures\n",checks,failures); return failures!=0;
}
