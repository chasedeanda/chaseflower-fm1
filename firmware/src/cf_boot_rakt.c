/* SPDX-License-Identifier: GPL-3.0-only
 * RAKT power-on rack, drawn at LCD resolution in the existing framebuffer.
 */
#define CF_RAKT_BOOT_MS 1680u
#define CF_RAKT_FRAME_MS 40u
#define CF_RAKT_READY_MS 1120u

static uint8_t cf_intro_on;
static uint32_t cf_intro_t, cf_rakt_started;
static uint16_t cf_rakt_last = UINT16_MAX;

static void cf_rakt_begin(void)
{
    cf_rakt_started = fm1_ms;
    cf_intro_t = 0;
    cf_rakt_last = UINT16_MAX;
}

static void cf_rakt_palette(void)
{
    cf_rack_colors();
    static const uint8_t surfaces[] = {FL_BG, FL_BG1, 48, FL_BG3};
    for (unsigned surface=0; surface<NELEM(surfaces); surface++) {
        for (unsigned ink=0; ink<2; ink++) {
            unsigned entry = CFN_AA_BASE+(surface*CFN_AA_ROLES+ink)*2;
            cf_pal[entry] = cfn_blend(cf_pal[surfaces[surface]], cf_pal[FL_INK+ink], 33);
            cf_pal[entry+1] = cfn_blend(cf_pal[surfaces[surface]], cf_pal[FL_INK+ink], 67);
        }
    }
    fb_palette(cf_pal, CFN_PAL_N);
}

/* The four letters share an eight-pixel stem and beveled corners. */
static void cf_rakt_wordmark(void)
{
    for (int row=0; row<36; row++) {
        int y=48+row;
        int r=40, a=82, k=124, t=165;
        int edge = row<4 ? 4-row : row>15 ? row-15 : 0;
        fb_hline(r, y, 8, FL_INK);
        if (row<20) {
            int right=35-edge;
            if (row<7 || row>=13) fb_hline(r+8, y, right-8, FL_INK);
            else fb_hline(r+26, y, right-26, FL_INK);
        }
        if (row>=16) fb_hline(r+13+(row-16)*13/19, y, 9, FL_INK);

        int inset = (35-row)/4;
        if (row<7 || (row>=19 && row<26)) fb_hline(a+inset, y, 37-inset*2, FL_INK);
        else {
            fb_hline(a+inset, y, 8, FL_INK);
            fb_hline(a+29-inset, y, 8, FL_INK);
        }

        int distance = row<16 ? 16-row : row>19 ? row-19 : 0;
        fb_hline(k, y, 8, FL_INK);
        fb_hline(k+8+distance*17/16, y, 10, FL_INK);
        if (row<8) fb_hline(t, y, 35, FL_INK);
        else fb_hline(t+13, y, 9, FL_INK);
    }
}

static void cf_rakt_nameplate(void)
{
    cf_rack_plate(RAKT_X,23,RAKT_W,80,RAKT_EAR,FL_BG3);
    for (int x=40; x<201; x+=8) {
        fb_rect(x, 33, 5, 4, RAKT_VENT);
        fb_hline(x, 37, 5, RAKT_EDGE);
    }
    cf_rakt_wordmark();
    fb_hline(40, 93, 39, RAKT_COPPER);
    fb_hline(161, 93, 39, RAKT_COPPER);
    cfn_center(&CFN_11, 120, 97, "FM-1 STUDIO", FL_INK2);
}

static void cf_rakt_mixer(int x, unsigned t)
{
    static const char *const names[] = {"CHORDS", "BASS", "DRUMS", "LAYER"};
    static const uint8_t levels[][4] = {
        {0,0,0,0}, {2,1,0,0}, {4,2,3,1}, {6,4,5,3}, {4,5,6,4}, {5,3,4,5}, {2,2,1,3}, {0,0,0,0},
    };
    unsigned step = t<400 ? 0 : (unsigned)clampi((int)(t-400)/80, 0, NELEM(levels)-1);
    cf_rack_plate(x,107,RAKT_W,RAKT_UNIT_H,RAKT_EAR,FL_BG2);
    for (unsigned part=0; part<NELEM(names); part++) {
        int cx = x+36+47*(int)part;
        if (part) fb_vline(cx-24, 115, 48, RAKT_SHADE);
        cfn_center(&CFN_11, cx, 120, names[part], FL_INK2);
        cf_rack_channel(cx,125,levels[step][part],139+(int)(part%3)*4);
    }
}

static void cf_rakt_output(int x, unsigned t)
{
    int ready = t>=CF_RAKT_READY_MS;
    cf_rack_plate(x,175,RAKT_W,42,RAKT_EAR,FL_BG1);
    cfn_text(&CFN_11, x+24, 192, "MAIN OUT", FL_INK2);
    for (int channel=0; channel<2; channel++) {
        int cx=x+30+channel*19;
        fb_rbox(cx-5, 198, 11, 11, 2, RAKT_NICKEL);
        fb_rbox(cx-3, 200, 7, 7, 2, FL_BLACK);
        fb_pset(cx+1, 205, RAKT_EDGE);
    }
    cf_rack_window(x+91,183,69,25,RAKT_GLASS);
    cfn_center(&CFN_12, x+125, 200, ready ? "READY" : "STANDBY", ready ? RAKT_GREEN : RAKT_AMBER);
    fb_rbox(x+174, 189, 14, 14, 2, FL_BLACK);
    fb_rbox(x+176, 191, 10, 10, 2, RAKT_EDGE);
    fb_rect(x+179, 194, 4, 4, ready ? RAKT_GREEN : RAKT_AMBER);
}

static void cf_rakt_draw(unsigned elapsed)
{
    unsigned t = !cf_motion || elapsed>=CF_RAKT_READY_MS ? CF_RAKT_READY_MS : elapsed;
    t = t/CF_RAKT_FRAME_MS*CF_RAKT_FRAME_MS;
    if (cf_rakt_last==t && !fb_full) return;
    if (cf_rakt_last==UINT16_MAX || fb_full) cf_rakt_palette();
    cf_rakt_last = (uint16_t)t;
    fb_unclip();
    fb_fill(FL_BG);
    cf_rack_cabinet(10,18,220,204,23,194);
    cf_rakt_nameplate();
    for (int x=14; x<=219; x+=205) {
        fb_rect(x, 107, 7, 110, RAKT_RAIL);
        fb_vline(x, 108, 108, RAKT_EDGE);
        for (int y=113; y<216; y+=11) fb_rbox(x+2, y, 4, 4, 1, FL_BLACK);
    }
    CF_PUMP();

    fb_clip(14, 107, 226, 171);
    cf_rakt_mixer(RAKT_X+cf_rack_slide(t<80?0:t-80,RAKT_W), t);
    fb_clip(14, 175, 226, 217);
    cf_rakt_output(RAKT_X+cf_rack_slide(t<200?0:t-200,-RAKT_W), t);
    fb_unclip();
    cf_present();
}

static void cf_rakt_finish(void)
{
    cf_intro_on = 0;
    cf_rakt_last = UINT16_MAX;
    cf_palette_build();
    fb_full = 1;
}

static int cf_intro_draw(uint32_t dt)
{
    if (!cf_intro_on || cr_safe) return 0;
    if (!cf_motion || cu.idle_ms<=dt) {
        cf_rakt_finish();
        return 0;
    }
    uint32_t actual = fm1_ms-cf_rakt_started;
    uint32_t next = dt>=CF_RAKT_BOOT_MS-cf_intro_t ? CF_RAKT_BOOT_MS : cf_intro_t+dt;
    cf_intro_t = actual>next ? actual : next;
    if (cf_intro_t>=CF_RAKT_BOOT_MS) {
        cf_rakt_finish();
        return 0;
    }
    cf_rakt_draw(cf_intro_t);
    return 1;
}
