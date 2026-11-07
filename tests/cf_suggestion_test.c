/* SPDX-License-Identifier: GPL-3.0-only
 * Real UI/ISR integration: browsing must not disturb performance or user data. */
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
static void drum_transitions(void)
{
    cs_defaults(&cs); cu.scr=SCR_HOME; cf_scene_set(CF_SCENE_PROGRESSIONS);
    cpg.idea=0; dm_view=0xFF;
    ch.mode=CF_H_AUTO; cf_send_config(); cf_send_metro(); render_blocks(4);
    cf_post(CFE_KEY,7,0,1); dm_play(dm_nth(4,0)); render_blocks(4);
    CHECK(dmp.on && cf_sn.dm_on && cfc.out.sounding);
    /* Queue 6/8, then cancel it by browsing back to a 4/4 idea before the bar. */
    ui_select(11); render_blocks(2);
    CHECK(cs.beats==4 && dmp.next<DM_N && DM_RHY[dmp.next].beats==6);
    ui_knob_turn(0,1); render_blocks(2);
    CHECK(dmp.next<DM_N && DM_RHY[dmp.next].beats==6 && cs.beats==4);
    ui_select(-11); render_blocks(2);
    CHECK(dmp.next==0xFF || DM_RHY[dmp.next].beats==4);
    /* Let the queued meter actually arrive through the existing bar boundary. */
    ui_select(11); render_blocks(2);
    for(unsigned i=0;i<5000 && DM_RHY[dmp.r].beats!=6;i++) render_blocks(1);
    dm_poll(); render_blocks(2);
    CHECK(dmp.on && DM_RHY[dmp.r].beats==6 && cs.beats==6 && cfc.out.sounding);
    CHECK(cs.chord.tonic==0 && cs.chord.scale==CF_SC_MAJOR);
    cf_post(CFE_KEY,7,0,0); cf_post(CFE_DRUM,0,0,0); render_blocks(4);
    CHECK(!dmp.on && !cfc.out.sounding);
    cf_harmony_defaults(&ch); cs_defaults(&cs); cf_send_config(); cf_send_metro();
    cpg.idea=CF_P_NONE; dm_view=0xFF; render_blocks(4);
}
static void controls(void)
{
    cf_knob_t knob = {0};
    cf_set_t settings = cs;
    cf_harmony_t harmony = ch;
    uint32_t patterns = cfs_fnv(&sq_st, sizeof sq_st);
    CHECK(sizeof(cf_set_t) == 188 && sizeof(sq_ev_t) == 22);
    CHECK(cu.scr==SCR_HOME && cf_scene()==CF_SCENE_PROGRESSIONS && screen_pages(SCR_GLO)==CF_GLO_NPAGES);
    CHECK(cf_progression_current()==cf_progression_get(0));
    ui_knob_info(0,&knob); CHECK(!strcmp(knob.name,"RHYTHM"));
    for(unsigned i=1;i<=cf_progression_count();i++) {
        ui_select(1); render_blocks(2);
        const cf_progression_t *p=cf_progression_get(i%cf_progression_count());
        CHECK(cpg.idea==i%cf_progression_count());
        CHECK(cs.chord.tonic==p->tonic && cs.chord.scale==p->scale && cs.beats==p->beats);
        CHECK(cs.snd[0]==settings.snd[0] && cs.snd[1]==settings.snd[1] && cs.bpm==settings.bpm);
        CHECK(!memcmp(cs.chord.top,settings.chord.top,sizeof cs.chord.top));
        CHECK(!memcmp(&harmony,&ch,sizeof ch));
        CHECK(cfs_fnv(&sq_st,sizeof sq_st)==patterns && !sq_dirty);
        for(int mode=0;mode<2;mode++) { ch.mode=(uint8_t)mode; cf_ui_draw(); }
        ch=harmony;
    }
    ui_select(-1); CHECK(cpg.idea==cf_progression_count()-1);
    ui_button(B_GLO); ui_button(B_HOME); CHECK(cu.scr==SCR_HOME && cpg.idea==cf_progression_count()-1);
    cs.chord.quantize=CF_Q_OFF; ui_select(1); render_blocks(2);
    CHECK(cs.chord.quantize==CF_Q_BASIC && cpg.idea==0);
    unsigned tonic=cs.chord.tonic; ui_knob_turn(1,1); CHECK(cs.chord.tonic==(tonic+1)%12 && cpg.idea==0);
    uint32_t rhythm=cpg.rhythm, beats=cs.beats; cf_chord_cfg_t chord=cs.chord;
    ui_knob_turn(0,1); render_blocks(2);
    CHECK(cpg.rhythm!=rhythm && cs.beats==beats);
    CHECK(!memcmp(&chord,&cs.chord,sizeof chord));
    sqk.on=1; unsigned idea=cpg.idea; rhythm=cpg.rhythm;
    ui_select(1); ui_knob_turn(0,1); CHECK(cpg.idea==idea && cpg.rhythm==rhythm); sqk.on=0;
    cf_intro_on=0; cu.pop=POP_NONE; cv_turn_ms=0;
    /* Highlight live chords, upper extensions, sequenced chords and HOLD.
     * Silence and single notes must not leave stale highlights. */
    cpg.idea=0; cs.chord.tonic=4; cs.chord.scale=CF_SC_MAJOR;
    const cf_progression_t *p=cf_progression_current();
    memset(&cf_sn,0,sizeof cf_sn); cf_sn.out.sounding=1; cf_sn.out.n=4;
    cf_sn.out.root_pc=11; cf_sn.out.mask=1u|(1u<<4)|(1u<<7)|(1u<<11);
    CHECK(cf_progression_live(p,CF_SC_MAJOR)==2);
    cf_sn.out.hold=1; CHECK(cf_progression_live(p,CF_SC_MAJOR)==2);
    cf_sn.out.single=1; CHECK(!cf_progression_live(p,CF_SC_MAJOR));
    cf_sn.out.sounding=0; cf_sn.sq_on=1; cf_sn.sq_dn=4; cf_sn.sq_droot=11; cf_sn.sq_dmask=cf_sn.out.mask;
    CHECK(cf_progression_live(p,CF_SC_MAJOR)==2);
    cf_sn.sq_dn=0; CHECK(!cf_progression_live(p,CF_SC_MAJOR));
    /* Every idea remains visible with unsupported live settings. */
    for(unsigned i=0;i<cf_progression_count();i++) {
        cpg.idea=(uint8_t)i;
        cs.chord.scale=CF_SC_MAJ_PENT; cs.chord.quantize=CF_Q_OFF;
        cf_ui_draw(); CHECK(cpg.idea==i);
    }
    cs=settings; ch=harmony; cf_send_config(); render_blocks(2);
}

static void render_audio(int browse, unsigned rate, const char *path)
{
    FILE *out=fopen(path,"wb"); CHECK(out); if(!out) return;
    int32_t mix[CTL*2];
    cf_intro_on=0; cf_boot_go=0; cu.pop=POP_NONE;
    cf_harmony_defaults(&ch); ch.mode=CF_H_AUTO; ch.slot[0]=CF_MOD_13;
    cu.scr=SCR_HOME; cf_scene_set(CF_SCENE_PROGRESSIONS);
    cf_send_config();
    for(unsigned b=0;b<64;b++) audio_block(mix,CTL);
    cf_post(CFE_KEY,1,0,1); cf_post(CFE_KEY,1,0,0); /* select 13 explicitly */
    cf_post(CFE_KEY,25,0,1); cf_post(CFE_KEY,25,0,0); /* HOLD */
    cf_post(CFE_KEY,7,0,1); cf_post(CFE_KEY,7,0,0);
    sq_fresh(&sq_st.pat[0],4); dm_fill(&sq_st.pat[0],dm_nth(4,0),0);
    sqp_start(0,0); cfm_restart(); cfm.running=1;
    ua_reset(); ua.cap_alt=1; ua.cap_hold=0; ua_rate_set(rate);
    uint32_t patterns=cfs_fnv(&sq_st,sizeof sq_st);
    for(unsigned b=0;b<FS*3u/CTL;b++) {
        if(b%13u==0) {
            uint32_t queue=cf_evq_w; cf_chord_t held=cfc;
            /* Identical browsing inputs in both processes; rendering is the
             * independent variable. Meter/key changes are intentional. */
            ui_select(1);
            queue=cf_evq_w; held=cfc;
            if(browse) { cf_snapshot(); cf_ui_draw(); }
            CHECK(queue==cf_evq_w && !memcmp(&held,&cfc,sizeof held));
        }
        uint32_t w=ua.cw; audio_block(mix,CTL);
        CHECK(cfc.out.hold && cfc.out.n==6 && sqp.on);
        fwrite(mix,sizeof mix,1,out);
        for(unsigned i=w;i<ua.cw;i++) fwrite(ua.cap+(i&(UA_CAP_RING-1u))*6u,6u*sizeof(int16_t),1,out);
        ua.cr=ua.cw;
    }
    CHECK(patterns==cfs_fnv(&sq_st,sizeof sq_st));
    fclose(out);
}
static void shots(const char *dir)
{
    cf_intro_on=0; cu.pop=POP_NONE; cu.scr=SCR_HOME; cf_scene_set(CF_SCENE_PROGRESSIONS);
    ch.mode=CF_H_AUTO; cs.retrig=CF_RT_MOVE;
    static const unsigned picks[]={0,9,19,40,44,49,52,78,100,102,111,113,115,116,118};
    for(unsigned i=0;i<sizeof picks/sizeof *picks;i++) {
        cpg.idea=CF_P_NONE; cf_progression_current(); ui_select((int)picks[i]);
        if(!picks[i]) { cs.chord.tonic=4; cs.chord.scale=CF_SC_MAJOR; cs.beats=4; cf_send_config(); }
        render_blocks(4);
        cf_post(CFE_KEY,7,0,1); render_blocks(4); cf_ui_draw();
        char path[1024]; snprintf(path,sizeof path,"%s/idea-%03u.ppm",dir,picks[i]+1);
        FILE *f=fopen(path,"wb"); CHECK(f); if(!f)continue;
        fprintf(f,"P6\n240 240\n255\n");
        for(unsigned j=0;j<FB_W*FB_H;j++) {
            uint32_t rgb=cf_pal[fb[j]&63]; uint8_t pixel[3]={rgb>>16,rgb>>8,rgb}; fwrite(pixel,3,1,f);
        }
        fclose(f); cf_post(CFE_KEY,7,0,0); render_blocks(4);
    }
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0); render_blocks(64);
    if(argc==3 && !strcmp(argv[1],"--shots")) shots(argv[2]);
    else if(argc==5 && !strcmp(argv[1],"--audio")) render_audio(atoi(argv[2]),(unsigned)atoi(argv[3]),argv[4]);
    else { drum_transitions(); controls(); }
    printf("suggestions: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
