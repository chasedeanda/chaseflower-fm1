# Signature 8 implementation and technical report

Eight original, fixed-recipe programs are implemented from [the supplied brief](BRIEF.md). Artist references describe the research team's aesthetic targets; they do not claim artist patch settings or audio similarity. No commercial samples, new engine, old-patch edit, Aquanode transposition change or release is included.

## Preview controls

Open **GLO → Sound library**. Set **K1 Chord** (or K3 Layer) to **Signature 8**. PRESETS now browses exactly eight new sounds in the supplied bank order. Bass has no Signature entries and safely keeps its current sound. Selecting a filter itself never loads a patch. Return to **All** to restore normal discovery.

**Primary** is an additional opt-in curated discovery view: the eight pilot programs plus the existing library with the twelve specified duplicate entries omitted. All original entries and their saved indices remain in **All** and their style banks; primary filtering does not delete or replace them. These two curated views are reversible session-local filters. The existing eight style-bank IDs, Existing and User retain their values.

The new programs retain their one-of-eight style-bank association in metadata. They are hidden from ordinary All/style browsing during this pilot; explicit global-index recall and complete saved Songs remain valid. Device names contain no artist names. Sounds load dry; the optional FX recipes are preview examples, not baked into the patches or auto-recalled by selecting a sound.

## Fixed recipes and integration

The [stable catalog](../../assets/sounds/signature8-catalog.json) assigns SIG8-001…008. New FM factory indices are 97…99; VA indices are 93…97. All old factory IDs and all old global list entries remain in their original order, including under bound/added user-sound fixtures.

**FM selector constraint:** the legacy PTCH namespace already has 128 entries and its user parameter is signed. Rather than change stored bytes or repurpose an old patch, three factory programs append through a shared factory-voice lookup and existing checked deep-patch blobs. The track's PTCH macro remains 0 while the full Signature voice is loaded. Deep editing and saved blobs preserve the actual voice. Explicitly turning PTCH selects a legacy slot, as documented here. This is a factory-entry extension, not three new PTCH numbers. Live loading, staged sequencer loading, deep-preset blobs and user save/recall use the same voice data. CEDAR MALLET's packed ten-character name is CEDAR MALT as supplied; its device label is CEDAR MALLET.

All literal FM operator values/algorithms and VA oscillator/modulation definitions match the brief. No safety trims or creative substitutions were applied. Legacy generated library files remain byte-identical. Ordinary Auto Harmony behavior is untouched.

## Audio measurements

Native real-engine stereo PCM at 44.1 kHz, part level 92 and MASTER unity. Each file contains C4 at velocities 45/80/115, Cmaj9 held two seconds, A3–C4–E4–G4 at 250 ms spacing, C3 and C5 register checks, then C4 sustained three seconds with at least two seconds release. Percussive voices include their natural decay; the final render extends until the voice and output settle, capped at 90 seconds. Dry tests bypass all effects. No normalization or post-render gain changes.

| Dry program | Peak dBFS | Whole-fixture RMS dBFS | Peak active voices incl. tails | Seconds |
| --- | ---: | ---: | ---: | ---: |
| EMBER KEYS | -20.597 | -47.917 | 5 | 24.000726 |
| GHOST CHOIR | -15.583 | -36.604 | 6 | 25.423673 |
| NOIR TINES | -13.437 | -37.616 | 5 | 24.000726 |
| APOLLO VEIL | -17.007 | -40.320 | 7 | 27.787029 |
| FROSTWIRE | -14.816 | -49.775 | 5 | 24.000726 |
| OFFBEAT DUB | -12.690 | -36.044 | 5 | 24.000726 |
| CEDAR MALLET | -21.834 | -50.249 | 5 | 24.000726 |
| RAZOR PULSE | -11.530 | -30.067 | 5 | 24.000726 |

All eight dry and eight fixed-FX previews: **zero clipped samples, no stuck voices, bounded releases, mean DC below 1% full-scale**. All 23 named comparison patches also rendered successfully. Sustained textures overlap release tails across fixture segments; voice peaks can therefore exceed the five-note chord size. Whole-file RMS includes deliberate silent gaps and tails and is not a loudness target.

See [all measurements](levels.csv) and [source/file identity manifest](render-manifest.json). All 16 Signature outputs reproduced byte-for-byte in independent fresh processes. WAVs and a local playback page are generated into `build/signature8/auditions/`; they are not stored as dozens of large source assets.

**Limits:** render checks establish technical behavior, not similarity to the artist recording or a guarantee of acoustic character. EMBER and CEDAR are intentionally soft with these exact recipes. No alternate recipe was invented. The host CPU-seconds/audio-second column includes file writing and is explicitly not a physical FM-1 CPU estimate. Base register/transpose and note events are tested; this report does not claim a spectral/subjective pitch-match study.

## Compatibility and budget evidence

- 495 focused assertions cover exact FM bytes, all VA pair ranges, eight-entry ordering/bank association, UI filter no-load behavior, empty Bass selection, prepared blobs, save/recall and drum-clock stress. Four-bar chord/drum workloads complete for each pilot without voice-shed counter increments in the host fixture.
- Existing library/VA regressions pass **47,046,636 / 230,966 checks**, respectively. Songs save/recall passes **2,136 checks** and **6,230 simulated power cuts** (54,660 storage checks). AddressSanitizer passes the focused 495-check suite.
- Portable legacy fingerprint covers every original FM/VA factory descriptor, all 97 original FM factory voices, all 93 original VA programs and old no-user global list entries. Independently compiled main snapshot and current firmware fingerprints match. The existing library regression additionally exercises five user/binding fixtures.
- USB idle/capture comparisons at 44.1/48 kHz verify identical speaker samples for all eight programs, plus capture/main sample agreement at 44.1 kHz. This uses the real capture renderer with a mock USB controller, not a real host/device timing measurement.
- Paired actual JieLi links against main `519f7aa`: XIP **545,168 → 546,768 bytes (+1,600)**; ordinary RAM **90,208 → 90,208**, POOL **316,792 → 316,792**, RAM code **3,128 → 3,128**. Remaining XIP 26,672 B, ordinary RAM 8,096 B, POOL 27,272 B. Build checks for layout, RAM code and protected regions pass. Linker-summary alignment accounting is retained in the build logs.

[Linked-build evidence](linked-builds.json) records both app identities. These are default local verification app builds; no firmware release/package was published. Physical CPU/deadline/USB latency and listening remain measurements for baseline issue #94, not claims inferred from host speed. No new synthesis buffers or voices were allocated.

## Reproduce

```sh
python3 tools/gen_signature8.py --check
python3 tools/build.py --gen-only  # pinned Pillow 11.1.0 / fontTools 4.66.1
sh tools/test_signature8.sh
sh tools/test_chase3.sh
python3 tools/render_signature8.py
```

The CI includes the focused checks; optional audition generation uses the same renderer without network or device writes. Source implementation and complete technical report are the deliverable; #96 and #98 remain the next work after this pilot.
