# CORD G1 — Haze and Drive

This stacks on G0 [PR #102](https://github.com/chasedeanda/cord-fm1/pull/102), commit `e31c9ae566399e5e6f07c452e7bc430ab0adb7e0`. The combined local development build is **1.0.3-dev.g1**. Combined testing was explicitly requested before hardware review or merge. The remaining fixed scenes, additional Vibes, Break Out, companion layer, new effects/artwork and recording/export are outside G1.

Choose Haze or Drive on HOME → Flow, then press PLAY. Engine and Flow show the committed Vibe, current progression/title and stable ID, key, chord, 16-bar position and pending boundary. The untouched stopped session defaults to the selected Vibe's suggested tempo (104/122 BPM); Haze defaults to E major when the key is untouched. Existing saved or explicitly selected key, meter and tempo are preserved. The pilot accepts 4/4 only. Saved Sound/Groove/FX settings start pinned; use Flow K4 and SEL to unpin them when auditioning the full Vibe palettes.

| Page | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| Engine | Energy | Feel | Space | Fresh Take clockwise, Back One counterclockwise |
| Flow | Haze / Drive | Repeat / Jam | Surprise 0–100 | Pin selector; SEL toggles it |
| ARP | Play It Auto / On / Off | Existing mode | Existing octave | Existing rate |

Timing and Live keys remain separate ARP pages. On forces the player's actual ARP settings at a safe chord onset; Off keeps held/gated chords; Auto follows Feel. Repeat also repeats the scheduled Random ARP pattern when that mode is selected. Core, Sound, Texture, Groove, Perform and the other existing banks remain accessible. Fill/Drop/Back stays manual; Drop persists across evolution until Back. REC retains manual ownership, and Engine changes are blocked while recording/replaying.

## Catalog and musical memory

All 130 catalog entries are scanned on the foreground thread. Eligibility is 4/4, current effective scale, valid accompaniment preparation, positive sum of the Vibe's tag weights, and three or four chords. Four-chord records map to `[1,2,3,4]` twice. The user-approved three-chord mapping is `[1,2,3,3]` twice. Each slot is two bars, so the third chord occupies four bars in each half-cycle. The source is not rewritten; degree, explicit color/alteration and slash bass flow through the existing preparation. The original three-chord slash-bass source, ID 133, is eligible without synthesizing a new chord.

A take stores the stable source ID, Vibe, independent chord/bass sound choices, groove, FX and Feel. Six seeded streams keep selections independent. Fresh Take excludes the current source if another is eligible, and prepares the complete unlocked set. Back One restores the preceding committed set while honoring pins. Live requests replace the pending request and commit at the next bar 1. User key edits still take effect at the normal chord boundary; a pending take is prepared again against that key. No compatible selection leaves the current audio and shows `No compatible ideas`.

Repeat holds the musical schedule. Jam changes at most one unlocked groove, Feel region or FX recipe at a loop boundary. Surprise 0 disables evolution; 1–49 schedules it after each two completed loops; 50–100 schedules it after every loop. Manual Play It On excludes automatic Feel changes that would have no audible effect. Manual Off permits only a held/gated alternation. Jam never changes the source or chord roots. Pinned categories and the global Energy/Space, key, BPM and manual ARP remain authoritative through profile changes.

A prepared phrase and compact take mailbox commit on the existing 96 PPQN accompaniment clock. No new sample buffer, sequencer, DSP algorithm, heap allocation or flash write was added to the renderer. Exact profile gains are reached at each Vibe's default Energy; adjustments use bounded interpolation and the existing 50 ms ramps. Space continues to scale only the wet contribution. Existing SAVE uses the current static 16-bar Song snapshot with its source ID and representable voicings; it does not record evolution.

## Verification

After generating headers (`python3 tools/build.py --gen-only`), run `sh tools/test_engine.sh`, `sh tools/test_g0.sh` and `sh tools/test_g1.sh`. G1 writes `catalog.csv`, actual per-event MIDI traces and demo WAVs to `build/g1/`. The report includes every scale and skip reason. The test executable accepts a 32-bit seed in its `--render` argument list. Full logs, firmware packages, UI captures and raw evidence are local.

With the reference major-scale fixture, **32 Haze** and **35 Drive** records are eligible. In 64 draws with the fixed seed, Haze reaches 29 distinct IDs and Drive 28. Source masks, slash bass and static Song round trips are checked. Zero-candidate scales are retained without a fallback key/scale change. See the generated catalog CSV for all scales and counts.

The 1,442 focused checks and four-cycle Haze/Drive × Repeat/Jam matrix verify ownership, queued commits, undo, manual key and pin priority, source preservation, exact Repeat events, bounded Jam changes, stable MIDI clock, byte-identical USB-active/idle physical PCM at 44.1 and 48 kHz, and deterministic duplicate renders. The G0 suite and 29 existing regression suites, ASan/UBSan, ARM syntax, installer/parser and device-link checks passed. These are host and link results. They do not establish hardware sound quality or playability.

| Real device link, same development flags | G0 | G1 | Delta | G1 headroom |
| --- | ---: | ---: | ---: | ---: |
| XIP | 553,972 | 561,120 | +7,148 | 12,320 / 573,440 |
| Ordinary static RAM | 90,624 | 91,088 | +464 | 7,216 / 98,304 |
| Synthesis pool | 316,792 | 316,792 | 0 | 27,272 / 344,064 |
| RAM code | 3,128 | 3,128 | 0 | 21,448 / 24,576 |
| NOINIT | 216 | 216 | 0 | 15,448 / 15,664 |

The G0 baseline is an unmodified archive built with `--release 1.0.3-dev.g1` so the local development version flag is identical. No tag or GitHub release was created. The [hardware checklist](HARDWARE-CHECKLIST.md) remains pending; hardware CPU, stack high-water, voice shedding, USB continuity and guitarist listening remain unmeasured.
