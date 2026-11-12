/* SPDX-License-Identifier: GPL-3.0-only
 * Chase FM-1 boot animation: a bass swims past, jumps, and leaves the screen.
 * Included by cf_splash.c. Q8 positions and Q14 rotation; no float, allocation or extra framebuffer.
 */
#define CF_SPLASH_MS 2400u

static int cf_boot_surface(int x, uint32_t t)
{
    return 116 + scene_sin(t / 15u + (uint32_t)x * 9u, 1);
}

static void cf_boot_water(flc_t *c, uint32_t t)
{
    int x, y, i;
    for (y = 0; y < FB_H; y++) {
        for (x = 0; x < FB_W; x++) {
            int shade = y < 116 ? y * y * 400 / (116 * 116) : 650 - (y - 116) * 4;
            c->px[y * FB_W + x] = fl_grad(FL_WATER, 5, shade, x, y);
        }
        if ((y & 15) == 15) CF_PUMP();
    }
    for (i = 0; i < 13; i++) {
        uint32_t h = fl_hash32(123u + (uint32_t)i * 97u);
        flc_pset(c, 10 + (int)(h % 220u), 12 + (int)((h >> 8) % 70u), FL_C(FL_R_SAGE, 2));
    }
    flc_dot(c, 198 * 256, 32 * 256, 4, FL_C(FL_R_SAND, 1));
    flc_dot(c, 200 * 256, 30 * 256, 4, FL_BG);
    for (i = 0; i < 9; i++) {
        int bx = i < 5 ? 5 + i * 5 : 220 + (i - 5) * 5;
        int top = 212 + i * 7 % 17;
        for (y = top; y < FB_H; y++)
            flc_pset(c, bx + scene_sin(t / 20u + (uint32_t)y * 7u + i * 40u, 2), y, FL_C(FL_R_JADE, 3));
    }
}

/* Return the number of fish pixels drawn, also used by the exit regression test. */
static uint32_t cf_boot_fish(flc_t *c, uint32_t t)
{
    int32_t cx, cy, angle = 0, ca, sa, sx0, sy0;
    int x, y, x0, x1, y0, y1;
    uint32_t visible = 0, ft = t + 3000u;
    flc_clear(&fl_cv, FL_BG);
    sx0 = 60 + scene_sin(ft / 18u, 10);
    sy0 = 42 + scene_sin(ft / 7u, 3);
    scene_bass_body(ft, &fl_cv, sx0, sy0, 0);
    if (t < 650u) {
        cx = -40 * 256 + (int32_t)t * 110 * 256 / 650;
        cy = (137 + scene_sin(t / 6u, 2)) * 256;
    } else if (t < 1700u) {
        int32_t a = (int32_t)(t - 650u) * 512 / 1050;
        cx = 70 * 256 + (int32_t)(t - 650u) * 105 * 256 / 1050;
        cy = 137 * 256 - fl_fsin(a) * 88 * 256 / 16384;
        angle = -fl_fcos(a) * 106 / 16384;
    } else {
        /* At 2300 ms even the scaled tail is beyond the right edge. */
        cx = 175 * 256 + (int32_t)(t - 1700u) * 155 * 256 / 700;
        cy = (137 + scene_sin(t / 6u, 2)) * 256;
    }
    ca = fl_fcos(angle);
    sa = fl_fsin(angle);
    x0 = cx / 256 - 66;
    x1 = cx / 256 + 66;
    y0 = cy / 256 - 55;
    y1 = cy / 256 + 55;
    if (x0 < 0) x0 = 0;
    if (x1 >= c->w) x1 = c->w - 1;
    if (y0 < 0) y0 = 0;
    if (y1 >= 175) y1 = 174;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            int32_t dx = (x * 256 - cx) * 4 / 5, dy = (y * 256 - cy) * 4 / 5;
            int sx = ((((ca * dx + sa * dy) >> 14) + 128) >> 8) + sx0;
            int sy = ((((-sa * dx + ca * dy) >> 14) + 128) >> 8) + sy0;
            uint8_t col;
            if ((unsigned)sx >= FLW || (unsigned)sy >= FLH) continue;
            col = fl_px[sy * FLW + sx];
            if (col == FL_BG) continue;
            if (y > cf_boot_surface(x, t) && ((x + y) & 3) == 0) col = FL_S2;
            flc_pset(c, x, y, col);
            visible++;
        }
        if ((y & 15) == 15) CF_PUMP();
    }
    return visible;
}

static void cf_boot_splash(flc_t *c, uint32_t t, uint32_t start, int cx)
{
    int i, age;
    if (t < start || t >= start + 450u) return;
    age = (int)(t - start);
    for (i = 0; i < 9; i++) {
        int x = cx + (i - 4) * 5 * age / 450;
        int y = 116 - (18 + (i * 7 % 14)) * 4 * age * (450 - age) / (450 * 450);
        flc_pset(c, x, y, FL_C(FL_R_SEA, 0));
        if (i & 1) flc_pset(c, x, y + 1, FL_C(FL_R_SAGE, 1));
    }
    scene_line(c, cx - age / 15, 118, cx + age / 15, 118, FL_S3);
}

static void cf_boot_draw(uint32_t t)
{
    static const char NAME[] = "RAKT";
    flc_t canvas = {fb, FB_W, FB_H};
    int i, x;
    if (t > CF_SPLASH_MS) t = CF_SPLASH_MS;
    fb_unclip();
    cf_boot_water(&canvas, t);
    cf_boot_fish(&canvas, t);
    for (x = 0; x < FB_W; x++)
        flc_pset(&canvas, x, cf_boot_surface(x, t), FL_C(FL_R_SEA, 2));
    cf_boot_splash(&canvas, t, 690u, 77);
    cf_boot_splash(&canvas, t, 1520u, 158);
    x = (FB_W - cfn_width(&CFN_16, NAME)) / 2;
    for (i = 0; NAME[i]; i++) {
        int age = (int)t - 850 - i * 40;
        if (age >= 0)
            { char glyph[2]={NAME[i],0};cfn_text(&CFN_16,x,196,glyph,
                     age > 180 ? FL_INK : age > 90 ? FL_C(FL_R_SEA, 0) : FL_C(FL_R_SAGE, 2)); }
        x += CFN_16.g[cfn_code((uint8_t)NAME[i])-32].adv;
    }
    if (t > 1250u) {
        scene_line(&canvas, 92, 216, 148, 216, FL_S3);
        flc_pset(&canvas, 120, 216, FL_C(FL_R_SEA, 0));
    }
}
