# ChaseFlower 0.6-chase3 verification

Final candidate: `chaseflower-0.6-chase3.fwsc` — 610,313 bytes.

SHA-256: `09e20dd145b8afd36f4ae16f8edcfad4f5eeaebfea23ed6654f1e7ef0bb79d00`.

## Device build

The real JieLi Linux x86 toolchain built the firmware through the existing Docker path on macOS. Build, boot-stub,
MMIO and linker-region checks passed. Compiler/toolchain and complete build output are supplied in the build log.

- Application plus padded stub: 395,988 bytes.
- XIP: 392,180 / 573,440 bytes.
- RAM: 94,940 / 98,304 bytes; 3,364 spare (288 more used than chase2).
- Engine pool: 329,756 / 344,064 bytes; 14,308 spare (unchanged from chase2).
- NOINIT: 216 / 15,664 bytes.

`size: .text 376156 B, .ram_text 2888 B, .c1_text 128 B (RAMTEXT 3016 of 24576), .data 13008 B, .bss 81932 B; XIP 392180 B of 573440 (68.4%), RAM 94940 B of 98304 (96.6%), POOL 329756 B of 344064 (95.8%), NOINIT 216 B of 15664 (1.4%)`

## Installer and update safety

The actual final .fwsc passes the existing browser installer's `checkPackage` function, including its CRC, identity,
layout and loader-marker checks. Identity remains `FM-1_930`, and the INFO family/version is `CoralFlower 0.6-chase3`.
The UI branding is ChaseFlower. The protected sector at 0x4000..0x4FFF and update loader are byte-identical to chase2.
No physical device was opened for an update or flashed by these checks.

## Host verification

- Chase3 integration: **45,944,834 checks, zero failures**. Full 120-instrument DSP render plus eight kit renders,
  captured through USB at 44.1/48 kHz; bank counts and filters; sequencer/LAYER choices; original pool/list positions
  with empty, partial and full user banks, replacements and duplicate bindings; save-as-new and replacement blob/level
  round trips and displayed save numbers; HOME view cycling, persistence, live/sequence following and extreme notes.
- Three 6/8 grooves: correct lengths/unique IDs; internal 60–300 quarter-note BPM, MIDI clock input, actual MIDI clock
  output, metronome accents on counts 1/4, conversion to a sequencer pattern, and matching sequencer bar timing.
  A 6/8 bar is 72 MIDI pulses; 4/4 remains 96. These are host timings, not measured cable/OS jitter.
- Layered stress: FM chord, VA layer, bass, new drums, all FX, Clean output and 48 kHz USB capture across all eight banks.
- AddressSanitizer: integration/library/stress run passed without a reported memory error.
- VA engine suite with the expanded library: **230,705 checks, zero failures**.
- Existing backup: 132; chords: 94; theory: 99; storage: 14,352 checks — all passed.
- USB suite: **1,131,194 protocol/packet/FIR/drift checks** plus 56 capture-active/idle physical-output comparisons at
  both advertised rates, and four Python tool-contract tests — passed. Six simulated 60-second clock runs are host
  simulations, not physical USB endurance measurements.
- Four representative original chase2/chase3 physical-output renders are byte-identical: original chord, original bass,
  original 4/4 drum machine with FX, and sequencer/all-FX/Clean output.
- ARM compilation with UAC enabled and disabled, existing Node installer suite, and deterministic library generator
  `--check` passed.
- Emulator screenshots were inspected for the Chord/Keyboard views, Library filters and all three new 6/8 rhythms.
  The final screenshots are supplied with the release outputs.

The supplied previews are short, deterministic dry renders at velocity 100, part level 92 and master ADC 724.
The instrument level CSV reports the measured final trimmed output. No individual audition render clipped in these
conditions. Source-bank quality and genre placement still need listening; rendering and peak checks are not a
perceptual audition or loudness-match claim.

## Compatibility and limits

Original factory indices, engine numbers, preexisting pool positions and combined-list prefixes are preserved.
New factory sounds follow the original pools' user entries, and new global sounds follow all original engine rows.
The eight restored hidden FM factory sounds are appended too. The settings record remains 188 bytes and user sound
records remain 192 bytes. HOME views use previously unused high bits of the hint byte; saved user trims use marked,
previously unused note bytes. Old records remain readable. Old builds ignore these additions.

The eight genre filters are session-local and reset to All at boot; saved sound choices and HOME view persist.
The existing 6/8 timing is corrected to three quarters per bar, so legacy 6/8 patterns run twice as fast at the same
numeric BPM; no stored pattern data is rewritten. Other meters retain their timing.

Chase2 was confirmed working by Chase. This new candidate has not yet been installed or auditioned on Chase's FM-1.
Physical CPU margin, sustained USB behavior, real output quality and listening preferences remain device checks.
The host emulator cannot establish hardware interrupt latency or underrun behavior.

## Reproduce

```sh
python3 tools/gen_chase_library.py --check
sh tools/test_chase3.sh
sh tools/test_usb.sh
mkdir -p build/chase3-audition
build/host/cf_chase3_test --render build/chase3-audition
```

For the device build, use the existing build prerequisites and `./build.sh --release 0.6-chase3`.
All source-bank revisions, original slot names, licenses and stable IDs are in `assets/sounds/chase3-catalog.json`.
