# FM-1 interface implementation plan

**Decision:** build the new interface from the user-supplied
`fm1_adaptive_motion_prototype.html`, with **Instrument Teal as the default**.
Motion is **On (adaptive) / Off**, with no Quiet/Expressive style or manual
intensity setting. This is the plan for the successor firmware; it is not a
claim that the browser prototype is running on the FM-1.

The musical delivery order remains **Play → Edit/save → Learn → Suggest**.
The full UI can be planned now while those capabilities ship in that order.
Replacing the display must preserve the existing sound engine, deliberate
harmony controls, note ownership, MIDI/USB, recovery and safe storage behavior.

## 1. Authoritative references

- [Selected prototype, defaulting to Instrument Teal](instrument-teal/prototype.html).
- [Untouched supplied adaptive prototype](instrument-teal/supplied-adaptive-motion-prototype.html).
- [Reference provenance and exact changes](instrument-teal/manifest.json).
- [Visual system](VISUAL-SYSTEM.md), [motion contract](MOTION-SPEC.md),
  [core screen definitions](core-screens.json) and
  [system screen definitions](system-screens.json).
- [Generated engineering handoff](ENGINEERING-HANDOFF.md) and
  [player guide](../../CHORD-FIRST-USER-GUIDE.md).

The supplied file is preserved byte-for-byte. The review copy changes only its
initial body theme, selected theme option and initial Appearance label to
Instrument Teal. Prior motion/skin experiments remain history, not competing
implementation directions.

## 2. Fixed visual decisions

| Token | Instrument Teal default |
| --- | --- |
| Background | `#061B1E` |
| Primary text | `#F2F8F4` |
| Secondary text | `#89A7A5` |
| Separator | `#355054` |
| Subtle surface | `#113034` |
| Accent | `#77DACB` |
| Text on accent | `#092D2C` |

Studio and Editorial are optional palettes with the same layouts and behavior.
Instrument Teal is the default on a fresh configuration or appearance reset;
migration preserves a valid explicit user theme. Semantic recording/error states
remain clear through a short label and shape as well as color.

Use the supplied prototype's proportions, restrained flat surfaces and large
chord identity. Keep four text-based control columns, a subtle bank indicator,
thin separators and stable harmonic context. Preserve its halo and Bloom motion
vocabulary where it communicates real activity. Avoid introducing additional
glass, gradients, glowing controls or a different decorative scene.

Review at **native 240 × 240**. The reference contains 8–11 px supporting text;
approval of its overall design does not make that text a device readability
result. Target 12 px for secondary labels, 14–16 px for important values/degrees,
and approximately 60–72 px for the main chord, then verify the final font on the
actual TFT. Scroll/window content before shrinking essential text. Compose
accidentals and extensions deliberately rather than horizontally compressing
long chord names. Keep degree above the planned chord name, preserving the
earlier requested degree/name pairing.

The prototype uses operating-system fonts. Ship a licensed embedded font subset
with suitable note/accidental/extension glyphs; evaluate the repository's OFL
Inter Tight rather than assuming Apple system fonts can be bundled. Measure
glyph-cache, scratch and flash costs before choosing the rendering path.

## 3. Screen and workflow coverage

Build a small set of shared layout families, then map every canonical screen
and state onto them. Do not build hundreds of unrelated custom layouts or hide
unmigrated advanced controls behind a broken route.

| Area | Screen/workflow to implement | Completion evidence |
| --- | --- | --- |
| Progressions / Play | Hero chord, actual degree/quality and notes, planned degree/name rail, true cursor, pending setup, five live banks | PLAY immediately starts the selected progression; key, scale, tempo, sound and compatible premade groove remain reachable |
| Progression catalogue | Entire existing catalogue, immediate starting key/mode/metre, ordered browsing and long-phrase windowing | No new tag/key/scale filter hides entries; no Apply step; repeated chords do not confuse position |
| Chord library | Readable chord/degree rows; explicit audition and insertion from the editor | Correct notes and interval labels; true diatonic reference distinguished from deliberate quality-following sevenths |
| Progression editor | Numbered slots, degree/quality/color/length, voicing/bass detail, insert/remove, undo and working draft | Edits affect the intended slot, survive save/recall and never silently discard unsaved work |
| Live control banks | Core, Sound, Texture and Groove using the prototype's clear bank transition | Header/chord stay fixed; changing banks changes no musical value; actual knob mappings are always visible |
| Groove / accompaniment | Compatible premade pattern, real beat/step activity, drum level/kit/swing/click and loop/mix details | Chords and drums share one clock; pattern changes commit at the documented boundary; unsupported metre is recoverable |
| Learn | Actual notes, intervals, scale degree and deliberate comparison audition | Display agrees with sounding notes and outside-scale tones; comparisons restore the previous ownership state |
| Harmony suggestions | Small offline shortlist, audition, insertion and undo | Ranking is deterministic/bounded, no network dependency; audition does not become an unrequested permanent chord |
| Songs | Saved editable progressions and recordings in one collection; empty, selected, details and storage/error states | SAVE opens Songs; the player can load, rename, duplicate, delete and clear recordings easily |
| Record / persistence | HOME armed/count-in/capture/finishing overlays, Pending/Saving/Saved and protected recovery | REC works directly from Progressions; the new recording remains visible and recoverable until durable save succeeds |
| Appearance / settings | Instrument Teal default, optional palettes/custom solid accent, one adaptive motion toggle; grouped system settings | Theme/motion persist; Off affects every screen; settings never obscure core musical controls |
| Sound / Bass / Envelope / FX | Consistent preset lists and parameter families, including save/overwrite/delete and advanced pages | Existing synthesis/effects functions remain reachable and accurate while their presentation migrates |
| System / recovery | Clock waiting/lost, MIDI/USB, backup/update, full/busy/write errors and destructive confirmations | No dead ends, false Saved message, silent loss of a pending take or broken existing backup path |

### Prototype studies that are not automatic engine scope

All nine supplied views are visual references. **Chord morph** and
**Generative / Bloom** do not by themselves authorize a new harmonic engine,
continuous pitch morphing or autonomous chord generation. Preserve their visual
and motion studies; connect a view only to an explicitly defined musical state
or retained capability. Keep unimplemented Grain/Shimmer/Freeze/Density-style
controls out of released menus until their behavior, ownership and resource
cost are defined. Do not ship a dummy parameter or a simulated waveform/meter.
These studies do not delay the playback/editor/learning/suggestion milestones.

## 4. Physical interaction contract

| Input | Required behavior |
| --- | --- |
| HOME from another screen | Return to Progressions **Core** |
| HOME while on Progressions | Cycle **Core → Sound → Texture → Groove → Perform → Core**; use the existing HOME-view gesture rather than adding a hardware button |
| SELECT on Progressions | Browse the complete progression catalogue; automatically use the selected idea's starting setup |
| SELECT inside a list/editor | Move the explicit focused item/slot; repeated pitch matches do not choose edit focus |
| PRESETS | Select the chord sound globally |
| ALGORITHM | Retain its existing scene/workspace role |
| K1–K4 | Change only the four parameters currently labelled on screen |
| SEL / EDIT | Context-labelled inspect/audition/enter and progression editing; define each action in the canonical screen contract |
| PLAY | One shared start/stop accompaniment transport |
| REC on Progressions | Start count-in or arm at the next bar; finish to a new recording Song |
| SAVE | Open Songs / explicit Save Current flow; confirmations execute only their labelled action |
| MASTER / OCT− / OCT+ | Volume / musical octave; never repurpose octave keys for theme or bank navigation |

The bank mappings use retained, explicit controls:

| Bank | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| Core | Rhythm | Key | Scale | Tempo |
| Sound | Chord sound | Bass sound | Chord level | Bass level |
| Texture | Reverb mix | Delay mix | Chorus mix | Glide |
| Groove | Kit | Drum level | Swing | Click |
| Perform | Fill | Drop drums | Bring back | Tempo |

The supplied browser's sample bank words are appearance studies; the canonical
mapping above is the engineering contract. Bank switching changes presentation
and encoder ownership immediately, with no synthetic knob movement or value
jump. Verify the HOME gesture on hardware and document it in the player guide.

## 4b. Live drum moments — v1 source implementation

HOME now has a fifth Perform bank: K1 Fill, K2 Drop drums, K3 Bring back,
K4 Tempo. Each nonzero turn queues one next-bar action. Fills last one current-metre
bar, retain the first-half groove, then add loose snare/tom motion or tighter trap
rolls. Drop preserves chords/bass and phrase phase; Back restores the groove on
its next downbeat. The same shared accompaniment clock schedules every transition.
The finite bloom starts from the committed audio event; fill tension follows bar
phase. Motion Off cancels all visual-only work, with static status preserved.

This is implemented in source, with native host/linked-build evidence described in
[DRUM-PERFORMANCE.md](../../DRUM-PERFORMANCE.md); physical listening/display/USB timing
review remains pending. V1 moments are live, not stored in the manual recorder.
They are disabled during recording and clock waiting/loss/pause. Multi-bar builds
and Breakdown to chords alone remain later work; no placeholder control is exposed.

## 5. Adaptive motion contract

**One setting: On / Off. Default: On (adaptive).** Instrument Teal is independent
of motion; switching palettes does not change playback or the motion preference.

On uses a bounded, smoothed activity value derived from output loudness/volume
and active musical parts. Prefer an existing safe engine envelope/snapshot;
otherwise use documented volume/activity metadata. Count semantic sounding parts
such as Chords, Bass and Drums, including audible tails where supported, rather
than treating six FM operators or six chord notes as six separate instruments.
Live notes/HOLD can be active with transport stopped. A muted/silent arrangement
must settle instead of generating artificial high activity.

Use a fast attack and slower release, plus a short activity hold/hysteresis, to
avoid pumping or flicker. The exact weights and envelope constants in
[MOTION-SPEC.md](MOTION-SPEC.md) are tuning targets, not device measurements.
Strength may change halo size/opacity, small emphasis and visual excursion;
**it must not change the musical tempo or create an independent beat clock**.
Bind continuous studies to actual beat/bar/parameter state, not free-running
CSS-like timers. Keep primary chord identity and scale degrees readable.

Off cancels **every** halo, pulse, orbit, breath, morph journey, bank/page/list
tween, glyph transform and visual transition. Chord, parameter, step, cursor,
clock and recording data still commit instantly to correct settled frames.
Disabling motion never stops audio, alters the progression or conceals a pending
save. No user-facing Quiet/Expressive/Reduced/intensity/speed submenu remains.

The prototype's loudness and 0–8 layer sliders are demo inputs, not firmware
settings or a promise of eight instrument parts. Its intensity floor, unsmoothed
weighted inputs, independent orbit/morph timers and pause behavior are browser
study details to replace with the engineering contract above.

## 6. Recording and Songs: define before implementation

1. From HOME, REC when stopped gives a one-bar count-in; while playing it arms
   the next bar. Cancelling count-in creates no empty Song.
2. Capture the arranged progression plus deliberate live overrides and groove,
   key and tempo performance. Drums-only accompaniment also works.
3. REC again finishes at the next bar and stops accompaniment. PLAY ends
   immediately, preserving the actual partial length rather than padding silence.
4. Create a **new numbered recording Song**, preserving the source Song. Show
   **Pending** while protected in RAM and **Saving** only when writing.
5. Wait for a safe quiet flash boundary without cutting independent HOLD or
   audible tails. Show **Saved** only after confirmed persistence. Keep the
   current musical screen; do not force a Review/Keep or collection screen.
6. SAVE opens Songs, where the new row is already visible with its true status.
   Load, Rename, Duplicate and Delete act on the explicitly selected row.
7. Clear Recordings / Clear All show an accurate count and default to Cancel.
   Exclude factory templates, the protected working draft, Pending capture and
   currently sounding content. Discarding a Pending capture is its own action.

The proposed recording capacity remains a measured-storage design target, not
a shipped guarantee. Editable semantic Songs transpose; recorded fixed-note
performances retain their captured notes and timing. Version/migrate records
without changing the meaning of existing saved sounds, patterns or takes.

## 7. Engineering delivery sequence

**Integration update at publication:** `main` has merged
[progression playback PR #53](https://github.com/chasedeanda/chaseflower-fm1/pull/53)
at `29c3ba6e8fe87c33f94c8c139157460568ce89b3`. Inspect and reuse that work.
Milestones A/B define the selected UI and audible-flow acceptance requirements;
they do not ask agents to duplicate a clock/player that is already implemented.
Check source and hardware evidence before marking any item complete.

### Milestone A — establish the foundation and one real HOME

- Capture the current linked RAM/POOL/flash and physical audio/USB/input baseline
  at a recorded source revision. Read repository instructions before touching
  firmware; other work may already be in progress.
- Implement Instrument Teal tokens, the font/glyph strategy, layout primitives,
  screen snapshot/invalidation model and an interruptible finite-tween helper.
- Build HOME at native 240 × 240, including the real sounding chord, actual notes,
  degree/name rail, Core bank and correct long-symbol/phrase behavior.
- Add versioned appearance settings: Instrument Teal default, On adaptive / Off,
  with safe migration. No flash operation runs per frame or in the audio callback.

**Exit:** the physical FM-1 shows the selected visual direction, with correct
live chord/modifier/HOLD behavior and a completely settled Off mode. Release
artifacts and current behavior are distinguishable from browser studies.

### Milestone B — make PLAY useful, then complete the live banks

- Wire selected progression PLAY to the shared chord/drum clock, premade groove
  selection, phase-preserving tempo and documented key/mode/groove boundaries.
- Add the five live banks, their visible mappings and restrained transition.
- Implement Groove/Loop/Mix/Click pages and external-clock waiting/loss/resume.
- Publish safe bounded activity metadata and tune adaptive motion on hardware.

**Exit:** select an idea, press PLAY, change groove/key/tempo and keep playing
without a stuck note, false cursor, input stall or drum/visual clock drift.
Explicit progression PLAY still plays its arranged chords with live Harmony Off.

### Milestone C — editor, direct recording and complete Songs management

- Build the slot/editor/chord-library families and one-step undo/recoverable draft.
- Implement complete semantic Song save/recall and the direct REC lifecycle above.
- Add Songs list/details/name/manage/empty/full/busy/error/confirmation families.
- Verify safe persistence and migration, including interruption and power loss.

**Exit:** the player can develop a progression, record from HOME, find the saved
copy, replay it and remove selected recordings safely without losing the source.

### Milestone D — learning and offline next-chord suggestions

- Add actual-note/interval/scale-degree learning and deliberate comparison.
- Add a small offline shortlist with bounded ranking, explicit audition,
  insertion and undo. No cloud or live Hooktheory dependency is required.
- Validate major/minor, seventh qualities, all supported modes/metres and honest
  out-of-scale labels. In particular preserve Emaj7 → Bmaj7 → D♯m7 under the
  agreed live 7 behavior. A true F-major diatonic reference uses C7 and Eø7 /
  viiø7, while a deliberately selected Cmaj7 remains a valid outside-scale choice.

**Exit:** labels agree with the notes heard; audition restores ownership and
neither learning nor suggestions silently changes global Harmony or the draft.

### Milestone E — migrate the rest of the interface and release documentation

- Apply shared families to Sound, Bass, Envelope, FX, MIDI/USB, system, recovery
  and retained advanced FM6/VA screens. Track each canonical state as Planned,
  Designed, Implemented and Device-verified rather than equating a mockup with
  a completed feature.
- Keep working advanced routes during migration. Rebuilding every advanced
  parameter page is not a gate for Milestone B.
- Audit retained Morph/Bloom studies separately before enabling any new musical
  engine; do not fill shipped menus with controls that only animate a browser.
- Regenerate the player guide, README links and engineering handoff from the
  same canonical controls/workflows. Add real native-size firmware captures and
  short motion captures, labelled separately from supplied/browser references.
- Run the focused-build trim only after linked measurements and compatibility
  review; deleting inactive source is not a claimed runtime performance gain.

**Exit:** every retained route has a complete screen/state mapping, usable
controls, a correct player instruction and the applicable physical validation.

## 8. Shared acceptance gates

- **Visual:** native-size legibility, long chord names/accidentals, four- and
  eight-slot windows, repeated chords, all palette/status combinations, no
  clipped rows/values, stable chord context through bank/navigation changes.
- **Motion:** quiet/sparse → loud/dense inputs, zero/muted activity, tails/HOLD,
  rapid retrigger/retarget, On→Off during every effect and bank/theme change
  while Off. Off leaves no active visual animation path or repeated dirty work.
- **Musical:** actual versus planned/focused/queued identity, accurate degrees,
  global sound access, live takeover/return, drum boundaries, phase-preserving
  tempo, external clock loss/resume and independent note release.
- **Storage:** count-in cancel, empty take, partial end, full/busy/write failure,
  protected Pending capture, safe quiet commit, source preserved, power-cycle
  replay and counted cancellation-safe clear/delete.
- **Performance:** comparable linked memory and physical timing before/after;
  chords+bass+drums+FX+long releases+USB workload; input/stop/panic serviced during
  drawing; bounded graphics work outside audio callbacks. No desktop-only CPU,
  FPS, polyphony or net memory-saving claims.
- **Documentation:** screen IDs, physical controls, player guide, issue scope
  and release screenshots describe the same implemented behavior.

## 9. Roadmap ownership

Keep the existing roadmap linked to this plan instead of creating duplicates:

| Issue | Deliverable |
| --- | --- |
| [#43](https://github.com/chasedeanda/chaseflower-fm1/issues/43) | Immediate progression PLAY, synchronized drums and live setup |
| [#44](https://github.com/chasedeanda/chaseflower-fm1/issues/44) | Selected adaptive interface, Instrument Teal and shared screen families |
| [#45](https://github.com/chasedeanda/chaseflower-fm1/issues/45) | Progressions HOME, full catalogue and consistent physical navigation/banks |
| [#46](https://github.com/chasedeanda/chaseflower-fm1/issues/46) | Editor, direct REC and Songs/save/manage/recovery |
| [#47](https://github.com/chasedeanda/chaseflower-fm1/issues/47) | Accurate learning and comparisons |
| [#48](https://github.com/chasedeanda/chaseflower-fm1/issues/48) | Simple offline next-chord suggestions |
| [#49](https://github.com/chasedeanda/chaseflower-fm1/issues/49) | Persisted palettes/solid accent and two-choice adaptive motion |
| [#50](https://github.com/chasedeanda/chaseflower-fm1/issues/50) | Resource baseline, bounded rendering and hardware timing evidence |
| [#51](https://github.com/chasedeanda/chaseflower-fm1/issues/51) | Measured focused-build cleanup and compatibility migration |
