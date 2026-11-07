# Chord-first resource and retirement audit

Initial investigation for [#51](https://github.com/chasedeanda/chaseflower-fm1/issues/51),
with the build-baseline portion of [#50](https://github.com/chasedeanda/chaseflower-fm1/issues/50).
Source: `5bfd5005901b002a19cc3b955b774d1395b1460b` (current main at investigation).
Product reference: [approved direction, revision 4654c34](https://github.com/chasedeanda/chaseflower-fm1/blob/4654c341b8b58a6a434f333a7f51543baf5a578a/docs/CHORD-FIRST-VISION.md).

This initial audit measures the existing exploration build and proposes decisions.
It originally removed no features and changed no save formats. The user then
approved removing scenes from the development default; that implementation and
paired measurements are recorded in [SCENE-FREE.md](SCENE-FREE.md). PLAY → selected
progression remains the next product milestone. Actual device timings, the
successor UI and storage migration remain open; this report does not complete
either issue.

## Measured baseline

Fresh JieLi device build, normal default options, `FELUCCA_SIZE` enabled,
FM6 + VA + drums, USB audio/MIDI, flash/updater, UART and CDC enabled.
The ELF is the source of these numbers, not the host emulator.

| Region | Linked bytes | Capacity | Headroom |
| --- | ---: | ---: | ---: |
| Ordinary RAM (`.data + .bss`) | 96,776 | 98,304 | 1,528 |
| POOL | 333,852 | 344,064 | 10,212 |
| XIP (including load images of RAM code/data) | 427,908 | 573,440 | 145,532 |
| RAM-resident code (`.ram_text + .c1_text`) | 3,016 | 24,576 | 21,560 |
| NOINIT | 216 | 15,664 | 15,448 |

CPU0 user/interrupt stacks occupy separate reserved ranges; ordinary-RAM
headroom is not a stack-watermark measurement. NOINIT capacity is not general
reclaimable working memory. The standard package, register-access and linker
checks pass. Compiler identity, flags, effective macros, dependency hashes and
all 1,850 sized linked symbols are in the
[machine report](../benchmarks/resources/5bfd500/report.json).
The [allocation tables](../benchmarks/resources/5bfd500/report.md) are a smaller view.

The actual compiler dependency closure contains **130 files**. Engine sources
include `eng_fm6.c`, `eng_va.c`, `eng_drum.c`, and **`eng_wheel.c`**. WHEEL remains
in the generic engine table even though the normal CF sound browser offers FM6
and VA. Any WHEEL cut needs a saved-preset/MIDI compatibility decision first.
`fm4_convert.c` is also active: disabled FM4 synthesis does not make its legacy
conversion tables unnecessary.

`ui.c`, `cr_ui.c`, `cr_engine.c`, `cr_draw.c`, `cr_gfx.c`, `chord.c`, and
`cr_settings.c` are absent from this closure. SAMPLE/SLICE/GRAIN, FM4 synthesis,
CZ, QUAD, ANALOG, PHASE, LOFI, VOICE, TRIO, NOISE and PHYS are disabled. Deleting
those sources does not remove active DSP or establish a binary improvement.
`CF` sequencing is active even though inherited `FELUCCA_SEQ` is 0.

`CR_CPU1=1` compiles support, but `cr_c1_off` defaults to 1 and the only settings
writer is in excluded `cr_settings.c`. Normal boot takes `C1_OFF`; the bass worker
buffers/job are absent from this ELF. The explicitly kept second-core entry
section still occupies 128 bytes. Hardware diagnostics must confirm the observed
state for a device comparison. No second-core speedup is assumed.

## Allocations worth investigating

| Allocation | Ordinary RAM | POOL | Interpretation |
| --- | ---: | ---: | --- |
| `sq_st` | 20,336 | — | Eight 2,536-byte patterns plus store/song metadata |
| `sqh` | 5,152 | — | Pattern/song pre-edit and undo copies; do not remove recoverability |
| `trk` | 12,640 | — | Four logical parts plus an outgoing-tail sound; 2,528 bytes each |
| `eng_state` | — | 27,792 | Three melodic engine slots plus tail; 6,948 bytes per slot |
| `va_vs` | — | 3,456 | Four VA state slots, 864 bytes each; patches/LFOs are additional |
| `flo` | — | 12,632 | Flora geometry, particles and state, additional to the two image buffers |
| `fl_px` + `scene_body_px` | — | 12,240 | Scene canvas + fish scratch shared with decorative boot |
| `cv_px` + `fb_dma` | — | 72,960 | Main display/recovery canvas and LCD DMA, not dedicated fish memory |
| `xd_buf` + `cv_buf` + `cho_buf` | — | 170,766 | Delay, CF reverb and chorus lines; `cv_buf` is audio, not a display buffer |
| `sq_loop_buf` | — | 2,016 | Legacy loop-conversion staging |

Patterns plus undo total **25,488 ordinary RAM bytes**. Keeping all eight
96-event fixed-note takes in the current 22-byte event layout alone costs
**16,896 bytes**, before drums, timing, metadata, draft and undo. A semantic
progression model does not automatically recover the whole store. Streaming or
compression would require its own measured design and reliability tests.

The scene canvas, fish scratch and flora model total **24,872 gross POOL bytes**.
The issue's 12,240-byte figure covers the image buffers only. Neither number is
net savings: the focused renderer needs resources, shared helpers may remain,
and only a paired linked build proves what disappears. The intro and all other
references to those allocations must be excluded too. Choosing Progressions or
Nothing already skips most scene tick/render work but leaves these objects linked.

The old generic CR glyph/resampling scratch is **not linked**. The AF_S/AF_M
font objects that *are* linked total **30,331 XIP bytes**. Recovery, crash/update
and other retained low-level screens use the older graphics path. Borrowing a
modern font hierarchy does not require importing `cr_gfx.c` and its extra scratch;
removing the existing canvas/fonts requires a replacement recovery rendering path.

Function sizes are diagnostic leads, not feature-removal deltas. Inlining puts
23,388 bytes in `cf_ui_draw` and 19,800 in `cf_ui_input`; those totals include
shared/inlined work. They cannot be credited to one hidden menu.

## Proposed feature decision matrix

“Retire” below is a target for the explicit focused successor, subject to the
listed gate. It does not remove anything from the exploration firmware now.

| Area | Decision | Player-visible consequence | Resource category / prerequisite |
| --- | --- | --- | --- |
| Progressions HOME, all ideas, Now, actual playback cursor | Keep / develop first | PLAY hears the selected idea; live matches stay distinct from playback position | Bounded semantic playback plan and shared clock, coordinated with playback issue |
| Top Lock, root keys, explicit modifiers, HOLD, linked/optional bass | Keep | Existing manual playing remains immediate | Keep ownership/release, inversion, MIDI and overload safeguards |
| Rich/Dreamy/Custom Auto behavior | Retired already | Simple Harmony On/Off; deliberate extensions | Legacy enum/record bytes remain reserved, not a new optimization |
| Catalogue tag/key/scale gates | Retired already | All 100 ideas remain accessible | Keep descriptive context and stable IDs; no CPU-saving claim |
| Extra live performance layouts | Retire from primary navigation | Top Lock remains the supported playing layout | Old algorithm values still decode existing recorded manual takes |
| Other useful scales | Advanced | Seven modes central; other useful scales remain reachable in detail | Small tables; primarily a navigation decision |
| Bass/Bonfire and fish intro | Keep in exploration; retire in explicit scene-free successor | Current choices remain until a separately identified build boots directly to musical HOME | Paired scene-free link/render/audio evidence; include boot and flora references in the cut |
| Premade drums, arp/strum, optional bass, live chord recording | Keep | These support songwriting and accompaniment | Reuse existing timing/ownership; preserve all supported metres and existing takes |
| Separate metro/rhythm/pattern/step-editor entry flows | Retire through one Accompaniment workspace | PLAY/stop consistently controls the selected accompaniment | First reuse the existing timing machinery; hiding screens alone does not remove state |
| Independent LAYER and per-lane mixed-kit editing | Defer retirement | Focused product uses chords/bass/drums; legacy content must remain recoverable | Explicit legacy playback/export/conversion fixture before changing part count or kit interpretation |
| General grids, pattern/song store | Defer replacement | Musical chord/duration editing becomes primary | Viable semantic model, fixed-note representation, bounded capacity, draft/undo and complete idea recall first |
| Full FM6/VA parameter editing | Advanced | Useful macros/envelope first; explicit deep-edit route remains | Mostly XIP/page work; retain engines, patch formats and custom sounds |
| Factory/user sound banks | Keep, curate deliberately | Preserve saved positions, names and user patches | Stable engine/catalogue IDs; dormant patch deletion does not accelerate the same playing voice |
| WHEEL in generic engine table | Defer evaluation | No normal browser change proposed | Audit legacy user records and MIDI routes before cutting a linked engine |
| FX and outgoing sound tails | Keep | Preserve musical space, releases, recall/bypass behavior | Large POOL allocations; no line-length/tail cut without listening and hardware comparisons |
| Diagnostics | Advanced / Tools-System | Musical HOME stays uncluttered; measurements/export remain | Menu move gives no inherent DSP or RAM saving |
| Old loop/format conversion | Defer retirement | Backups stay readable and old takes remain recoverable | Converter/archival route plus round-trip and interrupted-write fixtures first |
| USB 44.1/48 kHz, MIDI/UART, flash/update/recovery, panic/safe-mode guards | Keep | Recording and recovery continue to work | Protected platform boundary; preserve current host-capture and disabled-effect gates |

## Migration gates before structural cuts

| Existing content | Required handling before a focused build can replace it |
| --- | --- |
| CFS1 settings (188 bytes), CHH1 harmony and FXP1 user FX | Preserve checked versions and stable IDs. Ignore retired controls without reinterpreting their bytes. Introduce new idea records separately. |
| CFQ6 patterns/song across six flash objects | Retain all eight pattern slots, eight bars, 96 events per pattern, 32 song entries and supported metres until an explicitly documented replacement is available. No destructive in-place rewrite. |
| Recorded Auto takes | Preserve their fixed pitches, timing, mask, bass notes and releases. Their `rootkey=0xFF` is not permission to infer transposable degrees. Keep fixed-note events alongside future semantic events. |
| LAYER events, sounds and MIDI channel | Keep legacy playback or provide an explicit recoverable export/conversion route. Silently dropping LAYER or folding it into CHORDS is not acceptable. Archive original payloads and test independent note ownership. |
| Mixed per-lane kits | Preserve the actual kit on each lane in imported playback/export. A single-kit editor cannot silently flatten a saved mixed kit. |
| CFR1 pattern sounds, CFK1 tweaks, user sounds/FM6/VA stores | Retain references/ownership and sound identity through migration; preserve tail behavior and home/pattern associations. |
| Old CFL1 loops | Prove old backup → conversion/export → recovered recording with a fixture before removing `sq_loop_buf` or the decoder. |

The first semantic draft should have explicit capacity/version checks, one-step
undo and a recoverable working draft. A future longer-phrase format must preserve
catalogue durations/repeats and forms such as twelve-bar blues rather than
silently inheriting an eight-slot/four-chord assumption. No new memory estimate
is committed before that model is measured.

## Reproduce and compare

```sh
JIELI_TOOLCHAIN=/path/to/jieli/toolchain \
  build/venv/bin/python tools/audit_resources.py --build \
  --output build/resource-audit/main

# Build the later candidate with its explicit options, retaining its identity.
JIELI_TOOLCHAIN=/path/to/jieli/toolchain \
  build/venv/bin/python tools/audit_resources.py --build \
  --output build/resource-audit/focused \
  --compare benchmarks/resources/5bfd500/report.json

# Re-read an existing capture only when its ELF and dependency hashes still match.
build/venv/bin/python tools/audit_resources.py \
  --inputs build/resource-audit/main --output build/resource-audit/recheck

python3 tests/resource_audit_test.py
```

The audit calls the existing builder, captures the real target compiler's `-MM`
dependencies and `-dM` effective macros using the same `app_flags()`, reads ELF32
section/symbol records, and refuses a changed ELF or changed dependency contents.
The baseline snapshot includes that capture and source/compiler identity.
The ordinary firmware source and storage formats are unchanged by this audit.

For actual CPU/audio evidence use [performance.json](../benchmarks/performance.json)
and [the diagnostics capture procedure](DIAGNOSTICS.md#repeatable-feature-comparisons).
Run paired physical workloads for idle, six-note FM, long-release VA, chords/bass/
drums, an explicit LAYER baseline, all FX, and USB at both rates. Hold exact patch
edits, notes/velocities/releases, tempo/metre, scene, duration and polling constant.
Record audio pressure/lag, same-half work, late/shed/stale estimates, USB loss,
core state, input response and display work. Host timing is not device timing.

## Recommended first implementation slice

1. Finish the playback/session ownership boundary and #50's measured renderer
   baseline alongside the first useful PLAY flow.
2. Add a default-on scene build option and a separately identified scene-free
   candidate. Keep the exploration build unchanged. Exclude fish boot, scene
   geometry/state/canvases and their call sites; retain recovery rendering.
3. Produce paired reports, prove the intended symbols disappeared, inspect the
   first musical HOME, and compare audio/MIDI/USB and physical response.
4. Only then size the semantic draft/legacy-take model and decide LAYER/grid/store
   migration. Leave deep editors in Advanced and preserve audible FX meanwhile.

This order gives a measurable first cut without making data loss or a new
scheduler the price of a simpler instrument.
