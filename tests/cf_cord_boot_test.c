/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;if(failures<25)printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void capture(const char *directory,const char *name)
{
    if(!directory)return;char path[512];snprintf(path,sizeof path,"%s/%s.ppm",directory,name);
    FILE *f=fopen(path,"wb");CHECK(f);if(!f)return;fprintf(f,"P6\n240 240\n255\n");
    for(unsigned p=0;p<FB_W*FB_H;p++){uint32_t c=cf_pal[fb[p]];uint8_t rgb[]={c>>16,c>>8,c};fwrite(rgb,1,3,f);}fclose(f);
}
int main(int argc,char **argv)
{
    emu_fw_options(0,1,0,1);emu_hal.ready=2;emu_fw_init(0);cf_intro_on=0;cr_safe=0;
    unsigned writes=emu_flash_writes,queued=cf_evq_w;
    const uint32_t bg[]={0x06272C,0x0F1113,0xF1EEE7};
    for(unsigned skin=0;skin<3;skin++) {
        cf_skin=skin;cf_motion=1;cf_cord_last.valid=0;
        for(unsigned t=0;t<=1320;t+=40) {
            cf_cord_draw(t);CHECK(cf_pal[FL_BG]==bg[skin]);
            for(unsigned i=0;i<240;i++) {
                CHECK(fb[i]==FL_BG && fb[239*240+i]==FL_BG);
                CHECK(fb[i*240]==FL_BG && fb[i*240+239]==FL_BG);
            }
            for(unsigned p=0;p<240*240;p++)CHECK(fb[p]<CFN_PAL_N);
            if(argc==2 && skin==0){char name[32];snprintf(name,sizeof name,"boot-%04u",t);capture(argv[1],name);}
        }
        CHECK(fb[5*240+120]==C_ACC(2) && fb[159*240+120]==C_ACC(2));
        CHECK(fb[82*240+43]==C_ACC(2) && fb[82*240+197]==C_ACC(2));
        cf_motion=0;cf_cord_draw(0);unsigned tiles=fb_tiles_sent,hash=cfs_fnv(fb,240*240);
        cf_cord_draw(800);cf_cord_draw(1300);CHECK(tiles==fb_tiles_sent && hash==cfs_fnv(fb,240*240));
        if(argc==2){char name[32];snprintf(name,sizeof name,"theme-%u-static",skin);capture(argv[1],name);}
    }
    CHECK(writes==emu_flash_writes && queued==cf_evq_w);
    cf_motion=1;cf_intro_on=1;cf_intro_t=0;cf_cord_started=fm1_ms;cu.idle_ms=0x7fffffff;
    CHECK(cf_intro_draw(40));cu.idle_ms=0;CHECK(!cf_intro_draw(40) && !cf_intro_on);
    CHECK(cf_pal[FL_BG]==CF_SKIN_RGB[cf_skin][0]);
    cf_intro_on=1;cf_intro_t=0;cu.idle_ms=0x7fffffff;CHECK(!cf_intro_draw(1500) && !cf_intro_on);
    cf_intro_on=1;cf_intro_t=0;cf_motion=0;CHECK(!cf_intro_draw(1) && !cf_intro_on);
    cr_safe=1;cf_intro_on=1;cf_intro_t=0;CHECK(!cf_intro_draw(40) && !cf_intro_t);cr_safe=0;
    /* First splash must use persisted appearance, before UI initialization. */
    cf_skin=2;cf_motion=0;cpb_dirty=1;cpb_flush();CHECK(!cpb_dirty);
    cf_skin=0;cf_motion=1;cf_splash();CHECK(cf_skin==2 && !cf_motion && cf_pal[FL_BG]==bg[2] && !cf_intro_on);
    printf("CORD boot: %u checks, %u failures\n",checks,failures);return failures!=0;
}
