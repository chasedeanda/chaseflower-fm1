/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
/* Exercise classic HOME bank navigation. Native Studio routes have their own suite. */
#define CF_STUDIO 0
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do {checks++;if(!(x)){failures++;printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void blocks(unsigned n){int32_t out[CTL*2];while(n--)audio_block(out,CTL);cf_snapshot();cpa_ui_poll();}
static void home(void){cu.scr=SCR_HOME;home_boot_main=1;cf_scene_set(CF_SCENE_PROGRESSIONS);cf_home_bank=0;cu.pop=POP_NONE;cf_motion=0;}
static void setup(void){cpa_stop();sqp_stop();dmp_stop();cfp_panic();home();cpg.idea=0;cpg.rhythm=dm_nth(4,0);cpg.mode=CPA_BOTH;cs.chord.scale=CF_SC_MAJOR;cs.chord.tonic=0;cs.chord.quantize=CF_Q_BASIC;cs.beats=4;cs.bpm=120;cs.clock_in=0;cf_send_config();cf_send_metro();blocks(2);}
static uint32_t musical_body_hash(void)
{
    /* The compact bank cue moved to the context line to leave exactly two
     * bottom strips. All musical pixels outside that cue remain anchored. */
    uint32_t hash=2166136261u;
    for(unsigned y=0;y<198;y++)for(unsigned x=0;x<FB_W;x++) {
        if(y>=28 && y<46 && x>=156)continue;
        hash=(hash^fb[y*FB_W+x])*16777619u;
    }
    return hash;
}
static void navigation(void)
{
    setup();cf_post(CFE_KEY,7,0,1);blocks(2);
    cf_post(CFE_KEY,25,0,1);cf_post(CFE_KEY,25,0,0);cf_post(CFE_KEY,7,0,0);blocks(2);CHECK(cf_sn.out.hold);
    cf_ui_draw();unsigned body=musical_body_hash();
    cf_set_t settings=cs;unsigned queue=cf_evq_w,flash=emu_flash_writes,patterns=cfs_fnv(&sq_st,sizeof sq_st);cf_chord_out_t sounding=cf_sn.out;
    for(unsigned i=0;i<CF_HB_N*6u;i++) {
        CHECK(home_stage() && cf_home_bank==i%CF_HB_N);ui_button(B_HOME);cf_ui_draw();
        if(cf_home_bank!=CF_HB_LEARN && cf_home_bank!=CF_HB_ENGINE && cf_home_bank!=CF_HB_FLOW)CHECK(body==musical_body_hash());
        CHECK(!memcmp(&cs,&settings,sizeof cs) && cf_evq_w==queue && emu_flash_writes==flash);
        CHECK(cfs_fnv(&sq_st,sizeof sq_st)==patterns && !memcmp(&cf_sn.out,&sounding,sizeof sounding));
    }
    CHECK(cf_home_bank==0);
    cf_stage_model_t status;
    cf_sn.sq_on=1;cf_stage_model(&status);CHECK(!strcmp(status.status,"REPLAY"));cf_sn.sq_on=0;
    cf_sn.dm_on=1;cf_stage_model(&status);CHECK(!strcmp(status.status,"DRUMS"));cf_sn.dm_on=0;
    cf_sn.running=1;cf_stage_model(&status);CHECK(!strcmp(status.status,"CLOCK"));cf_sn.running=0;
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=0;ui_knob_turn(1,3);CHECK(home_view()==HV_DIAG);
    ui_button(B_GLO);CHECK(cu.scr==SCR_HOME && home_view()==HV_DIAG);cf_ui_draw();
    ui_button(B_HOME);CHECK(home_stage() && cf_home_bank==CF_HB_CORE);
    for(unsigned view=1;view<HV_N;view++){cu.scr=SCR_GLO;cu.page[SCR_GLO]=0;home_view_select((int)view);ui_button(B_GLO);CHECK(home_view()==view);cf_ui_draw();ui_button(B_HOME);CHECK(home_stage() && cf_home_bank==0);}
    for(unsigned bank=0;bank<CF_HB_N;bank++) {
        cf_home_bank=bank;unsigned idea=cpg.idea;ui_select(1);CHECK(((bank==CF_HB_ENGINE || bank==CF_HB_FLOW)?cpg.idea==idea:cpg.idea!=idea) && cf_home_bank==bank);
        ui_presets(1);CHECK(cs.snd[0]==cf_snd_pos(0) && cf_home_bank==bank);
    }
    /* Off cancels a bank tween and On cannot replay it. */
    home();cu.now=1000;cf_motion=1;ui_button(B_HOME);cf_stage_model_t m;cf_stage_model(&m);CHECK(m.bank_dx==10);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=0;ui_knob_turn(3,-1);cu.scr=SCR_HOME;cf_stage_model(&m);CHECK(!m.bank_dx && !cf_home_bank_ms);
    cu.scr=SCR_GLO;ui_knob_turn(3,1);cu.scr=SCR_HOME;cf_stage_model(&m);CHECK(!m.bank_dx);
    cf_post(CFE_KEY,25,0,1);cf_post(CFE_KEY,25,0,0);blocks(2);CHECK(!cf_sn.out.hold);
}
static void shared_toolbars(void)
{
    setup();
    unsigned page=cu.page[SCR_ACC]=CF_ACC_CLICK,queue=cf_evq_w;
    cgs.zoom=0;
    for(unsigned i=0;i<CF_HB_N*3;i++){
        if(cgs_home()){
            unsigned focus=cgs.focus;
            ui_algo(1);
            CHECK(cf_home_bank==CF_HB_ENGINE && cgs.focus!=focus);
            ui_button(B_HOME);
        } else ui_algo(1);
        CHECK(cf_home_bank==(i+1)%CF_HB_N);
    }
    CHECK(CF_HB_LEARN+1==CF_HB_N);
    ui_algo(-1);CHECK(cf_home_bank==CF_HB_LEARN && cf_nav_dir==-1);
    ui_algo(-CF_HB_N);CHECK(cf_home_bank==CF_HB_LEARN);
    ui_algo(0);CHECK(cf_home_bank==CF_HB_LEARN && cf_evq_w==queue);
    for(unsigned bank=CF_HB_LOOP;bank<=CF_HB_MIX;bank++) {
        unsigned tab=bank==CF_HB_LOOP?CF_ACC_LOOP:CF_ACC_MIX;
        for(int k=0;k<4;k++) {
            cf_knob_t a={0},b={0};cu.scr=SCR_HOME;cf_home_bank=bank;
            ui_knob_info(k,&a);CHECK(cu.page[SCR_ACC]==page);
            cu.scr=SCR_ACC;cu.page[SCR_ACC]=tab;ui_knob_info(k,&b);
            CHECK(!strcmp(a.name,b.name) && !strcmp(a.val,b.val) && a.dis==b.dis && a.bar==b.bar);
            cf_set_t saved=cs;typeof(cpg) idea=cpg;
            cu.scr=SCR_HOME;ui_knob_turn(k,1);cf_set_t home_cs=cs;typeof(cpg) home_idea=cpg;
            CHECK(cu.scr==SCR_HOME && cf_home_bank==bank && cu.page[SCR_ACC]==tab);
            cs=saved;cpg=idea;cu.scr=SCR_ACC;ui_knob_turn(k,1);
            CHECK(!memcmp(&cs,&home_cs,sizeof cs) && !memcmp(&cpg,&home_idea,sizeof cpg));
            page=cu.page[SCR_ACC]=CF_ACC_CLICK;
        }
    }
    home();
}
static void parameters(void)
{
    setup();cf_knob_t k;
    cf_home_bank=CF_HB_SOUND;
    ui_knob_turn(2,1000);CHECK(cs.level_chord==127);ui_knob_turn(2,-1000);CHECK(!cs.level_chord);
    ui_knob_turn(3,1000);CHECK(cs.bass_level==127);ui_knob_turn(3,-1000);CHECK(!cs.bass_level);
    unsigned old=cf_snd_pos(1);ui_knob_turn(1,1);CHECK(cf_snd_pos(1)!=old && cs.snd[1]==cf_snd_pos(1));
    cf_home_bank=CF_HB_TEXTURE;
    unsigned on=cs.fx.on,bypass=cfx_bypass;
    static const unsigned effect[]={CFX_REVERB,CFX_DELAY,CFX_CHORUS};
    for(unsigned i=0;i<3;i++){ui_knob_turn(i,1000);CHECK(cs.fx.v[effect[i]][2]==100);ui_knob_turn(i,-1000);CHECK(!cs.fx.v[effect[i]][2]);}
    CHECK(cs.fx.on==on && cfx_bypass==bypass);
    cs.fx.on&=(uint8_t)~(1u<<CFX_CHORUS);memset(&k,0,sizeof k);ui_knob_info(2,&k);CHECK(k.dis && strstr(k.val,"Off"));
    ui_knob_turn(3,1000);CHECK(trk[0].p[P_GLIDE]==127);ui_knob_turn(3,-1000);CHECK(!trk[0].p[P_GLIDE]);
    cf_home_bank=CF_HB_GROOVE;
    ui_knob_turn(1,1000);CHECK(cs.level_drums==127);ui_knob_turn(1,-1000);CHECK(!cs.level_drums);
    unsigned running=cfm.running;ui_knob_turn(3,1000);blocks(2);CHECK(cs.metro_level==100 && cfm.running==running && !cpa.on);ui_knob_turn(3,-1000);CHECK(!cs.metro_level);
    cpg.rhythm=CPA_OFF;unsigned before=cfs_fnv(&dmr,sizeof dmr);ui_knob_turn(0,1);ui_knob_turn(2,1);CHECK(before==cfs_fnv(&dmr,sizeof dmr));
    memset(&k,0,sizeof k);ui_knob_info(0,&k);CHECK(k.dis && !strcmp(k.val,"No rhythm"));
    home();cs.level_chord=104;cs.bass_level=96;cs.level_drums=104;cf_send_config();blocks(2);
}
static void queued(void)
{
    setup();ui_button(B_PLAY);blocks(2);CHECK(cpa.on);
    cf_home_bank=CF_HB_GROOVE;unsigned kit=cpa.plan.kit,swing=cpa.plan.swing;
    ui_knob_turn(0,sq_kit_step(kit,1)==kit?-1:1);ui_knob_turn(2,swing>=100?-1:1);
    CHECK(cpa.plan.kit==kit && cpa.plan.swing==swing && (cpa_edit.flags&CPA_GROOVE));
    cf_knob_t k={0};ui_knob_info(0,&k);CHECK(!strcmp(k.val,SQ_KITS[kit].name));
    unsigned nextkit=cpa_edit.plan.kit,nextswing=cpa_edit.plan.swing,revision=cpa.revision;
    for(unsigned i=0;i<FS*3u/CTL && cpa.revision==revision;i++)blocks(1);
    CHECK(cpa.plan.kit==nextkit && cpa.plan.swing==nextswing && !(cpa_edit.flags&CPA_GROOVE));
    /* Bank presentation alone never changes ownership, cursor, edits or transport. */
    unsigned slot=cpa.slot,flags=cpa_edit.flags,queue=cf_evq_w;cf_home_bank=0;
    for(unsigned i=0;i<CF_HB_N*2u;i++)ui_button(B_HOME);
    CHECK(cpa.on && cpa.slot==slot && cpa_edit.flags==flags && cf_evq_w==queue);
    ui_button(B_PLAY);blocks(2);CHECK(!cpa.on && !sqp.n && !sqp.nb);
}
static void source_rail(void)
{
    setup();cf_home_bank=CF_HB_CORE;ch.mode=CF_H_MANUAL;
    for(unsigned idea=0;idea<cf_progression_count();idea++) {
        const cf_progression_t *p=cf_progression_get(idea);
        const cf_source_chord_t *source=cf_progression_source(p);if(!source)continue;
        cpg.idea=idea;cs.chord.tonic=p->tonic;cs.chord.scale=p->scale;cu_spell_update();
        cf_sn.ac_on=1;cf_sn.ac_next=CF_P_NONE;cf_sn.ac_edits=0;
        for(unsigned slot=0;slot<p->n;slot++) {
            cf_sn.ac_slot=slot;cf_stage_model_t m;cf_stage_model(&m);
            int root=(p->tonic+cf_scale_step(p->scale,p->degree[slot])+source[slot].alter+12)%12;
            int bass=source[slot].bass<0?-1:(root+source[slot].bass)%12;char expected[24];
            cf_chord_name(root,source[slot].mask,bass,&cu_spell,expected);
            CHECK(!strcmp(m.rail[slot%4],expected));
        }
    }
    cf_sn.ac_on=0;
}
static void audio(const char *path,int cycle)
{
    setup();cf_motion=1;ui_button(B_PLAY);blocks(2);FILE *f=fopen(path,"wb");CHECK(f);if(!f)return;
    int32_t out[CTL*2];unsigned flash=emu_flash_writes;
    for(unsigned b=0;b<FS*4u/CTL;b++) {
        if(cycle && !(b%37)){cu.now=b*CTL*1000u/FS;if(b%74)ui_algo(1);else ui_button(B_HOME);cf_ui_draw();}
        audio_block(out,CTL);fwrite(out,sizeof out,1,f);cf_snapshot();cpa_ui_poll();
    }
    fclose(f);CHECK(flash==emu_flash_writes && cpa.on);ui_button(B_PLAY);blocks(2);CHECK(!cpa.on && !sqp.n && !sqp.nb);
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    if(argc==3)audio(argv[2],atoi(argv[1]));else {navigation();parameters();queued();source_rail();shared_toolbars();}
    printf("HOME banks: %u checks, %u failures\n",checks,failures);return failures!=0;
}
