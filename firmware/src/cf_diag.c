/* SPDX-License-Identifier: GPL-3.0-only */
/* Development dashboard: independent of visible view, no heap or flash writes.
 * Included after cf_settings.c, before cf_ui.c. Command 80 reads fresh data, regardless of display hold.
 * All times on CF_EMU are explicitly non-hardware. CPU0 stacks are not instrumented in this first iteration. */
enum {
    D_MS, D_SESSION_MS, D_BUDGET, D_CPU_Q8, D_AVG, D_WIN_PEAK, D_PEAK, D_WALL, D_PRESSURE, D_LAG,
    D_LATE, D_STALE, D_SHED, D_STEAL, D_KILL, D_CHORD, D_BASS, D_LAYER, D_DRUMS, D_TAIL,
    D_UI_HZ, D_UI_GAP, D_CORE1, D_C1_STACK, D_RAM, D_RAM_CAP, D_POOL, D_POOL_CAP, D_XIP, D_XIP_CAP,
    D_RAMTEXT, D_USB_STATE, D_USB_RATE, D_UNDER, D_OVER, D_MISS, D_RS_PEAK, D_RING, D_RING_CAP,
    D_HARDWARE, D_CHORD_ENG, D_BASS_ENG, D_LAYER_ENG, D_CHORD_ROW, D_BASS_ROW, D_LAYER_ROW,
    D_FX, D_BPM, D_BEATS, D_HALVES, D_PROFILE, D_N
};
enum { DP_OVERVIEW, DP_TIMING, DP_MEMORY, DP_USB, DP_N };
static const char *const CF_DIAG_PAGES[] = {"Overview", "Timing", "Memory", "USB"};
static const uint8_t CF_DIAG_HZ[] = {2, 4, 8};
static struct {
    uint32_t v[D_N], base[8], start, sampled;
    uint8_t page, hold, rate, booted;
} cfd = {.rate = 1};
_Static_assert(sizeof cfd + sizeof cf_perf <= 288u, "diagnostics state budget");

static void cf_diag_counters(uint32_t *v)
{
    v[0]=felucca_dbg.late; v[1]=cr_dbg.stale; v[2]=shed_count;
    v[3]=voice_steals; v[4]=voice_kills;
#if FELUCCA_UAC
    v[5]=ua.cap_underruns; v[6]=ua.cap_overruns; v[7]=ua.missed_frames;
#else
    v[5]=v[6]=v[7]=0;
#endif
}

static void cf_diag_reset(uint32_t now)
{
    /* Clear only our peaks and copy counter baselines. Never clear shared or retained counters. */
    fm1_irq_off();
    memset((void *)&cf_perf,0,sizeof cf_perf);
    cf_diag_counters(cfd.base);
    fm1_irq_on();
    cfd.start=now; cfd.booted=1; cfd.sampled=now; cfd.hold=0;
}

static void cf_diag_memory(uint32_t *v)
{
#ifndef CF_EMU
    extern uint32_t _data_start[],_data_end[],_data_load[],_bss_end[],_pool_start[],_pool_end[],_rt_start[],_rt_end[];
    extern char _diag_ram_cap[],_diag_pool_cap[],_diag_xip_origin[],_diag_xip_cap[];
    v[D_RAM]=(uint32_t)((uintptr_t)_bss_end-(uintptr_t)_data_start);
    v[D_RAM_CAP]=(uint32_t)(uintptr_t)_diag_ram_cap;
    v[D_POOL]=(uint32_t)((uintptr_t)_pool_end-(uintptr_t)_pool_start);
    v[D_POOL_CAP]=(uint32_t)(uintptr_t)_diag_pool_cap;
    v[D_XIP]=(uint32_t)(uintptr_t)_data_load+(uint32_t)((uintptr_t)_data_end-(uintptr_t)_data_start)-(uint32_t)(uintptr_t)_diag_xip_origin;
    v[D_XIP_CAP]=(uint32_t)(uintptr_t)_diag_xip_cap;
    v[D_RAMTEXT]=(uint32_t)((uintptr_t)_rt_end-(uintptr_t)_rt_start);
#else
    /* Host process sizes are not device memory usage. */
    v[D_RAM]=v[D_RAM_CAP]=v[D_POOL]=v[D_POOL_CAP]=v[D_XIP]=v[D_XIP_CAP]=v[D_RAMTEXT]=0;
#endif
}

static void cf_diag_sample(uint32_t *v, uint32_t now)
{
    uint32_t counters[8], i, p;
    v[D_MS]=now; v[D_SESSION_MS]=now-cfd.start; v[D_BUDGET]=HALF_FRAMES*1000000u/FS;
    /* Bounded snapshot of ISR-owned counters and active flags; no formatting/division here. */
    fm1_irq_off();
    v[D_CPU_Q8]=song.cpu_q8;
    v[D_PEAK]=cf_perf.work; v[D_WALL]=cf_perf.wall; v[D_PRESSURE]=cf_perf.pressure;
    v[D_LAG]=cf_perf.lag; v[D_HALVES]=cf_perf.halves;
    cf_diag_counters(counters);
    v[D_TAIL]=0;
    for(p=0;p<NSND;p++) {
        uint32_t n=0;
        for(i=0;i<NVOICE;i++) n+=(uint32_t)(trk[p].v[i].active!=0);
        if(p<4u) v[D_CHORD+p]=n;
        else if(p==NTRK) v[D_TAIL]=n;
    }
#if FELUCCA_UAC
    v[D_USB_STATE]=(usb.up && usb.config ? 1u : 0u) | (ua.cap_alt ? 2u : 0u) |
        (ua.cap_hold ? 4u : 0u) | (usb.suspended ? 8u : 0u) | (usb_cdc_on ? 16u : 0u);
    v[D_USB_RATE]=ua.r48?48000u:44100u;
    v[D_RS_PEAK]=ua.rs_max_ticks;
    v[D_RING]=ua.cw-ua.cr; v[D_RING_CAP]=UA_CAP_RING;
#else
    v[D_USB_STATE]=v[D_USB_RATE]=v[D_RS_PEAK]=v[D_RING]=v[D_RING_CAP]=0;
#endif
    fm1_irq_on();
    v[D_UNDER]=counters[5]-cfd.base[5]; v[D_OVER]=counters[6]-cfd.base[6]; v[D_MISS]=counters[7]-cfd.base[7];
    /* The first five deltas only; do not overwrite the voice counts. */
    for(i=0;i<5u;i++) v[(uint32_t)D_LATE+i]=counters[i]-cfd.base[i];
    v[D_WALL]/=FM1_TICKS_PER_US; v[D_PRESSURE]/=FM1_TICKS_PER_US;
    v[D_LAG]/=FM1_TICKS_PER_US; v[D_RS_PEAK]/=FM1_TICKS_PER_US;
    v[D_AVG]=cpu_last.avg_us; v[D_WIN_PEAK]=cpu_last.max_us;
    v[D_UI_HZ]=cpu_last.ui_frames; v[D_UI_GAP]=cpu_last.ui_max_ms;
    v[D_CORE1]=cr_dbg.c1; v[D_C1_STACK]=cr_dbg.c1_stack;
#ifdef CF_EMU
    v[D_HARDWARE]=0;
#else
    v[D_HARDWARE]=1;
#endif
    cf_diag_memory(v);
    for(p=0;p<3u;p++) { v[D_CHORD_ENG+p]=trk[p].engine; v[D_CHORD_ROW+p]=cf_snd_pos(p); }
    v[D_PROFILE]=CR_CPU1 && cr_dbg.prof_on;
    v[D_FX]=cs.fx.on; v[D_BPM]=cs.bpm; v[D_BEATS]=cs.beats;
}

static void cf_diag_poll(uint32_t now)
{
    if(!cfd.booted) { cf_diag_reset(now); cf_diag_sample(cfd.v,now); }
    uint32_t previous=cpu_last.t0;
    cpu_window(now); /* previously only the retired UI consumed these windows */
    if(previous!=cpu_last.t0 && cr_dbg.c1==C1_OK) cr_dbg.c1_stack=cr_c1_stack_used();
    if(!cfd.hold && (uint32_t)(now-cfd.sampled)>=1000u/CF_DIAG_HZ[cfd.rate]) {
        cf_diag_sample(cfd.v,now); cfd.sampled=now;
    }
}
