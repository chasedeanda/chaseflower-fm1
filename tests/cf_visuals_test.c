/* SPDX-License-Identifier: GPL-3.0-only
 * Real firmware UI, storage, scenes, boot and USB/DSP on the host HAL. No device access.
 */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

static void test_settings(void)
{
    CHECK(sizeof(cf_set_t) == 188u);
    cs_defaults(&cs);
    CHECK(cs.chord.algo == CF_ALG_TOP_LOCK && cf_scene() == CF_SCENE_BASS);
    for (unsigned mode = 0; mode <= CF_NALGS; mode++) {
        cs_defaults(&cs);
        cs.chord.algo = mode == CF_NALGS ? 255u : mode;
        cs.bpm = 137; cs.snd[0] = 3; cs.snd[1] = 2;
        cs.seq_hint = (HV_KEYS << HV_SHIFT) | 0x37u;
        cf_scene_set(CF_SCENE_FIRE);
        CHECK((cs.seq_hint & ~CF_SCENE_FIRE_BIT) == ((HV_KEYS << HV_SHIFT) | 0x37u));
        CHECK(cf_settings_write(0) == 1);
        memset(&cs, 0, sizeof cs);
        CHECK(cf_settings_load());
        CHECK(cs.chord.algo == CF_ALG_TOP_LOCK && cs.bpm == 137 && cs.snd[0] == 3 && cs.snd[1] == 2);
        CHECK(cf_scene() == CF_SCENE_FIRE && home_view() == HV_KEYS && (cs.seq_hint & 0x37u) == 0x37u);
        cf_scene_set(CF_SCENE_BASS);
        CHECK(cf_settings_write(0) == 1);
        cf_scene_set(CF_SCENE_FIRE);
        CHECK(cf_settings_load() && cf_scene() == CF_SCENE_BASS && home_view() == HV_KEYS);
    }
    /* Old records had no scene flag: their hints and HOME view still survive. */
    cs.seq_hint = (HV_CHORD << HV_SHIFT) | 7u;
    CHECK(cf_settings_write(0) == 1);
    CHECK(cf_settings_load() && cf_scene() == CF_SCENE_BASS && home_view() == HV_CHORD);
    cs.chord.algo = CF_ALG_SIDE;
    cf_send_config();
    CHECK(cs.chord.algo == CF_ALG_TOP_LOCK && cfc_shadow.algo == CF_ALG_TOP_LOCK);
}

static void controls(void)
{
    cs_defaults(&cs); cu.scr = SCR_HOME;
    cs.seq_hint = (HV_KEYS << HV_SHIFT) | 0x37u;
    uint32_t queue = cf_evq_w;
    cf_chord_cfg_t chord = cs.chord;
    uint8_t selected = cu.sel_btn;
    ui_algo(1);
    CHECK(cf_scene() == CF_SCENE_FIRE && home_view() == HV_FLOWER);
    CHECK(cf_evq_w == queue && !memcmp(&chord, &cs.chord, sizeof chord) && selected == cu.sel_btn);
    CHECK((cs.seq_hint & 0x37u) == 0x37u);
    ui_algo(-1); CHECK(cf_scene() == CF_SCENE_BASS);
    ui_algo(0); CHECK(cf_scene() == CF_SCENE_BASS);
    for (unsigned i = 0; i < 9; i++) {
        ui_button(B_HOME);
        CHECK(home_view() == (i + 1u) % HV_N && cf_scene() == CF_SCENE_BASS);
        CHECK((cs.seq_hint & 0x37u) == 0x37u);
    }
    cu.scr = SCR_GLO; ui_algo(1);
    CHECK(cu.scr == SCR_GLO && cf_scene() == CF_SCENE_FIRE && cs.chord.algo == CF_ALG_TOP_LOCK);
    uint8_t hints = cs.seq_hint;
    cu.scr = SCR_FX; cu.fx_pos = 0;
    uint8_t first = cs.fx.order[0], second = cs.fx.order[1];
    ui_algo(1);
    CHECK(cu.fx_pos == 1 && cs.fx.order[0] == second && cs.fx.order[1] == first && cs.seq_hint == hints);
    cu.scr = SCR_SQ; squ.pat = 0; squ.oct_down = 0; cf_sn.sq_on = 0;
    ui_algo(1);
    CHECK(squ.pat == 1 && cs.seq_hint == hints && cs.chord.algo == CF_ALG_TOP_LOCK);
    cu.scr = SCR_HOME;
}

static void drawing(void)
{
    uint8_t guarded[32 + FLW * FLH + 32];
    flc_t canvas = {guarded + 32, FLW, FLH};
    flora_init(&flo, 123u, 60, 71);
    for (unsigned scene = 0; scene < CF_SCENE_N; scene++)
        for (unsigned backdrop = 0; backdrop < FL_NBD; backdrop++)
            for (unsigned frame = 0; frame < 64; frame++) {
                memset(guarded, 0xA5, sizeof guarded);
                cf_scene_set(scene); flo.backdrop = backdrop;
                flo.t = frame * 37001u; flo.level = frame & 1u ? 65535 : 0;
                if (frame & 1u) flora_note_on(&flo, 60, 127);
                cf_scene_render(&flo, &canvas);
                for (unsigned i = 0; i < 32; i++) CHECK(guarded[i] == 0xA5 && guarded[sizeof guarded - 1 - i] == 0xA5);
                for (unsigned i = 0; i < FLW * FLH; i++) CHECK(canvas.px[i] < 64u);
            }
    /* Check the actual fixed-point boot renderer, not the floating-point preview. */
    flc_t screen = {fb, FB_W, FB_H};
    for (unsigned t = 0; t <= CF_SPLASH_MS; t += 10u) {
        uint32_t n = cf_boot_fish(&screen, t);
        if (!t || t >= 2300u) CHECK(n == 0);
        if (t >= 800u && t <= 1500u) CHECK(n > 0);
    }
    for (unsigned t = 0; t <= CF_SPLASH_MS; t += 100u) {
        cf_boot_draw(t);
        for (unsigned i = 0; i < FB_W * FB_H; i++) CHECK(fb[i] < 64u);
    }
    cf_intro_on = 1; cf_intro_t = 1000; cu.idle_ms = 0;
    CHECK(!cf_intro_draw(40) && !cf_intro_on); /* input skips immediately */
    cf_intro_on = 1; cf_intro_t = CF_SPLASH_MS - 20; cu.idle_ms = 0x7FFFFFFFu;
    CHECK(!cf_intro_draw(40) && !cf_intro_on); /* automatic transition to HOME */
    cf_intro_on = 1; cf_intro_t = 0; cr_safe = 1;
    CHECK(!cf_intro_draw(40) && cf_intro_t == 0); /* never cover SAFE MODE */
    cr_safe = 0; cf_intro_on = 0;
}

/* Independent processes render the same held chord and rhythm, with or without scene switching.
 * The shell compares every physical output word and all six USB capture channels. */
static void render_audio(int change, const char *path)
{
    FILE *out = fopen(path, "wb"); CHECK(out);
    if (!out) return;
    int32_t mix[CTL * 2];
    cf_intro_on = 0; cf_boot_go = 0; cu.scr = SCR_HOME;
    for (unsigned b = 0; b < 64; b++) audio_block(mix, CTL);
    cf_post(CFE_KEY, 1, 0, 1); cf_post(CFE_KEY, 1, 0, 0); /* latch M6 */
    cf_post(CFE_KEY, 7, 0, 1); /* hold a root */
    sq_fresh(&sq_st.pat[0], 4);
    dm_fill(&sq_st.pat[0], dm_nth(4, 0), 0);
    sqp_start(0, 0); cfm_restart(); cfm.running = 1;
    ua_reset(); ua.cap_alt = 1; ua.cap_hold = 0; ua_rate_set(44100);
    for (unsigned b = 0; b < FS * 3u / CTL; b++) {
        if (b % 13u == 0) {
            uint32_t queue = cf_evq_w;
            cf_chord_out_t held = cfc.out;
            if (change) ui_algo(1);
            CHECK(cf_evq_w == queue && !memcmp(&held, &cfc.out, sizeof held));
            cf_snapshot(); cf_feed_plant(40);
            cf_scene_render(&flo, &fl_cv);
        }
        uint32_t w = ua.cw;
        audio_block(mix, CTL);
        CHECK(ua.cw == w + CTL && cfc.cfg.algo == CF_ALG_TOP_LOCK);
        CHECK(cfc.out.sounding && cfc.out.n >= 3 && sqp.on);
        fwrite(mix, sizeof mix, 1, out);
        for (unsigned i = w; i < ua.cw; i++)
            fwrite(ua.cap + (i & (UA_CAP_RING - 1u)) * 6u, 6u * sizeof(int16_t), 1, out);
        ua.cr = ua.cw;
    }
    fclose(out);
}

static void shot(const char *dir, const char *name)
{
    char path[1024]; snprintf(path, sizeof path, "%s/%s.ppm", dir, name);
    FILE *out = fopen(path, "wb"); CHECK(out);
    if (!out) return;
    fprintf(out, "P6\n240 240\n255\n");
    for (unsigned i = 0; i < FB_W * FB_H; i++) {
        uint32_t rgb = cf_pal[fb[i] & 63u];
        uint8_t p[3] = {(uint8_t)(rgb >> 16), (uint8_t)(rgb >> 8), (uint8_t)rgb};
        fwrite(p, 3, 1, out);
    }
    fclose(out);
}

static void shots(const char *dir)
{
    cf_intro_on = 0; cu.scr = SCR_HOME; cu.pop = POP_NONE;
    cf_sn.sq_rec = 0xFFu;
    cs.seq_hint = 0; cs.backdrop = FL_BD_SCENE;
    for (unsigned frame = 0; frame < 49; frame++) {
        char name[32]; snprintf(name, sizeof name, "boot-%02u", frame);
        cf_boot_draw(frame * 50u); shot(dir, name);
    }
    for (unsigned scene = 0; scene < CF_SCENE_N; scene++) {
        cf_scene_set(scene);
        for (unsigned frame = 0; frame < 48; frame++) {
            char name[32]; snprintf(name, sizeof name, "%s-%02u", scene ? "bass" : "bonfire", frame);
            flo.t = 3000 + frame * 100u; flo.level = frame >= 12 && frame < 28 ? 18000 : 1000;
            if (frame == 12) flora_note_on(&flo, 60, 100);
            cf_ui_draw(); shot(dir, name);
        }
    }
}

int main(int argc, char **argv)
{
    emu_fw_options(NULL, 1, 0, 1); emu_hal.ready = 2; emu_fw_init(0);
    if (argc == 4 && !strcmp(argv[1], "--audio")) render_audio(atoi(argv[2]), argv[3]);
    else if (argc == 3 && !strcmp(argv[1], "--shots")) shots(argv[2]);
    else { test_settings(); controls(); drawing(); }
    printf("visuals: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
