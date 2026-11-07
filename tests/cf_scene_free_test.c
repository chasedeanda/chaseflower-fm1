/* SPDX-License-Identifier: GPL-3.0-only
 * Scene-free boot/save compatibility and cross-profile physical/USB/MIDI audio. */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0,int isr) { (void)t0;(void)isr;ua_service(); }
static unsigned checks,failures;
#define CHECK(x) do { checks++;if(!(x)){failures++;if(failures<20)printf("FAIL %d: %s\n",__LINE__,#x);} } while(0)
static void blocks(unsigned n) { int32_t out[CTL*2];while(n--)audio_block(out,CTL);cf_snapshot(); }
static void controls(void)
{
#if !CF_SCENES
    CHECK(!cf_intro_on && !cf_intro_draw(16) && cu.scr==SCR_HOME && home_view()==HV_FLOWER);
    CHECK(cf_scene()==CF_SCENE_PROGRESSIONS && !cf_scene_animated());
    CHECK(sizeof(cs)==188 && sizeof(sq_ev_t)==22 && sizeof(sq_st)==20336 && sizeof(sqh)==5152);
    /* Old scene/backdrop/HOME bits remain valid backup data; boot is musical. */
    cs.seq_hint=0xF8;cs.backdrop=FL_BD_RIPPLES;cs.bpm=117;cs.boot_sound=0;
    CHECK(cf_settings_write(0)==1);cf_set_t saved=cs;
    cf_splash();cf_ui_init();
    CHECK(!memcmp(&saved,&cs,sizeof cs) && home_view()==HV_FLOWER && cf_scene()==CF_SCENE_PROGRESSIONS);
    CHECK(!cf_intro_draw(4000));
    unsigned writes=emu_flash_writes,patterns=cfs_fnv(&sq_st,sizeof sq_st);
    for(unsigned i=0;i<20;i++){ui_algo(i%2?1:-1);CHECK(cf_scene()==CF_SCENE_PROGRESSIONS);}
    CHECK((cs.seq_hint&0x38)==0x38 && emu_flash_writes==writes && cfs_fnv(&sq_st,sizeof sq_st)==patterns);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=0;cf_knob_t knob={0};ui_knob_info(1,&knob);CHECK(!knob.name[0]);
    uint8_t backdrop=cs.backdrop;ui_knob_turn(1,1);CHECK(cs.backdrop==backdrop);
    /* Existing recovery screen still draws and remains outside the music UI. */
    cr_safe=1;cf_splash();unsigned ink=0;for(unsigned i=0;i<FB_W*FB_H;i++)ink+=fb[i]!=FL_BG;
    CHECK(ink>100);cr_safe=0;cf_splash();cf_ui_init();
    /* Draw all primary screens, including the pattern/editor and static diagrams. */
    for(unsigned screen=0;screen<SCR_N;screen++) {
        cu.scr=(uint8_t)screen;
        if(screen==SCR_SQED){squ.pat=0;squ.trk=SQ_CHORDS;sq_fresh(&sq_st.pat[0],4);}
        for(unsigned page=0;page<screen_pages(screen);page++){cu.page[screen]=(uint8_t)page;cf_ui_draw();}
    }
    cu.scr=SCR_HOME;home_boot_main=1;cf_ui_draw();
    ui_button(B_HOME);CHECK(home_view()==HV_CHORD);ui_button(B_HOME);CHECK(home_view()==HV_KEYS);
    ui_button(B_HOME);CHECK(home_view()==HV_DIAG);ui_button(B_HOME);CHECK(home_view()==HV_FLOWER);
#else
    CHECK(cf_intro_on && cf_intro_draw(16));
    cf_scene_set(CF_SCENE_BASS);CHECK(cf_scene()==CF_SCENE_BASS);
    cf_scene_set(CF_SCENE_FIRE);CHECK(cf_scene()==CF_SCENE_FIRE);
    cf_scene_set(CF_SCENE_OFF);CHECK(!cf_scene_animated());
#endif
}
static void render(unsigned rate,const char *path)
{
    FILE *file=fopen(path,"wb");CHECK(file);if(!file)return;
    int32_t out[CTL*2];cf_intro_on=0;cf_boot_go=0;cfb.on=0;cu.pop=POP_NONE;
    cu.scr=SCR_HOME;cs.seq_hint=0;home_boot_main=0;
    ch.mode=CF_H_AUTO;cu.arp_on=1;cs.arp.mode=CF_ARP_UP;cf_send_config();blocks(64);
    cf_post(CFE_KEY,1,0,1);cf_post(CFE_KEY,1,0,0);cf_post(CFE_KEY,25,0,1);cf_post(CFE_KEY,25,0,0);
    cf_post(CFE_KEY,7,0,1);cf_post(CFE_KEY,7,0,0);
    sq_fresh(&sq_st.pat[0],4);dm_fill(&sq_st.pat[0],dm_nth(4,0),0);sqp_start(0,0);cfm_restart();cfm.running=1;
    ua_reset();ua.cap_alt=1;ua.cap_hold=0;ua_rate_set(rate);
    unsigned patterns=cfs_fnv(&sq_st,sizeof sq_st),flash=emu_flash_writes;
    for(unsigned b=0;b<FS*3u/CTL;b++) {
        if(b%37==0){cf_snapshot();cf_ui_draw();}
        uint32_t w=ua.cw;audio_block(out,CTL);fwrite(out,sizeof out,1,file);
        for(unsigned i=w;i<ua.cw;i++)fwrite(ua.cap+(i&(UA_CAP_RING-1u))*6u,6*sizeof(int16_t),1,file);
        ua.cr=ua.cw;
        while(mo_r!=mo_w){uint32_t ev=midi_out_q[mo_r++%MQ];fwrite(&ev,sizeof ev,1,file);}
    }
    CHECK(flash==emu_flash_writes && patterns==cfs_fnv(&sq_st,sizeof sq_st) && cfc.out.hold && sqp.on);
    cf_post(CFE_KEY,25,0,1);cf_post(CFE_KEY,25,0,0);cf_post(CFE_SEQ,0,0,0);blocks(1);
    CHECK(!sqp.on);
    /* The new catalogue PLAY path must also match across scene profiles. */
    cpa_stop();dmp_stop();cfp_panic();
    cpg.idea=0;cpg.rhythm=dm_nth(4,0);cpg.mode=CPA_BOTH;
    const cf_progression_t *idea=cf_progression_get(0);
    cs.chord.tonic=idea->tonic;cs.chord.scale=idea->scale;cs.beats=idea->beats;
    cs.chord.quantize=CF_Q_BASIC;cs.bpm=120;cs.clock_in=0;cs.bass_on=1;
    cf_send_config();cf_send_metro();blocks(1);ui_button(B_PLAY);
    for(unsigned b=0;b<FS*6u/CTL;b++) {
        if(b==FS/CTL)ui_knob_turn(3,40);
        if(b==FS*2u/CTL){ui_select(11);ui_knob_turn(1,2);}
        uint32_t w=ua.cw;audio_block(out,CTL);cf_snapshot();cpa_ui_poll();
        if(b%37==0)cf_ui_draw();
        fwrite(out,sizeof out,1,file);
        for(unsigned i=w;i<ua.cw;i++)fwrite(ua.cap+(i&(UA_CAP_RING-1u))*6u,6*sizeof(int16_t),1,file);
        ua.cr=ua.cw;
        while(mo_r!=mo_w){uint32_t ev=midi_out_q[mo_r++%MQ];fwrite(&ev,sizeof ev,1,file);}
    }
    CHECK(cpa.on && cpg.idea==11 && flash==emu_flash_writes && patterns==cfs_fnv(&sq_st,sizeof sq_st));
    ui_button(B_PLAY);blocks(1);CHECK(!cpa.on && !sqp.n && !sqp.nb);fclose(file);
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    if(argc==4)render((unsigned)atoi(argv[2]),argv[3]);else controls();
    printf("scene-free profile %d: %u checks, %u failures\n",CF_SCENES,checks,failures);
    return failures!=0;
}
