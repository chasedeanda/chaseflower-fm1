/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void blocks(unsigned n){int32_t out[CTL*2];while(n--)audio_block(out,CTL);cf_snapshot();cpa_ui_poll();}
static void setup(void)
{
    cpa_stop();sqp_stop();dmp_stop();cfp_panic();cs_defaults(&cs);cf_harmony_defaults(&ch);
    cu.scr=SCR_HOME;home_boot_main=1;cf_scene_set(CF_SCENE_PROGRESSIONS);cf_home_bank=CF_HB_LEARN;
    cu.keys=0;cu.pop=POP_NONE;cf_motion=0;cf_intro_on=cf_boot_go=0;cf_learn_focus=255;
    cpg.idea=0;cpg.rhythm=dm_nth(4,0);cpg.mode=CPA_BOTH;cf_send_config();cf_send_metro();cu_spell_update();blocks(4);
}
static void harmony_knobs(void)
{
    setup();CHECK(CF_HB_N==6 && CF_HB_PERFORM==4 && CF_HB_LEARN==5);
    ui_button(B_PLAY);blocks(4);CHECK(cpa.on);
    unsigned cfg=cfs_fnv(&cs,sizeof cs),sounds=cfs_fnv(trk,sizeof trk),patterns=cfs_fnv(&sq_st,sizeof sq_st);
    unsigned plan=cfs_fnv(&cpa,sizeof cpa),grooves=cfs_fnv(&dmr,sizeof dmr),queue=cf_evq_w,writes=emu_flash_writes;
    for(int knob=2;knob<4;knob++) {
        cf_knob_t k={0};ui_knob_info(knob,&k);CHECK(k.dis && !k.name[0] && !k.val[0]);
        ui_knob_turn(knob,1000);ui_knob_turn(knob,-1000);ui_knob_turn(knob,0);
    }
    CHECK(cfg==cfs_fnv(&cs,sizeof cs) && sounds==cfs_fnv(trk,sizeof trk));
    CHECK(patterns==cfs_fnv(&sq_st,sizeof sq_st) && grooves==cfs_fnv(&dmr,sizeof dmr));
    CHECK(plan==cfs_fnv(&cpa,sizeof cpa) && queue==cf_evq_w && writes==emu_flash_writes);
    cf_knob_t control={0};ui_knob_info(0,&control);CHECK(!control.dis && !strcmp(control.val,"Off"));
    ui_knob_info(1,&control);CHECK(!control.dis && !strcmp(control.name,"RESET"));
    cf_chord_cfg_t manual=cs.chord;
    cf_harmony_t defaults;cf_harmony_defaults(&defaults);
    ui_knob_turn(0,1);blocks(2);CHECK(ch.mode==CF_H_AUTO && cfc.harmony.mode==CF_H_AUTO && cpa.on);
    cf_ui_draw();CHECK(cf_stage_last.learn.automatic);
    ui_knob_turn(0,-1);blocks(2);CHECK(ch.mode==CF_H_MANUAL && cfc.harmony.mode==CF_H_MANUAL && cpa.on);
    for(int direction=-1;direction<=1;direction+=2) {
        ch.mode=CF_H_AUTO;ch.slot[0]=CF_MOD_DIM;ch.inv[0]=2;ch.bass[0]=4;
        cf_harmony_t before=ch;ui_knob_turn(1,0);CHECK(!memcmp(&before,&ch,sizeof ch));
        ui_knob_turn(1,direction);blocks(2);
        CHECK(!memcmp(&defaults,&ch,sizeof ch) && !memcmp(&defaults,&cfc.harmony,sizeof ch));
        CHECK(!memcmp(&manual,&cs.chord,sizeof manual) && cpa.on && cf_home_bank==CF_HB_LEARN);
    }
    CHECK(patterns==cfs_fnv(&sq_st,sizeof sq_st) && grooves==cfs_fnv(&dmr,sizeof dmr));
    cu.pop=POP_NONE;
    cf_ui_draw();CHECK(cf_home_bank==CF_HB_LEARN && cf_stage_last.bank==CF_HB_LEARN);
    ui_button(B_HOME);CHECK(home_stage() && cf_home_bank==CF_HB_CORE && cpa.on);
    for(unsigned i=0;i<5;i++)ui_button(B_HOME);CHECK(cf_home_bank==CF_HB_LEARN);
    ui_button(B_GLO);ui_button(B_HOME);CHECK(home_stage() && cf_home_bank==CF_HB_CORE);
    ui_button(B_PLAY);blocks(4);CHECK(!cpa.on);
}
static void assignments(void)
{
    setup();cf_learn_model_t m;cf_learn_model(&m);CHECK(strstr(m.title,"Play a key"));
    /* Every physical key has its current role/label, including edited forms. */
    for(int key=0;key<CF_NKEYS;key++) {
        cf_keycap_t cap={0};ui_keycap(key,&cap);CHECK(!strcmp(m.cap[key].top,cap.top));
        cf_learn_focus=key;cf_learn_model(&m);CHECK(m.title[0] && m.help[0] && m.detail[0]);
        CHECK(cfn_width(&CFN_16,m.title)<=220 && cfn_width(&CFN_12,m.help)<=220 && cfn_width(&CFN_12,m.detail)<=220);
    }
    cf_learn_focus=1;cf_learn_model(&m);CHECK(strstr(m.title,"Major") && strstr(m.title,"6") && strstr(m.detail,"sixth"));
    cs.chord.top[0].type=CF_T_SUS4;cs.chord.top[0].add=0;cf_learn_model(&m);
    CHECK(strstr(m.help,"fourth") && !strcmp(m.cap[1].top,"s4"));
    ch.mode=CF_H_AUTO;
    for(unsigned mod=0;mod<CF_MOD_N;mod++) {
        ch.slot[0]=mod;cf_learn_model(&m);CHECK(!strcmp(m.cap[1].top,cf_modifier_label(mod)));
        CHECK(cfn_width(&CFN_16,m.title)<=220 && cfn_width(&CFN_12,m.help)<=220 && cfn_width(&CFN_12,m.detail)<=220);
    }
    ch.slot[0]=CF_MOD_SUS2;cf_learn_model(&m);CHECK(strstr(m.help,"Replaces") && strstr(m.help,"second"));
    cs.chord.quantize=CF_Q_OFF;cf_learn_model(&m);CHECK(strstr(m.help,"Unavailable"));
    cs.chord.quantize=CF_Q_BASIC;cf_learn_focus=25;cf_learn_model(&m);CHECK(!strcmp(m.title,"HOLD"));
    /* Transposition and OCT must change the guide without requiring a new press. */
    cf_learn_focus=7;cf_learn_model(&m);CHECK(!strcmp(m.cap[7].top,"C") && strstr(m.title,"C"));
    cs.chord.tonic=4;cs.chord.octave=1;cu_spell_update();cf_learn_model(&m);
    CHECK(!strcmp(m.cap[7].top,"E") && !strncmp(m.title,"E",1));
    CHECK(!strcmp(m.title,"E4 - Root")); /* the firmware's -1 base octave still applies */
}
static void live_keys(void)
{
    setup();ch.mode=CF_H_AUTO;cf_send_config();blocks(2);
    ui_note_key_aged(3,1,0);ui_note_key_aged(3,0,0);blocks(2); /* seventh modifier */
    CHECK(cf_learn_focus==3 && cf_sn.out.buttons);
    ui_note_key_aged(7,1,0);cu.keys=1u<<7;blocks(2);cf_ui_draw();
    CHECK(cf_sn.out.sounding && cf_sn.out.n==4 && cf_learn_focus==7);
    CHECK(cf_stage_last.learn.cap[7].style==KC_DOWN && strstr(cf_stage_last.notes,"B"));
    ui_note_key_aged(25,1,0);ui_note_key_aged(25,0,0);ui_note_key_aged(7,0,0);cu.keys=0;blocks(2);cf_ui_draw();
    CHECK(cf_sn.out.hold && cf_sn.out.sounding && !strcmp(cf_stage_last.learn.title,"HOLD"));
    unsigned draws=cf_stage_draws,tiles=fb_tiles_sent,writes=emu_flash_writes;
    for(unsigned i=0;i<10;i++){cu.now+=100;cf_ui_draw();}
    CHECK(draws==cf_stage_draws && tiles==fb_tiles_sent && writes==emu_flash_writes);
    ui_button(B_HOME);CHECK(cf_home_bank==CF_HB_CORE && cf_sn.out.hold);
    ui_note_key_aged(25,1,0);ui_note_key_aged(25,0,0);blocks(2);CHECK(!cf_sn.out.hold);
}
static void shot(const char *dir,const char *name)
{
    cf_ui_draw();char path[1024];snprintf(path,sizeof path,"%s/%s.ppm",dir,name);FILE *f=fopen(path,"wb");CHECK(f);if(!f)return;
    fprintf(f,"P6\n240 240\n255\n");for(unsigned i=0;i<FB_W*FB_H;i++){unsigned rgb=cf_pal[fb[i]];uint8_t p[]={rgb>>16,rgb>>8,rgb};fwrite(p,3,1,f);}fclose(f);
}
static void render(const char *dir)
{
    setup();shot(dir,"keyboard-idle");
    cf_learn_focus=1;shot(dir,"keyboard-sixth");
    ch.mode=CF_H_AUTO;cf_send_config();blocks(2);
    ui_note_key_aged(3,1,0);ui_note_key_aged(3,0,0);ui_note_key_aged(7,1,0);cu.keys=1u<<7;blocks(2);
    cf_learn_focus=3;shot(dir,"keyboard-seventh");
    cf_learn_focus=7;shot(dir,"keyboard-root");
    cf_learn_focus=8;shot(dir,"keyboard-sus2");
    cf_learn_focus=25;shot(dir,"keyboard-hold");
    for(unsigned skin=1;skin<CF_SKIN_N;skin++){cf_skin=skin;cf_palette_build();shot(dir,"keyboard-plum");}
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    if(argc==3 && !strcmp(argv[1],"--render"))render(argv[2]);else{harmony_knobs();assignments();live_keys();}
    printf("Keyboard: %u checks, %u failures\n",checks,failures);return failures!=0;
}
