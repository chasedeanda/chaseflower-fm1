/* SPDX-License-Identifier: GPL-3.0-only
 * Reuse the full physical/USB/MIDI stream fixture, including queued changes. */
#define CF_SCENE_FIXTURE_ONLY 1
#include "cf_scene_free_test.c"
static unsigned routes;
static void settle(void)
{
    cu.now+=1000;cf_ui_draw();unsigned tiles=fb_tiles_sent;
    unsigned draws=cf_stage_draws+cf_acc_draws+cfd_draws+cfn_page_draws;
    uint32_t hash=cfs_fnv(fb,FB_W*FB_H);cu.now+=1000;cf_ui_draw();
    CHECK(tiles==fb_tiles_sent && hash==cfs_fnv(fb,FB_W*FB_H));
    CHECK(draws==cf_stage_draws+cf_acc_draws+cfd_draws+cfn_page_draws);routes++;
}
static void capture(const char *dir,const char *name)
{
    if(!dir)return;char path[512];snprintf(path,sizeof path,"%s/%s.ppm",dir,name);
    FILE *f=fopen(path,"wb");CHECK(f);if(!f)return;fprintf(f,"P6\n240 240\n255\n");
    for(unsigned p=0;p<FB_W*FB_H;p++){uint32_t rgb=cf_pal[fb[p]];uint8_t c[]={rgb>>16,rgb>>8,rgb};fwrite(c,1,3,f);}fclose(f);
}
static void native_routes(const char *dir)
{
    cf_intro_on=0;cu.pop=0;cf_motion=0;squ.help=squ.help_hold=0;
    uint32_t sounds=cfs_fnv(cf_snd,sizeof cf_snd),patterns=cfs_fnv(&sq_st,sizeof sq_st);unsigned writes=emu_flash_writes;
    uint8_t engine_before[3];for(unsigned p=0;p<3;p++)engine_before[p]=trk[p].eng_req;
    for(unsigned skin=0;skin<CF_SKIN_N;skin++) {
        cf_skin=skin;cf_palette_build();
        for(unsigned scr=0;scr<SCR_N;scr++) {
            cu.scr=scr;
            for(unsigned pg=0;pg<screen_pages(scr);pg++){cu.page[scr]=pg;settle();}
        }
        /* Every advanced FM6 and VA section, page and part. */
        for(unsigned part=0;part<3;part++)for(unsigned engine=0;engine<2;engine++) {
            trk[part].eng_req=engine?ENGI_VA:ENGI_FM6;cu.edit_part=part;cu.scr=SCR_EDIT;
            for(unsigned sec=0;sec<8;sec++){cu.edit_sec=sec;for(unsigned pg=0;pg<cfe_npages(part,sec);pg++){cu.page[SCR_EDIT]=pg;settle();}}
        }
        for(unsigned view=HV_CHORD;view<HV_N;view++){cu.scr=SCR_HOME;home_boot_main=0;cs.seq_hint=(cs.seq_hint&63)|(view<<HV_SHIFT);settle();}
        for(unsigned page=0;page<DP_N;page++){cfd.page=page;settle();}
        for(unsigned mode=0;mode<3;mode++){cu.scr=SCR_FX;fxpu.mode=mode;settle();}
        for(unsigned pop=1;pop<=POP_SOUND_LAYER;pop++) {cu.pop=pop;str_cpy(cu.msg,"Saved sound\nReady to play",sizeof cu.msg);settle();}
        cu.pop=0;settle();
    }
    /* Rendering does not write recordings or flash. */
    CHECK(patterns==cfs_fnv(&sq_st,sizeof sq_st) && writes==emu_flash_writes);
    for(unsigned p=0;p<3;p++)trk[p].eng_req=engine_before[p];CHECK(sounds==cfs_fnv(cf_snd,sizeof cf_snd));
    cf_splash();cf_ui_init();cf_intro_on=0;cf_motion=0;cu.pop=0;cf_skin=0;cf_palette_build();
    blocks(2);cfd.page=0;fxpu.mode=FXP_EDIT;cu.edit_sec=0;cu.edit_part=0;home_boot_main=1;
    str_cpy(cu.save_name,"Soft lead",sizeof cu.save_name);
    const unsigned screens[]={SCR_HOME,SCR_ENV,SCR_EDIT,SCR_FX,SCR_GLO,SCR_BASS,SCR_SEL,SCR_ARP,SCR_SEQ,SCR_SAVE,SCR_SQ,SCR_SQED,SCR_DRUM};
    const char *names[]={"home","envelope","sound","effects","display","bass","chord-buttons","arp","metronome","save","recordings","record-grid","drum-grid"};
    for(unsigned i=0;i<sizeof screens/sizeof *screens;i++){cu.scr=screens[i];cu.page[cu.scr]=0;settle();capture(dir,names[i]);}
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=6;settle();capture(dir,"diagnostics");
    cu.scr=SCR_DRUM;squ.help=1;settle();capture(dir,"drum-help");squ.help=0;
    cu.scr=SCR_HOME;cu.pop=POP_SOUND_CHORD;settle();capture(dir,"sound-list");
    cu.pop=POP_MSG;str_cpy(cu.msg,"Saved sound\nReady to play",sizeof cu.msg);settle();capture(dir,"result");
    cu.pop=0;cu.upd_left=4;settle();capture(dir,"update");cu.upd_left=0;
    cr_safe=1;cf_splash();capture(dir,"safe-mode");cr_safe=0;
    /* Every factory percussion lane must fit the native overview. */
    for(unsigned r=0;r<DM_N;r++){unsigned n=0;for(unsigned l=0;l<SQ_NLANE;l++)n+=DM_RHY[r].lane[l]!=0;CHECK(n<=8);}
}
static void motion_checks(void)
{
    cf_motion=1;cu.now+=10000;cu.scr=SCR_ENV;cu.pop=0;cfn_motion_frame();
    cu.scr=SCR_BASS;cf_nav_dir=-1;cu.now++;cfn_motion_frame();CHECK(cfn_page_dx()==-8);
    cu.scr=SCR_ARP;cf_nav_dir=1;cu.now++;cfn_motion_frame();CHECK(cfn_page_dx()==8);
    cu.now+=170;cfn_motion_frame();CHECK(!cfn_page_dx());
    cu.knob_ms[2]=cu.now;CHECK(cfn_knob(2)==20);cu.now+=90;CHECK(!cfn_knob(2));
    CHECK(cfn_cursor(789)==60);cu.now+=120;CHECK(cfn_cursor(789)==100);
    CHECK(cfn_focus(789)==2);cu.now+=100;CHECK(!cfn_focus(789));
    cf_sn.visual_peak=16000;cf_sn.visual_ms=cu.now;cf_sn.visual_muted=0;cu.now+=60;cfn_motion_frame();CHECK(cfn_motion.energy>0);
    cf_motion=0;cfn_motion_frame();CHECK(!cf_visual_want && !cfn_motion.energy && !cfn_page_dx() && cfn_cursor(789)==100 && !cfn_focus(789));
    cf_motion=1;cfn_motion_frame();CHECK(!cfn_page_dx());
    cf_sn.visual_peak=0;for(unsigned n=0;n<80;n++){cu.now+=100;cfn_motion_frame();}CHECK(!cfn_motion.energy);
    cf_visual_want=0;blocks(2);CHECK(!cf_sn.visual_peak);
    cf_motion=0;cu.scr=SCR_HOME;home_boot_main=1;cu.pop=0;settle();
    /* Off cancels pre-existing finite transitions, including those owned by HOME. */
    cf_motion=1;cu.now++;cf_home_bank_ms=cu.now;cf_stage_chord_ms=cu.now;
    cf_motion=0;cu.now++;cf_ui_draw();cf_motion=1;cu.now++;cf_ui_draw();
    CHECK(!cf_stage_last.bank_dx && !cf_stage_last.chord_dy && !cfn_page_dx());
    /* Reset questions/countdown remain visible; drawing cannot execute an erase. */
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=4;cf_motion=0;cfn_page_t page;unsigned writes=emu_flash_writes;
    cfr.arm=1;cfn_page_model(&page);CHECK(strstr(page.row[3],"3 seconds"));
    cfr.fin=1;cfr.fin_ms=cu.now;cfn_page_model(&page);CHECK(strstr(page.row[3],"confirm") && strstr(page.detail,"6 seconds"));
    cfr.arm=cfr.fin=0;CHECK(writes==emu_flash_writes);
    char lines[3][64]={{0}};cfn_message_lines(lines,"Hold SAVE 3 seconds to clear this recording\nHOME cancels");
    CHECK(strstr(lines[2],"cancels") || strstr(lines[1],"cancels"));
    /* The meter agrees with actual delivered Q15 samples (fractional Clean bits
     * may differ by less than one Q15 sample); it never alters that stream. */
    cf_visual_want=1;cf_post(CFE_KEY,9,0,1);blocks(32);
    int32_t out[2*CTL];audio_block(out,CTL);uint32_t peak=0;
    for(unsigned i=0;i<2*CTL;i++){int32_t v=out[i]>>OUT_SHIFT;unsigned a=v<0?0u-(uint32_t)v:(uint32_t)v;if(a>peak)peak=a;}
    CHECK(peak+1>=cf_visual_peak && cf_visual_peak+1>=peak && cf_visual_peak>0);
    cf_post(CFE_KEY,9,0,0);blocks(2);
}
int main(int argc,char **argv)
{
    emu_fw_options(0,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    if(argc==5){cf_motion=atoi(argv[4]);cf_visual_want=cf_motion;render(atoi(argv[2]),argv[3]);}
    else {native_routes(argc==3?argv[2]:0);motion_checks();}
    printf("native pages and motion: %u checks / %u routes, %u failures\n",checks,routes,failures);return failures!=0;
}
