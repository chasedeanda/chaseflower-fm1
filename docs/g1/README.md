# CORD G1 — Haze and Drive

G1 stacks on G0 commit `e31c9ae566399e5e6f07c452e7bc430ab0adb7e0`. The combined local development build with the revised controls is **1.0.3-dev.g1.1**. [G0 PR #102](https://github.com/chasedeanda/cord-fm1/pull/102)

CORD starts on Jam. Press HOME once to open Flow, choose Haze or Drive with K1, then press PLAY. Jam and Flow keep the applied Vibe, progression title, key, chord, bar, Feel, Groove and FX visible alongside queued changes. Jam and Core show the same progression and bar when the band is playing. A fresh session suggests 104 BPM for Haze or 122 BPM for Drive. Haze defaults to E major when the key is untouched. Saved and manually selected key and tempo take priority. The pilot accepts 4/4.

| Page | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| Jam | Energy | Feel | Space | Fresh Take clockwise, Back One counterclockwise |
| Flow | Haze / Drive | Repeat / Jam | Surprise 0–100 | Open Pins (or press SEL) |
| ARP | Play It Auto / On / Off | Existing mode | Existing octave | Existing rate |

Turn SELECT on Jam or Core to preview a compatible progression, then press SEL to choose it. During playback it starts at bar 1 with the same key, sounds, groove, and FX. Without a preview, SEL opens Chord Lab. To inspect all five pins together, open Pins from Flow. K4 highlights an item, SEL changes it between Pinned and Free, and HOME returns to Flow. Saved Sound, Groove and FX settings start pinned. A direct Feel adjustment pins Feel.

On forces the player's ARP settings at the next chord boundary. Off plays held or gated chords. Auto follows Feel. Use ALGORITHM on ARP to reach Timing and Live keys. Fill, Drop, and Back are on Perform. Drop persists until Back.

## Catalog and musical memory

All 130 catalog entries are scanned on the foreground thread. Eligibility is 4/4, current effective scale, valid accompaniment preparation, positive sum of the Vibe's tag weights, and three or four chords. Four-chord records map to `[1,2,3,4]` twice. The user-approved three-chord mapping is `[1,2,3,3]` twice. Each slot is two bars, so the third chord occupies four bars in each half-cycle. The source is not rewritten; degree, explicit color/alteration and slash bass flow through the existing preparation. The original three-chord slash-bass source, ID 133, is eligible without synthesizing a new chord.

A take stores the stable source ID, Vibe, independent chord/bass sound choices, groove, FX and Feel. Six seeded streams keep selections independent. Fresh Take excludes the current source if another is eligible, and prepares the complete unlocked set. Back One restores the preceding committed set while honoring pins. Live requests replace the pending request and commit at the next bar 1. User key edits still take effect at the normal chord boundary; a pending take is prepared again against that key. No compatible selection leaves the current audio and shows `No compatible ideas`.

Repeat holds the musical schedule. Jam changes at most one unlocked groove, Feel region or FX recipe at a loop boundary. Surprise 0 disables evolution; 1–49 schedules it after each two completed loops; 50–100 schedules it after every loop. Manual Play It On excludes automatic Feel changes that would have no audible effect. Manual Off permits only a held/gated alternation. Jam never changes the source or chord roots. Pinned categories and the global Energy/Space, key, BPM and manual ARP remain authoritative through profile changes.

A prepared phrase and compact take mailbox commit on the existing 96 PPQN accompaniment clock. No new sample buffer, sequencer, DSP algorithm, heap allocation or flash write was added to the renderer. Exact profile gains are reached at each Vibe's default Energy; adjustments use bounded interpolation and the existing 50 ms ramps. Space continues to scale only the wet contribution. Existing SAVE uses the current static 16-bar Song snapshot with its source ID and representable voicings; it does not record evolution.

## Verification

After generating headers (`python3 tools/build.py --gen-only`), run `sh tools/test_engine.sh`, `sh tools/test_g0.sh` and `sh tools/test_g1.sh`. G1 writes `catalog.csv`, actual per-event MIDI traces and demo WAVs to `build/g1/`. The report includes every scale and skip reason. The test executable accepts a 32-bit seed in its `--render` argument list. Full logs, firmware packages, UI captures and raw evidence are local.

With the reference major-scale fixture, **32 Haze** and **35 Drive** records are eligible. In 64 draws with the fixed seed, Haze reaches 29 distinct IDs and Drive 28. Source masks, slash bass and static Song round trips are checked. Zero-candidate scales are retained without a fallback key/scale change. See the generated catalog CSV for all scales and counts.

The 1,485 focused checks cover startup on Jam, the Pins screen, shared progression selection across Jam and Core, manual ownership, queued commits, undo, key, pin priority and source preservation. The four-cycle Haze/Drive × Repeat/Jam matrix checks Repeat events, bounded Jam changes, MIDI clock and matching physical PCM with USB capture active or idle at 44.1 and 48 kHz. Hardware listening, CPU, stack and voice shedding remain part of the device pass.

| Real device link | G0 | G1.1 | Delta | G1.1 headroom |
| --- | ---: | ---: | ---: | ---: |
| XIP | 553,972 | 566,112 | +12,140 | 7,328 / 573,440 |
| Ordinary static RAM | 90,624 | 91,184 | +560 | 7,120 / 98,304 |
| Synthesis pool | 316,792 | 316,792 | 0 | 27,272 / 344,064 |
| RAM code | 3,128 | 3,128 | 0 | 21,448 / 24,576 |
| NOINIT | 216 | 216 | 0 | 15,448 / 15,664 |

The G0 baseline uses the same build profile with version string `1.0.3-dev.g1`. [Five-minute hardware checklist](HARDWARE-CHECKLIST.md)
