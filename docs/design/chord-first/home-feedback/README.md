# CORD boot, HOME motion and live chord feedback

Follow-up to release 0.6-chase8, based on main
`28aeabb2fcf08e97bdd694f9411714f78a3a0ef6`. Measured firmware:
`29104121ce7519f5e99c0d96be491d07f19cbbe8`.

![CORD boot in Instrument, Studio and Editorial](boot-themes.png)

![Actual CORD boot frames](boot.gif)

![Actual native playback frames](playing.gif)

![Live match](live-match.png) ![Matching with Motion Off](match-motion-off.png)

These are native 240 × 240 firmware frames from the host HAL, with the UI clock
deliberately offset from audio time. They are not hardware photographs.

The missing rings had a device-specific clock-origin bug: `cu.now` is system
uptime, while `cf_now_ms` restarts in `cf_out_init`. On a device the UI initializes
after audio and the startup wait; comparing those timestamps directly against a
150 ms freshness limit can reject every valid level. The emulator's aligned
clocks hid this. The new regression failed on both energy and halo before the fix.

Freshness now observes audio publication progress and expires a stopped publisher
using only UI elapsed time. A consumed peak window catches transients between
display frames, expires in 100 ms using the producer's clock, and is cleared by
mute/Motion Off. The snapshot copies bounded scalar state; rendering never reads
audio/voice buffers or changes gain, scheduling, input or persistence.

A round pulse expands and fades from accepted beat fraction and real output energy.
Its complete 84–104 px diameter stays inside the hero area (y=51–155), behind
foreground text. It replaces the flattened ellipses clipped to the old nameplate.
Four fixed accent blends provide bounded fading without another framebuffer.
The top-right four-dot indicator follows that same beat fraction while a clock
is playing, including at zero output volume. Live sound/tails use level-driven
activity without a fabricated BPM. Wait, pause and clock loss stop the clock
indicator; stale output metadata decays to rest. Motion Off removes both effects
immediately and does not replay an old live-chord accent when turned back On.

Live progression matches use the existing `cf_progression_highlight` contract:
generic ideas match root/third quality; exact-source ideas require their full
intervals. Inversions do not matter. All visible repeated matches fill, including
under HOLD, and release clears them. Single notes/fallback do not count. The
actual sequence underline and editor focus remain independent. Matching is
essential feedback, so it remains visible with Motion Off.

The supplied `cord_boot_3themes.html` and `fm1_adaptive_motion_prototype.html` are
visual references. CORD uses the user-selected piano-key/ring mark, a bounded
45 px tracked Inter Tight wordmark, subtitle and three supplied boot palettes.
Seven white keys and three dark keys assemble in the short startup presentation;
the complete ring and text fit inside 240 × 240. The Dm7 is illustrative logo art.
Saved theme/Motion are read before the first splash. Motion Off shows a finished
static logo; input skips, SAFE retains its recovery notice, and a finite 1.3 s
deadline includes elapsed startup time rather than adding an artificial wait.
The small READY hint appears only once the UI is initialized. Existing audio,
USB/INFO identities and update/recovery behavior remain intact.

Verification: 315 HOME assertions cover clock offsets, stalled/expired windows,
native header pixels, beat phase, actual Auto playing, repeated source matches,
HOLD/release, all-theme complete pulses and Off. Boot checks scan every pixel and
screen edge at 40 ms intervals across three themes, plus input/Off/SAFE and
persisted appearance (5,924,317 checks including capture writes). Native-page comparisons preserve byte-identical physical
output, six USB channels and MIDI at both capture rates; HOME banks,
Accompaniment, Chord Lab and Songs/storage suites also pass. The actual JieLi
link/check passes; [exact linked evidence](../../../../benchmarks/resources/home-feedback/report.md)
adds 192 bytes ordinary RAM and 5,128 bytes XIP, with no POOL/RAM-code change.
These static values do not establish device timing or readability.

Reproduce after generating headers:

```sh
sh tools/test_stage.sh
sh tools/test_cord_boot.sh
mkdir -p build/home-feedback
build/host/cf_stage_test build/home-feedback
sh tools/test_native_pages.sh
```

Physical playback, TFT visibility and input/audio/USB timing still need review on
the device. For animation, use **GLO → Display → K4 Motion On**; at mute the rings
settle while a running clock can still move the status indicator.
