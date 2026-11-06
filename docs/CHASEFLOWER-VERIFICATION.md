# ChaseFlower 0.6-chase1 verification

Built on November 6, 2026, from upstream commit `272cad9e85a444d879d72598807b7ca01a098d6d` plus the ChaseFlower
changes. This is a **first-boot candidate**, with USB audio and rhythm changes deferred until a physical FM-1 boot test.
No firmware was flashed during this work.

## Changes

- `firmware/src/cf_splash.c`: ChaseFlower startup title; show the version without the compatibility family prefix.
- `firmware/src/usb.c`: ChaseFlower manufacturer, MIDI product and future audio-input strings. The MIDI product
  still includes `FM-1`, which the existing installer uses to find the port, and fits the 16-character USB string limit.
- `firmware/src/coralflower.c`: default development version `CoralFlower 0.6-chase1`; the upstream INFO family prefix
  is preserved for installer and backup compatibility. USB audio remains disabled.
- `tests/cr_va_test.c`: restore the missing `up_src` stub required by `va_store.c`. The unchanged upstream fixture
  initially failed to compile without it; the correction affects tests only.
- `.github/workflows/check.yml`: run the existing theory and VA tests alongside the existing host checks.
- `.github/workflows/pages.yml`: keep site publication manual in the personal repository.
- `README.md`, `docs/CHASEFLOWER.md`, this report and `releases/0.6-chase1/`: provenance, installation/recovery,
  future milestones and the verified firmware package.

## Build

Environment: Apple Silicon Mac; Python 3.12 virtual environment; Pillow 11.1.0; fontTools 4.66.1. JieLi
`jieli-linux-toolchains-20260730.1`, clang 4.0.1, executed in `debian:bookworm-slim` Linux x86-64 containers by the
repository's existing Docker build path. The unchanged upstream baseline was built before the branding changes.
Both builds passed the existing linker, image, MMIO, stub-lock and memory checks.

```sh
PYTHON="$PWD/../work/venv/bin/python" \
JIELI_TOOLCHAIN="$PWD/../work/jieli/toolchain" \
./build.sh --release 0.6-chase1
```

| Measurement | Result |
|---|---:|
| Installable package | 610,313 bytes |
| Application image, including padded stub | 360,788 bytes |
| XIP application body | 356,980 / 573,440 bytes |
| RAM data + BSS | 92,268 / 98,304 bytes |
| POOL | 322,828 / 344,064 bytes |
| NOINIT | 216 / 15,664 bytes |
| Body size increase over baseline | 32 bytes |
| RAM / POOL increase over baseline | 0 bytes |

The high existing RAM/POOL use makes resource measurements necessary before enabling USB audio. Build sizes do
not establish runtime CPU headroom on the instrument.

## Results

| Check | Status | Evidence |
|---|---|---|
| Real JieLi firmware build and package generation | PASS | `.fwsc` generated; all built-in checks passed |
| ARM-target whole-unit syntax check | PASS | `clang -target armv7m-none-eabi ... -fsyntax-only firmware/src/coralflower.c` |
| Backup / restore | PASS | 132 checks, 0 failed |
| Chord model | PASS | 94 checks, 0 failed |
| Music theory | PASS | 99 checks, 0 failed |
| Storage log / simulated power interruptions | PASS | 14,352 checks, 0 failed; 2,087 power cuts |
| VA synth | PASS | 228,000 checks, 0 failed, after test-fixture correction |
| Existing Node installer suite | PASS | `node --experimental-default-type=module web/test_installer.mjs`: all passed |
| Actual final package in the browser installer's file validator | PASS | Accepted; CRCs, layout, loader marker and `FM-1_930` identity validated |
| Firmware-family compatibility | PASS | Existing classifier allows `CoralFlower 0.6-chase1` with identity `FM-1_930` |
| Boot stub / protected sector / update loader | PASS | Stub binary, decoded flash sector `0x4000..0x4FFF` and `ota.bin` match baseline byte for byte |
| Emulator startup and playback | PASS | Reached HOME, played a note, returned to idle, opened the metronome and returned HOME |
| Audio regression in that deterministic script | PASS | Captured stereo WAV matches baseline byte for byte |
| Normal HOME / metronome screens in that script | PASS | Screenshots match baseline byte for byte |
| ChaseFlower splash | PASS | Emulator screenshot inspected; full name and version fit the display |
| Physical FM-1 boot / preset retention / USB MIDI | NOT RUN | Requires manual installation and connected hardware validation |
| Physical audio quality, CPU timing and long-session behavior | NOT RUN | Requires the FM-1 |
| UAC enumeration / six-channel capture / 44.1 and 48 kHz / GarageBand | DEFERRED | Original USB-audio milestone follows successful physical boot |
| 6/8 rhythm changes | DEFERRED | No rhythm patterns changed in this candidate |

Native Node 10 on PATH could not run the installer suite. The installed Node 22.1.0 was used successfully with
`--experimental-default-type=module`. The initial VA compile and old-Node failures were resolved; the final checks
above passed. No claim of physical hardware verification is made.

The emulator WAV SHA-256 for baseline and ChaseFlower is
`b2cff26f20a85c66ff31677d2e752e2d7204966c977845b2f341cb5855b88b09`.

The installable firmware SHA-256 is
`09deab1ec52b3d944b820f1394c29e53681dd53b976b368a477d7c859298cd0a`.

## Reproduce host checks

First generate headers: `../work/venv/bin/python tools/build.py --gen-only`. Create `build/host`, then run:

```sh
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
  -o build/host/cf_backup_test tests/cf_backup_test.c -lm
build/host/cf_backup_test
cc -std=c99 -Wall -Wextra -Ifirmware/src -o build/host/cf_chord_test \
  tests/cf_chord_test.c firmware/src/cf_chord.c firmware/src/cf_theory.c
build/host/cf_chord_test
cc -std=c99 -Wall -Wextra -Ifirmware/src -o build/host/cf_theory_test \
  tests/cf_theory_test.c firmware/src/cf_theory.c
build/host/cf_theory_test
cc -std=gnu11 -O2 -Wall -Wno-unused-function -Ifirmware/src \
  -o build/host/st_log_test tests/st_log_test.c
build/host/st_log_test
cc -std=gnu11 -O2 -w -Ibuild/gen -Ifirmware/src -o build/host/cr_va_test tests/cr_va_test.c -lm
build/host/cr_va_test
clang -target armv7m-none-eabi -Os -fno-builtin -w -Ifirmware/hal -Ifirmware/src -Ibuild/gen \
  -fsyntax-only firmware/src/coralflower.c
node --experimental-default-type=module web/test_installer.mjs
```

See `docs/CHASEFLOWER.md` for manual installation, recovery and the physical boot checklist.
