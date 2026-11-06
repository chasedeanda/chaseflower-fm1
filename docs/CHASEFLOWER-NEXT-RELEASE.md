# ChaseFlower 0.6-chase3 — release plan and sound catalog

Updated 2026-11-06. **Planning and source research; no chase3 firmware has been built.**
Chase confirmed that the installed chase2 build boots and works. Keep that release available while developing this one.

## Release scope

1. Three original 6/8 rhythms: Ballad, Electronic and Lo-Fi.
2. Add Michael Norris's Chord and Keyboard HOME views from [CoralFlower issue #8](https://github.com/FicusFicusov/CoralFlower/issues/8).
3. A broad collection of ambient and lo-fi instrument patches, basses and drum kits, organized by style. Start with options and curate by listening over time.
4. Make the larger library practical to browse, and preserve existing saved sound selections, edits, patterns and USB recording behavior.

The initial sound-design target is **128 options: 96 chord/layer patches, 24 bass patches and eight drum kits**. This is a target, not a measured capacity or a claim that the sounds below already exist. External candidate voices can fill appropriate roles after auditioning; they are not additional to this target automatically.

## Three 6/8 rhythms

Use a clear 3+3 pulse: **ONE two three FOUR five six**. Two bars per rhythm, with a restrained second-bar variation.

| Rhythm | First-bar character | Second-bar variation | Sound direction |
| --- | --- | --- | --- |
| Ballad | Kick on 1; gentle snare/rim on 4; soft eighth-note hats/shaker | Quiet pickup near the end; leave room for chords | Soft kit, restrained room tail |
| Electronic | Deep kick establishes 1; clap/rim on 4; sparse contrasting percussion | Extra kick pickup or short hat opening | Round sub kick, clean but dark electronic percussion |
| Lo-Fi | Muffled kick on 1, dusty rim/snare on 4, dark hats | Sparse ghost percussion or one extra kick | Dusty kit, mild grit, quieter hats |

These are new original patterns, not copies of a particular recording. Keep the default straight 6/8 grouping; do not apply the existing 4/4 swing setting by habit. The user can still experiment with swing.

The source already has 6/8 labeling and a 12-step drum grid, but ships no 6/8 rhythms. Its clock currently treats the six counts as tempo beats; the inherited comment calls these eighths. Before publishing, verify actual bar duration, metronome accents and MIDI clock against a reference. Label the tempo unit clearly. A dotted-quarter display would need an explicit conversion and consistent behavior across the metronome, sequencer and clock; changing the global timebase silently would break existing patterns.

Append fresh rhythm IDs (48–50 are currently free) and preserve all older IDs and per-rhythm kit/swing settings. The persistent rhythm record supports IDs 0–63. Test EDIT-to-pattern, playback, bar-boundary rhythm changes, recording, stop/start and power-on resume in this meter.

## Sound banks: deliberately broad first pass

All names in this table are **proposed ChaseFlower design names**, not names of downloaded presets. Each row proposes 12 chord/layer sounds, three basses and one drum kit. Short device names can be assigned during implementation.

| Bank / style | 12 chord and layer options | Three bass options | Drum kit |
| --- | --- | --- | --- |
| **Dusty Room — lo-fi hip-hop** | Dusty EP; Felt Keys; Worn Tines; Muted Bell; Cassette Organ; Pillow Chords; Hazy Clav; Soft Toy Keys; Dull Pluck; Smoke Pad; Velvet Reed; Night Window | Rounded Sub; Muted Finger; Soft Punch | Dust Kit |
| **Tape Garden — ambient lo-fi** | Tape Bloom; Warped Choir; Dusk Strings; Slow Cassette; Drift Organ; Flutter Keys; Faded Glass; Brown Air; Warm Fog; Memory Pad; Uneven Bells; Sleep Drone | Tape Sub; Warm Triangle; Slow Bloom Bass | Tape Kit |
| **Night Jazz — mellow / neo-soul** | Velvet EP; Hollow Tines; Lounge Organ; Soft Vibra; Woody Keys; Warm Clav; Mellow Brass; Reed Chords; Velvet Strings; Gentle Mallet; Soft Poly; Late Bell | Round Finger; Plucked Wood; Warm Mono | Brush-like Kit |
| **Soft Focus — warm ambient** | Soft Horizon; Cloud Choir; Long Bloom; Quiet Strings; Breathing Pad; Still Water; Warm Ensemble; Low Halo; Slow Reed; Gentle Swell; Dark Velvet; Floating Organ | Deep Sine; Soft Floor; Warm Drift | Air Kit |
| **Glass Cloud — bright ambient** | Glass Rain; Ice Choir; Crystal Veil; Air Bells; Shimmer Bed; Star Mallet; Halo Glass; Thin Light; High Bloom; Slow Chimes; Soft Prism; Silver Organ | Clear Sub; Bell Bass; Glass Pulse | Glass Kit |
| **Dub Bloom — dub ambient / downtempo** | Dark Stab; Hollow Chord; Round Organ; Muted Skank; Filter Bloom; Fog Stab; Low Brass; Soft Wire; Resonant Pad; Dim Chime; Warm Pulse; Deep Reed | Dub Sub; Round Pulse; Soft Rubber | Dub Kit |
| **Wood & Water — organic / plucked** | Wood Mallet; Soft Kalimba; Muted Marimba; Bamboo Bell; Nylon-like Pluck; Hollow Reed; Dew Drops; Wind Organ; Wooden Keys; Water Chime; Air Flute; Gentle Bowl | Wood Pluck; Round Thumb; Low Marimba | Hand Kit |
| **Chill Circuit — chill electronic** | Soft PWM; Round Poly; Gentle Sync; Muted Arp; Faded Digital; Hush Lead; Slow Filter; Soft Circuit; Dream Pluck; Distant Bell; Motion Pad; Neon Haze | Clean Mono; Dark Pulse; Mellow Reese | Chill Kit |

The highest priority is Dusty Room, Tape Garden and Soft Focus. Keep the other banks available as options rather than cutting them during planning. These are synthesized interpretations: Felt Keys is a soft key-like patch, not a sampled acoustic piano; Nylon-like Pluck is not a guitar recording; Brush-like Kit is noise-shaped percussion, not recorded brushes.

New drum kits should offer genuinely different kick/snare/rim/hat voicing and balance, not just different names. Use the existing drum engine's pitch, noise, filtering, decay, velocity morph, grit and room controls. Preserve the existing kits and kit indices; append new ones. The three new rhythms should work with every kit, rather than having their sound baked into their patterns.

Existing part FX sends are deliberately preserved when switching sounds. Do not make a preset silently replace the user's mixer/FX settings. Include suggested reverb/delay/chorus settings in the audition guide for sounds that benefit from them.

## Actual external sounds found

### Aquanode: the strongest first audition source

[Aquanode Soundscapes](https://github.com/aquanodemusic/aquanode-soundscapes) describes its collection as aquatic, ambient and dubby. Its [Dexed notes](https://github.com/aquanodemusic/aquanode-soundscapes/tree/main/dexed) distinguish Aquanode's original banks from the Infinite Cycles remakes. The repository's [license](https://github.com/aquanodemusic/aquanode-soundscapes/blob/main/LICENSE) is CC0-1.0.

Downloaded the **five original Aquanode banks** at revision `f0e59287c9991038f688c5e7457db77117638124`. Each has 32 slots. The four valid banks are saved unchanged under `assets/sounds/candidates/aquanode/`, with the license, checksums and per-slot source audit.

| Bank | Slots | File checks | Next action |
| --- | --- | --- | --- |
| Aquanode Presets 1 | 32 | Valid bank header, length, MIDI data bytes and checksum | Audition |
| Aquanode Presets 2 | 32 | Valid | Audition |
| Aquanode Presets 3 | 32 | 19 bytes above 127 in the payload; checksum happens to pass | Excluded from candidate files; diagnose separately, do not silently mask bytes |
| Aquanode Presets 4 | 32 | Valid | Audition |
| Aquanode Aquazone Dexed Presets | 32 | Valid | Audition |

The four included banks contain **128 slots, 122 non-default slots and 80 distinct parameter sets**. Deduplication compares the first 118 packed voice bytes, excluding the ten-byte name. This is a data comparison, not a perceptual listening judgment. The five originals together have 106 distinct non-default parameter sets, including the malformed bank; only the 80 from valid banks are ready for the first audition round.

Useful names to investigate include DX Fade, DX Long, DX Swell, DX Flute, DX Subbass, DX Crackle and DX Filled. These names do not establish how they sound. Render them through ChaseFlower before deciding genre, part assignment, level or whether to keep them. No audio audition or device playback of these files has been performed yet.

### Other concrete sources

- [Idiomdrottning's original DX7 bank](https://idiomdrottning.org/dx7-old): creator-authored, published as CC BY-SA 4.0. The author specifically calls out ghostknock for minor chords, plus weepfriend and mellanchor. Tested by its author with Hexter; ChaseFlower sound and format need checking. Preserve the bank's own license and attribution if selected.
- [YSE original FM bank](https://github.com/yvanvds/yse-soundengine/blob/dev/CONTENT-LICENSES.md): 32 original DX7 voices explicitly identified as CC0, with a deterministic generator. A useful additional audition source; no claim yet that it suits lo-fi or ambient.
- [Adventure Kid Waveforms](https://github.com/KristofferKarlAxelEkstrand/AKWF-FREE): CC0 single-cycle waveforms, interesting for future textured oscillators. These need engine integration; they are not DX7 patches.
- [Versilian Community Sample Library](https://github.com/sgossner/VCSL): CC0 recorded instruments/percussion, useful for a future sample-based release. The current build's sample engine is disabled, so these are not drop-in sounds for chase3.

Large DX7 archives can help discovery, but a repository-wide license on an archive containing many manufacturers' and commercial authors' files does not establish each file's provenance. Prefer author-published originals with an explicit content license. Vital, TAL, Synth1 and other plugin presets also need sound-design translation or a different engine; their preset files cannot simply be copied into FM6/VA.

## Where the installed sounds come from

The current build synthesizes FM6 and virtual-analog instruments, and synthesized drums. Its lineage is CoralFlower → ChoralRoot → Felucca, with Melodee and other credited engine contributions. Felucca's original patches, Melodee's voices and CoralFlower's added VA/FM pad presets are documented in `LICENSING.md`, `tools/gen_fm6_patches.py`, `eng_fm6_rom.h` and `eng_va.c`.

There are also eight FM factory sounds currently hidden by `CF_DROP` in `cf_sound.c`: TINE EP, BELL, DX MARIMBA, DRAWBARS, GLASS PAD, KALIMBA, FLUTE and TUBULAR. Audition them alongside new sources. Revealing them is a library change and must preserve old selections just as adding new patches does.

## HOME chord visualization

Use Michael Norris / `norrismw`'s attached patch as the starting point, crediting the author and linking [issue #8](https://github.com/FicusFicusov/CoralFlower/issues/8). No upstream comment or message has been sent.

- Tap HOME while already on HOME: Flower → Chord → Keyboard, with a view-name message.
- From another screen, HOME returns home as before.
- Chord view: large chord name, scale degree, voiced note names and their degrees, bass note(s).
- Keyboard view: five octaves with separate chord/bass colors; window moves to follow out-of-range notes.
- Arpeggiator indication, plus sequencer chord tracking when no live chord is held. Retain the last displayed chord.
- The patch stores the view in bits 6–7 of `cs.seq_hint`; current hint readers use bits 0–2. Keep the 188-byte settings record and existing hints intact; validate invalid view values and reboot/restore behavior.

The downloaded patch passes `git apply --check` against chase2. It changes two files (251 insertions, two deletions); **it has not been applied, built or visually verified in this planning pass**. Its SHA-256 is `5448489150cded209271583cda8ffaedf137c66450645f5ef70e755795b1cd68`.

Check spelling, inversions, bass sharing a chord tone, HOLD, combined chord buttons, OCT ±3, arp octave range, Strum, sequencer playback, incoming MIDI and empty startup. Check clipping/overlap at long chord names, low/high notes and dense voicings. Keep the flower view available.

## Browsing and compatibility work

Genre organization must be usable on the device, not only in this document. Add a bank/category filter (All, Existing, the eight styles, User) for sound selection, with an explicit way to return to All. Chord and layer can share bank metadata; bass uses the matching bass subset. Final control placement needs a focused audit of the existing encoder/button actions so it does not hide an existing operation. Preserve user sounds and expose each sound's name and part.

Important implementation constraints found in the source:

- `cf_sound.c` currently caps each part's list at 160 entries. Existing sounds, user slots and new patches share that limit. Count the complete library and raise/redesign it if needed; do not silently truncate sounds.
- Pool positions are eight-bit values; factory indices, patch slots, user bindings and engine preset indices have their own limits. Verify all bounds with the actual candidate count before generating a library.
- Settings save sound selections as list positions. Appending FM presets can move the VA and user sounds in the combined list. Adding to an engine's factory bank can move its user entries too. **Appending arrays alone is not sufficient for compatibility.** Introduce stable sound identities or a tested migration of old indices for HOME, LAYER, pattern references and saved edits. No settings reset as a shortcut.
- The inherited DX7 parser/bank storage exists, but ChaseFlower's active UI loop does not currently call `fm6_service()`, and the Python sounds command rejects this firmware family. Finish and verify that import path if offering direct import. Bundling selected read-only factory patches is an alternative; do not claim direct .syx import already works.
- Current chase2 build has 3,652 bytes of RAM and 14,308 bytes of engine pool spare. There is considerably more XIP flash room. Keep patch data in read-only flash and measure the real link map; do not load all banks into RAM.
- More stored patches do not inherently increase voice count. Long releases, dense chords, layering, FX and USB can still raise runtime load. Test the expensive combinations.
- Keep boot/update sectors, storage layout, backup support and six-channel USB audio behavior intact. No automatic device flashing.

## Implementation and review order

1. Land the three 6/8 patterns and HOME views on a development branch; verify tempo and visualization behavior.
2. Make an audition renderer/catalog: same notes/chords, duration, velocity and documented dry/FX settings for each candidate. Measure levels and clipping, and create previews. Assign stable IDs, genre, part, source revision, original patch name and license.
3. Build the broad FM/VA library and eight original drum kits; add genre browsing and migration together. Keep the original sounds accessible. Candidate names and counts can change based on listening and resource checks.
4. Render/test the whole library and expensive combinations, including USB active. Run existing chord, theory, storage, backup, VA and USB checks; test old backup restore, saved sound identity and new view persistence.
5. Build the actual JieLi firmware, validate the installer package, record the map/size/checksums, publish source and the candidate .fwsc to Chase's repository. Provide one clearly named installer file and an audition guide. Chase installs manually and chooses keep/tweak/remove by stable ID.

The point of the first audition build is breadth. Do not remove a sound solely because it overlaps a familiar category. Keep the original source bank and metadata in Git when a sound is later hidden, so curation remains reversible.
