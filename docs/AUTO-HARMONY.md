# Phase 1: Auto harmony

Implementation for roadmap issues #5–#12, integrated with `origin/main` at
`502ff9c` (Chase4 visuals and diagnostics). The user authorized implementation
and an MR on November 6, 2026, with the combined device build after the other
features merge. Device listening, performance measurements and GarageBand
validation remain release checks; host results do not replace the Phase 0 baseline.

## Playing

Open **GLO**, turn **SELECT** to **Harmony**, then turn **K1** to **Auto**.
Manual remains the default. HOME retains Quantize, Key, Scale and Glide.

| Harmony knob | Choices |
| --- | --- |
| K1 Harmony | Manual / Auto |
| K2 Style | Basic / Rich / Dreamy / Custom |
| K3 Voicing | Root / Smooth (existing automatic inversion) |
| K4 Modifiers | Left: follow the layout (Latched in Lock, Moment otherwise). Right: Moment in every layout. |

Auto constructs triads by stacking scale thirds, using the actual scale tones.
It supports seven-note scales with Quantize Basic or Full. In C major, playing
I–V–vi–IV gives C–G–Am–F without a quality button. A seventh modifier gives
Cmaj7–G7–Am7–Fmaj7. Quality overrides are intentional chromatic choices in both
Basic and Full; Full does not undo an explicit Major/Minor/Dim override in Auto.

Quantize Off and scales with other than seven notes use single-note fallback,
with **AUTO?** and a scale-selection message. Auto remains selected so switching
back to a supported scale resumes it. MIDI roots are snapped by the existing
root mapping; an extreme register that clamps a root outside the scale also uses
the explicit fallback. MIDI Direct remains direct note playback.

Basic uses triads; Rich adds a diatonic seventh; Dreamy adds a diatonic seventh
and ninth. Root/Smooth remains a separate choice for every style. Custom uses
the extension selected with SEL K2 (None, 6, 7, 9, Add9, 11 or 13), plus the
same voicing and per-slot inversion/bass settings. These are deterministic
recipes; broader open voicings and elaborate Dreamy rules can follow separately.

## Physical keys and persistence

The root mapping is unchanged. The Chase4 UI remains fixed in Top Lock, with
16 lower root keys and upper modifier keys. ALGORITHM retains Bonfire/Bass and
its FX/sequencer contextual actions. The pure chord model also retains Top,
Side (seven left-side buttons and 20 roots) and Side Lock for regression coverage.

Default Top assignments, left to right:

```text
6  7  9 | Sus2 Sus4 | Add2 11 Add9 | Major Minor | HOLD
```

Side uses the first seven slots: `6 7 9 | Sus2 Sus4 Add2 11`. All playable
modifier slots can be reassigned in SEL, including 13, Add4, Dim, Aug and Power.
HOLD continues to occupy whichever Top button the saved Manual Off form uses;
it is not editable as a modifier. Side retains its existing lack of a physical
HOLD button.

| Action | Result |
| --- | --- |
| Non-Lock layout, or Moment override | Modifiers apply while held; release returns to underlying Auto harmony, subject to retrigger timing. |
| Lock layout, layout-following modifiers | A tap latches a selection. A new gesture replaces it. Simultaneous compatible slots combine. Tapping the sole latched slot again clears it. |
| New root | Inherits the current held/latched modifiers. No selection means the style's underlying harmony. |
| HOLD | Sustains roots only. A new root with none physically down replaces sustained roots. Turning HOLD off releases roots held only by HOLD. |
| Auto/Manual or persistence change | Clears modifier selection and inversion history; preserves root-down and HOLD sustain state. Manual forms remain intact. |

HOLD and modifier persistence remain independent. The Moment override supplies
temporary modifiers even if the separate scene feature fixes the unit in Top Lock.
Existing RETRIG behavior is preserved: **Wait** queues changes after its 50 ms
gather window until the next root; **Slide**, **Notes** and **Chord** use their
existing movement/attack policies. While HOLD sustains with no root physically
down, a modifier can change that sustained chord immediately, as before.

HOME shows AUTO/AUTO?, the actual generated chord and existing HOLD indication.
Key caps show compact modifier labels; conflicting choices show `!` and are
excluded from active lighting. In Wait, selection lighting describes the next
chord while the chord name continues to describe the sounding chord.

SEL keeps its original Manual editor. In Auto: K1 assigns the modifier, K2 sets
the Custom style extension, K3 sets that slot's inversion (Style/Smooth/1st/2nd/3rd),
K4 sets its bass tone. SELECT still picks the slot. The last applied slot supplies
its inversion/bass settings. Existing Manual qualities and editors remain accessible
by switching back to Manual.

## Modifier rules and note limits

- 7 adds the scale's seventh. 9 includes 7; Add9 does not.
- 11 includes 7 and 9. 13 includes 7 and 9, without automatically adding 11.
- Sus2/Sus4 replace the third with the actual scale second/fourth. Add2/Add4 retain it.
- The most recently pressed quality wins; the most recently pressed suspension
  wins. Quality and suspension can combine: Minor + Sus4 removes the third again.
- Extensions stay diatonic even with a quality override.
- Auto generates at most six chord notes, matching the sequencer's existing
  six-note event capacity. Selection prioritizes root, third/suspension, seventh,
  altered fifth and colors before the plain fifth. Ordinary synth allocation and
  overload protection still apply; six logical notes do not guarantee six hardware
  voices under every DSP workload. No new allocation or DSP algorithm is introduced.

## Recording and compatibility

Auto takes and step entry capture the six actual notes and bass. They play those
notes even after switching Auto/Manual, changing modifier assignments or changing
key/scale. Auto recordings intentionally remain fixed when a pattern's Follow is
on; existing Manual Follow events retain their original behavior. Reharmonizing
saved Auto events requires a later versioned performance-event design.

The legacy settings record remains **188 bytes**; pattern events remain **22 bytes**.
Manual forms, user sounds and CFQ6 pattern layout are unchanged. Auto preferences
use a separate checked/versioned `CHH1` record in the unused CoralFlower session
pair at **0xB4000/0xB5000**, with the existing logged A/B storage and quiet erase
policy. Missing/invalid harmony data defaults to Manual; Safe Mode ignores it.
Backup object **68** carries this record; both backup clients recognize it.
Old backups without this object leave existing Auto preferences alone. On an old
installation with no harmony record, Manual is the default.

Auto events use the previously unused high bit of the button field as a tag;
its low byte carries interval-mask bits 16–23. The existing mask holds bits 0–15,
and rootkey is 0xFF (fixed notes). Thus ninths/elevenths/thirteenths retain exact
names without enlarging events or losing saved-note registers. This tag is not
interpreted as physical button lighting. Older firmware still plays the fixed
notes, but may show incomplete extension names/lighting.

## Verification and remaining release work

`sh tools/test_auto.sh` exercises independent harmony fixtures for every degree in
C major, G major, A minor, D Dorian and G Mixolydian; both quantization settings;
all layouts; modifier pairs across seven-note scales; conflict rules; styles;
extreme MIDI registers; momentary/latch/HOLD/Wait; rapid changes; real UI/ISR/MIDI,
bass, arp, glide, recording, fixed playback, settings and harmony storage.

`sh tools/test_usb.sh` includes Auto thirteenth chords plus bass in both rates,
with FX off/on and both output modes, comparing capture-active/idle physical
output word for word. Existing Chase3, backup, storage power-cut and installer
tests remain required. The check workflow runs the new Auto suite.

Latest main was merged before submission. Diagnostics keeps GLO page 6 and
Harmony follows as page 7; all eight pages remain accessible. After merging the
other feature MRs, build/link the combined firmware, inspect RAM/pool/flash
headroom, run the Phase 0 benchmark matrix and verify controls, shedding, sustained
USB recording and listening on the FM-1. This MR does not produce or flash a
release package and does not claim those hardware gates are complete.
