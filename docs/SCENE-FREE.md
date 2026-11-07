# Scene-free chord-first build

The development default now excludes Bass/Bonfire animation, the flora model,
their pixel buffers and the decorative fish intro. This follows the user's
explicit approval to remove scenes after the #51 audit. The first live screen
is Progressions, including when an older backup selected a different scene or
HOME view. The optional startup sound keeps its existing On/Off control.

HOME's Chord, Keyboard and Diagnostics views remain available. ALGORITHM returns
to the main Progressions view; its FX-order and sequencer roles remain intact.
Display keeps theme color and startup sound, and omits the inactive backdrop
control. Old scene/backdrop bytes remain untouched in the 188-byte settings and
backups, so the compatibility build can still read them. No sound, pattern,
recording, undo, transport, MIDI or USB format changes are introduced.

## Measured device-link results

Both profiles use the same firmware inputs and toolchain, with only the scene
compiler option changed. Measurements below compare the scene-free profile with
the matching optional exploration profile, rather than mixing unrelated builds.

| Region | Exploration | Scene-free | Freed | New headroom |
| --- | ---: | ---: | ---: | ---: |
| Ordinary RAM | 96,776 | 96,328 | 448 | 1,976 |
| POOL | 333,852 | 308,980 | 24,872 | 35,084 |
| XIP/flash | 427,860 | 410,020 | 17,840 | 163,420 |
| RAM code | 3,016 | 3,016 | 0 | 21,560 |
| NOINIT | 216 | 216 | 0 | 15,448 |

These are net linker deltas in bytes. Scene/boot allocations `flo`, `fl_px` and
`scene_body_px` are absent from the focused ELF. `flora.c`, `flora_canvas.c` and
`cf_boot.c` are absent from its active compilation dependencies. Shared palette
and small integer helpers remain for static musical screens. No second graphics
buffer or replacement renderer was introduced in this cut.

Full captured inputs, symbol inventories and paired reports:
[exploration](../benchmarks/resources/scene-profiles/exploration/report.md),
[scene-free](../benchmarks/resources/scene-profiles/scene-free/report.md).
The original main baseline at `5bfd500` remains separately recorded. The small
build-flag/helper refactor changes the exploration XIP total by 48 bytes compared
with that original baseline; the table above uses the matched pair.

This creates graphics memory headroom; it does not establish a CPU percentage,
polyphony increase or hardware input-latency improvement. Progressions already
skipped most animation work. Physical listening, controls and timing checks remain
before release.

## Build and verify

`CF_SCENES` defaults to **0** in `cf_visual.h` for both device and emulator.
Build normally for the chord-first profile. `CF_SCENES=1` explicitly restores the
exploration renderer for comparison and compatibility tests.

```sh
JIELI_TOOLCHAIN=/path/to/jieli/toolchain \
  build/venv/bin/python tools/audit_resources.py --build \
  --output build/resource-audit/scene-free

CF_SCENES=1 JIELI_TOOLCHAIN=/path/to/jieli/toolchain \
  build/venv/bin/python tools/audit_resources.py --build \
  --output build/resource-audit/exploration

sh tools/test_scene_free.sh
sh tools/test_visuals.sh  # explicitly tests the optional scenes profile
```

The scene-free suite checks immediate musical HOME, old settings/backdrop bits,
unchanged recorder/undo sizes, static screens, safe-mode notice and HOME views.
Separate real-render processes compare every physical output word, all six USB
capture channels and MIDI between profiles at 44.1/48 kHz, with Auto/HOLD, arp,
bass, sequencer drums and FX. The optional scenes retain their existing renderer,
boot-exit/skip, persistence and audio regressions. The normal feature suites,
backup, storage power-cut tests, sanitizers and real device builds pass.

[Pattern and undo memory explained](PATTERN-MEMORY.md) describes the separate
25,488-byte ordinary-RAM allocation. Those capabilities are retained here.
