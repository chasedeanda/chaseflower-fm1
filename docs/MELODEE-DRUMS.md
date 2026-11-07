# Four small sound additions

Current development source adds two chord sounds and two drum kits, for **190 named melodic presets**
(155 chord/layer and 35 bass) and **24 drum kits**. These additions are not in the published firmware release.

| Chord sound | Source | Character to audition |
| --- | --- | --- |
| AQ33 DX SAX | Aquanode Presets 4, slot 26, original name DX SAX | Sustained FM reed/brass candidate |
| AQ34 FULLFLU | Aquanode Presets 4, slot 29, original name DX FULLFLU | Gentler FM flute/organ candidate |

Both chord voices are unchanged CC0 packed patches from the already downloaded Aquanode bank, revision
`f0e59287c9991038f688c5e7457db77117638124`. They use the existing six-operator FM engine's last two patch
slots, 126 and 127. Their native key transposition is zero; their active carriers sit an octave/two octaves
above the played note. The names are source identifiers, not a claim of acoustic saxophone/flute fidelity.
FULLFLU gets a 6 dB output trim to match its dry chord peak to roughly −14 dBFS; its packed patch is unchanged.

Find the new chord sounds at the end of **All**, or under **GLO → Library → Existing**. The third sound-list
pass appends them after the complete original FM/VA library, preserving existing saved sound positions.
The eight genre banks retain their original 12 chord/layer sounds and three basses.

| Kit | Intended character | Starting points |
| --- | --- | --- |
| M Retro | Round low kick, papery snare, soft noise hats, tambourine on the clap lane and a square-wave cowbell | NeoSoul, DreamPop, Waltz |
| M FM | Tonal FM kick, inharmonic snare/rim, metallic hats and a bell-like voice | Garage, DeepHouse, Techno |

On Rhythm Machine, turn **K1 KIT** to either new kit. Sequencer kits and individual drum-lane kits use the
same list. Existing groove defaults are preserved; selecting a kit is an optional change to that groove or
pattern. The new entries follow Chill. Their saved indices are 22 and 23; existing indices 0–21 retain their
names and recipes.

## Scope

This is a small preset-data addition using the existing FM and drum engines. It adds no synthesis engine,
sample loader, DSP state, buffer, voice allocator or settings format. Existing kit definitions and the original
128 catalog records are unchanged; the two chord records append to that catalog. The two new drum recipes
share the existing playback, hat choke and per-lane controls.

A Prophet port would require newer engine interfaces and more performance work. The current melodic library
already covers FM and virtual analog, so that larger project is deferred.

## Source and adaptation

The recipes derive from Kerem Kilic / Ellic Studio's original Melodee `CR_SOUNDS` and `FMK_SOUNDS`, under
GPL-3.0-only, pinned at revision `165d177a425944f3d30b0b669255a6ed0050c3a5`:

- [CR-78 recipe source](https://github.com/keremimo/melodee/blob/165d177a425944f3d30b0b669255a6ed0050c3a5/firmware/src/drum_cr78.c)
- [FM recipe source](https://github.com/keremimo/melodee/blob/165d177a425944f3d30b0b669255a6ed0050c3a5/firmware/src/drum_fm.c)

The complete adapted data and conversion notes are in `firmware/src/melodee_drum_rows.h`, including the
author credit. Source frequencies, ratios and decay targets are translated into the current renderer's scales.
Melodee's times to −40 dB are converted into exponential time constants before selecting the existing decay
parameter. Gains are balanced for this renderer. Both A/B recipes are identical; grit, room and random
variation are off.

These are recipe adaptations, not identical Melodee playback or calibrated emulations. The current DSP does
not provide Melodee's independent FM index decay/feedback or the CR-78 model's pulse-excited resonators and
ringing retriggers. One shared filter replaces separate component filters, and the kit's one tom recipe uses
the existing GM pitch offsets. No proprietary factory programs or external audio samples were imported.

## Audition and validation

Generate dry previews from the actual firmware DSP without downloading any new audio packs:

```sh
python3 tools/build.py --gen-only
sh tools/test_chase3.sh
mkdir -p build/sound-additions
build/host/cf_chase3_test --render build/sound-additions
```

The new chord files are `CF3-F065.wav` and `CF3-F066.wav`; the new kit files are `MEL-D001.wav` and
`MEL-D002.wav`. Chord previews hold for three seconds and release for one. The kit previews use the same
short 6/8 phrase for comparison. Effects and normalization are off.

To regenerate the complete drum-groove gallery:

```sh
python3 tools/render_drum_bank.py
```

The original drum-bank renderer disabled its player clock while isolating audio, so its earlier WAVs contained
only the first hits. The audition harness now explicitly advances the real player and checks that four bars
have elapsed. The gallery also handles Waltz and derives its groove count from the rendered catalog.
Firmware playback code is unchanged.

Validation covers original sound-order compatibility, the unchanged eight style banks, staged patch/blob
loading, slot 127 save/reload, both new kits through 48 kHz USB capture and mixed-kit pattern backup/restore.
The original 64 packed FM voices and first 128 catalog records are unchanged. A previous development-source
comparison also verified byte-identical original sound/kit audition WAVs and no added runtime RAM or voice
buffers. Physical-device listening and timing remain to be checked; no firmware has been flashed.
