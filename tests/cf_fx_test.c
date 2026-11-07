/* SPDX-License-Identifier: GPL-3.0-only
 * Native FX interaction, persistence and real DSP/render isolation. RAM-only HAL. */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

static void valid(void)
{
    CHECK(cs.fx.on < 64u);
    for (unsigned e = 0; e < CFX_N; e++) {
        for (unsigned k = 0; k < 3; k++) CHECK(cs.fx.v[e][k] <= 100u);
        if (e == CFX_FILTER || e == CFX_REVERB) CHECK(cs.fx.v[e][0] <= 2u);
        if (e == CFX_FILTER) CHECK(cs.fx.v[e][1] <= 99u);
    }
}
static void controls(void)
{
    cs_defaults(&cs); cfx_init(); fxpu.mode = FXP_BROWSE; cu.scr = SCR_HOME; ui_button(B_FX);
    CHECK(cu.scr == SCR_FX && cfxu.view == CFX_MACROS && FXP_FACTORY_N == 13u);
    cf_set_t before = cs;
    uint32_t queue = cf_evq_w;
    ui_select(9); CHECK(fxpu.pick == 9u && cfx_equal(&cs.fx, &before.fx));
    ui_algo(1); CHECK(cfxu.view == CFX_INDIVIDUAL);
    ui_select(2); CHECK(cu.fx_pos == 2u);
    ui_algo(-1); CHECK(cfxu.view == CFX_MACROS && cu.fx_pos == 2u);
    CHECK(!memcmp(&cs, &before, sizeof cs) && cf_evq_w == queue);
    for (unsigned i = 0; i < FXP_FACTORY_N; i++) {
        fxpu.pick = i; ui_button(B_SCL);
        CHECK(cfx_equal(&cs.fx, &FXP_FACTORY[i].fx) && cfx_equal(cfx_pending ? &cfx_target : &cfx_set, &cs.fx));
        CHECK(cfxu.loaded == i && !cfxu.edited);
        cf_set_t expected = before; expected.fx = FXP_FACTORY[i].fx;
        CHECK(!memcmp(&cs, &expected, sizeof cs));
        valid();
        cf_fx_t loaded = cs.fx;
        ui_algo(1); ui_button(B_SCL); ui_algo(-1);
        CHECK(cfx_equal(&loaded, &cs.fx)); /* SEL in Individual has no recall action. */
        for (int k = 0; k < 4; k++) {
            int anchor = cfxu.value[k];
            cfx_knob_turn(k, 100); valid(); CHECK(cfxu.value[k] == 100);
            cfx_knob_turn(k, -100); valid(); CHECK(cfxu.value[k] == 0);
            cfx_knob_turn(k, anchor); valid();
            CHECK(cfx_equal(&cs.fx, &loaded)); /* return to the exact audible anchor */
        }
        CHECK(!memcmp(cs.fx.order, before.fx.order, CFX_N));
        CHECK(cs.chord.tonic == before.chord.tonic && cs.bpm == before.bpm);
        CHECK(cf_evq_w == queue);
    }
    fxpu.pick = 9; cfx_recall();
    ui_algo(1); cu.fx_pos = 5; /* Reverb in the fixed chain */
    cfx_knob_turn(2, -33); /* real Individual decay edit */
    cf_fx_t edited = cs.fx;
    CHECK(cfxu.edited && cfxu.loaded == 9u);
    ui_algo(-1);
    CHECK(cfx_equal(&edited, &cs.fx));
    cfx_knob_turn(0, 1);
    CHECK(cs.fx.v[CFX_REVERB][1] >= edited.v[CFX_REVERB][1]);
    CHECK(cs.fx.v[CFX_REVERB][1] - edited.v[CFX_REVERB][1] <= 3);
    CHECK(cs.fx.v[CFX_DELAY][0] == edited.v[CFX_DELAY][0]);
    CHECK(cs.fx.v[CFX_REVERB][0] == edited.v[CFX_REVERB][0]);
    CHECK(!memcmp(cs.fx.v[CFX_FILTER], edited.v[CFX_FILTER], 3));
    ui_button(B_HOME); ui_button(B_FX);
    CHECK(cfxu.view == CFX_MACROS && cu.fx_pos == 5u);
    /* A restored legacy custom chain is retained until an explicit factory recall. */
    cs.fx.order[0] = CFX_REVERB; cs.fx.order[5] = CFX_FILTER; cfx_init();
    before = cs; ui_algo(1); ui_algo(-1); CHECK(!memcmp(&cs, &before, sizeof cs));
    CHECK(fxp_save_slot(0, "Temporary FX"));
    fxpu.pick=FXP_FACTORY_N;cfx_recall();
    CHECK(cfxu.loaded==FXP_FACTORY_N);
    cf_fx_t audible=cs.fx;
    CHECK(fxp_delete_slot(0));cfx_sync();
    CHECK(cfxu.loaded==CFX_CUSTOM && !strcmp(cfx_loaded_name(),"Custom") && cfx_equal(&audible,&cs.fx));
}
static void persistence(void)
{
    CHECK(sizeof(cf_set_t) == 188u);
    for (unsigned i = 0; i < FXP_FACTORY_N; i++) {
        fxpu.pick = i; cfx_recall();
        CHECK(cf_settings_write(0) == 1);
        cs.fx = CFX_DEFAULT; CHECK(cf_settings_load()); cfx_init();
        CHECK(cfxu.loaded == i && cfx_equal(&cs.fx, &FXP_FACTORY[i].fx));
    }
    fxpu.pick = 9; cfx_recall(); cfxu.view = CFX_MACROS; fxpu.mode = FXP_BROWSE; cfx_knob_turn(0, 2);
    cf_fx_t edited = cs.fx;
    CHECK(cf_settings_write(0) == 1);
    cs.fx = CFX_DEFAULT; CHECK(cf_settings_load()); cfx_init();
    CHECK(cfx_equal(&edited, &cs.fx) && cfxu.loaded == CFX_CUSTOM);
}
static void motion(void)
{
    cu.scr=SCR_FX;fxpu.mode=FXP_BROWSE;cf_intro_on=0;cu.pop=0;cf_motion=1;
    fxpu.pick=10;cfx_recall();memset(&cfn_motion,0,sizeof cfn_motion);
    cfxu.level=0;cfxu.vis_ms=0;cf_sn.visual_muted=0;cf_sn.visual_peak=0;cu.now=10000;
    cfn_motion_frame();CHECK(!cfxu.level && !cfxu.vis_ms);
    for(unsigned i=0;i<10;i++){cu.now+=40;cf_sn.visual_ms+=40;cf_sn.visual_peak=20000;cfn_motion_frame();}
    CHECK(cfxu.level>0 && cfxu.vis_ms>0);
    cf_sn.visual_peak=0;
    for(unsigned i=0;i<100;i++){cu.now+=40;cf_sn.visual_ms+=40;cfn_motion_frame();}
    CHECK(!cfxu.level);
    uint32_t stopped=cfxu.vis_ms;cu.now+=40;cfn_motion_frame();CHECK(cfxu.vis_ms==stopped);
    cf_motion=0;cf_sn.visual_peak=30000;cu.now+=40;cf_ui_draw();
    CHECK(!cfxu.level && !cfxu.vis_ms);
    uint8_t screen[FB_W*FB_H];memcpy(screen,fb,sizeof screen);
    cu.now+=500;cf_sn.visual_ms+=500;cf_ui_draw();CHECK(!memcmp(screen,fb,sizeof screen));
    cfx_knob_turn(0,-20);cf_ui_draw();CHECK(memcmp(screen,fb,sizeof screen)!=0);
    CHECK(cfn_page_last.kind==N_FX && !cfn_page_last.focus);
}
static void shot(const char *dir, const char *name)
{
    char path[512]; snprintf(path, sizeof path, "%s/%s.ppm", dir, name);
    FILE *f = fopen(path, "wb"); CHECK(f); if (!f) return;
    fprintf(f, "P6\n240 240\n255\n");
    for (unsigned i = 0; i < FB_W * FB_H; i++) {
        uint32_t c = cf_pal[fb[i] & 63u];
        uint8_t rgb[] = {c >> 16, c >> 8, c}; fwrite(rgb, 1, 3, f);
    }
    fclose(f);
}
static void prepare(void)
{
    int32_t out[CTL * 2];
    cf_intro_on = 0; cf_boot_note(); cf_ready = 1; cf_motion = cf_visual_want = 1;
    cs_defaults(&cs); cf_scene_set(CF_SCENE_PROGRESSIONS);
    cf_pool_load(0, ENGI_FM6, pool_pos_factory(ENGI_FM6, 0));
    trk[0].p[P_LEVEL] = 78; fx_output_set(1, 1);
    cf_fx_t dry = CFX_DEFAULT; dry.on = 0; cf_fx_apply(&dry);
    for (unsigned b = 0; b < FS * 2u / CTL; b++) audio_block(out, CTL);
    cfx_init(); fxpu.mode = FXP_BROWSE; fxpu.pick = 9; cfx_recall(); cu.scr = SCR_FX;
    for (unsigned b = 0; b < FS / 2u / CTL; b++) audio_block(out, CTL);
    cf_snapshot(); cu.pop = 0; memset(cu.knob_ms, 0, sizeof cu.knob_ms);
}
static void audio(int draw, const char *path)
{
    prepare();
    FILE *f = fopen(path, "wb"); CHECK(f); if (!f) return;
    int32_t out[CTL * 2];
    uint8_t notes[] = {60, 64, 67, 71};
    ua_reset(); ua.cap_alt = 1; ua.cap_hold = 0; ua_rate_set(44100);
    for (unsigned i = 0; i < 4; i++) trk_note_on(&trk[0], notes[i], 100);
    for (unsigned b = 0; b < FS * 3u / CTL; b++) {
        if (b == FS / CTL) for (unsigned i = 0; i < 4; i++) trk_note_off(&trk[0], notes[i]);
        uint32_t w = ua.cw; audio_block(out, CTL); fwrite(out, sizeof out, 1, f);
        for (unsigned i = 0; i < CTL; i++) fwrite(ua.cap + ((w + i) & (UA_CAP_RING - 1u)) * 6u, 12, 1, f);
        ua.cr = ua.cw;
        if (draw && b % 55u == 0) {
            cu.now = b * CTL * 1000u / FS; cf_snapshot();
            ui_select(1); ui_algo(1); ui_select(-1); cf_ui_draw();
            ui_algo(-1); cf_ui_draw();
        }
    }
    fclose(f);
}
static void captures(const char *dir)
{
    prepare();
    for (unsigned i = 0; i < FXP_FACTORY_N; i++) {
        fxpu.pick = i; cfx_recall(); cfxu.level = 0; cfxu.vis_ms = 0; cf_ui_draw();
        char name[32]; snprintf(name, sizeof name, "preset-%02u", i + 1); shot(dir, name);
    }
    fxpu.pick = 9; cfx_recall(); cfxu.view = CFX_INDIVIDUAL; fxpu.mode = FXP_EDIT; cu.fx_pos = 5; cf_ui_draw(); shot(dir, "individual");
    cfxu.view = CFX_MACROS; fxpu.mode = FXP_BROWSE; fxpu.pick = 10; cf_ui_draw(); shot(dir, "browsing");
    cfx_recall();
    int32_t out[CTL * 2];
    for (unsigned frame = 0; frame < 60; frame++) {
        if (frame % 20u == 0) {
            for (unsigned i = 0; i < 4; i++) trk_note_on(&trk[0], 60 + (int[]){0,4,7,11}[i], 100);
        }
        if (frame % 20u == 6) for (unsigned i = 0; i < 4; i++) trk_note_off(&trk[0], 60 + (int[]){0,4,7,11}[i]);
        for (unsigned b = 0; b < FS / 20u / CTL; b++) audio_block(out, CTL);
        cu.now = 2000 + frame * 50; cf_snapshot(); cf_ui_draw();
        char name[32]; snprintf(name, sizeof name, "motion-%02u", frame); shot(dir, name);
    }
}
int main(int argc, char **argv)
{
    emu_fw_options(NULL, 1, 0, 1); emu_hal.ready = 2; emu_fw_init(0);
    if (argc == 4 && !strcmp(argv[1], "--audio")) audio(atoi(argv[2]), argv[3]);
    else if (argc == 3 && !strcmp(argv[1], "--captures")) captures(argv[2]);
    else { controls(); persistence(); motion(); }
    printf("effects: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
