/* SPDX-License-Identifier: GPL-3.0-only */
#define CF_STUDIO 0
#include <stdio.h>
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do { checks++;if(!(x)){failures++;if(failures<24)printf("FAIL %d: %s\n",__LINE__,#x);} } while(0)
static void assets(void)
{
    const unsigned size[]={sizeof CFM_DEVIL_DATA,sizeof CFM_CAT_DATA};
    for(unsigned character=0;character<CF_SKIN_N;character++) {
        const cfm_art_t *a=&CFM_ART[character];
        for(unsigned face=0;face<CFM_ART_FRAMES;face++) {
            unsigned visible=0;
            for(unsigned y=0;y<CFM_ART_H;y++) {
                unsigned off=a->rows[face*CFM_ART_H+y];CHECK(off+2<=size[character]);
                const uint8_t *row=a->data+off;
                CHECK(row[0]+row[1]<=CFM_ART_W && off+2+row[1]<=size[character]);
                for(unsigned x=0;x<row[1];x++){CHECK(row[x+2]<=CFM_ART_COLORS);visible+=row[x+2]!=0;}
            }
            CHECK(visible>3500 && visible<10000);
        }
    }
}
static void poses(void)
{
    cf_motion=1;cf_sn.running=1;cf_sn.beats_done=1;cf_sn.ac_waiting=cf_sn.ac_lost=cf_sn.ac_paused=0;
    cfn_motion.active=cfn_motion.fresh=1;cfn_motion.energy=200;cf_sn.visual_muted=0;
    unsigned writes=emu_flash_writes,queue=cf_evq_w;
    for(unsigned character=0;character<CF_SKIN_N;character++) {
        cf_skin=character;cf_palette_build();
        uint32_t hashes[4]={0};
        for(unsigned phase=0;phase<32;phase++) {
            cf_sn.beat_acc=phase*(FS*60u/32u);cfm_pose_t p;cfm_model(&p,0);
            CHECK(p.character==character && p.face<CFM_ART_FRAMES && p.sx>=240 && p.sx<=272 && p.sy>=240 && p.sy<=272);
            CHECK(p.x>-256 && p.x<256 && p.y>-1200 && p.y<600 && p.angle>=-3 && p.angle<=3);
            for(unsigned face=0;face<CFM_ART_FRAMES;face++) {
                p.face=face;p.burst=0;fb_unclip();fb_fill(FL_BG);cfm_draw(&p);
                unsigned ink=0;
                for(unsigned y=0;y<FB_H;y++)for(unsigned x=0;x<FB_W;x++) {
                    uint8_t color=fb[y*FB_W+x];
                    if(x<1 || x>=116 || y<49 || y>=156)CHECK(color==FL_BG);
                    else if(color!=FL_BG){CHECK(color>=CFM_PAL_BASE && color<CFN_PAL_N);ink++;}
                }
                CHECK(ink>3000);if(!phase)hashes[face]=cfs_fnv(fb,FB_W*FB_H);
            }
        }
        CHECK(hashes[0]!=hashes[1] && hashes[0]!=hashes[2] && hashes[0]!=hashes[3]);
    }
    cfm_pose_t p;cf_sn.perf_fill=1;cfm_model(&p,0);CHECK(p.face==CFM_REACT && p.burst);
    cf_sn.perf_fill=0;cf_sn.perf_dropped=1;cfm_model(&p,0);CHECK(p.face==CFM_RELAX);
    cf_sn.perf_dropped=0;cf_sn.perf_fill=1;cfn_motion.active=0;cfm_model(&p,0);CHECK(!p.ring && !p.burst && !p.x && !p.y);cf_sn.perf_fill=0;cfn_motion.active=1;
    cf_sn.perf_dropped=0;cf_sn.perf_bloom=1;cf_sn.now_ms=100;cf_sn.perf_bloom_ms=0;cu.now=1000;cfn_moment_ms=cu.now;cfm_model(&p,0);CHECK(p.face==CFM_REACT);
    cf_sn.perf_bloom=0;cf_sn.ac_lost=1;cfm_model(&p,0);CHECK(!p.ring && !p.x && !p.y && p.sx==256 && p.sy==256);
    cf_sn.ac_lost=0;cf_motion=0;cfm_pose_t settled;cfm_model(&settled,0);
    for(unsigned phase=0;phase<32;phase++){cf_sn.beat_acc=phase*(FS*60u/32u);cf_sn.perf_fill=phase&1;cfm_model(&p,cu.now);CHECK(!memcmp(&p,&settled,sizeof p));}
    CHECK(writes==emu_flash_writes && queue==cf_evq_w);
    cf_sn.perf_fill=cf_sn.perf_dropped=0;cf_sn.perf_bloom=0;cf_sn.running=0;cfn_moment_ms=0;
}
static void preferences(void)
{
    CHECK(CF_SKIN_N==2 && sizeof(cs)==188 && sizeof(cpb_rec_t)==44);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=0;cu.now+=2000;cf_skin=0;
    cf_knob_t k={0};ui_knob_info(0,&k);CHECK(!strcmp(k.name,"CHARACTER") && !strcmp(k.val,"Devil"));
    unsigned writes=emu_flash_writes;ui_knob_turn(0,1);CHECK(cf_skin==1 && cpb_dirty && writes==emu_flash_writes);
    ui_knob_info(0,&k);CHECK(!strcmp(k.val,"Cat"));
    cf_palette_build();CHECK(cf_pal[FL_BG]==0x190F19 && cf_pal[C_ACC(0)]==0xF29BCD);
    cpb_flush();cpb_load();CHECK(cf_skin==1 && !cpb_dirty);
    cpb_rec_t rec;CHECK(st_load(OBJ_CFPREFS,&rec,sizeof rec)==sizeof rec && rec.version==5 && cpb_valid(&rec));
    rec.reserved[0]=(rec.reserved[0]&~3u)|2u;rec.check=cfs_fnv(&rec,__builtin_offsetof(cpb_rec_t,check));CHECK(!cpb_valid(&rec));
    /* All prior palettes still validate as their historical schemas. Their
     * favorites, Motion and Click survive while the new character defaults. */
    for(unsigned version=3;version<=4;version++)for(unsigned skin=0;skin<3;skin++) {
        memset(&rec,0,sizeof rec);rec.magic=CPB_MAGIC;rec.version=version;rec.mode=CPB_ALPHA;rec.favorites[5]=4;
        rec.reserved[0]=skin;rec.reserved[1]=0;
        if(version==4){rec.reserved[0]|=35u<<2;}
        rec.check=cfs_fnv(&rec,__builtin_offsetof(cpb_rec_t,check));CHECK(cpb_valid(&rec));
        CHECK(!st_save(OBJ_CFPREFS,&rec,sizeof rec));writes=emu_flash_writes;cpb_load();
        CHECK(cf_skin==CF_SKIN_EMBER && !cf_motion && cpb.favorites[5]==4 && !cpb_dirty && writes==emu_flash_writes);
        if(version==4)CHECK(cf_click_level==35);
    }
}
static void typography_and_strips(void)
{
    cf_skin=CF_SKIN_EMBER;cf_palette_build();fb_unclip();fb_fill(FL_BG);
    cfn_text(&CFN_11,10,20,"Rhythm",FL_INK);
    unsigned partial=0,solid=0;
    for(unsigned i=0;i<FB_W*FB_H;i++){partial+=fb[i]>=CFN_AA_BASE;solid+=fb[i]==FL_INK;}
    CHECK(partial>0 && solid>0);
    fb_fill(FL_BG);cfn_coverage(10,10,1,FL_INK);CHECK(fb[10*FB_W+10]==CFN_AA_BASE);
    cfn_coverage(10,10,2,FL_INK);CHECK(fb[10*FB_W+10]==CFN_AA_BASE+1);
    cfn_coverage(10,10,1,FL_INK);CHECK(fb[10*FB_W+10]==CFN_AA_BASE+1);
    cfn_coverage(10,10,3,FL_INK);CHECK(fb[10*FB_W+10]==FL_INK);
    unsigned before=cfs_fnv(fb,FB_W*FB_H);cfn_coverage(-1,-1,2,FL_INK);CHECK(before==cfs_fnv(fb,FB_W*FB_H));
    cf_motion=0;cf_sn.out.sounding=cf_sn.ac_on=cf_sn.sq_on=cf_sn.running=0;
    cu.scr=SCR_HOME;home_boot_main=1;cu.pop=0;cf_home_bank=0;cf_stage_valid=0;cf_stage_draw();
    /* One full-width rule, three short vertical dividers, no bank-name row
     * inserted into the bottom controls and no rule above the chord rail. */
    for(unsigned x=10;x<230;x++){CHECK(fb[198*FB_W+x]==FL_BG3);CHECK(fb[156*FB_W+x]==FL_BG);}
    for(unsigned x=60;x<=180;x+=60)for(unsigned y=203;y<237;y++)CHECK(fb[y*FB_W+x]==FL_BG3);
    for(unsigned x=10;x<230;x++)CHECK(fb[201*FB_W+x]==FL_BG);
}
int main(void)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);cf_intro_on=0;
    assets();poses();preferences();typography_and_strips();
    printf("Mascot art, bounds, motion and migration: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
