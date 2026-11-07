/* SPDX-License-Identifier: GPL-3.0-only */
/* ISR-owned session peaks. Constant work per half; no allocation, formatting or USB messages. */
typedef struct { uint32_t work, wall, pressure, lag, halves; } cf_perf_t;
static volatile cf_perf_t cf_perf;
static inline void cf_perf_half(uint32_t work_us, uint32_t wall_ticks, uint32_t lag_ticks)
{
    if (work_us > cf_perf.work) cf_perf.work = work_us;
    if (wall_ticks > cf_perf.wall) cf_perf.wall = wall_ticks;
    if (lag_ticks > cf_perf.lag) cf_perf.lag = lag_ticks;
    uint32_t pressure = wall_ticks + lag_ticks;
    if (pressure > cf_perf.pressure) cf_perf.pressure = pressure;
    cf_perf.halves++;
}
