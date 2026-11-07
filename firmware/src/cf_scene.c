/* SPDX-License-Identifier: GPL-3.0-only
 * Chase FM-1: approved bonfire and bass scenes. Integer rendering, no allocation.
 * seq_hint bit 3 selects Bonfire; bit 4 selects Nothing; bit 5 selects Bass.
 * All clear selects Progressions. Bits 0..2 are hints; 6..7 remain HOME views.
 * The 188-byte settings record and backup format do not change.
 */
enum { CF_SCENE_BASS, CF_SCENE_FIRE, CF_SCENE_OFF, CF_SCENE_PROGRESSIONS, CF_SCENE_N };
#define CF_SCENE_FIRE_BIT 0x08u
#define CF_SCENE_OFF_BIT 0x10u
#define CF_SCENE_BASS_BIT 0x20u
static const char *const CF_SCENE_NAME[CF_SCENE_N] = {"Bass", "Bonfire", "Nothing", "Progressions"};

static uint32_t cf_scene(void)
{
    if (cs.seq_hint & CF_SCENE_OFF_BIT) return CF_SCENE_OFF;
    if (cs.seq_hint & CF_SCENE_FIRE_BIT) return CF_SCENE_FIRE;
    return cs.seq_hint & CF_SCENE_BASS_BIT ? CF_SCENE_BASS : CF_SCENE_PROGRESSIONS;
}
static int cf_scene_animated(void) { return cf_scene() <= CF_SCENE_FIRE; }
static void cf_scene_set(uint32_t scene)
{
    cs.seq_hint = (uint8_t)((cs.seq_hint & ~(CF_SCENE_FIRE_BIT | CF_SCENE_OFF_BIT | CF_SCENE_BASS_BIT)) |
                           (scene == CF_SCENE_FIRE ? CF_SCENE_FIRE_BIT :
                            scene == CF_SCENE_OFF ? CF_SCENE_OFF_BIT :
                            scene == CF_SCENE_BASS ? CF_SCENE_BASS_BIT : 0u));
}
static int scene_sin(uint32_t a, int amplitude) { return fl_fsin((int32_t)(a & 1023u)) * amplitude / 16384; }
static void scene_line(flc_t *c, int x0, int y0, int x1, int y1, uint8_t col)
{
    flc_line(c, x0 * 256, y0 * 256, x1 * 256, y1 * 256, col);
}
static void scene_tri(flc_t *c, int x0, int y0, int x1, int y1, int x2, int y2, uint8_t col)
{
    flc_tri(c, x0 * 256, y0 * 256, x1 * 256, y1 * 256, x2 * 256, y2 * 256, col);
}

/* Each live or sequenced note gives a short visual accent, independent of master volume. */
static int scene_pulse(const flora_t *f)
{
    uint32_t i, age;
    int pulse = 0;
    for (i = 0; i < 12u; i++) {
        age = f->t - f->pulse_t[i];
        if (f->pulse_on[i] && age < 600u) {
            int p = (int)(600u - age) * 255 / 600;
            if (p > pulse) pulse = p;
        }
    }
    return pulse > 255 ? 255 : pulse;
}

/* Follow the audio clock's actual beat timestamp, including MIDI clock and 6/8.
 * Keep this separate from note accents: a held chord must not hide the beat.
 * Reading the UI snapshot adds no events, state or work to the audio engine. */
static int scene_beat_pulse(void)
{
    uint32_t age, duration, bpm;
    if ((!cf_sn.running && !cf_sn.metro_on) || !cf_sn.beats_done)
        return 0;
    bpm = cf_sn.ext_clock && cf_sn.ext_bpm ? cf_sn.ext_bpm : cs.bpm;
    if (bpm < 30u) bpm = 30u;
    duration = 45000u / (bpm * (cs.beats == 6u ? 2u : 1u));
    if (duration > 280u) duration = 280u;
    age = cf_sn.now_ms - cf_sn.beat_ms;
    if (age >= duration) return 0;
    return (int)((duration - age) * 255u / duration);
}

static void scene_fire(const flora_t *f, flc_t *c, int pulse, int beat)
{
    int x, y, i, cx = c->w / 2, base = c->h - 10;
    int energy = pulse / 32 + f->level / 8192 + beat / 24;
    int accent = pulse > beat ? pulse : beat;
    uint32_t t = f->t;
    /* A low ring of stones and a warm pool of light. */
    for (y = base - 2; y < c->h; y++)
        for (x = cx - 30; x <= cx + 30; x++) {
            int dx = x - cx, dy = (y - base - 3) * 4;
            if (dx * dx + dy * dy < 900 && ((x + y) & 1) == 0)
                flc_pset(c, x, y, FL_C(FL_R_FIRE, beat > 128 ? 2 : 3));
        }
    for (i = 0; i < 9; i++) {
        int a = i * 114;
        flc_dot(c, (cx + scene_sin(a + 256, 25)) * 256,
                (base + 4 + scene_sin(a, 5)) * 256, 2, FL_C(FL_R_SAND, 2));
    }
    /* Crossed logs, dark bark with lit upper edges. */
    flc_stroke(c, (cx - 19) * 256, (base + 6) * 256, (cx + 18) * 256, (base - 2) * 256,
               5 * 256, 5 * 256, FL_R_COPPER, 700, 500);
    flc_stroke(c, (cx - 18) * 256, (base - 2) * 256, (cx + 19) * 256, (base + 6) * 256,
               5 * 256, 5 * 256, FL_R_COPPER, 500, 700);
    /* One tall, pointed silhouette, with nested molten-gold tongues. The broader
     * foot and narrower crown suggest a ritual bonfire rather than a row of flames. */
    for (i = 0; i < 3; i++) {
        int height = 40 - i * 7 + energy + scene_sin(t / 2u + i * 149u, 3);
        int foot = 20 - i * 6 + beat / 85;
        if (height > base - 2) height = base - 2;
        for (y = 0; y < height; y++) {
            int width = (height - 1 - y) * foot / height;
            int curl = scene_sin(t / 3u + y * 13u + i * 101u, 1 + y / 10);
            int rag = scene_sin(t / 2u + y * 37u, 2) * y / height;
            width += rag;
            if (width < 0) width = 0;
            for (x = -width; x <= width; x++) {
                int edge = x < 0 ? -x : x;
                uint8_t col = FL_C(FL_R_FIRE, i == 0 ? 2 : i == 1 ? 1 : 0);
                if (i == 0 && edge == width) col = FL_C(FL_R_FIRE, 3);
                flc_pset(c, cx + curl + x, base - y, col);
            }
        }
    }
    /* A thin bright heart reaches towards the tip, fluttering at the original pace. */
    for (y = 4; y < 34 + energy / 2; y++) {
        int curl = scene_sin(t / 3u + y * 13u + 202u, 1 + y / 10);
        if (y < 23 || ((y + (int)(t / 90u)) & 3) != 0)
            flc_pset(c, cx + curl, base - y, FL_C(FL_R_FIRE, 0));
    }
    /* Curving ribbons break up the triangular mass, so it still reads as fire. */
    for (i = 0; i < 4; i++) {
        int height = 16 + i * 5 + energy / 3;
        int start = (i & 1 ? 1 : -1) * (6 + i);
        for (y = 3; y < height; y++) {
            int curl = scene_sin(t / 3u + y * 19u + i * 155u, 2 + y / 12);
            int xx = cx + start * (height - y) / height + curl;
            uint8_t col = FL_C(FL_R_FIRE, i & 1 ? 1 : 0);
            flc_pset(c, xx, base - y, col);
            if (y < height * 2 / 3) flc_pset(c, xx + 1, base - y, col);
        }
    }
    /* Persistent embers rise slowly at idle; note attacks light additional sparks. */
    for (i = 0; i < 12; i++) {
        uint32_t h = fl_hash32((uint32_t)i + 704u);
        int age = (int)((t / (i < 5 ? 24u : 14u) + h) % 46u);
        if (i >= 5 && accent < (i - 4) * 30) continue;
        x = cx + (int)(h % 25u) - 12 + scene_sin(t / 4u + i * 70u, age / 9);
        y = base - 9 - age;
        flc_pset(c, x, y, FL_C(FL_R_FIRE, age > 30 ? 2 : 0));
    }
}

/* The 80x48 sprite is shared by HOME and the boot jump. Keep scratch storage in POOL,
 * not on the small UI stack, and smaller than a second scene-sized canvas. */
#define CF_FISH_W 80
#define CF_FISH_H 48
#define CF_FISH_X 46
#define CF_FISH_Y 24
static uint8_t scene_body_px[CF_FISH_W * CF_FISH_H] __attribute__((section(".pool")));
static void scene_bass_body(uint32_t t, flc_t *target, int dstx, int dsty, int pulse)
{
    flc_t body = {scene_body_px, CF_FISH_W, CF_FISH_H}, *c = &body;
    int cx = CF_FISH_X, cy = CF_FISH_Y, x, y, i;
    int tail = scene_sin(t / 2u, 4) * (255 - pulse / 2) / 255 - pulse / 64;
    flc_clear(c, FL_BG);
    /* Broad tail and the bass's two-part spiny dorsal fin. */
    scene_tri(c, cx - 23, cy, cx - 39 + tail, cy - 11, cx - 37 + tail, cy + 11, FL_C(FL_R_JADE, 2));
    for (i = -8; i <= 8; i += 4)
        scene_line(c, cx - 24, cy, cx - 36 + tail, cy + i, FL_C(FL_R_LIME, 2));
    for (i = 0; i < 7; i++) {
        int bx = cx - 17 + i * 4;
        int tip = cy - 18 + (i < 3 ? 3 - i : i - 3);
        scene_tri(c, bx, cy - 6, bx + 1, tip, bx + 7, cy - 6, FL_C(FL_R_LIME, 2));
        scene_line(c, bx + 1, tip, bx + 4, cy - 6, FL_C(FL_R_SAND, 1));
    }
    scene_tri(c, cx - 9, cy + 5, cx - 15, cy + 18, cx + 2, cy + 9, FL_C(FL_R_JADE, 2));
    scene_tri(c, cx + 9, cy + 7, cx + 7, cy + 17, cx + 17, cy + 8, FL_C(FL_R_SAND, 1));
    /* Sculpted olive back, pale belly, broken dark lateral stripe: a largemouth bass. */
    for (x = -24; x <= 25; x++) {
        int half = 2 + (int)(121 - x * x / 6) / 12;
        if (half < 2) half = 2;
        for (y = -half; y <= half; y++) {
            uint8_t col = y < -half + 2 ? FL_C(FL_R_LIME, 2) :
                          y < 0 ? FL_C(FL_R_JADE, 1) : y > half / 2 ? FL_C(FL_R_SAND, 0) : FL_C(FL_R_SAND, 1);
            if (y >= -1 && y <= 1 + (x & 1) && (x + 25) % 7 < 5) col = FL_C(FL_R_JADE, 3);
            if (y < -2 && ((x * 3 + y * 7) & 7) == 0) col = FL_C(FL_R_LIME, 1);
            flc_pset(c, cx + x, cy + y, col);
        }
    }
    /* Cheek, gill cover, large jaw, eye and a gently waving pectoral fin. */
    scene_line(c, cx + 12, cy - 7, cx + 15, cy, FL_C(FL_R_JADE, 3));
    scene_line(c, cx + 15, cy, cx + 11, cy + 7, FL_C(FL_R_JADE, 2));
    scene_tri(c, cx + 20, cy + 2, cx + 29, cy - 1, cx + 25, cy + 5, FL_C(FL_R_SAND, 0));
    scene_line(c, cx + 20, cy + 2, cx + 28, cy - 1, FL_C(FL_R_JADE, 3));
    flc_dot(c, (cx + 20) * 256, (cy - 5) * 256, 2, FL_C(FL_R_FIRE, 0));
    flc_dot(c, (cx + 21) * 256, (cy - 5) * 256, 1, FL_BLACK);
    flc_pset(c, cx + 21, cy - 6, FL_INK);
    scene_tri(c, cx + 5, cy + 2, cx - 3, cy + 11 + scene_sin(t / 3u, 2), cx + 10, cy + 7, FL_C(FL_R_COPPER, 1));
    for (x = 0; x < CF_FISH_W; x++) {
        int behind = CF_FISH_X + 25 - x;
        int bend = scene_sin(t / 2u + (uint32_t)((x - CF_FISH_X + 44) * 9), 256);
        int dy_q8 = bend * behind * behind / (69 * 69) * (3 + pulse / 64);
        int dy = dy_q8 / 256;
        for (y = 0; y < CF_FISH_H; y++) {
            uint8_t col = body.px[y * CF_FISH_W + x];
            if (col != FL_BG)
                flc_pset(target, dstx + x - CF_FISH_X, dsty + y - CF_FISH_Y + dy, col);
        }
    }
}

static void scene_bass(const flora_t *f, flc_t *c, int pulse, int beat)
{
    uint32_t t = f->t;
    int accent = pulse > beat ? pulse : beat;
    int cx = c->w / 2 + scene_sin(t / 18u, 10) + beat * 6 / 255 + pulse * 2 / 255;
    int cy = c->h * 3 / 5 + scene_sin(t / 7u, 3) - beat * 4 / 255;
    int x, y, i;
    /* A small, distant school: muted silhouettes and only a few pixels of slow drift.
     * Draw before the bass so the main fish naturally passes in front of them. */
    if (f->backdrop == FL_BD_SCENE) {
        static const int8_t school[4][2] = {{17, 24}, {29, 19}, {40, 23}, {49, 16}};
        for (i = 0; i < 4; i++) {
            int bx = school[i][0] + scene_sin(t / 48u + i * 27u, 2) + beat * (2 + (i & 1)) / 255;
            int by = school[i][1] + scene_sin(t / 64u + i * 91u, 1) - beat * 2 / 255;
            int flick = (int)((t / 1600u + (uint32_t)i) & 1u);
            uint8_t shade = FL_C(FL_R_SAGE, 2);
            scene_line(c, bx - 1, by, bx + 2, by, shade);
            flc_pset(c, bx, by - 1, FL_S3);
            flc_pset(c, bx, by + 1, FL_S3);
            flc_pset(c, bx - 3, by - 1, shade);
            flc_pset(c, bx - 3 + flick, by + 1, shade);
            flc_pset(c, bx - 2, by, FL_S3);
        }
    }
    /* Swaying lake grass, kept below the fish and the musical labels. */
    if (f->backdrop != FL_BD_NONE)
        for (i = 0; i < 8; i++) {
            int bx = 6 + i * 15, top = c->h - 8 - (i * 7 % 13);
            for (y = top; y < c->h; y++) {
                int dx = scene_sin(t / 5u + i * 90u + y * 8u, (c->h - y) / 6);
                flc_pset(c, bx + dx, y, FL_C(FL_R_JADE, 3));
                flc_pset(c, bx + dx + 1, y, FL_C(FL_R_JADE, 2));
            }
        }
    scene_bass_body(t, c, cx, cy, accent);
    /* Small rising bubbles; more appear when notes strike. */
    for (i = 0; i < 5; i++) {
        int age = (int)((t / 35u + i * 13u) % 45u);
        if (i > 1 && accent < i * 50) continue;
        x = cx + 31 + scene_sin(t / 8u + i * 133u, 3);
        y = cy - 2 - age;
        flc_pset(c, x, y, FL_C(FL_R_SEA, 0));
        flc_pset(c, x + 1, y + 1, FL_C(FL_R_SEA, 2));
    }
}

static void cf_scene_render(flora_t *f, flc_t *c)
{
    /* draw_plant clears the display directly in Off mode. Skip all scene work,
     * including the background, beat envelope and intermediate canvas. */
    if (!cf_scene_animated()) return;
    int pulse = scene_pulse(f), beat = scene_beat_pulse();
    /* Use the existing Scene / Ripples / None backdrop setting and shared music clock. */
    f->species = cf_scene() == CF_SCENE_BASS ? FL_SP_CORAL : FL_SP_PLANT;
    fl_backdrop(f, c);
    if (cf_scene() == CF_SCENE_BASS) scene_bass(f, c, pulse, beat);
    else scene_fire(f, c, pulse, beat);
}
