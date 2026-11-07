/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do {checks++;if(!(x)){failures++;printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void blocks(unsigned n){int32_t out[CTL*2];while(n--)audio_block(out,CTL);cf_snapshot();}
static void appearance(void)
{
    CHECK(sizeof(cs)==188);CHECK(cf_skin==CF_SKIN_TEAL && cf_motion==1);
    cpb_legacy_rec_t old={0};old.magic=CPB_MAGIC;old.version=1;old.mode=CPB_CATALOG;
    old.favorites[0]=1;old.check=cfs_fnv(&old,__builtin_offsetof(cpb_legacy_rec_t,check));
    CHECK(cpb_legacy_valid(&old));CHECK(!st_save(OBJ_CFPREFS,&old,sizeof old));
    cf_skin=2;cf_motion=0;cpb_load();CHECK(cf_skin==CF_SKIN_TEAL && cf_motion && cpb.favorites[0]==1);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=0;cu.now=1000;
    unsigned writes=emu_flash_writes;
    ui_knob_turn(0,2);ui_knob_turn(3,-1);
    CHECK(cf_skin==CF_SKIN_EDITORIAL && !cf_motion && cpb_dirty);
    CHECK(writes==emu_flash_writes); /* turning/drawing never writes flash */
    cpb_flush();CHECK(!cpb_dirty);cpb_load();CHECK(cf_skin==2 && !cf_motion && cpb.favorites[0]==1);
    cpb_rec_t current;CHECK(st_load(OBJ_CFPREFS,&current,sizeof current)==sizeof current && cpb_valid(&current));
    current.reserved[1]=0x80;current.check=cfs_fnv(&current,__builtin_offsetof(cpb_rec_t,check));CHECK(!cpb_valid(&current));
    /* A real 28-byte v2 record keeps appearance and old favorites, zeroes new
     * IDs, and does not write flash until an explicit edit needs v4. */
    old.version=2;old.mode=CPB_ALPHA;old.reserved[0]=2;old.reserved[1]=0;
    old.favorites[15]=0x10;old.check=cfs_fnv(&old,__builtin_offsetof(cpb_legacy_rec_t,check));
    CHECK(sizeof old==28 && cpb_record_valid(&old,sizeof old));
    CHECK(!cpb_record_valid(&old,sizeof old-1));
    CHECK(!st_save(OBJ_CFPREFS,&old,sizeof old));writes=emu_flash_writes;cpb_load();
    CHECK(emu_flash_writes==writes && !cpb_dirty && cpb.mode==CPB_ALPHA && cf_skin==2 && !cf_motion);
    CHECK(cpb.favorites[0]==1 && cpb.favorites[15]==0x10);
    for(unsigned i=16;i<CPB_BYTES;i++)CHECK(!cpb.favorites[i]);
    cf_browser_star(&cpb,cf_progression_count()-1,1);cpb_changed_ui();cpb_flush();cpb_load();
    CHECK(cf_browser_favorite(&cpb,cf_progression_count()-1) && cpb.favorites[15]==0x10);
    CHECK(st_load(OBJ_CFPREFS,&current,sizeof current)==44 && current.version==4 && cpb_valid(&current));
    cf_skin=0;cf_palette_build();CHECK(cf_pal[FL_BG]==0x061B1E && cf_pal[FL_INK]==0xF2F8F4 && cf_pal[48]==0x77DACB);
}
static void stage(void)
{
    cf_intro_on=0;cu.scr=SCR_HOME;home_boot_main=1;cu.pop=POP_NONE;cs.boot_sound=0;cfb.on=0;
    cpa_stop();cfp_panic();blocks(2);cu.now=2000;cf_motion=0;cf_stage_valid=0;
    cf_ui_draw();unsigned draws=cf_stage_draws,tiles=fb_tiles_sent,writes=emu_flash_writes;
    for(unsigned i=0;i<20;i++){cu.now+=100;cf_sn.now_ms=cu.now;cf_ui_draw();}
    CHECK(cf_stage_draws==draws && fb_tiles_sent==tiles && emu_flash_writes==writes);
    /* Live identity is independent of the selected phrase, including release/Last. */
    cf_post(CFE_KEY,7,0,1);blocks(2);cf_ui_draw();
    CHECK(cf_stage_last.live && strcmp(cf_stage_last.chord,"--") && cf_stage_last.notes[0]);
    CHECK(cf_stage_last.slot==255 && cf_stage_last.degree[0]);
    char chord[24];str_cpy(chord,cf_stage_last.chord,sizeof chord);
    cf_post(CFE_KEY,7,0,0);blocks(2);cf_ui_draw();
    CHECK(!cf_stage_last.live && !strcmp(cf_stage_last.chord,chord));
    /* Actual position windows the phrase; repeated names cannot choose position. */
    for(unsigned idea=0;idea<cf_progression_count();idea++)if(cf_progression_get(idea)->n>4){cpg.idea=idea;break;}
    cf_sn.ac_on=1;cf_sn.ac_slot=5;cf_sn.ac_next=CF_P_NONE;cf_stage_model_t model;cf_stage_model(&model);
    CHECK(model.first==4 && model.slot==5 && model.total>4);cf_sn.ac_on=0;
    /* Off cancels a live envelope immediately; On starts from current data. */
    cf_stage_energy=200;cf_motion=0;cf_stage_model(&model);CHECK(!model.halo && !cf_stage_energy);
    /* The model and drawing are bounded on long altered/slash symbols. */
    static const char *names[]={"Bbmaj7","F#dim7/A","C7sus4","D#m7b5","Abadd9/Eb"};
    fb_unclip();for(unsigned i=0;i<NELEM(names);i++){fb_fill(FL_BG);cf_stage_chord(names[i],130,FL_INK);}
    /* Native drawing must retain the shared KEY knob's sharp/flat glyphs. */
    const char *encoded[]={"F\x80","B\x81"},*ascii[]={"F#","Bb"};
    for(unsigned i=0;i<2;i++) {
        fb_fill(FL_BG);cfn_text(&CFN_16,10,30,encoded[i],FL_INK);unsigned hash=cfs_fnv(fb,FB_W*FB_H);
        fb_fill(FL_BG);cfn_text(&CFN_16,10,30,ascii[i],FL_INK);
        CHECK(hash==cfs_fnv(fb,FB_W*FB_H) && cfn_width(&CFN_16,encoded[i])==cfn_width(&CFN_16,ascii[i]));
    }
    /* Every retained screen still renders in Off, including factual countdowns. */
    for(unsigned screen=1;screen<SCR_N;screen++) {
        cu.scr=screen;
        for(unsigned page=0;page<screen_pages(screen);page++) {
            cu.page[screen]=page;cf_ui_draw();unsigned hash=cfs_fnv(fb,FB_W*FB_H);
            cu.now+=1351;cf_sn.now_ms=cu.now;cf_ui_draw();CHECK(hash==cfs_fnv(fb,FB_W*FB_H));
        }
    }
}
static void source_labels(void)
{
    static const struct { unsigned id,first; const char *name[4]; } expected[]={
        {101,0,{"Gmaj7","D","Em","D"}},
        {110,0,{"Dm7","G7","Cmaj7","Fmaj7"}},
        {114,0,{"C","Em7","Fmaj7","F#m7b5"}},
        {114,4,{"G7","G7sus4","G7"}},
        {130,0,{"C","D","G","Am7"}},
        {132,0,{"Gbmaj7","Ab","Bbm"}},
        {133,0,{"F#","C#/E#","G#m7"}},
        {135,0,{"Bm","A","E"}},
    };
    cu.scr=SCR_HOME;cf_scene_set(CF_SCENE_PROGRESSIONS);
    for(unsigned e=0;e<NELEM(expected);e++) {
        unsigned idea=0;
        while(idea<cf_progression_count() && cf_progression_get(idea)->id!=expected[e].id)idea++;
        CHECK(idea<cf_progression_count());if(idea>=cf_progression_count())continue;
        cpg.idea=idea;const cf_progression_t *p=cf_progression_get(idea);
        cs.chord.tonic=p->tonic;cs.chord.scale=p->scale;cs.beats=p->beats;cu_spell_update();
        cf_sn.ac_on=1;cf_sn.ac_slot=expected[e].first;cf_sn.ac_next=CF_P_NONE;
        cf_stage_model_t model;cf_stage_model(&model);CHECK(model.first==expected[e].first);
        for(unsigned i=0;i<model.count;i++) {
            CHECK(expected[e].name[i] && !strcmp(model.rail[i],expected[e].name[i]));
            CHECK(cfn_width(&CFN_12,model.rail[i])<=58);
        }
    }
    cf_sn.ac_on=0;
}
int main(void)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);appearance();stage();source_labels();
    printf("Instrument Teal HOME: %u checks, %u failures\n",checks,failures);return failures!=0;
}
