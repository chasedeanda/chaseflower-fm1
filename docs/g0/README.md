# CORD G0 — live Engine controls

G0 only, based on open PR [#101](https://github.com/chasedeanda/cord-fm1/pull/101), verified head `ec966b80c35699e3ecbf5bb212ad3017e14a13e7`. Target branch: `codex/cord-engine-build-01`. The interrupted Build02 checkout and its uncommitted work remain untouched. All three supplied specifications were read before implementation. G1 remains gated on G0 hardware review and merge.

## Controls

| Engine HOME | Result | Timing |
| --- | --- | --- |
| K1 Energy, 0–100 | Bass/drum stage gain × Energy/100; chord stage gain × (0.60 + 0.40 × Energy/100). Reuses the existing chord-bed curve and removes early gain saturation. | Next audio control tick; 50 ms ramp. |
| K2 Feel, 0–24 | Held chords | Next two-bar chord onset. |
| K2 Feel, 25–64 | Half-note whole-chord reinforcements with 70% gate | Next two-bar chord onset. |
| K2 Feel, 65–100 | Scheduled eighth-note ARP in all eligible stages | Next two-bar chord onset. |
| K3 Space, 0–100 | Existing reverb/delay/chorus wet contribution, zero to current FX baseline; no saved FX/feedback edits | Begins next control tick, smoothed; settled within 250 ms in host checks. |
| K4 clockwise | Fresh Take from existing authored pilot candidates/role combinations | Whole unlocked combination at next bar 1; latest request wins. |
| K4 counterclockwise | Back One, shown as `-Back 1`, restores the preceding committed combination, respecting locks | Next bar 1; stopped requests apply immediately. |

HOME labels this as a temporary pilot and shows XX/16, chord, pin state and `Next` cues. Fresh Take is not catalog-backed in G0. DreamPop starts in bar 1 with the specified 28% stage multiplier, further scaled by Energy and player drum level; bass joins at bar 5. Existing Groove/Lift/Return stages and authored notes are retained. Energy scales gain rather than suppressing note owners, so turning back up from zero recovers the current bed/pulse on its scheduled hits.

ARP → Play It is independent of keyboard ARP: Auto follows Feel; On forces scheduled ARP throughout the cycle using actual manual mode/rate/octave/gate/swing; Off keeps held/gated articulation. Changes are sampled at the next safe chord onset. The Timing page remains; a separate Live keys page retains the keyboard switch while Engine is selected. Outside Engine the existing two-page ARP UI remains. Track 2 keeps its independent owner/settings path.

Perform retains Fill/Drop/Back; Drop persists until Back. Hidden automatic Variation fills are retired from the pilot. Sound/Groove/FX edits pin their category and cancel its queued generated replacement. GLO Engine Locks can unpin. Core and the existing HOME banks remain reachable. Loading preserves tonic, scale, octave, tempo and bass-enable choice. The pilot supports 4/4 only and refuses another selected meter instead of changing it.

Factory/catalog/source data, saved indices, Song/settings formats, REC and USB interfaces are unchanged. No new effects, artwork, companion layer, fixed scenes, recording/export or G1 implementation are included.

## Validation and reproduction

After `python3 tools/build.py --gen-only`:

```sh
sh tools/test_engine.sh
sh tools/test_g0.sh
# Only the same-arrangement, level-matched dry/wet patch pairs:
sh tools/test_g0.sh --ab-only
```

The real firmware host renderer writes full 64-bar MIDI traces and first-16-bar stereo PCM16 WAVs to `build/g0/`. The A/B script compares unchanged Ghost Choir and Breath Pad with identical harmony, groove, bass and FX; only a post-render level-match gain changes. Neither patch is edited or ranked. Supplied specification documents and full raw evidence are retained locally rather than uploaded to this public repository.

Results: 346 focused G0 checks, all 29 existing regression suites, additional theory/backup/storage/installer checks, ARM syntax, device linker/HAL checks, and ASan/UBSan passed. The tests verify actual rate/mode/gate/octave/swing output, On in bars 1/5/9/13, Off/Auto, queued onsets, full-take undo, manual pins, gain/FX response and Drop persistence. Over 64 bars, held/medium/high Feel produce 32/128/512 attack times. Interactive traces report no overlapping chord/bass note-ons or stale notes at stop. Independent repeat processes emit identical PCM/MIDI. Four-cycle physical PCM matches with USB capture active/idle at 44.1/48 kHz.

| A/B capture | Original peak / RMS | Match gain | Matched peak / RMS |
| --- | ---: | ---: | ---: |
| Ghost Choir dry | 6,497 / 1,311.820 | 1.000000 | 6,497 / 1,311.820 |
| Breath Pad dry | 6,497 / 1,679.373 | 0.781137 | 5,075 / 1,311.820 |
| Ghost Choir wet | 5,019 / 924.225 | 1.000000 | 5,019 / 924.225 |
| Breath Pad wet | 6,497 / 1,331.818 | 0.693958 | 4,509 / 924.226 |

Amplitudes are PCM16 units. Each dry/wet pair matches within 0.01 dB RMS. Hardware listening choice remains open.

## Real device link

Same default compiler flags and generated dependencies, comparing an unmodified PR #101 source archive with G0:

| Region | PR #101 | G0 | Delta | G0 headroom |
| --- | ---: | ---: | ---: | ---: |
| XIP | 552,756 | 553,956 | +1,200 | 19,484 / 573,440 |
| Ordinary static RAM | 90,624 | 90,624 | 0 | 7,680 / 98,304 |
| Synthesis pool | 316,792 | 316,792 | 0 | 27,272 / 344,064 |
| RAM code | 3,128 | 3,128 | 0 | 21,448 / 24,576 |
| NOINIT | 216 | 216 | 0 | 15,448 / 15,664 |

Reproduce with `tools/audit_resources.py --build --output build/g0-resource --compare BASELINE/report.json`, using the installed device toolchain. Full local reports retain source/dependency hashes and actual ELF identities. The earlier Build01 development-version build used a longer version string and was 16 XIP bytes larger than this same-flags baseline. No new audio buffer, allocator or flash write enters the renderer.

Hardware sound quality, guitarist playability, CPU, stack high-water and voice shedding remain **unmeasured**. Host results do not validate musical enjoyment. The [five-minute hardware checklist](HARDWARE-CHECKLIST.md) is pending. Nothing was flashed, merged, tagged or released.
