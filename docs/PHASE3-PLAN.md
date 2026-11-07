# Phase 3: progression suggestions

Status: first Suggestion implementation, November 6, 2026. The user narrowed
this phase to suggestions only. Generation, editing, Guided mode and sequencer
handoff are outside the current scope.

Branch: `codex/phase3-progression-planning`, created from `origin/main` at
`89d2599` and fast-forwarded to `7c56dd2` (Chase5 release metadata) before final
verification. The custom fork remains authoritative.

Roadmap: [#1](https://github.com/chasedeanda/chaseflower-fm1/issues/1).
This delivers the initial Suggestion subset of
[#16](https://github.com/chasedeanda/chaseflower-fm1/issues/16).
[#17](https://github.com/chasedeanda/chaseflower-fm1/issues/17) is deferred.

## Playing

Open **GLO → Progressions** using SELECT. Choose a tag and an idea, then play the
shown lower root keys. Key numbers count the 16 lower keys from left to right;
key 5 is the tonic. The browser does not play notes or change the harmony mode.

| Control | Action |
| --- | --- |
| K1 TAG | All, Familiar, Emotional, Dreamy, Dark, Uplifting, Jazzy, Energetic, Experimental. The full tag appears above the suggestion. |
| K2 IDEA | Browse the compatible ideas; the value shows position/count. Stops at either end. |
| K3 KEY | Change the existing tonic setting. |
| K4 SCALE | Change the existing scale setting. |
| SELECT | Navigate GLO pages. Harmony is immediately before Progressions. |
| PRESETS / ALGORITHM | Existing chord-sound / scene selection. |
| Keys / HOLD / PLAY / REC / SAVE | Existing performance, transport, recording and sound-save actions. |

On the adjacent Harmony page, choose Auto and Basic, Rich, Dreamy or Custom.
Quantize must be Basic or Full. Manual shows **USE AUTO** as a reminder; browsing
never enables Auto automatically. Suggestion names describe the underlying Auto
style. Live modifiers remain free to change what is actually played. **Now** shows
the sounding live/sequenced chord, and HOLD is shown separately.

Changing key or style preserves the selected idea. Changing tag or scale preserves
it if compatible, otherwise selects the first match. No match shows an explicit
empty state; an unsupported Auto configuration explains its requirements. KEY and
SCALE are disabled under Quantize Off, consistently with HOME. Leaving/reopening
GLO retains the idea for the session. Browser selection resets at boot.

## Initial library

Twenty original, scale-relative entries cover Major (8), Natural Minor (8),
Dorian (2) and Mixolydian (2). Tags overlap and do not change sounds, FX or tempo.
Other scales show an empty library or Auto eligibility message, as appropriate.

| Context | Degree sequences (one-based) |
| --- | --- |
| Major | 1–5–6–4; 6–4–1–5; 1–6–4–5; 2–5–1; 1–4–5; 1–4–2–5; 1–3–6–4; 1–6–2–5 |
| Natural Minor | 1–6–3–7; 1–4–7–3; 1–7–6–7; 1–6–7–1; 1–4–5–1; 6–7–1–1; 1–3–7–4; 1–2–6–5 |
| Dorian | 1–4–1–4; 1–7–4–1 |
| Mixolydian | 1–7–4–1; 1–4–7–1 |

Degrees and Roman numerals refer to the selected scale. In Natural Minor, degree 5
is minor. The existing numeral helper uses scale-relative numbering (for example,
Mixolydian VII rather than major-relative bVII). Shared degree sequences in
different contexts remain distinct entries. Chord names use existing key spelling
and Auto extension/omission rules, with no new harmony logic.

This is #16's initial useful subset. Expanding toward its approximately 30–50
entries and musical audition on the FM1 remain follow-ups; this change does not
claim the whole issue or its hardware gates complete.

## Implementation and compatibility

`cf_progression.c/.h` contains the const catalog, tag filtering, bounded selection,
underlying-style masks and physical root hints. The current UI uses a ninth GLO
page after Harmony; the original eight page indices and all four HOME views stay
intact. Display work runs in the UI, outside audio processing.

Browser state is two RAM bytes. There is no new framebuffer, heap allocation,
storage object, timer, playback engine, saved-content edit or browsing flash write.
The 188-byte settings, CHH1 harmony record and 22-byte CFQ6 events keep their
existing formats. KEY/SCALE edits use the existing configuration/save path.

## Verification

Run `sh tools/test_progressions.sh` after generating the standard headers. It runs
independent pitch fixtures for every degree in the four contexts, all 12 keys,
all styles, both supported quantization settings and all normal octaves. It also
checks catalog/filter bounds, empty results, physical root hints, actual chord
resolution, UI navigation, key/scale/style updates, selection retention, Manual,
unsupported settings and all catalog/tag rendering cases.

Separate deterministic processes compare every physical output word and all six
USB capture channels with and without browsing at 44.1/48 kHz, while holding a
six-note Auto chord with bass, HOLD, rhythm and FX. The test also checks that
browsing posts no new chord events and preserves the live model and pattern data.
The suite is included in `.github/workflows/check.yml`.

`tests/progression-suggestions.txt` drives the real headless emulator for visible
Manual/Auto, styles, live chord, tags, minor-mode, empty-filter and unsupported
states. Run it with:

```sh
sh tools/emu/cf_headless.sh tests/progression-suggestions.txt build/progression-preview
```

Existing Auto, Chase3, diagnostics, visuals, USB, backup, theory/chord, VA and
storage power-cut suites remain regression gates. An ARM-target syntax check
covers the device include path. These host checks do not establish device timing.

Before a device release: verify physical controls and legibility, listen to the
suggestions, inspect the combined device link map/stack headroom, and complete
FM1 diagnostic and sustained GarageBand recording checks. No device is flashed
and no release is published by this implementation.
