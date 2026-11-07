/* SPDX-License-Identifier: GPL-3.0-only
 * README captures from the actual native renderer and sample-driven playback.
 * The emulator's flash is in memory; no physical instrument is accessed.
 */
#include <stdio.h>
#include "emu/emu_fw.c"

static void blocks(unsigned n)
{
    int32_t out[CTL*2];while(n--)audio_block(out,CTL);
    cf_snapshot();cpa_ui_poll();cu.now=1000+cf_now_ms;
}
static void shot(const char *dir,const char *name)
{
    char path[512];snprintf(path,sizeof path,"%s/%s.ppm",dir,name);
    cf_ui_draw();FILE *f=fopen(path,"wb");if(!f){perror(path);exit(1);}
    fprintf(f,"P6\n240 240\n255\n");
    for(unsigned p=0;p<FB_W*FB_H;p++){
        uint32_t rgb=cf_pal[fb[p]];uint8_t c[]={rgb>>16,rgb>>8,rgb};fwrite(c,1,3,f);
    }
    fclose(f);
}
int main(int argc,char **argv)
{
    if(argc!=3){fprintf(stderr,"usage: capture_house_style SKIN OUTPUT\n");return 1;}
    emu_fw_options(0,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    cf_intro_on=0;cf_boot_go=0;cfb.on=0;cu.pop=0;cf_motion=1;cf_visual_want=1;
    cf_skin=atoi(argv[1])%CF_SKIN_N;cf_palette_build();
    cu.scr=SCR_HOME;home_boot_main=1;cf_home_bank=CF_HB_CORE;
    cs.clock_in=0;cs.bpm=120;cs.beats=4;cs.bass_on=1;song.master_q12=4096;
    cs.chord.tonic=0;cs.chord.scale=CF_SC_MAJOR;cs.chord.quantize=CF_Q_BASIC;
    cf_harmony_defaults(&ch);ch.mode=CF_H_AUTO;
    cpg.idea=0;cpg.mode=CPA_BOTH;cpg.rhythm=dm_nth(4,0);cpg.repeat_off=0;
    cf_send_config();cf_send_metro();blocks(2);ui_button(B_PLAY);
    /* Warm the real audio-energy envelope, then record one complete phrase. */
    for(unsigned f=0;f<20;f++){blocks(FS/CTL/20);cf_ui_draw();}
    unsigned done=0;
    for(unsigned frame=0;frame<160;frame++){
        unsigned target=(frame+1)*FS/(20*CTL);blocks(target-done);done=target;
        char name[32];snprintf(name,sizeof name,"motion-%03u",frame);shot(argv[2],name);
        if(frame==40)shot(argv[2],"home");
    }
    cu.scr=SCR_ACC;cu.page[SCR_ACC]=CF_ACC_GROOVE;blocks(FS/CTL/4);shot(argv[2],"groove");
    ui_button(B_HOME);ui_button(B_FX);ui_select(9);ui_button(B_SCL);
    for(unsigned f=0;f<8;f++){blocks(FS/CTL/20);cf_ui_draw();}shot(argv[2],"effects");
    cfl_open();blocks(FS/CTL/4);shot(argv[2],"lab");
    cfd_open();blocks(FS/CTL/4);shot(argv[2],"editor");
    ui_button(B_HOME);ui_button(B_ARP);ui_knob_turn(0,1);
    for(unsigned f=0;f<20;f++){blocks(FS/CTL/20);cf_ui_draw();}shot(argv[2],"arp");
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=0;blocks(FS/CTL/4);shot(argv[2],"display");
    ui_button(B_HOME);ui_button(B_PLAY);blocks(FS/CTL*4);
    csl_open();csl_save_current();
    for(unsigned f=0;f<1200 && csl_tx.pending;f++){
        blocks(FS/CTL/20);csl_poll(cu.now,1);
    }
    if(csl_tx.pending){fprintf(stderr,"Song did not reach its saved state\n");return 1;}
    cu.pop=0;shot(argv[2],"songs");
    printf("Captured %s: 160 playback frames and eight screens\n",CF_SKIN_NAME[cf_skin]);
    return 0;
}
