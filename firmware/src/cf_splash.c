/* SPDX-License-Identifier: GPL-3.0-only
 * Chase FM-1: the approved swimming/jumping bass intro, while the instrument already plays.
 * SAFE MODE keeps its notice; any key, button or encoder skips the animation.
 * The compatible CoralFlower INFO/USB/package identities remain unchanged.
 */
#include "cf_boot.c"

static void cf_splash_frame(uint32_t t) { cf_boot_draw(t); }

/* Draw the first frame with IRQs off; continue the animation in the live UI loop. */
static void cf_splash(void)
{
    cs_defaults(&cs);
    cf_harmony_defaults(&ch);
    cf_settings_load();                          /* (what was set before the power-off, unless SAFE MODE) */
    cfh_load();
    cf_palette_build();
    fb_full = 1;
    if (cr_safe) {
        fb_fill(FL_BG);
        fb_text_c(&CF_FONT_UI, 120, 90, "SAFE MODE", C_ACC(0), 2);
        fb_text_c(&CF_FONT_UI, 120, 130, "settings and sounds", FL_INK2, 1);
        fb_text_c(&CF_FONT_UI, 120, 144, "are not loaded", FL_INK2, 1);
        cf_present();
        return;
    }
    flora_init(&flo, 0x5EED1u, FLW / 2, FLH + 1);
    flora_set_key(&flo, cs.chord.tonic);
    flo.backdrop = cs.backdrop;
    flora_set_species(&flo, cf_scene() == CF_SCENE_BASS ? FL_SP_CORAL : FL_SP_PLANT);
    cf_splash_frame(0);                          /* fish starts completely off screen */
    cf_present();
}

/* A frame advances by at most 40 ms; any user input skips straight to HOME. */
static uint8_t cf_intro_on = 1;
static uint32_t cf_intro_t;
static int cf_intro_draw(uint32_t dt)
{
    if (!cf_intro_on || cr_safe)
        return 0;
    if (!cf_intro_t) {
        if (cs.boot_sound && !cr_safe)
            cf_boot_go = 1;                      /* keep the existing optional boot chime */
    }
    /* Bound a late first frame so the swim-in and jump still play in order. */
    cf_intro_t += dt ? (dt > 40u ? 40u : dt) : 1u;
    if (cf_intro_t >= CF_SPLASH_MS || cu.idle_ms < cf_intro_t / 2u) {
        cf_intro_on = 0;
        fb_full = 1;
        return 0;
    }
    cf_splash_frame(cf_intro_t);
    cf_present();
    return 1;
}
