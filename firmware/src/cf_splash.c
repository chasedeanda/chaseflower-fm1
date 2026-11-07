/* SPDX-License-Identifier: GPL-3.0-only
 * Native CORD boot; the optional legacy scene profile retains its fish intro.
 * SAFE MODE keeps its notice; any key, button or encoder skips the animation.
 * The compatible CoralFlower INFO/USB/package identities remain unchanged.
 */
#if CF_SCENES
#include "cf_boot.c"

static void cf_splash_frame(uint32_t t) { cf_boot_draw(t); }
#else
#include "cf_boot_cord.c"
#endif

/* Draw the first frame with IRQs off; continue the animation in the live UI loop. */
static void cf_splash(void)
{
    cs_defaults(&cs);
    cf_harmony_defaults(&ch);
    cf_settings_load();                          /* (what was set before the power-off, unless SAFE MODE) */
    cfh_load();
    fxp_load();
    cpb_load();                                  /* theme/Motion must be known before the first CORD frame */
    cf_palette_build();
    fb_full = 1;
    if (cr_safe) {
        fb_fill(FL_BG);
        cfn_center(&CFN_32,120,92,"Safe mode",C_ACC(0));
        cfn_center(&CFN_16,120,132,"Settings and sounds",FL_INK2);
        cfn_center(&CFN_16,120,154,"are not loaded",FL_INK2);
        cf_present();
        return;
    }
#if CF_SCENES
    flora_init(&flo, 0x5EED1u, FLW / 2, FLH + 1);
    flora_set_key(&flo, cs.chord.tonic);
    flo.backdrop = cs.backdrop;
    flora_set_species(&flo, cf_scene() == CF_SCENE_BASS ? FL_SP_CORAL : FL_SP_PLANT);
    cf_splash_frame(0);                          /* fish starts completely off screen */
#else
    cf_intro_t=0;cf_cord_started=fm1_ms;cf_intro_on=cf_motion;
    cf_cord_last.valid=0;cf_cord_draw(cf_motion?0:CF_CORD_BOOT_MS);
#endif
    cf_present();
}

/* A frame advances by at most 40 ms; any user input skips straight to HOME. */
#if CF_SCENES
static uint8_t cf_intro_on = 1;
static uint32_t cf_intro_t;
static int cf_intro_draw(uint32_t dt)
{
    if (!cf_motion) cf_intro_on = 0;
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

#endif
