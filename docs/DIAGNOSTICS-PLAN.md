# ChaseFlower diagnostics — fourth HOME view

Status: first implementation, 2026-11-06, on `codex/diagnostics-screen`.
Base source: main `83fe5a1` (chase3 firmware). Usage and benchmark instructions: [DIAGNOSTICS.md](DIAGNOSTICS.md).
The sections below retain the design rationale; CPU0 stack instrumentation and physical timing calibration remain follow-ups.

## Goal and navigation

Show trustworthy performance and memory readings while Chase plays, and retain peaks/counter deltas so a brief
overload remains visible after it ends. Help establish which combinations of sounds, voices, effects, graphics
and USB recording fit the device's real-time budget.

HOME cycles **Scene → Chord → Keyboard → Diagnostics → Scene**. Diagnostics is the fourth view, not a rapid
four-tap shortcut. From Scene, three HOME taps reach Diagnostics; the next returns to Scene. From another screen,
HOME still returns home without advancing the selected view.

Use HOME view value 3 in the existing two-bit field (bits 6–7 of `cs.seq_hint`); the first three values remain
unchanged. Persist the selected view using the existing 188-byte settings record. An older build already falls
back to Scene for value 3. Coordinate with the separately planned Bonfire/Bass scene selector: scene selection
must not share these view bits or change the chord mode.

## First-screen priorities

The overview occupies HOME's central 240 × 140 region; preserve its musical header, key labels and controls.
Use compact text, with the most useful readings visible together. This is a schematic, not live measurements:

```text
DIAGNOSTICS       LIVE 00:32
AUDIO 48%        PEAK 71%
WORK 1.39 / 2.90 ms
VOICES C4 B1 L0 D3
SHED 0  LATE 0  STALE~0
USB 48K  U0 O0 M0
RAM  92.7/96 KiB   FIXED
POOL 322/336 KiB   FIXED
STACK0 --         CORE1 OFF
```

The example CPU/voice/USB numbers are illustrative. The memory figures are the existing chase3 build's static
allocations and will change when diagnostic code/state is added. Verify the actual layout with the existing
pixel fonts at 240 × 240; abbreviate labels and keep all numeric buffers bounded.

| Reading | Meaning / implementation |
| --- | --- |
| Audio load, current | Smoothed real-time audio-render load; label it AUDIO rather than whole-chip utilization. Existing `song.cpu_q8` is an approximately 93 ms smoothed value excluding nested TIMER5 work. |
| Audio peak | Worst render load since session reset, plus raw microseconds on the Timing page. Preserve peaks while other views are open. |
| Work / budget | Measured audio half-buffer work versus the firmware's approximately 2.9 ms budget (128 frames at nominal 44.1 kHz). Derive from the running build constants, not a typed screen value. |
| Deadline pressure | Record entry lag plus wall time for the SAME half buffer. Keep this separate from render-only load. Never add independent lag/render peaks and call the result an observed event. |
| Voices | Actual active voices per chord/bass/layer/drum part, including release tails. Show the outgoing tail separately on the detailed page; it also consumes processing time. |
| Shed / steals | Overload shedding (`shed_count`) is different from normal voice reuse (`voice_steals`) and general voice removal (`voice_kills`). Never label all steals as dropouts. |
| Late / stale estimate | DMA-boundary late counts, and the existing profiler's estimate of stale DAC data. A late buffer does not automatically prove an audible glitch. Keep the estimate explicitly marked. |
| USB | Actual capture state/rate, underruns, overruns and missed frames; show OFF or HOLD when appropriate. Normal drift-correction nudges are informational, not errors. |
| RAM / pool | Fixed linker allocations, used/capacity and remaining bytes, separately for ordinary RAM and the large-buffer pool. These are not live heap meters. |
| Stack / core state | Minimum observed stack margin/high-water readings once instrumented, plus actual second-core state. OFF / unavailable must not appear as zero percent usage or a completely free stack. |

The main CPU and audio load are not interchangeable. Audio, UI work, USB service, interrupt nesting and entry delay
can compete differently. Detailed profiling should identify where time went rather than presenting one unexplained
"CPU percent" as the instrument's limit.

## Detailed pages and controls

Keep PRESETS, SELECT, ALGORITHM and KNOB 1–4 musical/contextual behavior on HOME. Chase must be able to change
sounds and play while watching diagnostics. Do not repurpose PLAY/REC or introduce a four-tap timing gesture.

Add a **GLO → Diagnostics** page for display controls:

- K1: Overview / Timing / Memory / USB.
- K2: Live / Hold display.
- K3: Reset session statistics, with a deliberate turn action. It resets measurement baselines, not the synth,
  audio engine, transport, lifetime error counters or persisted settings.
- K4: display refresh rate, default 4 Hz (initial choices 2 / 4 / 8 Hz, subject to overhead measurements).

Return HOME to view the selected diagnostic page. Hold freezes only the visible snapshot; collection continues.
Show HOLD and the capture timestamp clearly. The selected HOME view persists; diagnostic subpage, hold state and
session resets can be RAM-only initially. No flash writes per measurement or during continuous logging.

Timing page: render average/peak, same-buffer lag + wall peak, audio budget, stage times (tick/chords/bass/FX/master/
core wait), UI update rate and worst update gap, overload/late/stale counters, actual core-1 state. Call UI counts
"frames per second" only if they count completed display updates, rather than service-loop attempts.

Memory page: ordinary RAM and pool used/free/capacity, RAM-resident code, application flash occupancy, CPU0 user and
supervisor stack high-water/minimum margin, core-1 stack if active, and guard status. No combined "free RAM" figure
that treats the separate linker regions as one interchangeable heap.

USB page: active/closed/hold state, host-selected rate, ring occupancy and capacity, underrun/overrun/missed-frame
session deltas, resampler timing and stream restarts. Distinguish stream resets from counter wrap when taking deltas.

## What exists and what must be wired up

The source already provides a useful collection foundation:

- `audio.c`: `song.cpu_q8`, `audio_max_us`, `felucca_dbg`, audio window accumulators, entry/wall timings and overload
  shedding. `console.c` documents many counter meanings.
- `cr_cpu1.h`: per-stage timing, latency/stale-data estimates, core state, faults/fallback information and a
  core-1 user-stack watermark helper.
- `voice.c`: active voice state, voice steals/removals and the VA load-based voice cap.
- `usb_audio_stream.c` and `cf_backup.c`: stream counters and read-only SysEx command 79.
- `app.ld`: authoritative RAM/pool boundaries and occupied-section symbols.

Important integration finding: `cpu_window()` and the periodic core-1 stack scan are called by the old `cr_ui.c`,
not by the active CoralFlower/ChaseFlower UI. Existing last-second averages and `cr_dbg.c1_stack` cannot simply be
drawn as though the active loop updates them. Audit the active hooks, integrate the window consumer into the current
UI/service loop, and verify its reset/publish handshake before showing those fields. The ISR-driven smoothed load,
late counters and command-79 diagnostics already have their own active paths.

Core 1 is compiled in, but its off-by-default setting must not be mistaken for an active second worker. Show actual
state. This diagnostic feature does not enable the second core or change power/clock rails.

## Memory and stack measurement

The chase3 linker report has:

- Ordinary RAM: 94,940 / 98,304 bytes; **3,364 bytes** unallocated in that region.
- Large-buffer pool: 329,756 / 344,064 bytes; **14,308 bytes** unallocated in that region.
- Application XIP: 392,180 / 573,440 bytes.

These are build allocations. The source has no general malloc/calloc/realloc/sbrk path for instrument voices.
Playing more notes uses the preallocated voice slots and increases CPU work; it does not allocate more heap.
Do not imply that the RAM/pool percentages should rise and fall with each chord.

For new builds derive these figures from linker symbols/generated build metadata. Measure stack use separately:
CPU0 user/supervisor stacks occupy dedicated guarded regions; core-1's stack is already part of linked RAM.
Do not count it twice when presenting totals.

CPU0 stack watermarking needs deliberate startup instrumentation. Initialize only safe unused stack ranges, never
paint active frames, guards, shared buffers, or retained boot state. Scan incrementally in the main loop at low
frequency; never scan an entire stack inside the audio interrupt. Combine high-water marks with minimum observed
stack pointers/guards where feasible, and label them as observed usage, not a proof against all future overflow.
Until safely implemented and hardware checked, show STACK0 -- rather than fabricated remaining bytes.
Stack watermarks describe usage since boot. Resetting a diagnostic session must not repaint live stacks or reset
hardware guards; keep the since-boot stack maximum labeled separately from resettable timing peaks.

## Measurement overhead and thresholds

Collect independently of the visible screen, using existing counters plus small bounded counters/comparisons.
Snapshot coherently; format text and calculate presentation values outside the ISR. Avoid long interrupt-disabled
copies. Target no heap, no additional framebuffer and no more than approximately 256 bytes of new diagnostic state
for the first version; verify the actual link map before accepting the change.

Keep the normal scene update/render work when overlaying diagnostics, so hiding the artwork does not silently make
the instrument appear faster. Record session peaks across Scene/Chord/Keyboard views too, and compare panel on/off
to quantify diagnostic overhead. Coordinate this with the planned Bonfire/Bass renderer rather than inventing a
second animation timing path.

Initial colors are advisory: under 70% render load normal, 70–85% caution, above 85% high pressure. These are based
on the existing voice-tight/load-management thresholds, not a promise of dropout-free operation. VA allocation
already aims around 82%; the existing overload guard reacts around 85% deadline pressure after two consecutive
halves. Show shed/late/stale/USB counters because average CPU can look healthy after the engine has reduced voices.

Session errors/peaks stay visible until explicitly reset. Use separate warning semantics for late buffers versus
stale estimates/USB data losses. Never reset shared profiler/lifetime counters just by viewing a page or exporting
a snapshot; maintain diagnostic baselines and session-owned peaks instead. Use unsigned-safe delta arithmetic and
mark unavailable/warming-up values with --.

## Limits-discovery workflow

Use a repeatable session for each sound or feature combination. Record firmware/build identity, patch IDs, active
FX, scene, tempo/rhythm, voice counts, USB state/rate and elapsed time alongside the results.

1. Idle baseline for 30 seconds, then one held note.
2. Six-note ambient chord, then repeated chord changes with long release tails.
3. Add bass, layer and drums one at a time.
4. Add chorus, delay and reverb, then the all-FX case.
5. Compare USB closed, 44.1 kHz recording and 48 kHz recording.
6. Repeat with scene animation active and compare the diagnostic overlay's own cost.
7. Hold the demanding combination for several minutes; record peaks, stack minimums and counter deltas.

The useful limit is the first repeatable shedding, timing pressure, data-loss estimate, stack-margin problem or
audible degradation under a specified workload, not the highest average CPU number ever displayed.

The implementation adds audited, read-only command 80 and a host JSON capture/comparison tool. Command 79's existing
layout stays unchanged. JSON preserves exact counters and workload context for subsequent comparison.

## Implementation milestones and acceptance

1. Add fourth HOME state and compact overview with existing live counters; wire up the missing window service.
2. Add session baselines/held peaks, detailed pages and display controls. Show correct static memory metadata.
3. Add safe CPU0 stack instrumentation and validate on the device; expose core-1 helper only when relevant.
4. Run the repeatable manual workload matrix; retain a readable report. Add CSV logging afterward if useful.

Verify HOME cycling/old settings/backup round trips, freeze/reset during notes and transport, correct empty/disabled
states, full numeric ranges, counter rollover and coherent snapshots. Use synthetic timing fixtures for percentages
and same-half peak accounting, and actual link-map checks for memory figures. Compare rendered audio with diagnostics
enabled/disabled; run the existing library, backup and USB suites. Confirm no allocation, flash write or lengthy
critical section is introduced in the audio path. Finish with physical timing/stack tests: emulator CPU timing is
not a hardware measurement. No automatic flashing.
