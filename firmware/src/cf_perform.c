/* SPDX-License-Identifier: GPL-3.0-only
 * HOME drum moments. Audio-side, transient state only; included by cf_seqp.c.
 * Shared accompaniment and legacy players use the same boundary/score helpers.
 * No pattern, mix level, saved mute or melodic note ownership is changed. */
enum { CFPERF_NONE, CFPERF_FILL, CFPERF_DROP, CFPERF_BACK };
enum { CFPERF_LOOSE, CFPERF_TIGHT };
enum { CFPERF_FILL_MANUAL=1, CFPERF_FILL_AUTO };
enum { PF_K, PF_S, PF_C, PF_H, PF_O, PF_LT, PF_MT, PF_HT, PF_R, PF_B, PF_X };
static struct {
    uint8_t pending, fill, dropped, style;
    uint32_t bloom, bloom_ms;
} cf_moment;

static uint32_t cf_perf_kit_style(uint32_t kit); /* cf_drum.c: shared kit identities */
static void cf_perf_reset(void)
{
    cf_moment.pending = cf_moment.fill = cf_moment.dropped = 0;
    cf_moment.bloom = cf_moment.bloom_ms = 0;
}
static void cf_perf_queue(uint32_t action)
{
    if (action < CFPERF_FILL || action > CFPERF_BACK)
        return;
    /* Repeated turns cannot stack fills or extend one indefinitely. */
    if (action == CFPERF_FILL && (cf_moment.fill == CFPERF_FILL_MANUAL || cf_moment.dropped))
        return;
    cf_moment.pending = (uint8_t)action;           /* latest request wins at the next bar */
}
static void cf_perf_release_drums(void)
{
    /* One-shots ignore note-off. Fade without charging a voice-steal diagnostic. */
    for (unsigned i = 0; i < NVOICE; i++)
        if (trk[CF_PART_DRUM].v[i].active) {
            trk[CF_PART_DRUM].v[i].gate = 0;
            trk[CF_PART_DRUM].v[i].stage = 4;
        }
    vsq_drop(&trk[CF_PART_DRUM], 128u);
}
static void cf_perf_bar(uint32_t style, int automatic_fill)
{
    uint32_t action = cf_moment.pending;
    int returning = cf_moment.fill || cf_moment.dropped;
    int auto_turn = automatic_fill && !action && cf_moment.fill != CFPERF_FILL_MANUAL;
    cf_moment.pending = cf_moment.fill = 0;
    if (action == CFPERF_DROP) {
        cf_moment.dropped = 1;
        cf_perf_release_drums();
        return;
    }
    if ((action == CFPERF_FILL || auto_turn) && !cf_moment.dropped) {
        cf_moment.fill = auto_turn ? CFPERF_FILL_AUTO : CFPERF_FILL_MANUAL;
        cf_moment.style = (uint8_t)style;
        return;
    }
    if (action == CFPERF_BACK || (returning && !cf_moment.dropped)) {
        cf_moment.dropped = 0;
        if (returning) {
            cf_moment.bloom++;
            cf_moment.bloom_ms = cf_now_ms;
        }
    }
}

/* A one-bar turnaround: the first half keeps the groove, the second half
 * replaces it with snare/tom answers or increasingly close snare/hat rolls.
 * h/sd are the current meter's real drum grid, never an independent timer. */
static int cf_perf_fill_hit(unsigned style, unsigned lane, unsigned j, unsigned n)
{
    if (style == CFPERF_TIGHT) {
        if (lane == PF_H) return 1;
        if (lane == PF_S) return j >= n / 2u || !(j & 1u);
        if (lane == PF_K) return j == 0;
        return 0;
    }
    if (lane == PF_S) return j == 0 || j == n / 2u - 1u;
    if (lane == PF_HT) return j == n / 2u;
    if (lane == PF_MT) return j == n - 3u;
    if (lane == PF_LT) return j == n - 1u;
    if (lane == PF_R) return j == 1u;
    return 0;
}
static int cf_perf_hit(uint32_t lane, uint32_t h, uint32_t sd, int original)
{
    if (cf_moment.dropped) return 0;
    if (!cf_moment.fill || h < sd / 2u) return original;
    return cf_perf_fill_hit(cf_moment.style, lane, h - sd / 2u, sd - sd / 2u);
}
static unsigned cf_perf_fill_level(unsigned style, unsigned lane, unsigned h, unsigned sd, unsigned original)
{
    if (lane == PF_H || lane == PF_R) return style == CFPERF_TIGHT ? 34u : 28u;
    if (lane == PF_K) return original ? original : 85u;
    return (style == CFPERF_TIGHT ? 58u : 48u) + h * 24u / sd;
}
static uint32_t cf_perf_level(uint32_t lane, uint32_t h, uint32_t sd, uint32_t original)
{
    if (!cf_moment.fill || h < sd / 2u) return original;
    return cf_perf_fill_level(cf_moment.style, lane, h, sd, original);
}
