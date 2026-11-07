# Chord-first FM-1 engineering handoff

Engineering preview: native Instrument Teal CORE HOME, paired planned degrees/names, actual Now/Last chord and notes, real cursor/window, and persisted theme/Motion On or Off. GLO > Display: K1 theme, K4 Motion. Existing HOME Chord/Keys/Diagnostics navigation and recording behavior are retained in this first slice; the four-bank gesture, finite transitions, full Songs lifecycle and device verification remain pending. On uses documented volume/activity metadata, not a measured audio envelope.

**Status:** complete planned-interface inventory for review; not an implementation or hardware validation claim.

Start with the [implementation plan](IMPLEMENTATION-PLAN.md), [selected Instrument Teal prototype](instrument-teal/prototype.html) and [motion specification](MOTION-SPEC.md).

Design version 0.4, 2026-11-06. 205 screen/state definitions and 37 workflows share canonical JSON with the board and player guide.

## Product contracts

1. The large chord and its scale degree describe actual sounding harmony. The phrase rail describes planned slots and the real transport cursor; repeated pitch matches do not infer sequence position.
2. Each planned chord pairs a scale-degree numeral above its chord/root name. Major uses uppercase, minor lowercase. A sounding-degree/quality label remains visible beside the large chord. Key transposition preserves degree; mode/quality changes update it.
3. HOME K1–K4 are Rhythm, Key, Scale, Tempo. SELECT is one complete catalogue; metadata never hides ideas. Browsing supplies starting key/mode/metre without an Apply step.
4. One accompaniment clock owns progression timing and premade grooves. Running groove/setup changes land at defined musical boundaries; tempo preserves phase. Active and pending values must be labelled separately.
5. Live roots/HOLD have priority over temporary intentional audition, which has priority over scheduled harmony. The groove and transport cursor continue. Return to the current scheduled chord rather than restarting the phrase. The inventory specifies recording exceptions and cancellation.
6. Harmony is one On/Off choice. On builds the agreed complete major/minor chords; deliberate 7 follows quality, including Emaj7 → Bmaj7 → D♯m7 in E major. Learning labels actual notes and outside-scale tones accurately.
7. Harmony On/Off controls assistance for live root-key playing. Explicit Progression PLAY still plays its arranged chords when that assistance is Off, without switching the setting on. A recorded Take retains its fixed notes.
8. Songs is the one user-facing collection for saved progressions and recordings. Factory progression browsing remains on HOME. Editable Songs use semantic chord/degree/duration data; recorded Songs retain captured performance events and timing. Existing recorded Auto takes keep their fixed-note meaning and format.
9. REC is direct from HOME. It captures the scheduled progression plus deliberate live/groove/key/tempo changes rather than unexpectedly muting the arranged chords. Ending a capture creates a new numbered Song copy; no mandatory Review/Keep step. Pending, Saving and Saved are different states, and flash writes wait for an actually safe quiet boundary without cutting independent HOLD or audible tails.
10. Songs supports selected-row Load, Rename, Duplicate and Delete, plus counted Clear Recordings/Clear All actions. Bulk clear excludes factory templates, the protected working draft and Pending captures. A Pending capture can be discarded only through its own explicit selected action.
11. The selected visual reference is fm1_adaptive_motion_prototype.html. Instrument Teal is the factory default: background #061b1e, foreground #f2f8f4, secondary #89a7a5, separator #355054, subtle surface #113034, accent #77dacb, and on-accent text #092d2c. Studio and Editorial are optional palettes of the same typography-led interface. Preserve the large chord, paired degrees, thin separators, four text controls and the supplied prototype's restrained halo/Bloom visual vocabulary; earlier Coral Bloom, Aurora and Studio Rack are historical alternatives.
12. Motion has exactly two user choices: On (adaptive) and Off. On responds automatically to output level/volume and semantic active-instrument activity using a smoothed bounded envelope, with musical motion tied to the shared clock. There is no manual Quiet/Expressive/intensity/speed selector. Off cancels every pulse, orbit, morph travel and tween; truthful chord, value, playback and record/save data still update instantly. The supplied loudness/layer sliders are simulation inputs, not new firmware settings or evidence of eight instrument parts. Measure display, input, memory and audio/USB timing on the FM-1.
13. HOME from another screen returns to Progressions Core. Further HOME taps cycle Core/Sound/Texture/Groove without changing any parameter; SELECT still browses the complete catalogue, PRESETS still selects chord sound, ALGORITHM keeps its scene/workspace role and OCT± stays musical. Four text columns and a subtle bank indicator make each current mapping visible.
14. No image, label or example value in this board is a physical-device measurement. Fonts, partial redraws, storage limits and audio/USB timing need linked-build and FM-1 verification before release.
15. Advanced FM6/VA pages enumerate retained capabilities and repeatable control templates. Keep a usable Advanced Sound route while cosmetic migration proceeds; rewriting every advanced page is not a gate for the first audible progression-playback milestone.

## Delivery

- Play — first milestone.
- Edit and save — second milestone.
- Learn — third milestone.
- Suggest — fourth milestone.

Playback, HOME and the modern visual system are the first work. Optional accent modes, pruning and a comprehensive optimization project do not block initial playback. Measure linked memory and FM-1/USB timing; derive safe capacities rather than inventing limits.

## Screen contracts

### HOME_STAGE — Progressions · CORE

**Group:** Home · **Delivery:** Play — first milestone · **Template:** chord

Browse and hear the complete catalogue with a large chord hero, paired degree/name cue and four text control columns.

**Entry:** Boot, HOME from any musical workspace, or return from a completed action.
**Exit:** Any destination button opens its workspace without stopping playback.

| Visible field | Reference value/state |
| --- | --- |
| Song | Still Into You · Paramore chorus seed |
| Setup | F Major · 4/4 · 108 BPM |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Rail | IV/Bbmaj7 → V/C7 → vi/Dm7 → I/Fmaj7 |
| Position | 3 / 4 · bar 3 |
| Source | Progression |
| Groove | Lo-fi |
| Key / scale | F Major |
| Metre | 4/4 |
| Tempo | 108 BPM |
| Phrase | IV / Bbmaj7 → V / C7 → vi / Dm7 → I / Fmaj7 |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Browse one complete wrapping factory song list; use the pending selection as the next turn's origin. No tag/key/scale filtering or Apply step. |
| ALGORITHM | Keep the existing secondary-scene role; bank switching uses HOME, not ALGORITHM. Other workspaces keep their named-tab role. |
| K1 | Rhythm: Lo-fi / compatible grooves / Off |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 quarter-note BPM |
| HOME | While already on Progressions, cycle CORE → SOUND → TEXTURE → GROOVE. From any other screen or secondary scene, return Progressions CORE. |
| PLAY | Start this selected progression at slot 1 with its groove; stop if transport is running. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open LAB_NOTES on Now or the last sounding chord. |
| SAVE | Open Songs library; this alone neither saves nor changes the loaded Song. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Stopped browsing applies the seed key/mode/metre immediately without starting sound.
- Running cursor comes from the player; match marks and edit focus have different shapes.
- Each planned numeral sits above its chord name. Key changes preserve degree; mode changes can change numeral case.
- Live roots/HOLD take over harmony while the groove and progression cursor continue.
- No current notes: keep the last chord dimmed with Last, not sounding Now.
- Arpeggio steps animate note activity without changing harmonic identity.
- External Receive without clock: Waiting / armed, Tempo EXT; local PLAY arms rather than free-running.
- External pulse loss: stop/release only accompaniment, retain cursor and independent live HOLD, show Clock lost. Pulses alone never restart.
- Explicit external Start begins slot 1; Continue resumes retained position. Choose Internal in System, then local PLAY, for deliberate internal playback.
- Progression PLAY is an explicit arranged-chord source even when live Harmony is Off; it does not enable or rewrite the global live-root assistance setting. Take source keeps its recorded pitches.
- REC leaves HOME visible: stopped one-bar count-in, running next-bar arm; record what is heard without suppressing automatic chords.
- Every non-empty finished capture stops accompaniment and creates a new numbered Song copy with truthful Pending/Saving/Saved status.
- When a Recording Song is loaded, its fixed note/control/tempo timeline is authoritative. Key/mode/rhythm/tempo fields read Recorded during exact playback; EDIT develops its separate starting progression as a new draft.
- Coral Bloom, Aurora and Studio Rack are historical explorations, not selected. The owner-provided fm1_adaptive_motion_prototype.html is the authoritative visual/motion base; the static nine-screen image remains a layout reference.
- Motion has exactly On / Off. On adapts to smoothed volume/output level and active instrument count. No separate Quiet/Expressive/Reduced/Calm, Intensity or Speed motion preference.
- Use the prototype’s large chord hero, flat aligned type, thin separators, paired degree/name cue and four text control columns with bank name/count/dots. Instrument Teal is default; Studio and Editorial are palettes of the same layout.
- Use resolved playing/ownership and the shared clock. A smoothed output/active-part snapshot requires implementation; prototype-supplied levels are simulations, not hardware FFT/velocity measurements.
- Off stops every animation/transition, including CSS pulses/halos, Morph/Bloom and parameter/bank/focus/save fades. Chord/notes/degree/cursor/status still update instantly; OS reduced motion may force effective Off privately.
- Measure and cap foreground cadence, dirty regions, pixel work and scratch memory under demanding audio/USB. Do not promise FPS, CPU savings or polyphony from a concept.
- The F-major seventh-color demonstration is explicitly arranged IVmaj7 → V7 → vi7 → Imaj7: Bbmaj7 → C7 → Dm7 → Fmaj7, with C7 notes C E G Bb. This stored V7 does not change deliberate live 7 following major/minor quality, including Emaj7 → Bmaj7 → D♯m7.
- Instrument Teal palette: bg #061b1e, fg #f2f8f4, muted #89a7a5, line #355054, subtle #113034, accent #77dacb, onAccent #092d2c; custom solid accents must preserve contrast.
- Retain all nine prototype studies, including Chord Morph and Generative / Bloom. Unsupported morph/generative DSP, Grain/Shimmer/Freeze/Human/Fill and all pictured capabilities are not thereby implemented.
- CORE is Rhythm / Key / Scale / Tempo. PRESETS stays global chord sound, HOME SELECT stays the complete catalogue, and OCT−/OCT+ stay musical.
- HOME tap cycling reuses the existing view gesture and needs physical verification. No hold chord or encoder push. A bank switch changes no sound, value, note ownership, recording source or transport.
- The latest adaptive-motion prototype is preserved byte-for-byte as the visual reference. Its up-to-eight demo layers are simulated response inputs, not a promise of eight device parts/engines/tracks/voices.

**Player instructions:**
1. Turn SELECT to a song in the complete catalogue.
2. In CORE use K1 Rhythm, K2 Key, K3 Scale and K4 Tempo.
3. Press PLAY; tap HOME while on Progressions to cycle the visible live banks.

**Related:** HOME_PENDING, ACC_GROOVE, EDIT_HARMONY, LAB_NOTES, IDEA_LIBRARY, HOME_SOUND, HOME_TEXTURE, HOME_GROOVE, CTRL-BANKS

### HOME_PENDING — Progressions · Next Song Queued

**Group:** Home · **Delivery:** Play — first milestone · **Template:** chord

Keep the current sound truthful while a browsed song waits for the next bar.

**Entry:** Turn SELECT to another song during accompaniment.
**Exit:** Next bar commits the complete pending setup and returns to HOME_STAGE. PLAY stops and makes the pending song the stopped selection without sounding it.

| Visible field | Reference value/state |
| --- | --- |
| Active | Still Into You · F Major · 4/4 |
| Now | Dm7 |
| Next | Low Lantern · E Phrygian · 4/4 |
| Switch | Next bar · slot 1 |
| Groove | Lo-fi retained |
| Tempo | 108 BPM |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | 3 / 4 |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Replace the queued song; turning back to the active song cancels the queue. |
| ALGORITHM | Same HOME scene cycle; never commits a queued song early. |
| K1 | Rhythm: active song's groove |
| K2 | Key: active song's key |
| K3 | Scale: active song's mode |
| K4 | Tempo: active transport tempo |
| HOME | On Progressions cycle the current bank without committing/cancelling the queued song; from elsewhere return Progressions CORE. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Inspect actual Now; do not inspect the pending chord by accident. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Active name/key/mode/metre stay authoritative until commit.
- HOME knobs target the active song until commit. Their key/mode requests are discarded if the queued song replaces it first.
- Latest complete request wins; missing compatible groove becomes Off/Click rather than a hidden metre change.
- Queued catalogue recall changes seed key/mode/metre; it preserves user tempo and compatible selected groove.
- Clock loss retains the pending song and cursor but stops accompaniment. External Start/Continue deliberately resumes under the external-clock contract; pulses alone do not commit or restart.
- Adaptive On follows actual active harmony and ownership. Pending-idea emphasis never changes the displayed active chord/degree or implies early musical commitment.
- The current bank remains visible while queueing. Knobs target the active setup until the queued switch; this example shows CORE.

**Player instructions:**
1. Browse while playing.
2. Read Next separately from Now.
3. Wait for the bar or browse back to cancel.

**Related:** HOME_STAGE, ACC_NO_GROOVE

### ACC_GROOVE — Groove

**Group:** Accompaniment · **Delivery:** Play — first milestone · **Template:** picker

Select premade synthesized drum patterns and the small controls that make them fit the chords.

**Entry:** SEQ from another workspace.
**Exit:** SEQ or HOME returns home; ALGORITHM selects another accompaniment tab.

| Visible field | Reference value/state |
| --- | --- |
| Groove | Lo-fi |
| Meter | 4/4 · 2 bars |
| Kit | Dust |
| Level | 68% |
| Swing | 38% |
| Suggested | 78 BPM · actual 108 |

| Input | Action |
| --- | --- |
| SELECT | Browse compatible grooves; stopped selection changes the setup, running selection queues for next bar. |
| ALGORITHM | Choose Groove / Loop / Mix / Click / Arrange; Arrange is phase 2. |
| K1 | Groove: compatible list plus Off |
| K2 | Kit: Dust |
| K3 | Drum level: 68% |
| K4 | Drum swing: 38% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start/stop the current accompaniment mode, not a separate drum transport. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Restore this groove's own kit and swing; preserve tempo and level. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Return HOME. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Changing a groove never overwrites user tempo.
- Off disables drum notes without stopping chord clock.
- Choosing the currently sounding groove cancels another pending groove.
- Synthesized pattern data is not imported audio; no file-import promise.

**Player instructions:**
1. Press SEQ.
2. Turn SELECT for a groove.
3. Adjust kit, level or swing; press SEQ to return.

**Related:** ACC_LOOP, ACC_MIX, ACC_CLICK, ACC_NO_GROOVE

### ACC_NO_GROOVE — No Matching Groove

**Group:** Accompaniment · **Delivery:** Play — first milestone · **Template:** status

Keep unusual metres usable without silently replacing the song's time signature.

**Entry:** The selected song/metre has no compatible premade groove.
**Exit:** Choosing an song with a compatible groove restores that metre's last groove; HOME remains accessible.

| Visible field | Reference value/state |
| --- | --- |
| Song | Dorian Waltz |
| Meter | 3/4 |
| Drums | No matching groove |
| Available | Off or Click |
| Tempo | 108 BPM |

| Input | Action |
| --- | --- |
| SELECT | Continue full song browsing with normal immediate/queued setup rules. |
| ALGORITHM | Same HOME scene cycle. |
| K1 | Rhythm: Off / Click |
| K2 | Key: D |
| K3 | Scale: Dorian |
| K4 | Tempo: 108 BPM |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Play chords with Off or Click at the chosen metre. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open LAB_NOTES. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open ACC_CLICK. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- The absence is explicit; no 4/4 fallback.
- Click follows the song's metre and quarter-note tempo convention.
- Drums Off still permits automatic progression playback.

**Player instructions:**
1. Leave drums Off or select Click.
2. Press PLAY to hear the chords.

**Related:** HOME_STAGE, ACC_CLICK

### ACC_LOOP — Playback and Loop

**Group:** Accompaniment · **Delivery:** Play — first milestone · **Template:** timeline

Choose chord-plus-drums or drums-only without introducing another transport.

**Entry:** SEQ, then ALGORITHM to Loop.
**Exit:** SEQ/HOME returns home.

| Visible field | Reference value/state |
| --- | --- |
| Mode | Chords + Drums |
| Source | Progression or explicitly loaded Recording Song |
| Range | Full phrase · slots 1–4 |
| Loop | On |
| Position | 3 / 4 |

| Input | Action |
| --- | --- |
| SELECT | Move a visible read-only slot focus in phase 1; phase 2 uses it as loop/edit focus. |
| ALGORITHM | Choose Groove / Loop / Mix / Click / Arrange; Arrange is phase 2. |
| K1 | Mode: Chords + Drums / Drums only |
| K2 | Loop start: slot 1; full phrase in phase 1 |
| K3 | Loop end: slot 4; full phrase in phase 1 |
| K4 | Repeat: On / Off |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start selected source/range from its beginning or stop. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Focus the shown slot for EDIT; no sound starts. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Return HOME. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Phase 1 range is the full catalogue phrase; start/end become editable in phase 2.
- Repeat Off stops after the full phrase or selected phase-2 range.
- Drums-only keeps manual chord performance immediate; automatic cursor can continue as a guide.
- Range/mode changes during playback apply at next bar, keeping a single clock.
- Recording Songs default to Play once at their actual stored duration; explicit Loop repeats that fixed timeline.
- Drums only is the deliberate preparation for recording your own chords; REC does not change accompaniment mode itself.

**Player instructions:**
1. Choose Chords + Drums or Drums only.
2. Press PLAY.
3. After phase 2, narrow the loop with start/end.

**Related:** ACC_GROOVE, IDEA_LIBRARY, EDIT_PHRASE

### ACC_MIX — Mix

**Group:** Accompaniment · **Delivery:** Play — first milestone · **Template:** params

Balance chord, linked bass and drums without exposing an independent layer track.

**Entry:** SEQ, then ALGORITHM to Mix.
**Exit:** SEQ/HOME returns home.

| Visible field | Reference value/state |
| --- | --- |
| Chord | 78% |
| Bass | 55% |
| Drums | 68% |
| LinkedBass | On |

| Input | Action |
| --- | --- |
| SELECT | Choose the visible part label for focus only; does not load a sound. |
| ALGORITHM | Choose Groove / Loop / Mix / Click / Arrange; Arrange is phase 2. |
| K1 | Chord level: 78% |
| K2 | Bass level: 55% |
| K3 | Drum level: 68% |
| K4 | Linked bass: On / Off |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open Chord Lab on Now. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Return HOME. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open ENV_ENVELOPE with current part target. |
| LFO | Open BASS_SOUND. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Levels apply promptly through bounded audio controls.
- Bass Off releases accompaniment bass, preserving separately owned direct MIDI notes.
- No LAYER part or general multitrack editor is introduced.

**Player instructions:**
1. Set the three levels.
2. Use LFO for bass sound/register details.

**Related:** BASS_SOUND, ENV_ENVELOPE, ACC_GROOVE

### ACC_CLICK — Click

**Group:** Accompaniment · **Delivery:** Play — first milestone · **Template:** params

Configure a metronome that follows the same clock and metre as the song.

**Entry:** SEQ, ALGORITHM to Click; or SEQ from No Matching Groove.
**Exit:** SEQ/HOME returns home.

| Visible field | Reference value/state |
| --- | --- |
| Click | Off |
| Level | 35% |
| Sound | Soft |
| Meter | 4/4 |
| Tempo | 108 quarter-note BPM |

| Input | Action |
| --- | --- |
| SELECT | No additional selection; all four controls are direct. |
| ALGORITHM | Choose Groove / Loop / Mix / Click / Arrange; Arrange is phase 2. |
| K1 | Click: Off / On |
| K2 | Click level: 35% |
| K3 | Click sound: Soft |
| K4 | Tempo: 108 BPM |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start/stop the same accompaniment; click alone is available with Drums only and groove Off. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open Chord Lab on Now. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Return HOME. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Click Off does not stop the clock.
- External MIDI clock disables local Tempo with Ext shown.
- In 6/8, six eighth-note counts accent 1 and 4; BPM remains quarters.
- Metre is shown here; change it deliberately in EDIT_PHRASE, not by browsing a mismatched groove.

**Player instructions:**
1. Turn Click On.
2. Set its level.
3. Press PLAY for the shared clock.

**Related:** ACC_LOOP, EDIT_PHRASE

### ACC_ARRANGE — Arrange A/B

**Group:** Accompaniment · **Delivery:** Edit and save — second milestone · **Template:** timeline

Plan a short sequence of explicitly stored A/B sections, without returning to the old general song workstation.

**Entry:** SEQ, ALGORITHM to Arrange after an editable song exists.
**Exit:** SEQ/HOME returns home; changes become a draft edit.

| Visible field | Reference value/state |
| --- | --- |
| Arrangement | A ×2 → B ×1 → A ×1 |
| Step | 2 / 3 |
| Section | B |
| Repeats | 1 |
| End | Loop arrangement |

| Input | Action |
| --- | --- |
| SELECT | Choose arrangement step 1–8; retain a visible insertion position at the end. |
| ALGORITHM | Choose Groove / Loop / Mix / Click / Arrange; Arrange is phase 2. |
| K1 | Section: A / B |
| K2 | Repeats: 1–8 |
| K3 | Action: Add after / Remove / None |
| K4 | End: Loop / Stop |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start/stop this explicitly selected arrangement source. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the selected section in EDIT_HARMONY. |
| SEL | Execute Add after, or open removal confirmation; None does nothing. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Return HOME. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Planned after core edit/save; not required for first playback.
- A exists as the current phrase. Creating B explicitly duplicates A and is undoable.
- No more than eight arrangement entries, each pointing to A or B; an empty chain plays A.
- Running arrangement changes commit at next bar; deleting the sounding entry does not interrupt a note mid-onset.
- Capacity and empty-section errors keep the previous chain intact.

**Player instructions:**
1. Choose a step.
2. Set A/B and repeat count.
3. Use SEL to add; PLAY hears the chain.

**Related:** EDIT_HARMONY, ARRANGE_REMOVE_CONFIRM, IDEA_UNDO

### ARRANGE_REMOVE_CONFIRM — Remove Arrangement Step?

**Group:** Accompaniment · **Delivery:** Edit and save — second milestone · **Template:** confirm

Remove one chain entry without deleting its A/B phrase.

**Entry:** ACC_ARRANGE Action Remove, then SEL.
**Exit:** Return ACC_ARRANGE after either choice.

| Visible field | Reference value/state |
| --- | --- |
| Target | Step 2 · B ×1 |
| Result | Section B remains saved |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel or Remove; default Cancel. |
| ALGORITHM | No tab change in confirmation. |
| K1 | Action: Cancel / Remove |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel and return HOME. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Execute selected action; Remove is one-step undoable. |
| SAVE | Execute the selected Cancel/Remove choice; HOME cancels without removing. |
| SEQ | Cancel and return ACC_ARRANGE. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Never deletes the section's chord data.
- Removing the last entry leaves A as the playable fallback.

**Player instructions:**
1. Check the numbered step.
2. Choose Remove and press SAVE or SEL; HOME cancels.

**Related:** ACC_ARRANGE, IDEA_UNDO

### EDIT_HARMONY — Edit Chord

**Group:** Progression Editor · **Delivery:** Edit and save — second milestone · **Template:** chord

Change a numbered planned chord while preserving actual Now and global Harmony rules.

**Entry:** EDIT; last explicit slot focus, otherwise a unique live match, otherwise slot 1. Clone a factory seed into the protected draft first.
**Exit:** EDIT/HOME returns home; ALGORITHM opens other slot tabs.

| Visible field | Reference value/state |
| --- | --- |
| Draft | Still Into You · Draft * |
| Target | A · slot 3 / 4 |
| Chord | Dm7 |
| Degree | vi · minor |
| Now | Dm7 · Loop |
| Length | 1 bar |

| Input | Action |
| --- | --- |
| SELECT | Select a numbered slot; never change the catalogue song from this workspace. |
| ALGORITHM | Choose Chord / Timing / Voicing / Bass / Phrase / Library. |
| K1 | Degree/root: 6 |
| K2 | Quality: Follow mode / explicit major / minor / dim / aug / power |
| K3 | Color: selected deliberate modifier combination |
| K4 | Length: 1 bar |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return HOME; keep draft and playback. |
| SEL | Audition this planned slot for one bar; live input has priority. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Factory and saved originals remain untouched until an explicit save.
- Quality Follow mode uses the agreed complete major/minor rule. Explicit alternatives are local overrides.
- Color never silently selects Rich/Dreamy styles.
- Running edits publish a complete plan at next chord onset; active notes remain named as Now until then.
- At most 16 planned slots in the new editor; phase 1 catalogue playback still accepts every existing slot.
- Editing a loaded Recording Song develops a separate Progression draft from its stored semantic start; it never rewrites fixed performance events.

**Player instructions:**
1. Press EDIT and read the slot number.
2. Turn Degree, Quality, Color or Length.
3. Press SEL to hear it; use SAVE to keep the full song.

**Related:** EDIT_TIMING, EDIT_VOICING, EDIT_BASS, EDIT_PHRASE, IDEA_DRAFT_CONFLICT, CHORD_LIBRARY

### CHORD_LIBRARY — Chord Library

**Group:** Progression Editor · **Delivery:** Edit and save — second milestone · **Template:** picker

Browse named chord/degree candidates, audition deliberately and replace the explicitly focused slot without changing global Harmony.

**Entry:** EDIT → ALGORITHM Library; keep the numbered destination slot and its draft. This is a chord picker, not HOME progression-catalogue filtering.
**Exit:** ALGORITHM returns to an editor tab; HOME returns Progressions Core; opening/closing the picker does not change a slot.

| Visible field | Reference value/state |
| --- | --- |
| Context | F Major · Diatonic reference |
| Destination | Slot 3 / 4 |
| Rows | Fmaj7 / I; Gm7 / ii; Am7 / iii; Bbmaj7 / IV; C7 / V; Dm7 / vi; Eø7 / viiø7 |
| Selected | Dm7 · D F A C |
| Audition | SEL Hear · EDIT Replace |

| Input | Action |
| --- | --- |
| SELECT | Browse the full chord-reference list for the current project key/mode; scroll a readable row window, never squeeze seven rows into the LCD. |
| ALGORITHM | Return to the other editor tabs; keep focus and draft. |
| K1 | Register: performance range |
| K2 | Inversion: root / 1 / 2 / 3 |
| K3 | Color: explicit selected chord variant; label diatonic versus deliberate override |
| K4 | Length: destination slot duration |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Replace the named destination slot with this exact candidate, retain length/voicing as shown, create one-step undo and return to Edit Chord. |
| SEL | Audition the selected candidate for one bar; no slot change until explicit EDIT Replace. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Browse is silent. SEL explicitly auditions one candidate through the shared audition owner; live HOLD retains priority.
- EDIT replaces the named destination slot and returns to Edit Chord with one-step undo; running changes publish at the documented chord boundary.
- No capacity-free insertion or duplicate slot is implied by Replace. Explicit next-chord insertion remains in the Next workflow.
- Strict diatonic reference in F Major uses C7 and Eø7 / viiø7. Deliberate Cmaj7 is allowed as an honestly labelled local override; the agreed live major/minor seventh rule is unchanged.
- Keep numeral above chord name when rows become the planned phrase; the row itself pairs both labels with actual notes.
- No valid destination opens the editor fallback before choosing a replacement; do not mutate an arbitrary repeated-chord match.

**Player instructions:**
1. Open EDIT and select a numbered destination.
2. Turn ALGORITHM to Library, then SELECT to browse.
3. Use SEL to hear a candidate and EDIT to replace the named slot.

**Related:** EDIT_HARMONY, EDIT_VOICING, IDEA_UNDO, LAB_NOTES

### EDIT_TIMING — Chord Timing

**Group:** Progression Editor · **Delivery:** Edit and save — second milestone · **Template:** params

Set duration and articulation with explicit time units.

**Entry:** EDIT, ALGORITHM to Timing.
**Exit:** EDIT/HOME or another editor tab.

| Visible field | Reference value/state |
| --- | --- |
| Target | A · slot 3 / 4 · Dm7 |
| Unit | Bars |
| Length | 1 bar |
| Gate | 100% |
| Tie | Off |
| Meter | 4/4 |

| Input | Action |
| --- | --- |
| SELECT | Select slot; retain timing tab. |
| ALGORITHM | Choose Chord / Timing / Voicing / Bass / Phrase. |
| K1 | Unit: Bars / Quarter notes |
| K2 | Length: 1 bar |
| K3 | Gate: 100% |
| K4 | Tie: Off / On |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return HOME. |
| SEL | Audition this slot using its chosen duration. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Bar durations follow the song's metre. Quarter-note durations retain absolute quarter-note length when metre changes.
- Bars: 1–8. Quarter notes: 0.25–32 in 0.25 steps. Display the unit, never an ambiguous beat count.
- Tie carries identical resolved notes across the next slot boundary without a new attack; changed pitches articulate normally.
- Duration edits affect the next onset; they do not shorten a currently held scheduled note abruptly.
- Gate is a percent of slot duration; linked bass follows unless its slot override is Off.

**Player instructions:**
1. Choose Bars or Quarter notes.
2. Set Length and Gate.
3. Use Tie only when the repeated chord should sustain.

**Related:** EDIT_HARMONY, EDIT_PHRASE

### EDIT_VOICING — Chord Voicing

**Group:** Progression Editor · **Delivery:** Edit and save — second milestone · **Template:** params

Choose inversion/register deliberately; do not invent unavailable synthesis controls.

**Entry:** EDIT, ALGORITHM to Voicing.
**Exit:** EDIT/HOME or another editor tab.

| Visible field | Reference value/state |
| --- | --- |
| Target | A · slot 3 / 4 · Dm7 |
| Inversion | Smooth |
| Register | 0 oct |
| VoicedNotes | C4 D4 F4 A4 |

| Input | Action |
| --- | --- |
| SELECT | Select slot; retain voicing tab. |
| ALGORITHM | Choose Chord / Timing / Voicing / Bass / Phrase. |
| K1 | Inversion: Root / Smooth / 1st / 2nd / 3rd |
| K2 | Register: −2 to +2 octaves |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return HOME. |
| SEL | Audition planned voicing; do not change live latched modifiers. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open MOTION_FEEL for sound Glide. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Unavailable inversions clamp to the chord's available voices and show the resolved result.
- Smooth uses the prior scheduled chord, not unrelated recent live/audition notes.
- Out-of-range notes are octave-bounded safely, with actual resolved notes shown.
- Spread is not exposed until a distinct supported implementation is designed.

**Player instructions:**
1. Choose Smooth or a specific inversion.
2. Adjust register and audition.

**Related:** EDIT_HARMONY, MOTION_FEEL, LAB_COMPARE

### EDIT_BASS — Slot Bass

**Group:** Progression Editor · **Delivery:** Edit and save — second milestone · **Template:** params

Set a bass choice for one planned chord, separate from global bass sound and level.

**Entry:** EDIT, ALGORITHM to Bass.
**Exit:** EDIT/HOME or another editor tab.

| Visible field | Reference value/state |
| --- | --- |
| Target | A · slot 3 / 4 · Dm7 |
| Bass | Root · D |
| Register | −1 octave from chord |
| State | Follow global Bass On |

| Input | Action |
| --- | --- |
| SELECT | Select slot; retain bass tab. |
| ALGORITHM | Choose Chord / Timing / Voicing / Bass / Phrase. |
| K1 | Bass tone: Root / 2nd chord tone / 3rd / remaining available tone |
| K2 | Bass register: −2 to +1 octaves |
| K3 | Slot bass: Follow / Off |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return HOME. |
| SEL | Audition chord with its planned bass. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open BASS_SOUND; retain slot focus for return. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Tone choices refer to resolved chord tones, not an unexplained scale-degree number.
- Global Bass Off overrides a slot's Follow setting.
- Saved fixed-note takes retain their recorded bass notes; these controls edit the semantic phrase.

**Player instructions:**
1. Choose a chord tone and register.
2. Use LFO for the bass patch or overall level.

**Related:** EDIT_HARMONY, BASS_SOUND

### EDIT_PHRASE — Phrase and Metre

**Group:** Progression Editor · **Delivery:** Edit and save — second milestone · **Template:** timeline

Manage slot order and metre in a bounded phrase without losing longer musical forms.

**Entry:** EDIT, ALGORITHM to Phrase.
**Exit:** EDIT/HOME or another editor tab.

| Visible field | Reference value/state |
| --- | --- |
| Section | A |
| Slots | 4 / 16 |
| Target | Before slot 3 |
| Meter | 4/4 |
| Action | Duplicate |
| Duration | 4 bars |

| Input | Action |
| --- | --- |
| SELECT | Choose the numbered destination/target slot. |
| ALGORITHM | Choose Chord / Timing / Voicing / Bass / Phrase. |
| K1 | Action: Insert / Duplicate / Remove |
| K2 | Position: slot 3 |
| K3 | Metre: 2/4 / 3/4 / 4/4 / 5/4 / 6/8 / 7/4 |
| K4 | Section: A / B |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return HOME. |
| SEL | Execute Insert/Duplicate or open EDIT_REMOVE_CONFIRM. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Insert creates a tonic Follow-mode triad of one bar before the target; Duplicate copies the target after it.
- Remove never removes the final remaining slot.
- Creating B explicitly duplicates A once and records an undo step.
- Phrase limit is 16 slots; repeated durations can express twelve-bar forms without a four-card assumption.
- Metre changes queue complete chord/groove setup at next bar; bar-length slots follow the new metre, quarter-note slots do not.
- At capacity, Insert/Duplicate are disabled with 16-slot limit shown; no existing slot is overwritten.

**Player instructions:**
1. Choose action and position.
2. Press SEL to change the phrase.
3. Change Metre explicitly here; use SEQ to arrange A/B.

**Related:** EDIT_HARMONY, EDIT_REMOVE_CONFIRM, ACC_ARRANGE

### EDIT_REMOVE_CONFIRM — Remove Chord Slot?

**Group:** Progression Editor · **Delivery:** Edit and save — second milestone · **Template:** confirm

Make slot deletion deliberate and reversible.

**Entry:** EDIT_PHRASE Remove, then SEL.
**Exit:** Return EDIT_PHRASE.

| Visible field | Reference value/state |
| --- | --- |
| Target | A · slot 3 · Dm7 |
| PhraseAfter | 3 slots |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel or Remove. |
| ALGORITHM | No tab change in confirmation. |
| K1 | Action: Cancel / Remove |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel and return HOME. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Cancel and return EDIT_PHRASE. |
| SEL | Execute selected action; removal is undoable. |
| SAVE | Execute the selected Cancel/Remove choice; HOME cancels without removing. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Default Cancel.
- Last-slot removal is disabled before this dialog.
- Running deletion publishes a replacement plan at the next bar; existing ownership remains safe.

**Player instructions:**
1. Check the slot label.
2. Choose Remove and press SAVE or SEL; HOME cancels.

**Related:** EDIT_PHRASE, IDEA_UNDO

### IDEA_MENU — Manage Songs

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** picker

Make selected-Song and library-wide actions direct, explicit and visibly scoped.

**Entry:** EDIT on Songs.
**Exit:** Return to Songs after a completed action; HOME never changes the highlighted record.

| Visible field | Reference value/state |
| --- | --- |
| Selected | Still Into Y 03 |
| Type | Recording · Saved |
| Action | Rename |
| Library | 20 Saved · 1 Pending |

| Input | Action |
| --- | --- |
| SELECT | Choose an available action. The selected Song name stays visible. |
| ALGORITHM | No nested take categories or hidden recording-management page. |
| K1 | Action: Rename / Duplicate / Delete / Details / Edit starting progression / Clear Recordings / Clear All Songs / Return to Draft / Undo |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel Manage and return Progressions. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return to Songs without action. |
| SEL | Execute selected route. Rename → IDEA_RENAME; Duplicate → new numbered copy with same type, safe save and no load; Delete → one-row confirmation; Clear → counted confirmation. |
| SAVE | Execute the selected clearly labelled action, same as SEL. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Duplicate creates a new stable ID; it never overwrites the source or changes the currently loaded source.
- Pending row: Rename changes its RAM name; Duplicate is disabled until Saved; Delete becomes Discard Pending with its own confirmation.
- No selected row disables row actions but keeps Return to Draft/Undo and valid bulk actions visible.
- Clear Recordings and Clear All Songs always have one explicit counted Cancel-default confirmation.
- Edit starting progression on a Recording opens a separate semantic Progression draft. Its existing fixed performance remains intact.

**Player instructions:**
1. Press EDIT in Songs.
2. Choose the named row action or a counted clear action.
3. Press SAVE or SEL to continue.

**Related:** IDEA_LIBRARY, IDEA_RENAME, IDEA_DELETE_CONFIRM, SONG_PENDING_DISCARD_CONFIRM, SONGS_CLEAR_RECORDINGS_CONFIRM, SONGS_CLEAR_ALL_CONFIRM, IDEA_DETAILS, EDIT_HARMONY, IDEA_DRAFT_RETURN, IDEA_UNDO

### IDEA_LIBRARY — Songs

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** picker

See, load, save and manage both saved progressions and recorded performances in one place.

**Entry:** SAVE from Progressions or any musical workspace; auto-finished captures are selected here on the next open.
**Exit:** HOME returns to the loaded/current Progressions Stage; selection alone does not change it.

| Visible field | Reference value/state |
| --- | --- |
| Current | Still Into You · Progression |
| Highlighted | Still Into Y 03 |
| Type | Recording · fixed performance |
| Duration | 00:18 · 8 bars |
| State | Saved |
| View | All · 20 saved + 1 Pending |

| Input | Action |
| --- | --- |
| SELECT | Highlight a row only. Highlighting never loads, starts, previews or overwrites it. |
| ALGORITHM | No hidden library tabs. K1 offers optional views; All always includes both saved types and Pending. |
| K1 | View: All / Progressions / Recordings / Favorites |
| K2 | — |
| K3 | Favorite: highlighted row Off / On |
| K4 | — |
| HOME | Return to Progressions Stage; preserve draft/current audible ownership. |
| PLAY | Start/stop the CURRENT loaded source; never silently play a merely highlighted row. |
| REC | Return to Progressions and begin the normal new capture; a protected unsaved Pending capture must be saved/discarded first. |
| EDIT | Open IDEA_MENU, Manage, for the highlighted row or library-wide actions when empty. |
| SEL | Explicitly load the highlighted Saved Song; stopped applies it, running queues the complete setup at next bar. Pending offers its labelled RAM preview without claiming Saved. |
| SAVE | Save the CURRENT loaded/edited setup, not the highlighted row. New → prefilled naming; edited existing → Save As New / explicit Update. Clean saved current → Already saved. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- All is one library of Progression Songs and Recording Songs; factory seeds remain the complete wrapping HOME list.
- Use distinct type marks and text: ◇ Progression with phrase bars/time; ● Recording with actual elapsed duration; ◌ Pending with reason. Status never relies on color alone.
- Recordings include captured fixed notes and timed performance controls plus an editable semantic starting progression. Neither is an audio WAV file.
- A newly completed non-empty REC creates a new numbered row automatically. Opening Songs selects that row; no separate take browser or mandatory review.
- Current source, highlighted row and dirty protected draft are distinct. The row list never discards the draft.
- Legacy recordings appear here as Recording · Legacy format, using their preserved playback behavior, not a nested browser.
- Clear actions count only validated Saved rows. Pending rows and protected draft/current audible data are not bulk-deleted.

**Player instructions:**
1. Press SAVE to see Songs.
2. Turn SELECT to a row; SEL loads it.
3. SAVE saves current; EDIT manages the highlighted row.

**Related:** IDEA_MENU, IDEA_DETAILS, IDEA_LIBRARY_EMPTY, IDEA_NAME, IDEA_SAVE_OPTIONS, SONG_PENDING

### IDEA_DETAILS — Song Details

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** text

Explain saved Song type, provenance, timing and editable start without hiding recordings in another tree.

**Entry:** Manage → Details.
**Exit:** HOME returns current source; SEL/SAVE returns Songs.

| Visible field | Reference value/state |
| --- | --- |
| Song | Still Into Y 03 |
| Type | Recording · fixed performance |
| Duration | 00:18 · 8 bars |
| Start | Still Into You · F Major · 4/4 |
| Performance | Notes + groove/key/tempo/control events |
| Starting progression | 4 semantic slots · editable copy |
| Status | Saved |

| Input | Action |
| --- | --- |
| SELECT | Scroll the bounded detail page; do not load another Song. |
| ALGORITHM | Choose Summary / Starting progression / Performance detail pages. |
| K1 | Page: Summary / Starting progression / Performance |
| K2 | — |
| K3 | Favorite: Off / On |
| K4 | — |
| HOME | Return to Progressions Stage; preserve draft/current audible ownership. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open an editable copy of the starting semantic progression; retain the recorded timeline unchanged. |
| SEL | Return Songs; this is metadata, not an audition command. |
| SAVE | Return Songs; saving current is an explicit library action. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- A Progression Song has semantic degree/duration data. A Recording Song has fixed performed notes and timed control/tempo events, with its semantic start stored separately.
- Recording playback follows its stored timeline; editing the start does not silently transpose/rewrite recorded notes.
- Song-section seeds retain artist/section and simplified-seed labels rather than claiming an exact transcription.

**Player instructions:**
1. Read Type and Duration.
2. Use EDIT only to develop the starting progression as a new draft.

**Related:** IDEA_LIBRARY, EDIT_HARMONY

### IDEA_LIBRARY_EMPTY — Songs · Empty View

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** status

Offer a direct first recording/save route without clearing the current music.

**Entry:** No Saved/Pending rows match the chosen Songs view.
**Exit:** HOME returns Progressions.

| Visible field | Reference value/state |
| --- | --- |
| Title | No Songs yet |
| Current | Still Into You · Progression |
| Hint | REC on Progressions records what you hear |
| View | All |

| Input | Action |
| --- | --- |
| SELECT | No row selection in an empty view. |
| ALGORITHM | No hidden library tabs. |
| K1 | View: All / Progressions / Recordings / Favorites |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions Stage; preserve draft/current audible ownership. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | Return HOME and start normal count-in/arm. |
| EDIT | Open Manage; row actions disabled; Return to Draft/Undo remain accessible. |
| SEL | No row to load; current source remains intact. |
| SAVE | Save current progression through prefilled IDEA_NAME. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- An empty filter never clears the loaded Song/draft.
- Clear actions with zero Saved targets are disabled.

**Player instructions:**
1. Press HOME and REC for a performance.
2. Or press SAVE here to save the current progression.

**Related:** IDEA_LIBRARY, IDEA_NAME, IDEA_MENU, REC_COUNT_IN

### IDEA_NAME — Save New Song

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** text

Save a new semantic Song quickly, with optional naming and no extra free-slot review.

**Entry:** SAVE on Songs when current progression is new, or Save As New.
**Exit:** Success returns Songs with new row selected. Busy/full keeps a visible Pending row; errors never report Saved.

| Visible field | Reference value/state |
| --- | --- |
| Name | Evening Return 01 |
| Type | Progression |
| Scope | Complete current setup |
| Cursor | 1 / 16 |
| Action | Save New |

| Input | Action |
| --- | --- |
| SELECT | Move the caret. The name starts prefilled; editing it is optional. |
| ALGORITHM | No tab change during naming. |
| K1 | Character: selected character |
| K2 | Cursor: 1–16 |
| K3 | Set: A–Z / a–z / digits / symbols |
| K4 | Action: Save New / Cancel |
| HOME | Cancel naming; retain current draft. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute the same visible action. |
| SAVE | Execute Save New directly or Cancel. No extra review for a new free slot. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Use a stable new ID. Matching names never imply overwrite.
- Name is 1–16 characters; auto suffix uses at least two digits and truncates only the base to fit.
- Recorded captures bypass naming and create their own numbered new rows automatically.

**Player instructions:**
1. Keep the suggested name or edit it.
2. Press SAVE once to save the new free-slot Song.

**Related:** IDEA_LIBRARY, SONG_PENDING, IDEA_STORAGE_FULL, IDEA_WRITE_ERROR

### IDEA_SAVE_OPTIONS — Save Current Song

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** picker

Separate new-copy saving from an intentional update of an existing saved Song.

**Entry:** SAVE on Songs for a current setup that edits an existing saved Song.
**Exit:** Chosen save route or Songs.

| Visible field | Reference value/state |
| --- | --- |
| Current | Evening Return * |
| Original | Evening Return · Saved |
| Action | Save As New |

| Input | Action |
| --- | --- |
| SELECT | Choose action; default Save As New. |
| ALGORITHM | No tab change. |
| K1 | Action: Save As New / Update original / Cancel |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel and return HOME with draft intact. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute the same visible action. |
| SAVE | Save As New opens prefilled naming; Update opens the named overwrite confirmation; Cancel returns Songs. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Highlighting another row never changes the save target: this action always saves current.
- A clean saved current shows Already saved instead of silently creating another row.
- Editing a Recording Song’s starting progression creates a new Progression draft; default Save As New leaves its recorded timeline untouched.

**Player instructions:**
1. Choose Save As New to preserve both.
2. Choose Update only when the named original should change.

**Related:** IDEA_NAME, IDEA_OVERWRITE_CONFIRM, IDEA_LIBRARY

### IDEA_OVERWRITE_CONFIRM — Update Saved Song?

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** confirm

Make overwriting a saved Song an explicit final action.

**Entry:** Save Current Song → Update original.
**Exit:** Saved update returns Songs; busy → SONGS_OPERATION_PENDING; write failure preserves the prior valid record.

| Visible field | Reference value/state |
| --- | --- |
| Target | Evening Return |
| Type | Progression |
| Scope | Complete current Song setup |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel or Update; default Cancel. |
| ALGORITHM | No tab change. |
| K1 | Action: Cancel / Update |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel with no write. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute selected choice. |
| SAVE | Execute the selected choice; Update writes only the named stable ID once safe. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- REC never enters this dialog automatically: every capture is a new copy.
- Update/Replace occurs only after explicit selection and confirmation.

**Player instructions:**
1. Check the target.
2. Choose Update and press SAVE.

**Related:** IDEA_LIBRARY, SONGS_OPERATION_PENDING, IDEA_WRITE_ERROR

### IDEA_RENAME — Rename Song

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** text

Rename the highlighted saved or Pending Song without changing its music.

**Entry:** Manage → Rename.
**Exit:** Return Songs with renamed row selected.

| Visible field | Reference value/state |
| --- | --- |
| Target | Still Into Y 03 |
| Name | Night Drive 03 |
| Type | Recording |
| Cursor | 1 / 16 |
| Action | Rename |

| Input | Action |
| --- | --- |
| SELECT | Move caret. |
| ALGORITHM | No tab change. |
| K1 | Character: selected character |
| K2 | Cursor: 1–16 |
| K3 | Set: A–Z / a–z / digits / symbols |
| K4 | Action: Rename / Cancel |
| HOME | Cancel and keep old name. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute same labelled choice. |
| SAVE | Execute Rename/Cancel; Saved records write safely, Pending names update in RAM. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Stable ID, fixed performance and semantic starting progression are unchanged.
- Renaming never turns Pending into Saved.

**Player instructions:**
1. Enter the desired name.
2. Press SAVE; HOME cancels.

**Related:** IDEA_LIBRARY, SONGS_OPERATION_PENDING, IDEA_WRITE_ERROR

### IDEA_DELETE_CONFIRM — Delete Selected Song?

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** confirm

Delete exactly one named Saved row while preserving draft and current audible data.

**Entry:** Manage → Delete on a Saved row.
**Exit:** Immediate success returns Songs; otherwise show explicit queued operation without falsely removing the row.

| Visible field | Reference value/state |
| --- | --- |
| Target | Still Into Y 03 |
| Type | Recording · 00:18 |
| Count | 1 Saved Song |
| Protected | Draft / current audible copy / Pending rows |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel/Delete; default Cancel. |
| ALGORITHM | No tab change. |
| K1 | Action: Cancel / Delete |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel; delete nothing. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute same choice. |
| SAVE | Confirm selected action. Delete removes only the named Saved ID once storage is quiet/safe. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- If the deleted ID is loaded, its in-RAM music keeps sounding and becomes an unsaved current copy.
- This does not discard the protected draft or Pending capture.
- No automatic factory/sound deletion accompanies Song deletion.

**Player instructions:**
1. Check the name and count 1.
2. Choose Delete and press SAVE; HOME cancels.

**Related:** IDEA_LIBRARY, SONGS_OPERATION_PENDING

### IDEA_DRAFT_RETURN — Return to Working Draft

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** status

Recover same-session edits after browsing another factory or saved song.

**Entry:** Songs → EDIT Manage → Return to Draft → SEL/SAVE.
**Exit:** Return → HOME; Stay → Songs.

| Visible field | Reference value/state |
| --- | --- |
| Draft | Evening Return * |
| Changes | Chord 3 · groove · tempo |
| Checkpoint | Last stopped checkpoint · recoverable |
| Action | Return |

| Input | Action |
| --- | --- |
| SELECT | Choose Return or Stay browsing. |
| ALGORITHM | No tab change. |
| K1 | Action: Return / Stay browsing |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Return selects the protected draft stopped or queues it at next bar; Stay returns Songs. |
| SAVE | Return to Songs; keep the protected draft. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Factory browsing never overwrites the protected mutable draft.
- Running return preserves active sound until the queued bar boundary.
- A durable recovery checkpoint is created only while stopped; changes since it remain RAM-only.

**Player instructions:**
1. Press SAVE to open Songs, then EDIT for Manage.
2. Choose Return to Draft and execute Return.

**Related:** HOME_PENDING, IDEA_LIBRARY, IDEA_RECOVERY

### IDEA_DRAFT_CONFLICT — Keep the Existing Draft?

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** confirm

Resolve editing a second song without silently replacing an unsaved working draft.

**Entry:** EDIT on another song while a dirty protected draft exists.
**Exit:** Existing draft → EDIT_HARMONY; save route → IDEA_NAME/IDEA_SAVE_OPTIONS; confirmed discard → new EDIT_HARMONY.

| Visible field | Reference value/state |
| --- | --- |
| Draft | Evening Return * |
| Requested | Low Lantern |
| Action | Return to existing draft |

| Input | Action |
| --- | --- |
| SELECT | Choose one explicit action; default Return to draft. |
| ALGORITHM | No tab change. |
| K1 | Action: Return to draft / Save then edit new / Discard draft and edit new |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel and return HOME. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Execute choice. Save then edit new completes naming/saving before cloning the requested seed. |
| SAVE | Execute the visible selected draft action; HOME cancels. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Requested source ID is retained during naming; failed/cancelled save leaves the old draft intact.
- Discard clears only the unsaved draft, not any original saved song.

**Player instructions:**
1. Keep editing your draft, save it first, or explicitly discard it.
2. Press SEL on the chosen route.

**Related:** EDIT_HARMONY, IDEA_NAME, IDEA_SAVE_OPTIONS

### IDEA_UNDO — Undo / Redo

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** picker

Provide visible one-step recovery with the first editor.

**Entry:** Songs → EDIT Manage → Undo → SEL/SAVE.
**Exit:** SEL returns to the edited workspace; SAVE returns menu.

| Visible field | Reference value/state |
| --- | --- |
| Action | Undo |
| Change | Slot 3 · Dm → Dm7 |
| Target | Evening Return * |

| Input | Action |
| --- | --- |
| SELECT | Choose the available action. |
| ALGORITHM | No tab change. |
| K1 | Action: Undo / Redo |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Apply the one stored edit snapshot; action flips to Redo/Undo. |
| SAVE | Return to Songs; keep the protected draft. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- One complete gesture is one step; multiple detents within the same short edit gesture coalesce.
- New edit after Undo replaces Redo.
- No history available: show Nothing to undo and disable SEL.
- Save, rename and irreversible saved-record deletion do not masquerade as musical-edit undo.
- Running undo publishes a complete plan at its normal musical boundary.

**Player instructions:**
1. Open Songs, press EDIT for Manage, then choose Undo.
2. Execute Undo; the one stored edit restores safely.

**Related:** EDIT_HARMONY, ACC_ARRANGE, IDEA_LIBRARY

### IDEA_RECOVERY — Recover Draft Checkpoint?

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** confirm

Restore a validated stopped-state draft checkpoint after restart.

**Entry:** Boot detects a valid recovery record distinct from the last committed user song.
**Exit:** HOME stopped; playback never auto-starts after recovery.

| Visible field | Reference value/state |
| --- | --- |
| Draft | Evening Return * |
| Recovered | Last validated stopped checkpoint |
| Pending recordings | Never-written RAM capture not claimed durable |
| Action | Recover |

| Input | Action |
| --- | --- |
| SELECT | Choose Recover or Discard checkpoint; default Recover. |
| ALGORITHM | No tab change. |
| K1 | Action: Recover / Discard checkpoint |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Keep the checkpoint for later and open HOME stopped. |
| PLAY | Start the selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Recover opens the draft stopped; Discard removes only the recovery record. |
| SAVE | Execute the visible selected draft action; HOME cancels. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Invalid/incomplete record is rejected with the prior saved song intact.
- Only stopped checkpoints are durable; recording/live edits after the checkpoint can be lost on power failure.
- Same-session draft return remains available independently of durable checkpoint status.
- Recorded Song auto-save and protected semantic draft checkpoint are separate; never imply a Pending capture survived power loss.

**Player instructions:**
1. Choose Recover to continue the checkpoint.
2. Press PLAY when ready.

**Related:** IDEA_DRAFT_RETURN, HOME_STAGE

### IDEA_STORAGE_FULL — Songs Full · New Song Pending

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** status

Keep a completed capture/new save visible instead of dropping or mislabelling it when storage is full.

**Entry:** No free space exists for a new Song copy.
**Exit:** Songs → free space → automatic safe retry; explicit Pending discard is separate.

| Visible field | Reference value/state |
| --- | --- |
| Song | Still Into Y 03 |
| Type | Recording · 00:18 |
| State | Pending · not Saved |
| Reason | Song storage full |
| Action | Manage Songs |

| Input | Action |
| --- | --- |
| SELECT | Choose action. |
| ALGORITHM | No tab change. |
| K1 | Action: Manage Songs / Keep Pending |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with Pending banner. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open Manage Songs. |
| SEL | Open Manage Songs; clear/delete only explicit saved targets. |
| SAVE | Open Songs with Pending row highlighted. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Never replace a Saved Song automatically.
- Bulk clear excludes this Pending capture.
- Second REC is blocked until this buffer saves or is explicitly discarded.

**Player instructions:**
1. Press SAVE or EDIT to manage Songs.
2. Delete one Saved row or use counted Clear Recordings/Clear All, then let Pending save.

**Related:** IDEA_LIBRARY, IDEA_MENU, SONG_PENDING

### IDEA_WRITE_ERROR — Song Not Saved Yet

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** status

Preserve the new capture and prior valid library when a write fails.

**Entry:** Automatic or explicit Song write fails validation.
**Exit:** Validated retry → Saved; otherwise Pending remains visible.

| Visible field | Reference value/state |
| --- | --- |
| Song | Still Into Y 03 |
| State | Pending · not Saved |
| Previous | Source Saved Song unchanged |
| Action | Retry when quiet |

| Input | Action |
| --- | --- |
| SELECT | Choose visible action. |
| ALGORITHM | No tab change. |
| K1 | Action: Retry / Manage Songs / Keep Pending |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with Pending status. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open Songs Manage. |
| SEL | Execute same choice. |
| SAVE | Retry selected write if safe, or execute the selected Manage/Keep action. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- No false Saved indicator.
- No automatic replacement or dropped capture after failure.

**Player instructions:**
1. Retry when quiet, or open Songs to manage storage.
2. Keep Pending if the operation cannot complete yet.

**Related:** IDEA_LIBRARY, SONG_PENDING, SONG_SAVING

### REC_ARMED — Progressions · Record Armed

**Group:** Recording · **Delivery:** Edit and save — second milestone · **Template:** chord

Record the performance heard on Progressions while HOME stays visible.

**Entry:** REC while accompaniment is running, with no unresolved Pending capture.
**Exit:** Next bar → REC_RECORDING; REC cancels arm.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | 3 / 4 |
| Phrase | IV/Bb → V/C → vi/Dm → I/F |
| Recording | Armed · next bar |
| Starts | Next downbeat |
| Source | Current audible progression/manual/groove |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Normal complete HOME catalogue browsing/queued changes remain available and are captured when they become audible. |
| ALGORITHM | HOME scene cycle; recording state remains an overlay, not a forced take-editor page. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | On Progressions cycle banks without cancelling capture/arm; from another workspace return Progressions CORE. |
| PLAY | Cancel arm and stop current accompaniment; preserve live HOLD. |
| REC | Cancel arm and leave current playback unchanged; create no row. |
| EDIT | Open semantic editor; audible committed changes are captured without changing the original saved Song. |
| SEL | Open Chord Lab on Now; inside Songs explicitly load the highlighted row. |
| SAVE | Open Songs without stopping capture; record badge persists. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Current accompaniment keeps playing while armed.
- At next bar capture begins at the current phrase position; do not reset its progression.
- Include currently sounding held chord/bass and active groove/control state at the capture boundary.
- Use the owner’s prototype recording language with anchored REC/count/time. Adaptive On follows actual output/active parts; Off stops all animation while status updates instantly.

**Player instructions:**
1. Press REC directly on Progressions.
2. Watch the HOME recording badge; there is no required review screen.

**Related:** HOME_STAGE, IDEA_LIBRARY, SONG_PENDING, SONG_SAVING, SONG_SAVED

### REC_COUNT_IN — Progressions · Recording Count-in

**Group:** Recording · **Delivery:** Edit and save — second milestone · **Template:** chord

Record the performance heard on Progressions while HOME stays visible.

**Entry:** REC while stopped, with no unresolved Pending capture.
**Exit:** Count-in completes → REC_RECORDING; REC/PLAY cancels with no row.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | 3 / 4 |
| Phrase | IV/Bb → V/C → vi/Dm → I/F |
| Recording | Count-in · 1 bar |
| Count | 3 → 2 → 1 |
| Source | Selected progression + groove |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Normal complete HOME catalogue browsing/queued changes remain available and are captured when they become audible. |
| ALGORITHM | HOME scene cycle; recording state remains an overlay, not a forced take-editor page. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | On Progressions cycle banks without cancelling capture/arm; from another workspace return Progressions CORE. |
| PLAY | Cancel count-in and stay stopped; no empty Song. |
| REC | Cancel count-in, stop its temporary click and restore prior stopped state; create no Song row. |
| EDIT | Open semantic editor; audible committed changes are captured without changing the original saved Song. |
| SEL | Open Chord Lab on Now; inside Songs explicitly load the highlighted row. |
| SAVE | Open Songs without stopping capture; record badge persists. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- After one full bar, start the selected progression/groove and capture together; automatic chords are not suppressed.
- Count-in click is not captured as musical content.
- No replacement prompt: an existing saved recording never blocks a new copy.
- Use the owner’s prototype recording language with anchored REC/count/time. Adaptive On follows actual output/active parts; Off stops all animation while status updates instantly.

**Player instructions:**
1. Press REC directly on Progressions.
2. Watch the HOME recording badge; there is no required review screen.

**Related:** HOME_STAGE, IDEA_LIBRARY, SONG_PENDING, SONG_SAVING, SONG_SAVED

### REC_RECORDING — Progressions · Recording

**Group:** Recording · **Delivery:** Edit and save — second milestone · **Template:** chord

Record the performance heard on Progressions while HOME stays visible.

**Entry:** Count-in/arm reaches its capture start.
**Exit:** REC → REC_FINISHING; PLAY/limit/clock loss → close capture, stop accompaniment, then Pending/Saving/Saved.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | 3 / 4 |
| Phrase | IV/Bb → V/C → vi/Dm → I/F |
| Recording | Recording · 00:12 |
| Capture | What you hear · notes + performance events |
| Limit | 128 bars / 1024 musical events design bound |
| Source | Progression + live overrides + groove |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Normal complete HOME catalogue browsing/queued changes remain available and are captured when they become audible. |
| ALGORITHM | HOME scene cycle; recording state remains an overlay, not a forced take-editor page. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | On Progressions cycle banks without cancelling capture/arm; from another workspace return Progressions CORE. |
| PLAY | Close at the current moment, stop accompaniment promptly and create a short Song with actual duration; no padded bar. |
| REC | Queue end at next bar and show Finishing; repeated REC while Finishing leaves that request in place. |
| EDIT | Open semantic editor; audible committed changes are captured without changing the original saved Song. |
| SEL | Inspect Now; audible intentional auditions are captured too. |
| SAVE | Open Songs without stopping capture; record badge persists. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Do not mute or suppress scheduled chords when capture starts. Manual-over-auto retains the normal ownership priority; capture the resolved heard result.
- Capture fixed performed chord/bass note states and timed groove, key/mode, tempo/metre, sound, motion and FX control changes; do not run automatic and recorded chord notes twice on playback.
- Record the initial semantic progression separately, including its starting cursor; its editable copy never replaces the fixed performance timeline.
- To record only your own chords, choose Drums only before REC; this is the same capture mechanism.
- No flash operations during active capture. Reaching the planned bound reserves space for note closure and finishes safely without overwriting early events.
- External clock loss finishes captured content safely, stops accompaniment and queues a new Pending Song; clock pulses alone never resume recording.
- Use the owner’s prototype recording language with anchored REC/count/time. Adaptive On follows actual output/active parts; Off stops all animation while status updates instantly.

**Player instructions:**
1. Press REC directly on Progressions.
2. Watch the HOME recording badge; there is no required review screen.

**Related:** HOME_STAGE, IDEA_LIBRARY, SONG_PENDING, SONG_SAVING, SONG_SAVED

### REC_LIMIT — Progressions · Recording Limit Reached

**Group:** Recording · **Delivery:** Edit and save — second milestone · **Template:** chord

Record the performance heard on Progressions while HOME stays visible.

**Entry:** Capture reaches 128 bars or 1024 musical events.
**Exit:** Immediately enter normal Pending/Saving/Saved flow.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | 3 / 4 |
| Phrase | IV/Bb → V/C → vi/Dm → I/F |
| Recording | Recording ended at capacity |
| Result | Captured content retained |
| Next | New Song copy · Pending/Saving |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Normal complete HOME catalogue browsing/queued changes remain available and are captured when they become audible. |
| ALGORITHM | HOME scene cycle; recording state remains an overlay, not a forced take-editor page. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | On Progressions cycle banks without cancelling capture/arm; from another workspace return Progressions CORE. |
| PLAY | Stop accompaniment; independent live HOLD remains. |
| REC | Wait for the protected capture to save or be explicitly discarded. |
| EDIT | Open semantic editor; audible committed changes are captured without changing the original saved Song. |
| SEL | Open Chord Lab on Now; inside Songs explicitly load the highlighted row. |
| SAVE | Open Songs without stopping capture; record badge persists. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Bounds are explicit planned product targets requiring budget validation, not current firmware measurements.
- Reserve terminal note/control events; do not truncate into stuck notes.
- Normal automatic new-copy flow follows; no mandatory take review.
- Use the owner’s prototype recording language with anchored REC/count/time. Adaptive On follows actual output/active parts; Off stops all animation while status updates instantly.

**Player instructions:**
1. Press REC directly on Progressions.
2. Watch the HOME recording badge; there is no required review screen.

**Related:** HOME_STAGE, IDEA_LIBRARY, SONG_PENDING, SONG_SAVING, SONG_SAVED

### LAB_NOTES — Notes and Degree

**Group:** Chord Lab · **Delivery:** Learn — third milestone · **Template:** chord

Explain Now or a named planned slot with truthful note spelling, degree and scale membership.

**Entry:** SEL from HOME defaults to Now; during silence use Last. SELECT can then choose numbered slots.
**Exit:** SEL/HOME returns home; ALGORITHM changes Lab tab.

| Visible field | Reference value/state |
| --- | --- |
| Target | Now · loop |
| Chord | Bmaj7 |
| Degree | V · major · degree 5 in E |
| Notes | B D♯ F♯ A♯ |
| Scale | E Major |
| Outside | A♯ is outside E Major |
| Now | Bmaj7 |

| Input | Action |
| --- | --- |
| SELECT | Change target Now/Last/numbered slot; sets explicit edit focus only for a slot. |
| ALGORITHM | Choose Notes / Intervals / Compare / Next; Next is phase 4. |
| K1 | Target: Now / Last / slot 1–16 |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Control the existing shared transport; inspection itself is silent. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly selected slot, or the normal unambiguous edit fallback. |
| SEL | Return HOME from this Lab root. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Actual notes and quality determine labels; no strict-diatonic reinterpretation of the agreed Auto rules.
- Now follows live takeover, intentional audition or scheduled sound by ownership priority.
- Now uses compact degree/quality text; planned slot numerals retain uppercase major/lowercase minor.
- Outside-scale membership is factual, not an error warning.
- No current/last chord: show Play a root or choose a slot; do not invent a chord.

**Player instructions:**
1. Press SEL to inspect what you hear.
2. Turn SELECT to inspect a specific planned slot.

**Related:** LAB_INTERVALS, LAB_COMPARE, NEXT_SHORTLIST, EDIT_HARMONY

### LAB_INTERVALS — Chord Intervals

**Group:** Chord Lab · **Delivery:** Learn — third milestone · **Template:** params

Connect each actual note to its interval and scale membership.

**Entry:** SEL, ALGORITHM to Intervals.
**Exit:** SEL/HOME or another Lab tab.

| Visible field | Reference value/state |
| --- | --- |
| Target | Now · Bmaj7 |
| Notes | B D♯ F♯ A♯ |
| Intervals | 1 3 5 7 |
| Degree | V · major |
| Membership | 1 3 5 in scale · 7 outside |

| Input | Action |
| --- | --- |
| SELECT | Choose the target as in Notes; K2 chooses a note within it. |
| ALGORITHM | Choose Notes / Intervals / Compare / Next; Next is phase 4. |
| K1 | Target: Now / Last / numbered slot |
| K2 | Note focus: B / D♯ / F♯ / A♯ |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Edit the explicitly focused planned slot. |
| SEL | Return HOME. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Ninth/eleventh/thirteenth retain their compound interval names rather than being reduced silently to 2/4/6.
- Inversion changes displayed register/order, not chord-relative identity.
- Locrian forced-perfect-fifth output is shown accurately as outside scale when applicable.

**Player instructions:**
1. Choose Intervals.
2. Use K2 to focus a note and its interval.

**Related:** LAB_NOTES, LAB_COMPARE

### LAB_COMPARE — Compare Chord Sounds

**Group:** Chord Lab · **Delivery:** Learn — third milestone · **Template:** picker

Choose one explicit comparison without replacing the song or global modifiers.

**Entry:** SEL, ALGORITHM to Compare.
**Exit:** HOME returns home; ALGORITHM changes tab without audition.

| Visible field | Reference value/state |
| --- | --- |
| Target | Slot 3 · Dm7 |
| Comparison | Triad / selected extension |
| A | Dm · D F A |
| B | Dm7 · D F A C |
| Selected | A |

| Input | Action |
| --- | --- |
| SELECT | Choose comparison type; K2 selects A/B. |
| ALGORITHM | Choose Notes / Intervals / Compare / Next; Next is phase 4. |
| K1 | Comparison: Triad/Color / Root/Smooth |
| K2 | Side: A / B |
| K3 | — |
| K4 | — |
| HOME | Return HOME silently unless the original transport is already running. |
| PLAY | Start the explicitly selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Edit the focused slot after stopping comparison. |
| SEL | Start the selected side for one bar in LAB_COMPARE_PLAYING. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Only supported, meaningful comparisons are enabled; no chosen extension means Triad/Color is disabled with Choose a color shown.
- Root/Smooth uses the same fixed target/progression context for both sides.
- Browsing a comparison never emits notes.

**Player instructions:**
1. Choose Triad/Color or Root/Smooth.
2. Select A or B.
3. Press SEL to hear it.

**Related:** LAB_COMPARE_PLAYING, LAB_NOTES, EDIT_VOICING

### LAB_COMPARE_PLAYING — Comparison Audition

**Group:** Chord Lab · **Delivery:** Learn — third milestone · **Template:** chord

Hear an A/B choice temporarily, then resume the current live/loop context.

**Entry:** SEL on an enabled LAB_COMPARE side.
**Exit:** One bar or SEL → LAB_COMPARE; restore current scheduled harmony for its remaining time, unless live ownership is active.

| Visible field | Reference value/state |
| --- | --- |
| Target | Slot 3 · comparison B |
| Now | Dm7 |
| Notes | D F A C |
| Duration | One bar |
| Background | Groove and cursor continue |

| Input | Action |
| --- | --- |
| SELECT | Switch A/B without changing the planned slot. |
| ALGORITHM | End audition and return LAB_COMPARE before changing tabs. |
| K1 | Comparison: locked for this audition |
| K2 | Side: A / B; switching restarts a one-bar audition |
| K3 | — |
| K4 | — |
| HOME | End audition and return HOME; original transport continues. |
| PLAY | Stop audition and running transport; preserve live roots/HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | End audition and open the focused slot. |
| SEL | Stop audition and return LAB_COMPARE. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Live root/HOLD has priority: it cancels/blocks audition and displays Release live chord to audition.
- Audition never changes the draft, global Harmony, latch state or harmonic suggestion history.
- Never restore an obsolete chord snapshot after the clock advances.
- Only audition-owned notes are released.

**Player instructions:**
1. Listen for one bar.
2. Press SEL to stop earlier or switch A/B.
3. Resume the same song without resetting its clock.

**Related:** LAB_COMPARE, HOME_STAGE

### NEXT_SHORTLIST — What Could Come Next?

**Group:** Next Chord · **Delivery:** Suggest — fourth milestone · **Template:** picker

Offer three offline continuations with explicit audition and insertion.

**Entry:** Chord Lab, ALGORITHM to Next.
**Exit:** HOME; audition; or explicit insertion.

| Visible field | Reference value/state |
| --- | --- |
| Context | F Major · Bb → C → Dm |
| Candidate1 | F · I · Return home |
| Candidate2 | Bb · IV · Continue |
| Candidate3 | Gm · ii · Contrast |
| Selected | F |
| InsertTarget | A · slot 4 |
| Method | Curated suggestions |

| Input | Action |
| --- | --- |
| SELECT | Explore the three candidates without sound or insertion. |
| ALGORITHM | Choose Notes / Intervals / Compare / Next; Next is phase 4. |
| K1 | Candidate: 1–3 |
| K2 | Context: Live history / Draft path |
| K3 | Insert slot: A 4 |
| K4 | — |
| HOME | Return HOME without changing notes. |
| PLAY | Start the explicitly selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Insert the selected candidate into the visible draft slot, with one-step undo, then open EDIT_HARMONY. |
| SEL | Audition selected candidate in NEXT_AUDITION. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Only committed harmonic changes update history; ignore arp notes, repeated attacks and inversion-only changes.
- Actual key/mode/quality drive the rules; sparse modes show Theory-guided fallback.
- Refresh never plays, replaces or inserts a chord.
- Use suggestions rather than unsupported percentages; any local frequency must name its local corpus.
- No network/account or external-statistics import is required.

**Player instructions:**
1. Open Next.
2. Browse three choices.
3. SEL hears one; EDIT deliberately inserts it into the displayed slot.

**Related:** NEXT_AUDITION, NEXT_INSERTED, EDIT_HARMONY, NEXT_UNSUPPORTED

### NEXT_AUDITION — Audition Next Chord

**Group:** Next Chord · **Delivery:** Suggest — fourth milestone · **Template:** chord

Hear a candidate without committing it to the draft or history.

**Entry:** SEL on NEXT_SHORTLIST.
**Exit:** One bar/SEL → shortlist; EDIT → insertion; HOME → home.

| Visible field | Reference value/state |
| --- | --- |
| Candidate | F · I |
| Reason | Return home |
| Notes | F A C |
| Duration | One bar |
| InsertTarget | A · slot 4 |
| Now | F |

| Input | Action |
| --- | --- |
| SELECT | Intentionally switch auditioned candidate; each switch is a bounded explicit audition. |
| ALGORITHM | End audition and return NEXT_SHORTLIST before changing Lab tab. |
| K1 | Candidate: 1–3; switching intentionally auditions the new choice |
| K2 | — |
| K3 | Insert slot: A 4 |
| K4 | — |
| HOME | End audition and return HOME; preserve original clock. |
| PLAY | Stop audition and transport; preserve live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | End audition and insert candidate into the visible slot, then EDIT_HARMONY. |
| SEL | Stop audition and return NEXT_SHORTLIST. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Live root/HOLD takes priority and cancels audition.
- Auditioned candidates do not enter committed harmonic history.
- Candidate quality is explicit and local; no silent change to global Harmony or latched modifiers.

**Player instructions:**
1. Listen once.
2. Press EDIT only to commit the displayed candidate.

**Related:** NEXT_SHORTLIST, NEXT_INSERTED

### NEXT_INSERTED — Candidate Inserted

**Group:** Next Chord · **Delivery:** Suggest — fourth milestone · **Template:** status

Acknowledge the exact undoable slot replacement without adding a confirmation before every creative edit.

**Entry:** EDIT in NEXT_SHORTLIST or NEXT_AUDITION.
**Exit:** After the brief acknowledgment, EDIT_HARMONY opens on the named slot.

| Visible field | Reference value/state |
| --- | --- |
| Inserted | F · I |
| Target | A · slot 4 |
| Draft | Evening Return * |
| Undo | Available |

| Input | Action |
| --- | --- |
| SELECT | No additional selection during the brief acknowledgment. |
| ALGORITHM | No tab change during acknowledgment. |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with edited draft. |
| PLAY | Start the explicitly selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Continue immediately to EDIT_HARMONY on the inserted slot. |
| SEL | Open Chord Lab on Now. |
| SAVE | Open IDEA_UNDO. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- The prior slot is the one-step Undo snapshot.
- Live playback receives the new plan only at its normal boundary.
- The complete song remains unsaved until explicit Save.

**Player instructions:**
1. Read the inserted slot.
2. Continue editing or use SAVE → Undo.

**Related:** EDIT_HARMONY, IDEA_UNDO, NEXT_SHORTLIST

### MOTION_ARP — Arpeggio and Strum

**Group:** Motion · **Delivery:** Play — first milestone · **Template:** params

Retain the existing useful chord-delivery modes under one shared tempo.

**Entry:** ARP from another workspace.
**Exit:** ARP/HOME returns home; ALGORITHM to Feel.

| Visible field | Reference value/state |
| --- | --- |
| Enabled | On |
| Mode | Up |
| Octaves | 1 |
| Rate | 1/8 |
| Applies | Live and scheduled chords |

| Input | Action |
| --- | --- |
| SELECT | Choose the mode; never change the catalogue song from this workspace. |
| ALGORITHM | Choose Arpeggio / Feel. |
| K1 | Arp: Off / On |
| K2 | Mode: Up / Down / Inclusive / Exclusive / Random / Order / Repeat / Strum Up / Strum Down |
| K3 | Octaves: 1–4 |
| K4 | Rate: musical division; strum spacing in ms |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Control accompaniment; Arp itself does not launch another transport. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open Chord Lab on Now. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Return HOME. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Shared musical tempo; Random is note order, not progression generation.
- Strum rate becomes inter-note milliseconds; one strum per chord onset.
- Bass stays held/linked beneath arpeggio rather than being arpeggiated.
- Mode changes apply at next chord onset; rate changes retain phase where applicable.
- Arp note motion does not replace the large chord's harmonic identity.

**Player instructions:**
1. Press ARP.
2. Enable it and choose a mode.
3. Set rate; use ALGORITHM for Feel.

**Related:** MOTION_FEEL, HOME_STAGE

### MOTION_FEEL — Motion Feel and Glide

**Group:** Motion · **Delivery:** Play — first milestone · **Template:** params

Keep expressive controls available after Tempo replaces Glide on HOME.

**Entry:** ARP, ALGORITHM to Feel.
**Exit:** ARP/HOME; Advanced Sound is a separate explicit destination.

| Visible field | Reference value/state |
| --- | --- |
| Gate | 75% |
| ArpSwing | 56% |
| Latch | Off |
| SoundGlide | 120 ms |

| Input | Action |
| --- | --- |
| SELECT | No secondary list; direct parameters. |
| ALGORITHM | Choose Arpeggio / Feel. |
| K1 | Arp gate: 20–100% |
| K2 | Arp swing: 50–75% |
| K3 | Manual arp latch: Off / On |
| K4 | Chord-sound Glide: 120 ms |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected playback source from its beginning, or stop the running transport and audition; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open chord-sound Glide behavior selector in the existing Advanced Sound route; do not use SEL as an invisible toggle. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Return HOME. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Strum disables Gate/Swing because its notes sustain to chord end.
- Manual arp latch sustains live takeover independently of physical roots; turning it Off releases only arp-latched ownership.
- HOLD and arp latch are separately labelled sustain sources; either may keep the live owner active.
- Glide belongs to the chord sound and linked bass, rather than automatic global harmony style.
- Legato/Always behavior remains an Advanced Sound parameter.

**Player instructions:**
1. Set Gate and Swing.
2. Use Glide for transitions without leaving tempo hidden on HOME.

**Related:** MOTION_ARP, EDIT_VOICING

### BASS_SOUND — Bass Sound and Level

**Group:** Bass · **Delivery:** Play — first milestone · **Template:** params

Retain the physical LFO button's musically useful Bass destination.

**Entry:** LFO from any musical workspace.
**Exit:** LFO/HOME returns home; ALGORITHM to Behavior.

| Visible field | Reference value/state |
| --- | --- |
| Sound | Round Bass |
| Enabled | On |
| Level | 55% |
| Register | −1 oct |
| Source | Linked chord bass |

| Input | Action |
| --- | --- |
| SELECT | Browse bass sounds; no catalogue progression change. |
| ALGORITHM | Choose Sound / Behavior. |
| K1 | Bass sound: Round Bass |
| K2 | Bass level: 55% |
| K3 | Bass register: −2 to +4 octaves |
| K4 | Bass: Off / On |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Open SOUND_PRESETS with Bass target. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open ENV_ENVELOPE with Bass target. |
| LFO | Return HOME. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Running bass-sound changes commit at next chord onset; stopped held live sound changes use safe retrigger/release handling.
- Sound selection does not start notes when silent.
- Per-slot bass tone overrides remain in EDIT_BASS; this register is the sound/performance default.
- PRESETS remains chord sound even on Bass; SELECT/K1 are the explicitly labelled bass selector.

**Player instructions:**
1. Press LFO.
2. Choose bass with SELECT; set level and register.

**Related:** BASS_BEHAVIOR, SOUND_PRESETS, EDIT_BASS, ENV_ENVELOPE

### BASS_BEHAVIOR — Linked Bass Behavior

**Group:** Bass · **Delivery:** Play — first milestone · **Template:** params

Expose retained bass behavior separately from deep sound modulation.

**Entry:** LFO, ALGORITHM to Behavior.
**Exit:** LFO/HOME or Sound tab.

| Visible field | Reference value/state |
| --- | --- |
| Mode | Follow chord |
| Octaver | Off |
| Glide | Follow chord sound |
| SlotOverride | Root · A slot 3 |

| Input | Action |
| --- | --- |
| SELECT | No extra list; inspect the current slot-bass label only. |
| ALGORITHM | Choose Sound / Behavior. |
| K1 | Bass: Off / On |
| K2 | Octaver: Off / On |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open EDIT_BASS on the focused progression slot. |
| SEL | Open Chord Lab on Now. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Return HOME. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Linked bass follows resolved chord/bass choices rather than introducing an independent generative bassline.
- Octaver retains the existing supported bass octave-doubling behavior; explicit labels show both notes.
- Glide follows the chord sound; deep parameters live in GLO → Sound → Advanced.
- No independent LAYER or multitrack bass recorder is added.

**Player instructions:**
1. Choose On/Off or Octaver.
2. Use EDIT for a particular chord's bass tone.

**Related:** BASS_SOUND, EDIT_BASS, MOTION_FEEL

### ENV_ENVELOPE — Envelope

**Group:** Sound · **Delivery:** Play — first milestone · **Template:** params

Keep the direct envelope controls useful without exposing operator pages by default.

**Entry:** ENV; retains the last explicit Chord/Bass/Linked target, default Linked.
**Exit:** ENV/HOME returns home.

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord + Bass linked |
| Attack | 180 ms |
| Decay | 1.2 s |
| Sustain | 68% |
| Release | 2.4 s |

| Input | Action |
| --- | --- |
| SELECT | Choose Chord / Bass / Chord + Bass linked. |
| ALGORITHM | No hidden operator tabs; deep FM6/VA pages are GLO → Sound → Advanced. |
| K1 | Attack: 180 ms |
| K2 | Decay: 1.2 s |
| K3 | Sustain: 68% |
| K4 | Release: 2.4 s |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Open SOUND_PRESETS for the explicit Chord/Bass target; Linked uses Chord with Linked target shown. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Return HOME. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Display heard envelope units using supported FM6-carrier/VA envelope helpers.
- Linked edits preserve each engine's intended time mapping and clearly affect both chord and bass.
- Changing target is not itself an edit.
- No engine algorithm or operator selection steals EDIT from the progression.

**Player instructions:**
1. Press ENV.
2. Choose the target with SELECT.
3. Set Attack, Decay, Sustain and Release.

**Related:** SOUND_PRESETS, BASS_SOUND, MOTION_FEEL

### SOUND_PRESETS — Sound Presets

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** picker

Browse useful chord/bass banks and favorites while preserving stable sound identity.

**Entry:** GLO → Sound → Presets; SEL from Bass or Envelope provides an explicit target.
**Exit:** HOME or originating workspace button; selection remains applied with normal sound-change timing.

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord |
| Bank | All |
| Sound | Dusty EP |
| Favorite | On |
| Engine | FM6 |
| Level | 78% |

| Input | Action |
| --- | --- |
| SELECT | Browse sounds within the explicit bank and target; a bank change alone leaves the loaded sound intact until a sound is chosen. |
| ALGORITHM | No deep-edit tab; GLO → Sound → Advanced is separate. |
| K1 | Target: Chord / Bass |
| K2 | Bank: All / Existing / User / style banks / Favorites |
| K3 | Favorite: Off / On |
| K4 | Target level: 78% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Open SOUND_ACTIONS for this target sound. |
| SAVE | Open Songs library; whole-Song save remains the default. Use SEL Actions for explicit sound-only saving. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open ENV_ENVELOPE for this target. |
| LFO | Open BASS_SOUND. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Running sounds queue for next chord onset; stopped live-held changes use safe retrigger/release handling.
- All remains reachable; Favorites/User may be empty without clearing the loaded sound.
- PRESETS globally changes chord sound; SELECT here is the target sound selector.
- Stable IDs keep factory/user/favorite references valid after curation.

**Player instructions:**
1. Open GLO → Sound → Presets.
2. Choose Chord/Bass and a bank.
3. Browse sounds; use K3 to favorite one.

**Related:** SOUND_ACTIONS, SOUND_SAVE, ENV_ENVELOPE, BASS_SOUND, SOUND_EMPTY

### SOUND_ACTIONS — Sound Actions

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** picker

Keep sound-only management explicit and separate from whole-song saving.

**Entry:** SEL in SOUND_PRESETS.
**Exit:** Selected route or originating workspace.

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord · Dusty EP |
| Kind | Factory |
| Action | Save As New |
| Available | Save As New · Favorite · Return |

| Input | Action |
| --- | --- |
| SELECT | Choose an available action. |
| ALGORITHM | No tab change. |
| K1 | Action: Save As New / Replace user sound / Delete user sound / Favorite / Return |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Save/Replace opens SOUND_SAVE; Delete opens SOUND_DELETE_CONFIRM; Favorite toggles explicitly; Return goes back. |
| SAVE | Return SOUND_PRESETS. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Factory sounds cannot be replaced/deleted in place.
- Replace/Delete are enabled only for a saved user-sound record.
- This workflow never overwrites the complete saved song automatically.

**Player instructions:**
1. Choose Save As New to keep a patch edit.
2. Use Replace/Delete only on the named user sound.

**Related:** SOUND_SAVE, SOUND_DELETE_CONFIRM, SOUND_PRESETS

### SOUND_SAVE — Name and Save Sound

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** text

Save a sound alone when deliberately requested.

**Entry:** GLO → Sound → Presets → SEL Actions → Save As New / Replace; or a scoped Sound save command.
**Exit:** Success returns SOUND_PRESETS; capacity or write failure uses the same storage/error contract as song saves with Sound target explicitly shown.

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord · edited Dusty EP |
| Name | Evening Keys |
| Cursor | 1 / 16 |
| Action | Save As New |

| Input | Action |
| --- | --- |
| SELECT | Move caret; choose Chord/Bass before entering this screen via the explicit Sound target. |
| ALGORITHM | No tab change during naming. |
| K1 | Character: selected character |
| K2 | Cursor: 1–16 |
| K3 | Set: A–Z / a–z / digits / symbols |
| K4 | Action: Save As New / Replace user / Cancel |
| HOME | Cancel naming, retain sound edits and return HOME. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Save As New writes a free user-sound slot; Replace opens SOUND_OVERWRITE_CONFIRM; Cancel retains edits. |
| SAVE | Execute the visible Save As New / Replace user / Cancel action. A new free slot saves directly; Replace opens SOUND_OVERWRITE_CONFIRM. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Replace is unavailable for a factory target.
- 1–16 characters; duplicate names do not imply overwrite.
- The complete current song remains dirty until explicitly saved after its sound identity changes.
- SAVE and SEL execute the visible action; HOME is the reliable cancel. SAVE never silently cancels naming.

**Player instructions:**
1. Enter a sound name.
2. Choose Save As New and press SAVE; choose Replace only to review a named overwrite.

**Related:** SOUND_OVERWRITE_CONFIRM, SOUND_PRESETS, SOUND_WRITE_ERROR, SOUND_STORAGE_FULL

### SOUND_OVERWRITE_CONFIRM — Replace User Sound?

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** confirm

Confirm sound-only overwrite separately from song overwrite.

**Entry:** SOUND_SAVE Action Replace user, then SEL.
**Exit:** Success returns SOUND_PRESETS; failure shows target-labelled write error.

| Visible field | Reference value/state |
| --- | --- |
| Target | Evening Keys · Chord user sound |
| Scope | Sound patch only |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel/Replace; default Cancel. |
| ALGORITHM | No tab change. |
| K1 | Action: Cancel / Replace |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel and return HOME. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Execute choice; validated safe write preserves previous valid record on failure. |
| SAVE | Execute the selected Cancel/Replace choice; HOME cancels without writing. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Does not overwrite other sounds or the complete song record.

**Player instructions:**
1. Check the sound target.
2. Choose Replace and press SAVE or SEL; HOME cancels.

**Related:** SOUND_SAVE, SOUND_PRESETS

### SOUND_DELETE_CONFIRM — Delete User Sound?

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** confirm

Delete only a named saved user sound through a visible confirmation.

**Entry:** SOUND_ACTIONS Delete user sound, then SEL.
**Exit:** Return SOUND_PRESETS.

| Visible field | Reference value/state |
| --- | --- |
| Target | Evening Keys · user sound |
| FactoryFallback | Dusty EP restored where applicable |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel/Delete; default Cancel. |
| ALGORITHM | No tab change. |
| K1 | Action: Cancel / Delete |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel and return HOME. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Delete the saved record; keep a sounding in-RAM patch until the next deliberate sound change. |
| SAVE | Execute the selected Cancel/Delete choice; HOME cancels without deleting. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Factory record is not deleted.
- An song referring to a removed user sound is shown as Missing sound and uses an explicit safe factory fallback on recall; no stale flash reference.

**Player instructions:**
1. Check the saved sound name.
2. Choose Delete and press SAVE or SEL; HOME cancels.

**Related:** SOUND_ACTIONS, SOUND_PRESETS

### FX_FILTER — Filter

**Group:** Effects · **Delivery:** Play — first milestone · **Template:** params

Use the existing supported filter with its actual three musical controls.

**Entry:** FX opens the last effect; SELECT chooses this effect along the chain.
**Exit:** FX/HOME returns home; ALGORITHM changes FX tab.

| Visible field | Reference value/state |
| --- | --- |
| Effect | Filter |
| Enabled | On |
| Position | In current six-effect chain |
| Type | Low pass |
| Cutoff | 2.5 kHz |
| Resonance | 20% |

| Input | Action |
| --- | --- |
| SELECT | Choose another effect along the visible chain; its parameter screen replaces this one. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | On: Off / On |
| K2 | Type: Low pass / Band pass / High pass |
| K3 | Cutoff: 2.5 kHz |
| K4 | Resonance: 20% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | No hidden action; the On switch is K1. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Foreground parameter controls use supported DSP values; no new effect algorithm is implied.
- Bypass stops new input while supported delay/reverb tails ring out.
- Edits affect the current setup promptly and mark the song dirty; they do not silently overwrite an FX preset.

**Player instructions:**
1. Press FX.
2. Use SELECT to choose Filter.
3. Turn K1–K4 for the labelled controls.

**Related:** FX_ORDER, FX_PRESETS, IDEA_LIBRARY

### FX_DRIVE — Drive

**Group:** Effects · **Delivery:** Play — first milestone · **Template:** params

Use the existing supported drive with its actual three musical controls.

**Entry:** FX opens the last effect; SELECT chooses this effect along the chain.
**Exit:** FX/HOME returns home; ALGORITHM changes FX tab.

| Visible field | Reference value/state |
| --- | --- |
| Effect | Drive |
| Enabled | On |
| Position | In current six-effect chain |
| Drive | 18% |
| Tone | 55% |
| Level | 80% |

| Input | Action |
| --- | --- |
| SELECT | Choose another effect along the visible chain; its parameter screen replaces this one. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | On: Off / On |
| K2 | Drive: 18% |
| K3 | Tone: 55% |
| K4 | Level: 80% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | No hidden action; the On switch is K1. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Foreground parameter controls use supported DSP values; no new effect algorithm is implied.
- Bypass stops new input while supported delay/reverb tails ring out.
- Edits affect the current setup promptly and mark the song dirty; they do not silently overwrite an FX preset.

**Player instructions:**
1. Press FX.
2. Use SELECT to choose Drive.
3. Turn K1–K4 for the labelled controls.

**Related:** FX_ORDER, FX_PRESETS, IDEA_LIBRARY

### FX_CHORUS — Chorus

**Group:** Effects · **Delivery:** Play — first milestone · **Template:** params

Use the existing supported chorus with its actual three musical controls.

**Entry:** FX opens the last effect; SELECT chooses this effect along the chain.
**Exit:** FX/HOME returns home; ALGORITHM changes FX tab.

| Visible field | Reference value/state |
| --- | --- |
| Effect | Chorus |
| Enabled | On |
| Position | In current six-effect chain |
| Rate | 0.35 Hz |
| Depth | 28% |
| Mix | 18% |

| Input | Action |
| --- | --- |
| SELECT | Choose another effect along the visible chain; its parameter screen replaces this one. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | On: Off / On |
| K2 | Rate: 0.35 Hz |
| K3 | Depth: 28% |
| K4 | Mix: 18% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | No hidden action; the On switch is K1. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Foreground parameter controls use supported DSP values; no new effect algorithm is implied.
- Bypass stops new input while supported delay/reverb tails ring out.
- Edits affect the current setup promptly and mark the song dirty; they do not silently overwrite an FX preset.

**Player instructions:**
1. Press FX.
2. Use SELECT to choose Chorus.
3. Turn K1–K4 for the labelled controls.

**Related:** FX_ORDER, FX_PRESETS, IDEA_LIBRARY

### FX_PHASER — Phaser

**Group:** Effects · **Delivery:** Play — first milestone · **Template:** params

Use the existing supported phaser with its actual three musical controls.

**Entry:** FX opens the last effect; SELECT chooses this effect along the chain.
**Exit:** FX/HOME returns home; ALGORITHM changes FX tab.

| Visible field | Reference value/state |
| --- | --- |
| Effect | Phaser |
| Enabled | On |
| Position | In current six-effect chain |
| Rate | 0.18 Hz |
| Depth | 22% |
| Mix | 12% |

| Input | Action |
| --- | --- |
| SELECT | Choose another effect along the visible chain; its parameter screen replaces this one. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | On: Off / On |
| K2 | Rate: 0.18 Hz |
| K3 | Depth: 22% |
| K4 | Mix: 12% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | No hidden action; the On switch is K1. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Foreground parameter controls use supported DSP values; no new effect algorithm is implied.
- Bypass stops new input while supported delay/reverb tails ring out.
- Edits affect the current setup promptly and mark the song dirty; they do not silently overwrite an FX preset.

**Player instructions:**
1. Press FX.
2. Use SELECT to choose Phaser.
3. Turn K1–K4 for the labelled controls.

**Related:** FX_ORDER, FX_PRESETS, IDEA_LIBRARY

### FX_DELAY — Delay

**Group:** Effects · **Delivery:** Play — first milestone · **Template:** params

Use the existing supported delay with its actual three musical controls.

**Entry:** FX opens the last effect; SELECT chooses this effect along the chain.
**Exit:** FX/HOME returns home; ALGORITHM changes FX tab.

| Visible field | Reference value/state |
| --- | --- |
| Effect | Delay |
| Enabled | On |
| Position | In current six-effect chain |
| Time | 420 ms |
| Feedback | 30% |
| Mix | 18% |

| Input | Action |
| --- | --- |
| SELECT | Choose another effect along the visible chain; its parameter screen replaces this one. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | On: Off / On |
| K2 | Time: 420 ms |
| K3 | Feedback: 30% |
| K4 | Mix: 18% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | No hidden action; the On switch is K1. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Foreground parameter controls use supported DSP values; no new effect algorithm is implied.
- Bypass stops new input while supported delay/reverb tails ring out.
- Edits affect the current setup promptly and mark the song dirty; they do not silently overwrite an FX preset.

**Player instructions:**
1. Press FX.
2. Use SELECT to choose Delay.
3. Turn K1–K4 for the labelled controls.

**Related:** FX_ORDER, FX_PRESETS, IDEA_LIBRARY

### FX_REVERB — Reverb

**Group:** Effects · **Delivery:** Play — first milestone · **Template:** params

Use the existing supported reverb with its actual three musical controls.

**Entry:** FX opens the last effect; SELECT chooses this effect along the chain.
**Exit:** FX/HOME returns home; ALGORITHM changes FX tab.

| Visible field | Reference value/state |
| --- | --- |
| Effect | Reverb |
| Enabled | On |
| Position | In current six-effect chain |
| Type | Hall |
| Decay | 62% |
| Mix | 24% |

| Input | Action |
| --- | --- |
| SELECT | Choose another effect along the visible chain; its parameter screen replaces this one. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | On: Off / On |
| K2 | Type: Room / Hall / Plate |
| K3 | Decay: 62% |
| K4 | Mix: 24% |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | No hidden action; the On switch is K1. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Foreground parameter controls use supported DSP values; no new effect algorithm is implied.
- Bypass stops new input while supported delay/reverb tails ring out.
- Edits affect the current setup promptly and mark the song dirty; they do not silently overwrite an FX preset.

**Player instructions:**
1. Press FX.
2. Use SELECT to choose Reverb.
3. Turn K1–K4 for the labelled controls.

**Related:** FX_ORDER, FX_PRESETS, IDEA_LIBRARY

### FX_ORDER — Effect Order

**Group:** Effects · **Delivery:** Play — first milestone · **Template:** timeline

Retain reorderable six-effect routing under a consistent tab-navigation rule.

**Entry:** FX, ALGORITHM to Order.
**Exit:** FX/HOME or Detail/Presets tab.

| Visible field | Reference value/state |
| --- | --- |
| Chain | Filter → Drive → Chorus → Phaser → Delay → Reverb |
| Selected | Delay · position 5 |

| Input | Action |
| --- | --- |
| SELECT | Select an effect in the chain. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | Move selected effect: one position per detent |
| K2 | Selected effect: Off / On |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Open the selected effect's Detail screen. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- ALGORITHM changes tabs, rather than secretly moving an effect as in the old firmware.
- K1 is the explicit reorder control; SELECT remains selected effect.
- Reordering updates the current song and is included in its save.

**Player instructions:**
1. Choose Order with ALGORITHM.
2. Select an effect, then turn K1 to move it.

**Related:** FX_FILTER, FX_DRIVE, FX_CHORUS, FX_PHASER, FX_DELAY, FX_REVERB, FX_PRESETS

### FX_PRESETS — Effect Presets

**Group:** Effects · **Delivery:** Edit and save — second milestone · **Template:** picker

Recall a complete configuration of the existing six effects without treating it as a whole song.

**Entry:** FX, ALGORITHM to Presets.
**Exit:** FX/HOME or another FX tab.

| Visible field | Reference value/state |
| --- | --- |
| Preset | Current chain |
| State | Edited |
| Scope | Six supported effects and order |
| Action | Recall |

| Input | Action |
| --- | --- |
| SELECT | Browse names without changing sound; SEL explicitly recalls the highlighted effect preset. |
| ALGORITHM | Choose Detail / Order / Presets; Presets is phase 2. |
| K1 | Preset: current / factory / user FX configurations |
| K2 | — |
| K3 | — |
| K4 | Action: Recall / Save As New |
| HOME | Return to Progressions; keep transport and unsaved draft. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Recall applies the highlighted chain and stores Undo; Save As New opens FX_SAVE. |
| SAVE | Open Songs library; preserve the loaded source, draft and transport. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Return HOME. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Factory configurations use only Filter, Drive, Chorus, Phaser, Delay and Reverb.
- Recall leaves harmony, key, groove, tempo and sound patches intact.
- Save As New must show FX preset as the target; its exact shared naming/confirmation controls match SOUND_SAVE.
- Capacity/write errors preserve the edited current chain.

**Player instructions:**
1. Browse an FX configuration.
2. Press SEL to recall it.
3. Save the whole song separately when ready.

**Related:** FX_ORDER, FX_REVERB, IDEA_LIBRARY, SOUND_SAVE, FX_SAVE

### FX_SAVE — Name Effect Preset

**Group:** Effects · **Delivery:** Edit and save — second milestone · **Template:** text

Save the supported six-effect configuration as a named preset without implying a sound or song save.

**Entry:** FX_PRESETS Action Save As New, then SEL.
**Exit:** Success returns FX_PRESETS; storage failure keeps current chain and uses target-labelled FX_WRITE_ERROR.

| Visible field | Reference value/state |
| --- | --- |
| Target | Effects only |
| Name | Evening Space |
| Cursor | 1 / 16 |
| Action | Save New |

| Input | Action |
| --- | --- |
| SELECT | Move caret; no sound/song selection. |
| ALGORITHM | No tab change during naming. |
| K1 | Character: selected character |
| K2 | Cursor: 1–16 |
| K3 | Set: A–Z / a–z / digits / symbols |
| K4 | Action: Save New / Cancel |
| HOME | Cancel and return HOME; edited chain remains. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Save a new FX preset with stable ID, or Cancel. |
| SAVE | Execute Save New / Cancel. New free-slot FX presets save directly without an extra review. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Only effect order, bypass and supported parameters are saved.
- New preset never overwrites by matching name.
- Name length and safe-write rules match other named records.
- SAVE and SEL execute the visible action; HOME is the reliable cancel. SAVE never silently cancels naming.

**Player instructions:**
1. Enter a name.
2. Choose Save New and press SAVE; HOME cancels.

**Related:** FX_PRESETS, FX_WRITE_ERROR, FX_STORAGE_FULL

### REC_EMPTY — Progressions · Nothing to Save

**Group:** Recording · **Delivery:** Edit and save — second milestone · **Template:** chord

Record the performance heard on Progressions while HOME stays visible.

**Entry:** A finished/cancelled capture has no musical note/groove content.
**Exit:** Brief notice returns HOME_STAGE; source/draft remain intact.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | 3 / 4 |
| Phrase | IV/Bb → V/C → vi/Dm → I/F |
| Recording | No musical content captured |
| Result | No Song row created |
| Current | Source saved Song unchanged |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Normal complete HOME catalogue browsing/queued changes remain available and are captured when they become audible. |
| ALGORITHM | HOME scene cycle; recording state remains an overlay, not a forced take-editor page. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | On Progressions cycle banks without cancelling capture/arm; from another workspace return Progressions CORE. |
| PLAY | Start/stop the current source normally. |
| REC | Begin a new normal count-in/arm. |
| EDIT | Open semantic editor; audible committed changes are captured without changing the original saved Song. |
| SEL | Open Chord Lab on Now; inside Songs explicitly load the highlighted row. |
| SAVE | Open Songs without stopping capture; record badge persists. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Count-in click, silent control turns and cancelled arm do not create clutter rows.
- Active groove with actual hits counts as musical content even without chords.
- A held audible chord captured at start counts as content.
- Use the owner’s prototype recording language with anchored REC/count/time. Adaptive On follows actual output/active parts; Off stops all animation while status updates instantly.

**Player instructions:**
1. Press REC directly on Progressions.
2. Watch the HOME recording badge; there is no required review screen.

**Related:** HOME_STAGE, IDEA_LIBRARY, SONG_PENDING, SONG_SAVING, SONG_SAVED

### SOUND_EMPTY — No Sounds in This Bank

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** status

Keep an empty User/Favorites sound view recoverable without clearing the loaded patch.

**Entry:** SOUND_PRESETS selects an empty User/Favorites bank.
**Exit:** All/non-empty bank returns SOUND_PRESETS.

| Visible field | Reference value/state |
| --- | --- |
| Target | Bass |
| Bank | Favorites |
| Result | No favorites yet |
| Loaded | Round Bass stays loaded |
| Action | Choose All |

| Input | Action |
| --- | --- |
| SELECT | No sound selection in an empty bank. |
| ALGORITHM | No deep-edit tab. |
| K1 | Target: Chord / Bass |
| K2 | Bank: Favorites / All / Existing / User / style banks |
| K3 | — |
| K4 | Target level: 55% |
| HOME | Return HOME with loaded sound intact. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicit progression-slot focus; otherwise a unique match, otherwise slot 1. |
| SEL | Choose All and return SOUND_PRESETS. |
| SAVE | Open Songs library; use the scoped Sound save route for patch-only saving. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- No silent loading of Init or clearing the current patch.
- Favorite unavailable rows cannot be toggled without a selected sound.

**Player instructions:**
1. Choose All.
2. Favorite a sound with K3 on Sound Presets.

**Related:** SOUND_PRESETS, SOUND_SAVE

### NEXT_UNSUPPORTED — Next Needs a Seven-Note Mode

**Group:** Next Chord · **Delivery:** Suggest — fourth milestone · **Template:** status

Make unsupported advanced-scale context explicit instead of fabricating recommendations.

**Entry:** Chord Lab Next when the active advanced scale is not supported by the seven-note suggestion rules.
**Exit:** HOME or another Lab tab.

| Visible field | Reference value/state |
| --- | --- |
| Scale | Minor Pentatonic |
| Suggestions | Unavailable for this context |
| Action | Choose a seven-note mode on HOME |
| GlobalHarmony | Unchanged |

| Input | Action |
| --- | --- |
| SELECT | No candidate selection; existing draft and history stay intact. |
| ALGORITHM | Choose Notes / Intervals / Compare / Next. |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME to choose mode deliberately. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the focused draft slot without inserting a suggestion. |
| SEL | Return LAB_NOTES. |
| SAVE | Cancel and return SOUND_PRESETS. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- Never silently switch scale, global Harmony or modifiers.
- Sparse supported seven-note modes use labelled theory-guided fallback; this screen is for unsupported context.

**Player instructions:**
1. Press HOME.
2. Choose a supported seven-note mode with K3.
3. Return to Next when ready.

**Related:** NEXT_SHORTLIST, LAB_NOTES, HOME_STAGE

### SONGS_CLEAR_RECORDINGS_CONFIRM — Clear Saved Recordings?

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** confirm

Confirm one counted bulk library action without clearing current/draft/Pending content.

**Entry:** Manage → Clear Recordings.
**Exit:** Success returns Songs, including any surviving Pending row; unsafe storage shows SONGS_OPERATION_PENDING.

| Visible field | Reference value/state |
| --- | --- |
| Target | Recording Songs |
| Count | 12 Saved Songs |
| Excluded | 1 Pending capture + protected draft/current audible copy |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel or the explicitly counted clear action; default Cancel. |
| ALGORITHM | No tab change. |
| K1 | Action: Cancel / Clear Recordings |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel with no deletion. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute same labelled choice. |
| SAVE | Execute selected choice. Revalidate count at commit; changed target membership returns to this counted confirmation. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Counts include only validated Saved rows of the named types; Pending is excluded.
- One confirmation covers the whole batch; no per-row confirmation maze.
- Protected draft/current audible copy and sound presets are not deleted.
- Use a versioned library transaction so interruption does not masquerade as a successful complete clear.

**Player instructions:**
1. Read the exact Saved count and exclusions.
2. Choose the clear action and press SAVE.

**Related:** IDEA_LIBRARY, SONGS_OPERATION_PENDING

### SONGS_CLEAR_ALL_CONFIRM — Clear All Saved Songs?

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** confirm

Confirm one counted bulk library action without clearing current/draft/Pending content.

**Entry:** Manage → Clear All Songs.
**Exit:** Success returns Songs, including any surviving Pending row; unsafe storage shows SONGS_OPERATION_PENDING.

| Visible field | Reference value/state |
| --- | --- |
| Target | all Progression and Recording Songs |
| Count | 20 Saved Songs |
| Excluded | 1 Pending capture + protected draft/current audible copy |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel or the explicitly counted clear action; default Cancel. |
| ALGORITHM | No tab change. |
| K1 | Action: Cancel / Clear All Songs |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel with no deletion. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute same labelled choice. |
| SAVE | Execute selected choice. Revalidate count at commit; changed target membership returns to this counted confirmation. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Counts include only validated Saved rows of the named types; Pending is excluded.
- One confirmation covers the whole batch; no per-row confirmation maze.
- Protected draft/current audible copy and sound presets are not deleted.
- Use a versioned library transaction so interruption does not masquerade as a successful complete clear.

**Player instructions:**
1. Read the exact Saved count and exclusions.
2. Choose the clear action and press SAVE.

**Related:** IDEA_LIBRARY, SONGS_OPERATION_PENDING

### SONG_PENDING_DISCARD_CONFIRM — Discard Pending Song?

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** confirm

Make discarding unsaved captured content a separate named action rather than hiding it inside bulk clear.

**Entry:** Manage → Delete on a Pending row.
**Exit:** Return Songs; after explicit discard another REC can begin.

| Visible field | Reference value/state |
| --- | --- |
| Target | Low Lantern 01 |
| State | Pending · not Saved |
| Count | 1 unsaved capture |
| Action | Cancel |

| Input | Action |
| --- | --- |
| SELECT | Choose Cancel or Discard Pending; default Cancel. |
| ALGORITHM | No tab change. |
| K1 | Action: Cancel / Discard Pending |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Cancel; keep Pending capture. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute same choice. |
| SAVE | Execute choice. Discard releases only this Pending RAM capture, never the source Saved Song. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- No bulk clear silently discards Pending content.
- Source Saved Song, draft and current live ownership remain intact.

**Player instructions:**
1. Check the Pending name.
2. Choose Discard Pending and press SAVE only if intended.

**Related:** IDEA_LIBRARY, HOME_STAGE

### SONGS_OPERATION_PENDING — Library Change Pending

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** status

Keep a requested save/update/delete/clear truthful while flash is busy or sound must remain protected.

**Entry:** A confirmed library operation cannot yet reach the safe storage boundary.
**Exit:** On safe validated completion return Songs with exact result; Cancel retains prior saved records.

| Visible field | Reference value/state |
| --- | --- |
| Operation | Clear 12 Saved Recordings |
| State | Pending · nothing reported cleared |
| Reason | Accompaniment running |
| Action | Stop to finish |
| Protected | Live HOLD and effect tails |

| Input | Action |
| --- | --- |
| SELECT | Choose visible action. |
| ALGORITHM | No tab change. |
| K1 | Action: Stop to finish / Cancel queued operation |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with operation Pending; no false success. |
| PLAY | Stop accompaniment; do not cut independent live HOLD. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Execute same action. |
| SAVE | Execute selected Stop to finish or Cancel. Stop releases accompaniment only; if live notes/tails remain, wait visibly for quiet. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Large Song writes/erases wait until transport is stopped, no live/HOLD/pedal owners remain and measured audio is quiet through the existing quiet guard.
- Do not force a flash erase through a sounding tail; storage_hw.c erases silence the DMA/audio path.
- Busy USB/storage/restore gates remain respected. No operation runs in the audio callback.
- A confirmation’s target snapshot/count remains stable; changed membership requires the counted confirmation again.

**Player instructions:**
1. Choose Stop to finish, then release live-held notes if asked.
2. Wait for the real completed result.

**Related:** IDEA_LIBRARY, IDEA_WRITE_ERROR, HOME_STAGE

### SONG_PENDING — Song Pending · Kept in Songs

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** chord

Keep HOME visible and show a new automatic capture row honestly when it cannot yet be persisted.

**Entry:** Non-empty REC ends; auto-copy cannot save immediately because of live sound, storage load or no free capacity.
**Exit:** Quiet/capacity available → SONG_SAVING → SONG_SAVED; explicit discard removes only this Pending row.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | Stopped after 8 bars |
| Song | Still Into Y 03 |
| Duration | 00:18 |
| State | Pending · not Saved |
| Reason | Release held notes to finish |
| Next | SAVE opens Songs / Manage |

| Input | Action |
| --- | --- |
| SELECT | Factory progression browsing remains available; Pending capture stays protected separately. |
| ALGORITHM | HOME scene cycle; Pending status stays visible. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | Remain on Progressions; Pending banner stays until resolved. |
| PLAY | Stop accompaniment if running; preserve live HOLD, then retry safe save automatically. |
| REC | Do not overwrite the protected Pending buffer. Show Save or discard Pending Song first. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Open Songs and selected Manage options without claiming Saved. |
| SAVE | Open Songs with this Pending row highlighted. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- One protected Pending capture buffer: no new REC may overwrite it. No audio performance is sacrificed to hide a storage problem.
- Storage full shows Manage to make room; normal busy sound shows Stop/release notes to finish.
- Pending rows are playable from RAM by explicit Songs SEL load, but are not durable across power loss.
- Auto-name is source based plus monotonic suffix, maximum 16 characters; never overwrite by name.
- Any expressive pending/saving treatment remains visibly different from Saved and follows the actual storage state; animation duration is not evidence that persistence completed.

**Player instructions:**
1. Press SAVE to see the Pending Song.
2. Manage saved rows for space, or stop/release notes to finish saving.

**Related:** IDEA_LIBRARY, IDEA_STORAGE_FULL, SONG_SAVING, SONG_SAVED, SONG_PENDING_DISCARD_CONFIRM

### SONG_SAVING — Saving New Song

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** chord

Acknowledge automatic new-copy persistence on HOME without a mandatory review route.

**Entry:** Non-empty captured performance finishes and reaches safe storage.
**Exit:** Validated write → SONG_SAVED; failure → Pending/error.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | Stopped |
| Song | Still Into Y 03 |
| Type | Recording · fixed performance |
| Duration | 00:18 |
| State | Saving |

| Input | Action |
| --- | --- |
| SELECT | Normal HOME catalogue browsing; capture row identity stays in the acknowledgment. |
| ALGORITHM | HOME scene cycle. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | Remain on Progressions. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | Wait until current save completes; keep the capture protected. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Open Chord Lab on Now; inside Songs explicitly load the highlighted row. |
| SAVE | Open Songs with this new row highlighted. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Saved appears only after the new record and library index validate.
- No naming/review/Keep click is required after capture.
- Source saved Song is unchanged; new Song has its own stable ID.
- After capture, the current Progression/source remains selected; saving does not auto-load or restart the recorded performance.
- Any expressive pending/saving treatment remains visibly different from Saved and follows the actual storage state; animation duration is not evidence that persistence completed.

**Player instructions:**
1. Read the new Song name and status.
2. Press SAVE to find it directly in Songs.

**Related:** IDEA_LIBRARY, SONG_PENDING, HOME_STAGE

### SONG_SAVED — New Song Saved

**Group:** Songs · **Delivery:** Edit and save — second milestone · **Template:** chord

Acknowledge automatic new-copy persistence on HOME without a mandatory review route.

**Entry:** Non-empty captured performance finishes and reaches successful read-back validation.
**Exit:** Brief acknowledgment returns HOME_STAGE; Song remains in Songs.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | Stopped |
| Song | Still Into Y 03 |
| Type | Recording · fixed performance |
| Duration | 00:18 |
| State | Saved |

| Input | Action |
| --- | --- |
| SELECT | Normal HOME catalogue browsing; capture row identity stays in the acknowledgment. |
| ALGORITHM | HOME scene cycle. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | Remain on Progressions. |
| PLAY | Start/stop the loaded source. During capture, finish promptly and stop accompaniment; preserve independent live HOLD. |
| REC | Start a new recording copy through normal count-in/arm; never overwrite this saved Song. |
| EDIT | Open focused progression slot; in Songs, manage the highlighted row instead. |
| SEL | Open Chord Lab on Now; inside Songs explicitly load the highlighted row. |
| SAVE | Open Songs with this new row highlighted. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- Saved appears only after the new record and library index validate.
- No naming/review/Keep click is required after capture.
- Source saved Song is unchanged; new Song has its own stable ID.
- The new row is highlighted when Songs opens; the original Progression remains current until the player explicitly loads the Recording with SEL.
- Saved feedback follows durable storage acknowledgment and identifies the new Songs row. Off shows the settled result immediately; animation never turns Pending/Saving into Saved.

**Player instructions:**
1. Read the new Song name and status.
2. Press SAVE to find it directly in Songs.

**Related:** IDEA_LIBRARY, SONG_PENDING, HOME_STAGE

### REC_FINISHING — Progressions · Finishing Recording

**Group:** Recording · **Delivery:** Edit and save — second milestone · **Template:** chord

Record the performance heard on Progressions while HOME stays visible.

**Entry:** REC during active capture.
**Exit:** Boundary → Song row Pending/Saving/Saved; no extra user action.

| Visible field | Reference value/state |
| --- | --- |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Key / scale | F Major |
| Metre | 4/4 |
| Position | 3 / 4 |
| Phrase | IV/Bb → V/C → vi/Dm → I/F |
| Recording | Finishing · next bar |
| Current | What you hear is still captured |
| Next | Stop accompaniment and save a new Song copy |
| Bank | CORE 1/4 |

| Input | Action |
| --- | --- |
| SELECT | Normal complete HOME catalogue browsing/queued changes remain available and are captured when they become audible. |
| ALGORITHM | HOME scene cycle; recording state remains an overlay, not a forced take-editor page. |
| K1 | Rhythm: Lo-fi |
| K2 | Key: F |
| K3 | Scale: Major |
| K4 | Tempo: 108 BPM |
| HOME | On Progressions cycle banks without cancelling capture/arm; from another workspace return Progressions CORE. |
| PLAY | Finish immediately at actual time and stop accompaniment. |
| REC | Keep the finish request; do not start another take or require another confirmation. |
| EDIT | Open semantic editor; audible committed changes are captured without changing the original saved Song. |
| SEL | Open Chord Lab on Now; inside Songs explicitly load the highlighted row. |
| SAVE | Open Songs without stopping capture; record badge persists. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced and Controls → Modifiers remain explicit destinations. |

**State coverage:**
- At the next downbeat close captured events, stop/release accompaniment only, and create the new numbered recording row.
- Independent live root/HOLD/pedal ownership survives; such sound can keep the new row Pending until safe.
- No review/name/Keep menu interrupts the normal end.
- Use the owner’s prototype recording language with anchored REC/count/time. Adaptive On follows actual output/active parts; Off stops all animation while status updates instantly.

**Player instructions:**
1. Press REC directly on Progressions.
2. Watch the HOME recording badge; there is no required review screen.

**Related:** HOME_STAGE, IDEA_LIBRARY, SONG_PENDING, SONG_SAVING, SONG_SAVED

### SOUND_STORAGE_FULL — Sound Storage Full

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** status

Keep the edited sound and prior valid record intact without displaying Song-capture status.

**Entry:** Sound save encounters no free named-record slot.
**Exit:** Return to SOUND_SAVE or successful scoped save result.

| Visible field | Reference value/state |
| --- | --- |
| Target | Evening Keys |
| Scope | Sound only |
| Result | Not saved |
| Reason | No free named-record slot |
| Current edits | Kept in RAM |

| Input | Action |
| --- | --- |
| SELECT | Choose Retry or Return. |
| ALGORITHM | No tab change. |
| K1 | Action: Retry / Return to edits |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with sound/effect edits kept. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return to the scoped sound/effect save route, not Songs Manage. |
| SEL | Execute same selected action. |
| SAVE | Execute Retry if safe or Return to edits. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- This is not a Pending Recording Song and does not block REC by claiming the capture buffer.
- No existing record is automatically replaced; old valid patch/FX preset survives failure.
- Factory/user sound and FX capacity remain separate from Songs library clear.

**Player instructions:**
1. Read the scoped target.
2. Retry or return without losing the edited setup.

**Related:** SOUND_SAVE

### SOUND_WRITE_ERROR — Sound Could Not Save

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** status

Keep the edited sound and prior valid record intact without displaying Song-capture status.

**Entry:** Sound save encounters write validation failed.
**Exit:** Return to SOUND_SAVE or successful scoped save result.

| Visible field | Reference value/state |
| --- | --- |
| Target | Evening Keys |
| Scope | Sound only |
| Result | Not saved |
| Reason | Write validation failed |
| Current edits | Kept in RAM |

| Input | Action |
| --- | --- |
| SELECT | Choose Retry or Return. |
| ALGORITHM | No tab change. |
| K1 | Action: Retry / Return to edits |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with sound/effect edits kept. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return to the scoped sound/effect save route, not Songs Manage. |
| SEL | Execute same selected action. |
| SAVE | Execute Retry if safe or Return to edits. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- This is not a Pending Recording Song and does not block REC by claiming the capture buffer.
- No existing record is automatically replaced; old valid patch/FX preset survives failure.
- Factory/user sound and FX capacity remain separate from Songs library clear.

**Player instructions:**
1. Read the scoped target.
2. Retry or return without losing the edited setup.

**Related:** SOUND_SAVE

### FX_STORAGE_FULL — FX preset Storage Full

**Group:** Effects · **Delivery:** Edit and save — second milestone · **Template:** status

Keep the edited fx preset and prior valid record intact without displaying Song-capture status.

**Entry:** FX preset save encounters no free named-record slot.
**Exit:** Return to FX_SAVE or successful scoped save result.

| Visible field | Reference value/state |
| --- | --- |
| Target | Evening Space |
| Scope | FX preset only |
| Result | Not saved |
| Reason | No free named-record slot |
| Current edits | Kept in RAM |

| Input | Action |
| --- | --- |
| SELECT | Choose Retry or Return. |
| ALGORITHM | No tab change. |
| K1 | Action: Retry / Return to edits |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with sound/effect edits kept. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return to the scoped sound/effect save route, not Songs Manage. |
| SEL | Execute same selected action. |
| SAVE | Execute Retry if safe or Return to edits. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- This is not a Pending Recording Song and does not block REC by claiming the capture buffer.
- No existing record is automatically replaced; old valid patch/FX preset survives failure.
- Factory/user sound and FX capacity remain separate from Songs library clear.

**Player instructions:**
1. Read the scoped target.
2. Retry or return without losing the edited setup.

**Related:** FX_SAVE

### FX_WRITE_ERROR — FX preset Could Not Save

**Group:** Effects · **Delivery:** Edit and save — second milestone · **Template:** status

Keep the edited fx preset and prior valid record intact without displaying Song-capture status.

**Entry:** FX preset save encounters write validation failed.
**Exit:** Return to FX_SAVE or successful scoped save result.

| Visible field | Reference value/state |
| --- | --- |
| Target | Evening Space |
| Scope | FX preset only |
| Result | Not saved |
| Reason | Write validation failed |
| Current edits | Kept in RAM |

| Input | Action |
| --- | --- |
| SELECT | Choose Retry or Return. |
| ALGORITHM | No tab change. |
| K1 | Action: Retry / Return to edits |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return HOME with sound/effect edits kept. |
| PLAY | Start the explicitly selected source or stop the running shared transport; external Receive follows the waiting/Start/Continue contract. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Return to the scoped sound/effect save route, not Songs Manage. |
| SEL | Execute same selected action. |
| SAVE | Execute Retry if safe or Return to edits. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets / Advanced remain explicit destinations. |

**State coverage:**
- This is not a Pending Recording Song and does not block REC by claiming the capture buffer.
- No existing record is automatically replaced; old valid patch/FX preset survives failure.
- Factory/user sound and FX capacity remain separate from Songs library clear.

**Player instructions:**
1. Read the scoped target.
2. Retry or return without losing the edited setup.

**Related:** FX_SAVE

### HOME_SOUND — Progressions · SOUND

**Group:** Home · **Delivery:** Play — first milestone · **Template:** chord

Keep the chord visible while the four physical knobs expose audited SOUND controls.

**Entry:** On Progressions tap HOME until SOUND 2/4 appears, or open this bank from CTRL-BANKS.
**Exit:** Any destination button opens its workspace without stopping playback.

| Visible field | Reference value/state |
| --- | --- |
| Song | Still Into You · Paramore chorus seed |
| Setup | F Major · 4/4 · 108 BPM |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Rail | IV/Bbmaj7 → V/C7 → vi/Dm7 → I/Fmaj7 |
| Position | 3 / 4 · bar 3 |
| Source | Progression |
| Groove | Lo-fi |
| Key / scale | F Major |
| Metre | 4/4 |
| Tempo | 108 BPM |
| Phrase | IV / Bbmaj7 → V / C7 → vi / Dm7 → I / Fmaj7 |
| Bank | SOUND 2/4 |
| Chord sound | Dusty EP |
| Bass sound | Round Bass |
| Chord level | 78% |
| Bass level | 55% |

| Input | Action |
| --- | --- |
| SELECT | Browse one complete wrapping factory song list; use the pending selection as the next turn's origin. No tag/key/scale filtering or Apply step. |
| ALGORITHM | Keep the existing secondary-scene role; bank switching uses HOME, not ALGORITHM. Other workspaces keep their named-tab role. |
| K1 | Chord sound: Dusty EP |
| K2 | Bass sound: Round Bass |
| K3 | Chord level: 78% |
| K4 | Bass level: 55% |
| HOME | While already on Progressions, cycle CORE → SOUND → TEXTURE → GROOVE. From any other screen or secondary scene, return Progressions CORE. |
| PLAY | Start this selected progression at slot 1 with its groove; stop if transport is running. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open LAB_NOTES on Now or the last sounding chord. |
| SAVE | Open Songs library; this alone neither saves nor changes the loaded Song. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Stopped browsing applies the seed key/mode/metre immediately without starting sound.
- Running cursor comes from the player; match marks and edit focus have different shapes.
- Each planned numeral sits above its chord name. Key changes preserve degree; mode changes can change numeral case.
- Live roots/HOLD take over harmony while the groove and progression cursor continue.
- No current notes: keep the last chord dimmed with Last, not sounding Now.
- Arpeggio steps animate note activity without changing harmonic identity.
- External Receive without clock: Waiting / armed, Tempo EXT; local PLAY arms rather than free-running.
- External pulse loss: stop/release only accompaniment, retain cursor and independent live HOLD, show Clock lost. Pulses alone never restart.
- Explicit external Start begins slot 1; Continue resumes retained position. Choose Internal in System, then local PLAY, for deliberate internal playback.
- Progression PLAY is an explicit arranged-chord source even when live Harmony is Off; it does not enable or rewrite the global live-root assistance setting. Take source keeps its recorded pitches.
- REC leaves HOME visible: stopped one-bar count-in, running next-bar arm; record what is heard without suppressing automatic chords.
- Every non-empty finished capture stops accompaniment and creates a new numbered Song copy with truthful Pending/Saving/Saved status.
- When a Recording Song is loaded, its fixed note/control/tempo timeline is authoritative. Key/mode/rhythm/tempo fields read Recorded during exact playback; EDIT develops its separate starting progression as a new draft.
- Coral Bloom, Aurora and Studio Rack are historical explorations, not selected. The owner-provided fm1_adaptive_motion_prototype.html is the authoritative visual/motion base; the static nine-screen image remains a layout reference.
- Motion has exactly On / Off. On adapts to smoothed volume/output level and active instrument count. No separate Quiet/Expressive/Reduced/Calm, Intensity or Speed motion preference.
- Use the prototype’s large chord hero, flat aligned type, thin separators, paired degree/name cue and four text control columns with bank name/count/dots. Instrument Teal is default; Studio and Editorial are palettes of the same layout.
- Use resolved playing/ownership and the shared clock. A smoothed output/active-part snapshot requires implementation; prototype-supplied levels are simulations, not hardware FFT/velocity measurements.
- Off stops every animation/transition, including CSS pulses/halos, Morph/Bloom and parameter/bank/focus/save fades. Chord/notes/degree/cursor/status still update instantly; OS reduced motion may force effective Off privately.
- Measure and cap foreground cadence, dirty regions, pixel work and scratch memory under demanding audio/USB. Do not promise FPS, CPU savings or polyphony from a concept.
- The F-major seventh-color demonstration is explicitly arranged IVmaj7 → V7 → vi7 → Imaj7: Bbmaj7 → C7 → Dm7 → Fmaj7, with C7 notes C E G Bb. This stored V7 does not change deliberate live 7 following major/minor quality, including Emaj7 → Bmaj7 → D♯m7.
- Instrument Teal palette: bg #061b1e, fg #f2f8f4, muted #89a7a5, line #355054, subtle #113034, accent #77dacb, onAccent #092d2c; custom solid accents must preserve contrast.
- Retain all nine prototype studies, including Chord Morph and Generative / Bloom. Unsupported morph/generative DSP, Grain/Shimmer/Freeze/Human/Fill and all pictured capabilities are not thereby implemented.
- HOME tap cycling reuses the existing view gesture and needs physical verification. No hold chord or encoder push. A bank switch changes no sound, value, note ownership, recording source or transport.
- K1–K4: Chord sound / Bass sound / Chord level / Bass level. These are existing controls; bank selection alone never edits them.
- The latest adaptive-motion prototype is preserved byte-for-byte as the visual reference. Its up-to-eight demo layers are simulated response inputs, not a promise of eight device parts/engines/tracks/voices.

**Player instructions:**
1. Tap HOME on Progressions to show SOUND.
2. Read the bank/count and values before turning a knob.
3. Keep playing; tap HOME again to continue the cycle.

**Related:** HOME_STAGE, CTRL-BANKS

### HOME_TEXTURE — Progressions · TEXTURE

**Group:** Home · **Delivery:** Play — first milestone · **Template:** chord

Keep the chord visible while the four physical knobs expose audited TEXTURE controls.

**Entry:** On Progressions tap HOME until TEXTURE 3/4 appears, or open this bank from CTRL-BANKS.
**Exit:** Any destination button opens its workspace without stopping playback.

| Visible field | Reference value/state |
| --- | --- |
| Song | Still Into You · Paramore chorus seed |
| Setup | F Major · 4/4 · 108 BPM |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Rail | IV/Bbmaj7 → V/C7 → vi/Dm7 → I/Fmaj7 |
| Position | 3 / 4 · bar 3 |
| Source | Progression |
| Groove | Lo-fi |
| Key / scale | F Major |
| Metre | 4/4 |
| Tempo | 108 BPM |
| Phrase | IV / Bbmaj7 → V / C7 → vi / Dm7 → I / Fmaj7 |
| Bank | TEXTURE 3/4 |
| Reverb mix | 24% |
| Delay mix | 18% |
| Chorus mix | 18% |
| Glide | 120 ms |

| Input | Action |
| --- | --- |
| SELECT | Browse one complete wrapping factory song list; use the pending selection as the next turn's origin. No tag/key/scale filtering or Apply step. |
| ALGORITHM | Keep the existing secondary-scene role; bank switching uses HOME, not ALGORITHM. Other workspaces keep their named-tab role. |
| K1 | Reverb mix: 24% |
| K2 | Delay mix: 18% |
| K3 | Chorus mix: 18% |
| K4 | Glide: 120 ms |
| HOME | While already on Progressions, cycle CORE → SOUND → TEXTURE → GROOVE. From any other screen or secondary scene, return Progressions CORE. |
| PLAY | Start this selected progression at slot 1 with its groove; stop if transport is running. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open LAB_NOTES on Now or the last sounding chord. |
| SAVE | Open Songs library; this alone neither saves nor changes the loaded Song. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Stopped browsing applies the seed key/mode/metre immediately without starting sound.
- Running cursor comes from the player; match marks and edit focus have different shapes.
- Each planned numeral sits above its chord name. Key changes preserve degree; mode changes can change numeral case.
- Live roots/HOLD take over harmony while the groove and progression cursor continue.
- No current notes: keep the last chord dimmed with Last, not sounding Now.
- Arpeggio steps animate note activity without changing harmonic identity.
- External Receive without clock: Waiting / armed, Tempo EXT; local PLAY arms rather than free-running.
- External pulse loss: stop/release only accompaniment, retain cursor and independent live HOLD, show Clock lost. Pulses alone never restart.
- Explicit external Start begins slot 1; Continue resumes retained position. Choose Internal in System, then local PLAY, for deliberate internal playback.
- Progression PLAY is an explicit arranged-chord source even when live Harmony is Off; it does not enable or rewrite the global live-root assistance setting. Take source keeps its recorded pitches.
- REC leaves HOME visible: stopped one-bar count-in, running next-bar arm; record what is heard without suppressing automatic chords.
- Every non-empty finished capture stops accompaniment and creates a new numbered Song copy with truthful Pending/Saving/Saved status.
- When a Recording Song is loaded, its fixed note/control/tempo timeline is authoritative. Key/mode/rhythm/tempo fields read Recorded during exact playback; EDIT develops its separate starting progression as a new draft.
- Coral Bloom, Aurora and Studio Rack are historical explorations, not selected. The owner-provided fm1_adaptive_motion_prototype.html is the authoritative visual/motion base; the static nine-screen image remains a layout reference.
- Motion has exactly On / Off. On adapts to smoothed volume/output level and active instrument count. No separate Quiet/Expressive/Reduced/Calm, Intensity or Speed motion preference.
- Use the prototype’s large chord hero, flat aligned type, thin separators, paired degree/name cue and four text control columns with bank name/count/dots. Instrument Teal is default; Studio and Editorial are palettes of the same layout.
- Use resolved playing/ownership and the shared clock. A smoothed output/active-part snapshot requires implementation; prototype-supplied levels are simulations, not hardware FFT/velocity measurements.
- Off stops every animation/transition, including CSS pulses/halos, Morph/Bloom and parameter/bank/focus/save fades. Chord/notes/degree/cursor/status still update instantly; OS reduced motion may force effective Off privately.
- Measure and cap foreground cadence, dirty regions, pixel work and scratch memory under demanding audio/USB. Do not promise FPS, CPU savings or polyphony from a concept.
- The F-major seventh-color demonstration is explicitly arranged IVmaj7 → V7 → vi7 → Imaj7: Bbmaj7 → C7 → Dm7 → Fmaj7, with C7 notes C E G Bb. This stored V7 does not change deliberate live 7 following major/minor quality, including Emaj7 → Bmaj7 → D♯m7.
- Instrument Teal palette: bg #061b1e, fg #f2f8f4, muted #89a7a5, line #355054, subtle #113034, accent #77dacb, onAccent #092d2c; custom solid accents must preserve contrast.
- Retain all nine prototype studies, including Chord Morph and Generative / Bloom. Unsupported morph/generative DSP, Grain/Shimmer/Freeze/Human/Fill and all pictured capabilities are not thereby implemented.
- HOME tap cycling reuses the existing view gesture and needs physical verification. No hold chord or encoder push. A bank switch changes no sound, value, note ownership, recording source or transport.
- K1–K4: Reverb mix / Delay mix / Chorus mix / Glide. These are existing controls; bank selection alone never edits them.
- The latest adaptive-motion prototype is preserved byte-for-byte as the visual reference. Its up-to-eight demo layers are simulated response inputs, not a promise of eight device parts/engines/tracks/voices.

**Player instructions:**
1. Tap HOME on Progressions to show TEXTURE.
2. Read the bank/count and values before turning a knob.
3. Keep playing; tap HOME again to continue the cycle.

**Related:** HOME_STAGE, CTRL-BANKS

### HOME_GROOVE — Progressions · GROOVE

**Group:** Home · **Delivery:** Play — first milestone · **Template:** chord

Keep the chord visible while the four physical knobs expose audited GROOVE controls.

**Entry:** On Progressions tap HOME until GROOVE 4/4 appears, or open this bank from CTRL-BANKS.
**Exit:** Any destination button opens its workspace without stopping playback.

| Visible field | Reference value/state |
| --- | --- |
| Song | Still Into You · Paramore chorus seed |
| Setup | F Major · 4/4 · 108 BPM |
| Now | Dm7 |
| Degree | vi · minor |
| Notes | D F A C |
| Rail | IV/Bbmaj7 → V/C7 → vi/Dm7 → I/Fmaj7 |
| Position | 3 / 4 · bar 3 |
| Source | Progression |
| Groove | Lo-fi |
| Key / scale | F Major |
| Metre | 4/4 |
| Tempo | 108 BPM |
| Phrase | IV / Bbmaj7 → V / C7 → vi / Dm7 → I / Fmaj7 |
| Bank | GROOVE 4/4 |
| Kit | Dust |
| Drum level | 68% |
| Swing | 38% |
| Click | Off |

| Input | Action |
| --- | --- |
| SELECT | Browse one complete wrapping factory song list; use the pending selection as the next turn's origin. No tag/key/scale filtering or Apply step. |
| ALGORITHM | Keep the existing secondary-scene role; bank switching uses HOME, not ALGORITHM. Other workspaces keep their named-tab role. |
| K1 | Kit: Dust |
| K2 | Drum level: 68% |
| K3 | Swing: 38% |
| K4 | Click: Off |
| HOME | While already on Progressions, cycle CORE → SOUND → TEXTURE → GROOVE. From any other screen or secondary scene, return Progressions CORE. |
| PLAY | Start this selected progression at slot 1 with its groove; stop if transport is running. |
| REC | From Progressions: stopped → one-bar count-in; running → arm next bar. During capture → finish at next bar. Every capture creates a new Song copy. |
| EDIT | Open the explicitly focused progression slot; otherwise a unique live match, otherwise slot 1. |
| SEL | Open LAB_NOTES on Now or the last sounding chord. |
| SAVE | Open Songs library; this alone neither saves nor changes the loaded Song. |
| SEQ | Open Accompaniment on Groove. |
| ARP | Open Motion on Arpeggio. |
| ENV | Open Envelope. |
| LFO | Open Bass on Sound. |
| FX | Open Effects on the last effect. |
| GLO | Open System; Sound → Presets and Sound → Advanced remain reachable there. |

**State coverage:**
- Stopped browsing applies the seed key/mode/metre immediately without starting sound.
- Running cursor comes from the player; match marks and edit focus have different shapes.
- Each planned numeral sits above its chord name. Key changes preserve degree; mode changes can change numeral case.
- Live roots/HOLD take over harmony while the groove and progression cursor continue.
- No current notes: keep the last chord dimmed with Last, not sounding Now.
- Arpeggio steps animate note activity without changing harmonic identity.
- External Receive without clock: Waiting / armed, Tempo EXT; local PLAY arms rather than free-running.
- External pulse loss: stop/release only accompaniment, retain cursor and independent live HOLD, show Clock lost. Pulses alone never restart.
- Explicit external Start begins slot 1; Continue resumes retained position. Choose Internal in System, then local PLAY, for deliberate internal playback.
- Progression PLAY is an explicit arranged-chord source even when live Harmony is Off; it does not enable or rewrite the global live-root assistance setting. Take source keeps its recorded pitches.
- REC leaves HOME visible: stopped one-bar count-in, running next-bar arm; record what is heard without suppressing automatic chords.
- Every non-empty finished capture stops accompaniment and creates a new numbered Song copy with truthful Pending/Saving/Saved status.
- When a Recording Song is loaded, its fixed note/control/tempo timeline is authoritative. Key/mode/rhythm/tempo fields read Recorded during exact playback; EDIT develops its separate starting progression as a new draft.
- Coral Bloom, Aurora and Studio Rack are historical explorations, not selected. The owner-provided fm1_adaptive_motion_prototype.html is the authoritative visual/motion base; the static nine-screen image remains a layout reference.
- Motion has exactly On / Off. On adapts to smoothed volume/output level and active instrument count. No separate Quiet/Expressive/Reduced/Calm, Intensity or Speed motion preference.
- Use the prototype’s large chord hero, flat aligned type, thin separators, paired degree/name cue and four text control columns with bank name/count/dots. Instrument Teal is default; Studio and Editorial are palettes of the same layout.
- Use resolved playing/ownership and the shared clock. A smoothed output/active-part snapshot requires implementation; prototype-supplied levels are simulations, not hardware FFT/velocity measurements.
- Off stops every animation/transition, including CSS pulses/halos, Morph/Bloom and parameter/bank/focus/save fades. Chord/notes/degree/cursor/status still update instantly; OS reduced motion may force effective Off privately.
- Measure and cap foreground cadence, dirty regions, pixel work and scratch memory under demanding audio/USB. Do not promise FPS, CPU savings or polyphony from a concept.
- The F-major seventh-color demonstration is explicitly arranged IVmaj7 → V7 → vi7 → Imaj7: Bbmaj7 → C7 → Dm7 → Fmaj7, with C7 notes C E G Bb. This stored V7 does not change deliberate live 7 following major/minor quality, including Emaj7 → Bmaj7 → D♯m7.
- Instrument Teal palette: bg #061b1e, fg #f2f8f4, muted #89a7a5, line #355054, subtle #113034, accent #77dacb, onAccent #092d2c; custom solid accents must preserve contrast.
- Retain all nine prototype studies, including Chord Morph and Generative / Bloom. Unsupported morph/generative DSP, Grain/Shimmer/Freeze/Human/Fill and all pictured capabilities are not thereby implemented.
- HOME tap cycling reuses the existing view gesture and needs physical verification. No hold chord or encoder push. A bank switch changes no sound, value, note ownership, recording source or transport.
- K1–K4: Kit / Drum level / Swing / Click. These are existing controls; bank selection alone never edits them.
- The latest adaptive-motion prototype is preserved byte-for-byte as the visual reference. Its up-to-eight demo layers are simulated response inputs, not a promise of eight device parts/engines/tracks/voices.

**Player instructions:**
1. Tap HOME on Progressions to show GROOVE.
2. Read the bank/count and values before turning a knob.
3. Keep playing; tap HOME again to continue the cycle.

**Related:** HOME_STAGE, CTRL-BANKS

### SYS-ROOT — Settings

**Group:** Settings · **Delivery:** Play — first milestone · **Template:** picker

Reach controls and service functions without crowding the musical HOME.

**Entry:** GLO from a musical workspace
**Exit:** HOME → Progressions; GLO → previous musical workspace

| Visible field | Reference value/state |
| --- | --- |
| Selected | Harmony |
| Also available | Controls · Display · MIDI/USB |
| More | Sound · Tools/System |

| Input | Action |
| --- | --- |
| SELECT | Choose Harmony, Controls, Display, MIDI/USB, Sound, or Tools/System |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Returning retains selected row
- No settings write/transport change just from opening

**Player instructions:**
1. Press GLO.
2. Turn SELECT to choose a group.
3. Press SEL to open it.

**Related:** SYS-HARMONY, CTRL-ROOT, DISPLAY-STYLE, SYS-MIDI, SYS-USB, SND-ROOT, SYS-TOOLS

### SYS-HARMONY — Harmony

**Group:** Settings · **Delivery:** Play — first milestone · **Template:** params

Control live root-key chord assistance only; explicit Progression PLAY and fixed-note recordings retain their own content.

**Entry:** SYS-ROOT → Harmony
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Harmony | On |
| Scope | Live root keys only |
| Example with 7 | Emaj7 → Bmaj7 → D#m7 |

| Input | Action |
| --- | --- |
| SELECT | No item to select |
| ALGORITHM | No tabs |
| K1 | Harmony · On |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back to Settings |
| SEL | Enter highlighted item |

**State coverage:**
- Off leaves normal playing alone
- On + Quantize Off or non-seven-note scale: Auto?; explain fallback
- On/Off clears selected modifiers/inversion history but preserves held roots/HOLD
- No Rich/Dreamy/Custom styles
- Explicit Progression PLAY plays arranged full chords with live Harmony Off; it does not switch global Harmony On or collapse the plan to single notes
- A recorded Take/Song plays its fixed notes regardless of live Harmony
- Unsupported-Auto single-note fallback applies to live root assistance, not arranged Progression playback

**Player instructions:**
1. Use K1 On/Off for live root-key assistance.
2. With On, play roots and choose colors with modifier buttons.
3. PLAY hears arranged Progression chords even when live Harmony is Off.

**Related:** SYS-ROOT, CTRL-PLAY, CTRL-MODIFIERS

### CTRL-ROOT — Controls

**Group:** Controls · **Delivery:** Play — first milestone · **Template:** picker

Group musical performance rules and deliberate button assignments.

**Entry:** SYS-ROOT → Controls
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Selected | Play behavior |
| Options | Play behavior · Modifiers · Manual forms |

| Input | Action |
| --- | --- |
| SELECT | Choose page |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Top Lock is the supported primary layout
- Legacy layouts are import/compatibility concerns, not a new HOME mode selector

**Player instructions:**
1. Choose Play behavior or an assignment editor with SELECT, then press SEL.

**Related:** SYS-ROOT, CTRL-PLAY, CTRL-MODIFIERS, CTRL-FORMS, CTRL-BANKS

### CTRL-PLAY — Play behavior

**Group:** Controls · **Delivery:** Play — first milestone · **Template:** params

Control touch and chord movement without restoring global harmony styles.

**Entry:** CTRL-ROOT → Play behavior
**Exit:** GLO → CTRL-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Velocity | 100 |
| Glide rule | Legato |
| Key lights | Scale |
| Retrigger | Wait |

| Input | Action |
| --- | --- |
| SELECT | Change page: Play behavior / Quantize and glide |
| ALGORITHM | No action |
| K1 | Velocity · 100 |
| K2 | Glide rule · Legato |
| K3 | Lights · Scale |
| K4 | Retrigger · Wait |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Retrigger choices: Wait, Notes, Slide, Chord; guide must describe exact audible differences
- No new global harmony style

**Player instructions:**
1. Use K1–K4 to change the visible values.
2. Use SELECT for Quantize and glide.

**Related:** CTRL-QUANTIZE, CTRL-ROOT

### CTRL-QUANTIZE — Quantize and glide

**Group:** Controls · **Delivery:** Play — first milestone · **Template:** params

Move Glide off HOME while keeping key/scale setup understandable.

**Entry:** CTRL-PLAY → SELECT next page
**Exit:** GLO → CTRL-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Quantize | Basic |
| Glide | Off |
| Applies to | Chord and linked bass |
| Seven-note Auto | Ready |

| Input | Action |
| --- | --- |
| SELECT | Change page: Play behavior / Quantize and glide |
| ALGORITHM | No action |
| K1 | Quantize · Basic |
| K2 | Glide · Off |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Off / Basic / Full
- Quantize Off: key/scale editing disabled with a direct route to this setting
- Browsing an idea can restore Basic as specified by the progression workflow
- Unsupported Auto: single-note fallback; never falsely label a full automatic chord

**Player instructions:**
1. Choose Basic or Full for Auto.
2. Turn K2 for the chord sound's Glide.
3. Return HOME for direct Rhythm/Key/Scale/Tempo controls.

**Related:** CTRL-PLAY, SYS-HARMONY

### CTRL-MODIFIERS — Modifier assignments

**Group:** Controls · **Delivery:** Play — first milestone · **Template:** params

Assign an upper modifier slot without competing with Chord Lab's SEL destination.

**Entry:** CTRL-ROOT → Modifiers
**Exit:** GLO → CTRL-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Slot | 2 · upper row |
| Modifier | 7 |
| Inversion | Smooth |
| Bass tone | Root |

| Input | Action |
| --- | --- |
| SELECT | Choose an assignable upper button |
| ALGORITHM | No tabs; HOME ALGORITHM scene cycle is not active here |
| K1 | Modifier · 7 |
| K2 | — |
| K3 | Inversion · Smooth |
| K4 | Bass tone · Root |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Choices: None, 6, 7, 9, Add9, 11, 13, Sus2, Sus4, Add2, Add4, Major, Minor, Dim, Aug, Power
- Inversion: Root / Smooth / 1st / 2nd / 3rd
- Bass selects an available chord tone; display the resolved note
- HOLD slot is read-only: HOLD is not a modifier
- Conflicting selections show ! and actual applied modifiers remain clear
- Harmony Off: assignments retained; Manual forms remain separate

**Player instructions:**
1. Choose the slot with SELECT.
2. Turn K1 for the modifier, K3 for inversion and K4 for bass tone.
3. Play notes to hear the result; selecting a slot does not itself change the playing root.

**Related:** CTRL-ROOT, SYS-HARMONY, CTRL-FORMS

### CTRL-FORMS — Manual chord forms

**Group:** Controls · **Delivery:** Edit and save — second milestone · **Template:** params

Preserve user chord forms for Harmony Off and imported content under an advanced route.

**Entry:** CTRL-ROOT → Manual forms
**Exit:** GLO → CTRL-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Button | Major |
| Type | Major |
| Added tones | None |
| Voicing | Root · bass root |

| Input | Action |
| --- | --- |
| SELECT | Choose a form button |
| ALGORITHM | No tabs |
| K1 | Type · Major |
| K2 | Add · None |
| K3 | Inversion · Root |
| K4 | Bass tone · Root |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- HOLD/Off slot read-only
- Harmony On: explain these are retained manual forms; do not silently switch Harmony
- Type/Add/Inversion/Bass ranges follow the active form model

**Player instructions:**
1. Choose a form button.
2. Use K1–K4 for the visible form choices.
3. Turn Harmony Off explicitly to use manual forms.

**Related:** CTRL-ROOT, SYS-HARMONY

### DISPLAY-STYLE — Appearance · Theme

**Group:** Display · **Delivery:** Play — first milestone · **Template:** params

Use Instrument Teal by default, with optional Studio/Editorial palettes and a custom solid accent in the same layout.

**Entry:** SYS-ROOT → Display
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Theme | Instrument Teal |
| Background / primary | #061b1e / #f2f8f4 |
| Secondary / line | #89a7a5 / #355054 |
| Subtle / accent / onAccent | #113034 / #77dacb / #092d2c |

| Input | Action |
| --- | --- |
| SELECT | Choose Theme / Accent / Motion / Legacy scenes |
| ALGORITHM | No action |
| K1 | Theme · Instrument Teal |
| K2 | Accent · #77dacb |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- The owner-provided fm1_adaptive_motion_prototype.html is the authoritative visual/motion reference. Preserve its original bytes separately from any working adaptation.
- Instrument Teal is default: bg #061b1e, fg #f2f8f4, muted #89a7a5, line #355054, subtle #113034, accent #77dacb, onAccent #092d2c. Studio and Editorial are optional palettes of the same layout.
- Retain all nine approved visual studies, including Chord Morph and Generative / Bloom. This does not implement unsupported morph/generative DSP, new engines, macros or every pictured capability.
- A custom accent is solid. Chord/degree anchors, thin separators, four text controls and a bank indicator remain readable; optional palette changes do not alter music or layout.
- Coral Bloom, Aurora and Studio Rack are historical explorations and are not selected; the supplied prototype’s own Morph/Bloom studies remain part of the approved reference.
- Motion has only On / Off. The earlier Quiet/Expressive/Reduced selector is superseded by adaptive output/active-part response; there are no independent Intensity, Speed or Calm motion preferences.

**Player instructions:**
1. Choose Instrument Teal, Studio or Editorial with K1.
2. Use SELECT for custom solid Accent detail or Motion.
3. Return HOME; the same chord/degree, banks and musical controls remain.

**Related:** DISPLAY-ACCENT, DISPLAY-MOTION, DISPLAY-SCENES

### DISPLAY-MOTION — Motion

**Group:** Display · **Delivery:** Play — first milestone · **Template:** params

Choose adaptive On or totally Off, with no additional motion-style, intensity or speed selector.

**Entry:** DISPLAY-STYLE → SELECT Motion
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Motion | On |
| Response | Adaptive · output + active parts |
| Note names | On |
| Key lights | Scale |

| Input | Action |
| --- | --- |
| SELECT | Choose Theme / Accent / Motion / Legacy scenes |
| ALGORITHM | No action |
| K1 | Motion · On / Off |
| K2 | Notes · On |
| K3 | Lights · Scale |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Exactly two user options: On / Off. No Quiet, Expressive, Reduced, Calm, Intensity or Speed motion preference.
- On derives a bounded response from smoothed volume/output level and active instrument-part count. Smooth level/activity changes so short percussion transients do not abruptly change the response.
- Use the shared clock and resolved playing/ownership. A real output envelope and active-part snapshot require implementation/validation; prototype loudness/layers sliders are simulated inputs, not extra device Motion controls.
- The demonstration’s up-to-eight layer count is not a promise of eight hardware instrument parts, engines, tracks or voices.
- Retain functional beat/playhead, chord transition, parameter/bank/focus, REC and confirmed Saved feedback across all nine studies, including Morph/Bloom visual motion. Unsupported musical features remain explicitly studies.
- Off stops every animation and transition, including CSS pulses/halos, fades/interpolation, focus/parameter/bank, Morph/Bloom and Saved responses. Chord/notes/degree/cursor/status update immediately to the correct settled state.
- OS reduced motion may privately force effective Off; it must not create a third Reduced user option.
- Tune the hidden adaptive curve through engineering and physical validation, not a manual Motion Intensity/Speed dial. Measure and cap cadence, dirty regions, pixel work, scratch memory and foreground latency with demanding sound/drums/USB; no browser FPS/CPU/polyphony promises.

**Player instructions:**
1. Turn K1 for Motion On or Off.
2. On: compare quiet/sparse and louder/more-active playing; response adapts automatically.
3. Off: check that all tween/pulse/CSS motion stops while musical values update instantly.
4. Turn notes and key lights independently with K2/K3.

**Related:** DISPLAY-STYLE, DISPLAY-ACCENT

### DISPLAY-SCENES — Secondary scenes

**Group:** Display · **Delivery:** Play — first milestone · **Template:** params

Retain the existing scenes out of the primary writing workflow until deliberate retirement.

**Entry:** DISPLAY-STYLE → SELECT Legacy scenes
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| HOME view | Progressions |
| Secondary views | Bass · Bonfire · Nothing |
| Boot sound | Off |
| Backdrop | None |

| Input | Action |
| --- | --- |
| SELECT | Choose Accent / Motion / Legacy scenes |
| ALGORITHM | No action |
| K1 | HOME view · Progressions |
| K2 | Backdrop · None |
| K3 | Boot sound · Off |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- HOME ALGORITHM cycle remains Progressions → Bass → Bonfire → Nothing
- Scene-free successor build: show Scenes not included, not broken entries
- Boot sound is a deliberate optional preference; turning it on must not disrupt playing
- No LCD backlight toggle: that pin also serves the key matrix

**Player instructions:**
1. Keep Progressions as HOME.
2. Use ALGORITHM on HOME for the secondary scenes.
3. Change optional scene preferences here.

**Related:** DISPLAY-STYLE, SYS-INFO

### SYS-MIDI — MIDI

**Group:** MIDI/USB · **Delivery:** Play — first milestone · **Template:** params

Preserve MIDI note routing and external synchronization without hiding it in a technical dashboard.

**Entry:** SYS-ROOT → MIDI/USB → MIDI
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| MIDI output | On |
| Clock | Internal |
| Notes input | FM-1 keys |
| Input channel | All |

| Input | Action |
| --- | --- |
| SELECT | Choose MIDI / Channels / USB capture |
| ALGORITHM | No action |
| K1 | MIDI out · On |
| K2 | Clock · Internal |
| K3 | Notes in · FM-1 keys |
| K4 | Input channel · All |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Clock Internal / Receive / Send maps active Off / In / Out
- Notes input: FM-1 keys / Roots / Direct
- Direct disables generic input channel; its part channels apply instead
- Changing input interpretation/channel releases owned incoming notes
- MIDI Out Off also disables clock-send output; make this visible

**Player instructions:**
1. Set Notes in and the input channel for an external keyboard.
2. Choose Receive for an external MIDI clock.
3. Use SELECT for the part channels or USB capture status.

**Related:** SYS-MIDI-CHANNELS, SYS-CLOCK-WAIT, SYS-CLOCK-LOST, SYS-USB

### SYS-MIDI-CHANNELS — MIDI channels

**Group:** MIDI/USB · **Delivery:** Play — first milestone · **Template:** params

Show part routing explicitly, including compatibility-only Layer where retained.

**Entry:** SYS-MIDI → SELECT Channels
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Chord | Channel 1 |
| Bass | Channel 2 |
| Drums | Channel 10 |
| Legacy Layer | Channel 3 |

| Input | Action |
| --- | --- |
| SELECT | Choose MIDI / Channels / USB capture |
| ALGORITHM | No action |
| K1 | Chord channel · 1 |
| K2 | Bass channel · 2 |
| K3 | Drums channel · 10 |
| K4 | Layer channel · 3 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- 1–16 per available part
- Layer is labelled Legacy or unavailable after retirement
- Overlapping channels are allowed deliberately; explain Direct input ambiguity if applicable
- Channel changes release previous routing's owned notes

**Player instructions:**
1. Set the destination channel for each part.
2. In Direct input mode, use the relevant part channel on the sending device.

**Related:** SYS-MIDI, SYS-LEGACY-LAYER

### SYS-CLOCK-WAIT — Waiting for clock

**Group:** MIDI/USB · **Delivery:** Play — first milestone · **Template:** status

Explain armed external synchronization instead of showing a misleading editable local tempo.

**Entry:** SYS-MIDI Clock Receive; no pulses received
**Exit:** GLO → SYS-MIDI; HOME → externally armed Progressions

| Visible field | Reference value/state |
| --- | --- |
| Clock source | External MIDI |
| Status | Waiting |
| Tempo | EXT — |
| Local tempo | Unavailable |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions; remain armed |
| GLO | Back to MIDI |
| PLAY | Arm/stop locally; wait for explicit external Start/Continue |

**State coverage:**
- Planned successor policy: Receive without pulses is Waiting/armed; local tempo stays EXT and is not editable
- External Start begins the phrase; Continue resumes the kept position; external Stop stops accompaniment
- Pulses alone after loss do not restart playback
- This is a successor design decision, not a claim about current firmware

**Player instructions:**
1. Set Clock to Receive.
2. Start the master transport when ready.
3. Use the master device's Start or Continue command; clock pulses alone do not restart a stopped phrase.

**Related:** SYS-MIDI, SYS-CLOCK-LOST

### SYS-CLOCK-LOST — Clock lost

**Group:** MIDI/USB · **Delivery:** Play — first milestone · **Template:** status

Stop accompaniment safely on lost external clock, keep position, and preserve independent live HOLD.

**Entry:** Clock Receive loses incoming pulses
**Exit:** GLO → SYS-MIDI; HOME → Progressions with clock warning

| Visible field | Reference value/state |
| --- | --- |
| Clock | External MIDI |
| Status | Clock lost · stopped |
| Position | Kept for Continue |
| Resume | External Start/Continue or choose Internal |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions with Clock lost visible |
| GLO | Back to MIDI; choose Internal deliberately |
| PLAY | Keep accompaniment stopped/armed; do not bypass external source silently |

**State coverage:**
- Stops/releases accompaniment-owned notes only; independent live HOLD remains
- Retains the last phrase position for external Continue
- Clock pulses alone do not restart; external Start/Continue or choosing Internal is required
- Planned timeout: two last-estimated quarter-note periods, clamped 500–4000 ms; validate on device before release
- Player guide omits numeric timeout; current firmware's old roughly-300 ms lock detector is not this successor policy

**Player instructions:**
1. Check the external clock source.
2. Send Start to restart or Continue to resume its kept position.
3. Choose Internal explicitly to regain local tempo control; then use PLAY deliberately.

**Related:** SYS-MIDI, SYS-CLOCK-WAIT

### SYS-USB — USB capture

**Group:** MIDI/USB · **Delivery:** Play — first milestone · **Template:** status

Show the actual host-selected audio stream and routing; avoid an invented on-device rate switch.

**Entry:** SYS-MIDI → SELECT USB capture
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Connection | Audio + MIDI |
| Capture | Host closed |
| Rate | Host selects 44.1 / 48 kHz |
| Channels | 1/2 master · 3/4 chord · 5/6 bass |

| Input | Action |
| --- | --- |
| SELECT | Choose MIDI / Channels / USB capture |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Disconnected / connected-closed / capturing 44.1 / capturing 48 / suspended / reconnecting
- Safe Mode: MIDI + console, audio capture unavailable
- Computer-to-FM-1 audio playback is not provided
- Capture channels retain documented master/stem level and FX behavior
- No new sample-rate or recording toggle is implied by this page

**Player instructions:**
1. Connect a USB data cable.
2. Choose the FM-1 input and rate in the recording app.
3. Record inputs 1/2 for the whole mix; select stems deliberately.

**Related:** SYS-MIDI, SYS-DIAG-USB, SYS-SAFE

### SND-ROOT — Sound

**Group:** Sound · **Delivery:** Play — first milestone · **Template:** picker

Keep sound selection and advanced editing available behind the writing workflow.

**Entry:** SYS-ROOT → Sound
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord |
| Sound | Warm Pad |
| Options | Choose sound · Envelope · Advanced · Save sound |
| Edited | No |

| Input | Action |
| --- | --- |
| SELECT | Highlight Presets / Envelope / Advanced sound / Sound actions; SEL enters the canonical route |
| ALGORITHM | No tabs |
| K1 | Target · Chord |
| K2 | Target preset · Warm Pad |
| K3 | Bank · All |
| K4 | Level · 82% |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| PRESETS | Change chord sound globally |
| ENV | Open ENV_ENVELOPE with named normal target; legacy Layer has an explicit compatibility route |
| LFO | Open BASS_SOUND |

**State coverage:**
- Target Chord / Bass; Layer only via compatibility route
- Explicit target is shown on every sound page
- Bank filter affects sound browsing only; never the progression list
- No matching sound in a bank: say Empty bank and allow All
- Preview edits are not represented as saved until persistence succeeds
- PRESETS globally changes chord sound; K2 or SOUND_PRESETS SELECT explicitly chooses the named target sound

**Player instructions:**
1. Open GLO → Sound.
2. Choose Presets, Envelope, Advanced sound or Sound actions with SELECT then SEL.
3. Use explicit Chord/Bass target controls; PRESETS always changes the chord sound.

**Related:** SOUND_PRESETS, SOUND_ACTIONS, ENV_ENVELOPE, BASS_SOUND, SND-ADVANCED, SND-ADVANCED-VA, SYS-LEGACY-LAYER

### SND-ADVANCED — Advanced sound

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** picker

Group the retained engine's deep pages without taking note keys away for navigation.

**Entry:** SND-ROOT → Advanced
**Exit:** GLO → SND-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord |
| Engine | FM6 |
| Section | OP 1 |
| Page | Envelope Rate |

| Input | Action |
| --- | --- |
| SELECT | Choose a named engine section; SEL enters its first/last page |
| ALGORITHM | Shortcut: choose OP1–OP6 / Pitch / Global for FM6; corresponding VA section for VA |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- FM6 and VA retain their real current editor pages
- No deep editor: clear Not available for this engine
- Changing target/engine retargets safely and names it; no stale wrong-part edits
- Note keys remain performance keys in the successor design

**Player instructions:**
1. Open Sound → Advanced.
2. Choose the named section with SELECT then SEL; ALGORITHM is a section shortcut.
3. Inside a parameter page, SELECT chooses its page, ALGORITHM chooses section, and K1–K4 edit the visible values.

**Related:** SND-FM6-OP1-RATE, SND-VA-OSC1, SOUND_SAVE, SND-ADVANCED-VA

### SND-FM6-OP1-RATE — FM6 OP 1 · Envelope Rate

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 1 → Envelope Rate
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate 1 | 99 |
| Rate 2 | 72 |
| Rate 3 | 50 |
| Rate 4 | 45 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Rate 1 · 99 |
| K2 | Rate 2 · 72 |
| K3 | Rate 3 · 50 |
| K4 | Rate 4 · 45 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP1-LEVEL — FM6 OP 1 · Envelope Level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 1 → Envelope Level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Level 1 | 99 |
| Level 2 | 85 |
| Level 3 | 75 |
| Level 4 | 0 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Level 1 · 99 |
| K2 | Level 2 · 85 |
| K3 | Level 3 · 75 |
| K4 | Level 4 · 0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP1-TUNING — FM6 OP 1 · Tuning

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 1 → Tuning
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Detune | +0 |
| Coarse | 1 |
| Fine | 0 |
| Mode | Ratio |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Detune · +0 |
| K2 | Coarse · 1 |
| K3 | Fine · 0 |
| K4 | Mode · Ratio |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP1-SENS — FM6 OP 1 · Sensitivity and level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 1 → Sensitivity and level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| AM sensitivity | 0 |
| Velocity sensitivity | 2 |
| Output level | 85 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | AM sensitivity · 0 |
| K2 | Velocity sensitivity · 2 |
| K3 | Output level · 85 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP1-SCALING — FM6 OP 1 · Keyboard scaling

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 1 → Keyboard scaling
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left depth | 0 |
| Right depth | 0 |
| Break point | C3 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left depth · 0 |
| K2 | Right depth · 0 |
| K3 | Break point · C3 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP1-CURVES — FM6 OP 1 · Scaling curves

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 1 → Scaling curves
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left curve | -LIN |
| Right curve | -LIN |
| Rate scaling | 0 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left curve · -LIN |
| K2 | Right curve · -LIN |
| K3 | Rate scaling · 0 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP2-RATE — FM6 OP 2 · Envelope Rate

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 2 → Envelope Rate
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate 1 | 99 |
| Rate 2 | 72 |
| Rate 3 | 50 |
| Rate 4 | 45 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Rate 1 · 99 |
| K2 | Rate 2 · 72 |
| K3 | Rate 3 · 50 |
| K4 | Rate 4 · 45 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP2-LEVEL — FM6 OP 2 · Envelope Level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 2 → Envelope Level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Level 1 | 99 |
| Level 2 | 85 |
| Level 3 | 75 |
| Level 4 | 0 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Level 1 · 99 |
| K2 | Level 2 · 85 |
| K3 | Level 3 · 75 |
| K4 | Level 4 · 0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP2-TUNING — FM6 OP 2 · Tuning

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 2 → Tuning
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Detune | +0 |
| Coarse | 1 |
| Fine | 0 |
| Mode | Ratio |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Detune · +0 |
| K2 | Coarse · 1 |
| K3 | Fine · 0 |
| K4 | Mode · Ratio |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP2-SENS — FM6 OP 2 · Sensitivity and level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 2 → Sensitivity and level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| AM sensitivity | 0 |
| Velocity sensitivity | 2 |
| Output level | 85 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | AM sensitivity · 0 |
| K2 | Velocity sensitivity · 2 |
| K3 | Output level · 85 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP2-SCALING — FM6 OP 2 · Keyboard scaling

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 2 → Keyboard scaling
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left depth | 0 |
| Right depth | 0 |
| Break point | C3 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left depth · 0 |
| K2 | Right depth · 0 |
| K3 | Break point · C3 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP2-CURVES — FM6 OP 2 · Scaling curves

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 2 → Scaling curves
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left curve | -LIN |
| Right curve | -LIN |
| Rate scaling | 0 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left curve · -LIN |
| K2 | Right curve · -LIN |
| K3 | Rate scaling · 0 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP3-RATE — FM6 OP 3 · Envelope Rate

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 3 → Envelope Rate
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate 1 | 99 |
| Rate 2 | 72 |
| Rate 3 | 50 |
| Rate 4 | 45 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Rate 1 · 99 |
| K2 | Rate 2 · 72 |
| K3 | Rate 3 · 50 |
| K4 | Rate 4 · 45 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP3-LEVEL — FM6 OP 3 · Envelope Level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 3 → Envelope Level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Level 1 | 99 |
| Level 2 | 85 |
| Level 3 | 75 |
| Level 4 | 0 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Level 1 · 99 |
| K2 | Level 2 · 85 |
| K3 | Level 3 · 75 |
| K4 | Level 4 · 0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP3-TUNING — FM6 OP 3 · Tuning

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 3 → Tuning
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Detune | +0 |
| Coarse | 1 |
| Fine | 0 |
| Mode | Ratio |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Detune · +0 |
| K2 | Coarse · 1 |
| K3 | Fine · 0 |
| K4 | Mode · Ratio |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP3-SENS — FM6 OP 3 · Sensitivity and level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 3 → Sensitivity and level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| AM sensitivity | 0 |
| Velocity sensitivity | 2 |
| Output level | 85 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | AM sensitivity · 0 |
| K2 | Velocity sensitivity · 2 |
| K3 | Output level · 85 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP3-SCALING — FM6 OP 3 · Keyboard scaling

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 3 → Keyboard scaling
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left depth | 0 |
| Right depth | 0 |
| Break point | C3 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left depth · 0 |
| K2 | Right depth · 0 |
| K3 | Break point · C3 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP3-CURVES — FM6 OP 3 · Scaling curves

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 3 → Scaling curves
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left curve | -LIN |
| Right curve | -LIN |
| Rate scaling | 0 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left curve · -LIN |
| K2 | Right curve · -LIN |
| K3 | Rate scaling · 0 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP4-RATE — FM6 OP 4 · Envelope Rate

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 4 → Envelope Rate
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate 1 | 99 |
| Rate 2 | 72 |
| Rate 3 | 50 |
| Rate 4 | 45 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Rate 1 · 99 |
| K2 | Rate 2 · 72 |
| K3 | Rate 3 · 50 |
| K4 | Rate 4 · 45 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP4-LEVEL — FM6 OP 4 · Envelope Level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 4 → Envelope Level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Level 1 | 99 |
| Level 2 | 85 |
| Level 3 | 75 |
| Level 4 | 0 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Level 1 · 99 |
| K2 | Level 2 · 85 |
| K3 | Level 3 · 75 |
| K4 | Level 4 · 0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP4-TUNING — FM6 OP 4 · Tuning

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 4 → Tuning
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Detune | +0 |
| Coarse | 1 |
| Fine | 0 |
| Mode | Ratio |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Detune · +0 |
| K2 | Coarse · 1 |
| K3 | Fine · 0 |
| K4 | Mode · Ratio |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP4-SENS — FM6 OP 4 · Sensitivity and level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 4 → Sensitivity and level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| AM sensitivity | 0 |
| Velocity sensitivity | 2 |
| Output level | 85 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | AM sensitivity · 0 |
| K2 | Velocity sensitivity · 2 |
| K3 | Output level · 85 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP4-SCALING — FM6 OP 4 · Keyboard scaling

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 4 → Keyboard scaling
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left depth | 0 |
| Right depth | 0 |
| Break point | C3 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left depth · 0 |
| K2 | Right depth · 0 |
| K3 | Break point · C3 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP4-CURVES — FM6 OP 4 · Scaling curves

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 4 → Scaling curves
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left curve | -LIN |
| Right curve | -LIN |
| Rate scaling | 0 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left curve · -LIN |
| K2 | Right curve · -LIN |
| K3 | Rate scaling · 0 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP5-RATE — FM6 OP 5 · Envelope Rate

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 5 → Envelope Rate
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate 1 | 99 |
| Rate 2 | 72 |
| Rate 3 | 50 |
| Rate 4 | 45 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Rate 1 · 99 |
| K2 | Rate 2 · 72 |
| K3 | Rate 3 · 50 |
| K4 | Rate 4 · 45 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP5-LEVEL — FM6 OP 5 · Envelope Level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 5 → Envelope Level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Level 1 | 99 |
| Level 2 | 85 |
| Level 3 | 75 |
| Level 4 | 0 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Level 1 · 99 |
| K2 | Level 2 · 85 |
| K3 | Level 3 · 75 |
| K4 | Level 4 · 0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP5-TUNING — FM6 OP 5 · Tuning

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 5 → Tuning
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Detune | +0 |
| Coarse | 1 |
| Fine | 0 |
| Mode | Ratio |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Detune · +0 |
| K2 | Coarse · 1 |
| K3 | Fine · 0 |
| K4 | Mode · Ratio |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP5-SENS — FM6 OP 5 · Sensitivity and level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 5 → Sensitivity and level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| AM sensitivity | 0 |
| Velocity sensitivity | 2 |
| Output level | 85 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | AM sensitivity · 0 |
| K2 | Velocity sensitivity · 2 |
| K3 | Output level · 85 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP5-SCALING — FM6 OP 5 · Keyboard scaling

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 5 → Keyboard scaling
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left depth | 0 |
| Right depth | 0 |
| Break point | C3 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left depth · 0 |
| K2 | Right depth · 0 |
| K3 | Break point · C3 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP5-CURVES — FM6 OP 5 · Scaling curves

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 5 → Scaling curves
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left curve | -LIN |
| Right curve | -LIN |
| Rate scaling | 0 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left curve · -LIN |
| K2 | Right curve · -LIN |
| K3 | Rate scaling · 0 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP6-RATE — FM6 OP 6 · Envelope Rate

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 6 → Envelope Rate
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate 1 | 99 |
| Rate 2 | 72 |
| Rate 3 | 50 |
| Rate 4 | 45 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Rate 1 · 99 |
| K2 | Rate 2 · 72 |
| K3 | Rate 3 · 50 |
| K4 | Rate 4 · 45 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP6-LEVEL — FM6 OP 6 · Envelope Level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 6 → Envelope Level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Level 1 | 99 |
| Level 2 | 85 |
| Level 3 | 75 |
| Level 4 | 0 |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Level 1 · 99 |
| K2 | Level 2 · 85 |
| K3 | Level 3 · 75 |
| K4 | Level 4 · 0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP6-TUNING — FM6 OP 6 · Tuning

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 6 → Tuning
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Detune | +0 |
| Coarse | 1 |
| Fine | 0 |
| Mode | Ratio |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Detune · +0 |
| K2 | Coarse · 1 |
| K3 | Fine · 0 |
| K4 | Mode · Ratio |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP6-SENS — FM6 OP 6 · Sensitivity and level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 6 → Sensitivity and level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| AM sensitivity | 0 |
| Velocity sensitivity | 2 |
| Output level | 85 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | AM sensitivity · 0 |
| K2 | Velocity sensitivity · 2 |
| K3 | Output level · 85 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP6-SCALING — FM6 OP 6 · Keyboard scaling

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 6 → Keyboard scaling
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left depth | 0 |
| Right depth | 0 |
| Break point | C3 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left depth · 0 |
| K2 | Right depth · 0 |
| K3 | Break point · C3 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-OP6-CURVES — FM6 OP 6 · Scaling curves

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current FM6 operator editor on a shared four-parameter template.

**Entry:** SND-ADVANCED → OP 6 → Scaling curves
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Left curve | -LIN |
| Right curve | -LIN |
| Rate scaling | 0 |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose Rate / Level / Tuning / Sensitivity / Scaling / Curves |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Left curve · -LIN |
| K2 | Right curve · -LIN |
| K3 | Rate scaling · 0 |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Per-operator values are bounded to active firmware ranges
- Fixed/Ratio changes the interpretation of Coarse/Fine
- Any unused knob is dim and does nothing
- Carrier/modulator role follows the selected algorithm; no automatic harmony changes
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target and operator.
2. Choose a page with SELECT.
3. Turn only labelled knobs; save the sound explicitly if wanted.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-PITCH-RATE — FM6 Pitch Rate

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony.

**Entry:** SND-ADVANCED → Pitch → FM6 Pitch Rate
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate 1 | 99 |
| Rate 2 | 99 |
| Rate 3 | 99 |
| Rate 4 | 99 |

| Input | Action |
| --- | --- |
| SELECT | Pitch: Rate / Level; Global: Voice / LFO / LFO 2 |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Rate 1 · 99 |
| K2 | Rate 2 · 99 |
| K3 | Rate 3 · 99 |
| K4 | Rate 4 · 99 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Transpose can release/retarget engine notes safely
- LFO waves: Tri, Saw Down, Saw Up, Square, Sine, S&H
- This is the active CF editor; extra FM6 engine-only pages are not silently claimed
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Select the target section/page.
2. Use the four labelled knobs.
3. Save the target sound explicitly.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-PITCH-LEVEL — FM6 Pitch Level

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony.

**Entry:** SND-ADVANCED → Pitch → FM6 Pitch Level
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Level 1 | 50 |
| Level 2 | 50 |
| Level 3 | 50 |
| Level 4 | 50 |

| Input | Action |
| --- | --- |
| SELECT | Pitch: Rate / Level; Global: Voice / LFO / LFO 2 |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Level 1 · 50 |
| K2 | Level 2 · 50 |
| K3 | Level 3 · 50 |
| K4 | Level 4 · 50 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Transpose can release/retarget engine notes safely
- LFO waves: Tri, Saw Down, Saw Up, Square, Sine, S&H
- This is the active CF editor; extra FM6 engine-only pages are not silently claimed
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Select the target section/page.
2. Use the four labelled knobs.
3. Save the target sound explicitly.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-GLOBAL-VOICE — FM6 Voice

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony.

**Entry:** SND-ADVANCED → Global → FM6 Voice
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Algorithm | 1 |
| Feedback | 0 |
| Oscillator sync | On |
| Transpose | +0 |

| Input | Action |
| --- | --- |
| SELECT | Pitch: Rate / Level; Global: Voice / LFO / LFO 2 |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Algorithm · 1 |
| K2 | Feedback · 0 |
| K3 | Oscillator sync · On |
| K4 | Transpose · +0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Transpose can release/retarget engine notes safely
- LFO waves: Tri, Saw Down, Saw Up, Square, Sine, S&H
- This is the active CF editor; extra FM6 engine-only pages are not silently claimed
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Select the target section/page.
2. Use the four labelled knobs.
3. Save the target sound explicitly.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-GLOBAL-LFO — FM6 LFO

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony.

**Entry:** SND-ADVANCED → Global → FM6 LFO
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Wave | Sine |
| Speed | 35 |
| Pitch depth | 0 |
| Amplitude depth | 0 |

| Input | Action |
| --- | --- |
| SELECT | Pitch: Rate / Level; Global: Voice / LFO / LFO 2 |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Wave · Sine |
| K2 | Speed · 35 |
| K3 | Pitch depth · 0 |
| K4 | Amplitude depth · 0 |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Transpose can release/retarget engine notes safely
- LFO waves: Tri, Saw Down, Saw Up, Square, Sine, S&H
- This is the active CF editor; extra FM6 engine-only pages are not silently claimed
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Select the target section/page.
2. Use the four labelled knobs.
3. Save the target sound explicitly.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-FM6-GLOBAL-LFO2 — FM6 LFO 2

**Group:** Advanced FM6 · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony.

**Entry:** SND-ADVANCED → Global → FM6 LFO 2
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Delay | 0 |
| Pitch sensitivity | 3 |
| LFO sync | On |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Pitch: Rate / Level; Global: Voice / LFO / LFO 2 |
| ALGORITHM | Choose OP1–OP6 / Pitch / Global |
| K1 | Delay · 0 |
| K2 | Pitch sensitivity · 3 |
| K3 | LFO sync · On |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Transpose can release/retarget engine notes safely
- LFO waves: Tri, Saw Down, Saw Up, Square, Sine, S&H
- This is the active CF editor; extra FM6 engine-only pages are not silently claimed
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Select the target section/page.
2. Use the four labelled knobs.
3. Save the target sound explicitly.

**Related:** SND-ADVANCED, SOUND_SAVE

### SND-VA-OSC1 — VA · Oscillator 1

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 1 → Oscillator 1
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Wave | Saw |
| Level | 65% |
| Coarse | +0 st |
| Fine | +0 ct |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 1 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Wave · Saw |
| K2 | Level · 65% |
| K3 | Coarse · +0 st |
| K4 | Fine · +0 ct |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Wave/shape labels adapt to the selected oscillator mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-OSC1-DETAIL — VA · Oscillator 1 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 1 → Oscillator 1 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Mode | Basic |
| Shape | 50% |
| Key tracking | On |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 1 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Mode · Basic |
| K2 | Shape · 50% |
| K3 | Key tracking · On |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Mode: Basic / Morph / Noise; shape meaning follows mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-OSC2 — VA · Oscillator 2

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 2 → Oscillator 2
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Wave | Saw |
| Level | 65% |
| Coarse | +0 st |
| Fine | +0 ct |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 2 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Wave · Saw |
| K2 | Level · 65% |
| K3 | Coarse · +0 st |
| K4 | Fine · +0 ct |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Wave/shape labels adapt to the selected oscillator mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-OSC2-DETAIL — VA · Oscillator 2 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 2 → Oscillator 2 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Mode | Basic |
| Shape | 50% |
| Key tracking | On |
| Sync | Off |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 2 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Mode · Basic |
| K2 | Shape · 50% |
| K3 | Key tracking · On |
| K4 | Sync · Off |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Mode: Basic / Morph / Noise; shape meaning follows mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-OSC3 — VA · Oscillator 3

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 3 → Oscillator 3
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Wave | Saw |
| Level | 65% |
| Coarse | +0 st |
| Fine | +0 ct |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 3 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Wave · Saw |
| K2 | Level · 65% |
| K3 | Coarse · +0 st |
| K4 | Fine · +0 ct |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Wave/shape labels adapt to the selected oscillator mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-OSC3-DETAIL — VA · Oscillator 3 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 3 → Oscillator 3 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Mode | Basic |
| Shape | 50% |
| Key tracking | On |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 3 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Mode · Basic |
| K2 | Shape · 50% |
| K3 | Key tracking · On |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Mode: Basic / Morph / Noise; shape meaning follows mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-OSC4 — VA · Oscillator 4

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 4 → Oscillator 4
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Wave | Saw |
| Level | 65% |
| Coarse | +0 st |
| Fine | +0 ct |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 4 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Wave · Saw |
| K2 | Level · 65% |
| K3 | Coarse · +0 st |
| K4 | Fine · +0 ct |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Wave/shape labels adapt to the selected oscillator mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-OSC4-DETAIL — VA · Oscillator 4 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → OSC 4 → Oscillator 4 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Mode | Basic |
| Shape | 50% |
| Key tracking | On |
| Ring | Off |

| Input | Action |
| --- | --- |
| SELECT | Choose the current OSC 4 section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Mode · Basic |
| K2 | Shape · 50% |
| K3 | Key tracking · On |
| K4 | Ring · Off |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Mode: Basic / Morph / Noise; shape meaning follows mode
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-FILTER — VA · Filter

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Filter → Filter
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Cutoff | 4.2 kHz |
| Resonance | 20% |
| Type | LP |
| Envelope amount | +0% |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Filter section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Cutoff · 4.2 kHz |
| K2 | Resonance · 20% |
| K3 | Type · LP |
| K4 | Envelope amount · +0% |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Type moves through LP/BP/HP/Notch as supported; display the resolved type
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-FILTER-DETAIL — VA · Filter detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Filter → Filter detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Key tracking | 50% |
| Unused | — |
| Spread | 20% |
| Drive | 0% |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Filter section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Key tracking · 50% |
| K2 | — |
| K3 | Spread · 20% |
| K4 | Drive · 0% |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV1 — VA · Envelope 1

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 1
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Attack | 35 ms |
| Decay | 220 ms |
| Sustain | 80% |
| Release | 1.2 s |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Attack · 35 ms |
| K2 | Decay · 220 ms |
| K3 | Sustain · 80% |
| K4 | Release · 1.2 s |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Envelope 1 is the amplitude envelope used by the simple ENV view
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV1-DETAIL — VA · Envelope 1 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 1 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Hold | 0 ms |
| Velocity | 50% |
| Unused | — |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Hold · 0 ms |
| K2 | Velocity · 50% |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV2 — VA · Envelope 2

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 2
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Attack | 35 ms |
| Decay | 220 ms |
| Sustain | 80% |
| Release | 1.2 s |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Attack · 35 ms |
| K2 | Decay · 220 ms |
| K3 | Sustain · 80% |
| K4 | Release · 1.2 s |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Envelope 1 is the amplitude envelope used by the simple ENV view
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV2-DETAIL — VA · Envelope 2 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 2 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Hold | 0 ms |
| Unused | — |
| Unused | — |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Hold · 0 ms |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV3 — VA · Envelope 3

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 3
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Attack | 35 ms |
| Decay | 220 ms |
| Sustain | 80% |
| Release | 1.2 s |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Attack · 35 ms |
| K2 | Decay · 220 ms |
| K3 | Sustain · 80% |
| K4 | Release · 1.2 s |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Envelope 1 is the amplitude envelope used by the simple ENV view
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV3-DETAIL — VA · Envelope 3 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 3 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Hold | 0 ms |
| Unused | — |
| Unused | — |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Hold · 0 ms |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV4 — VA · Envelope 4

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 4
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Attack | 35 ms |
| Decay | 220 ms |
| Sustain | 80% |
| Release | 1.2 s |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Attack · 35 ms |
| K2 | Decay · 220 ms |
| K3 | Sustain · 80% |
| K4 | Release · 1.2 s |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Envelope 1 is the amplitude envelope used by the simple ENV view
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-ENV4-DETAIL — VA · Envelope 4 detail

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Envelope → Envelope 4 detail
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Hold | 0 ms |
| Unused | — |
| Unused | — |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Envelope section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Hold · 0 ms |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-LFO1 — VA · LFO 1

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → LFO → LFO 1
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate | 0.6 Hz |
| Wave | Sine |
| Depth | 35% |
| Fade | 0 ms |

| Input | Action |
| --- | --- |
| SELECT | Choose the current LFO section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Rate · 0.6 Hz |
| K2 | Wave · Sine |
| K3 | Depth · 35% |
| K4 | Fade · 0 ms |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Rate becomes a tempo division when that LFO's sync is enabled
- External clock status applies to tempo-synced modulation
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-LFO2 — VA · LFO 2

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → LFO → LFO 2
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate | 0.6 Hz |
| Wave | Sine |
| Depth | 35% |
| Fade | 0 ms |

| Input | Action |
| --- | --- |
| SELECT | Choose the current LFO section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Rate · 0.6 Hz |
| K2 | Wave · Sine |
| K3 | Depth · 35% |
| K4 | Fade · 0 ms |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Rate becomes a tempo division when that LFO's sync is enabled
- External clock status applies to tempo-synced modulation
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-LFO3 — VA · LFO 3

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → LFO → LFO 3
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate | 0.6 Hz |
| Wave | Sine |
| Depth | 35% |
| Fade | 0 ms |

| Input | Action |
| --- | --- |
| SELECT | Choose the current LFO section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Rate · 0.6 Hz |
| K2 | Wave · Sine |
| K3 | Depth · 35% |
| K4 | Fade · 0 ms |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Rate becomes a tempo division when that LFO's sync is enabled
- External clock status applies to tempo-synced modulation
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-LFO4 — VA · LFO 4

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → LFO → LFO 4
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Rate | 0.6 Hz |
| Wave | Sine |
| Depth | 35% |
| Fade | 0 ms |

| Input | Action |
| --- | --- |
| SELECT | Choose the current LFO section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Rate · 0.6 Hz |
| K2 | Wave · Sine |
| K3 | Depth · 35% |
| K4 | Fade · 0 ms |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Rate becomes a tempo division when that LFO's sync is enabled
- External clock status applies to tempo-synced modulation
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-LFO-SYNC — VA · LFO sync

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → LFO → LFO sync
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Sync 1 | On |
| Sync 2 | Off |
| Sync 3 | Off |
| Sync 4 | Off |

| Input | Action |
| --- | --- |
| SELECT | Choose the current LFO section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Sync 1 · On |
| K2 | Sync 2 · Off |
| K3 | Sync 3 · Off |
| K4 | Sync 4 · Off |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-VOICE — VA · Voice spread

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → LFO → Voice spread
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Unison spread | 20% |
| Unused | — |
| Unused | — |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current LFO section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Unison spread · 20% |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD1 — VA · Modulation 1

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 1
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | LFO 1 |
| Destination | Cutoff |
| Amount | +20% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · LFO 1 |
| K2 | Destination · Cutoff |
| K3 | Amount · +20% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD2 — VA · Modulation 2

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 2
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | Off |
| Destination | Off |
| Amount | +0% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · Off |
| K2 | Destination · Off |
| K3 | Amount · +0% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD3 — VA · Modulation 3

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 3
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | Off |
| Destination | Off |
| Amount | +0% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · Off |
| K2 | Destination · Off |
| K3 | Amount · +0% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD4 — VA · Modulation 4

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 4
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | Off |
| Destination | Off |
| Amount | +0% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · Off |
| K2 | Destination · Off |
| K3 | Amount · +0% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD5 — VA · Modulation 5

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 5
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | Off |
| Destination | Off |
| Amount | +0% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · Off |
| K2 | Destination · Off |
| K3 | Amount · +0% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD6 — VA · Modulation 6

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 6
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | Off |
| Destination | Off |
| Amount | +0% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · Off |
| K2 | Destination · Off |
| K3 | Amount · +0% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD7 — VA · Modulation 7

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 7
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | Off |
| Destination | Off |
| Amount | +0% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · Off |
| K2 | Destination · Off |
| K3 | Amount · +0% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-VA-MOD8 — VA · Modulation 8

**Group:** Advanced VA · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the VA engine's current parameter page on the shared advanced template.

**Entry:** SND-ADVANCED → Mod → Modulation 8
**Exit:** GLO → SND-ADVANCED; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Source | Off |
| Destination | Off |
| Amount | +0% |
| Unused | — |

| Input | Action |
| --- | --- |
| SELECT | Choose the current Mod section's page |
| ALGORITHM | Choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | Source · Off |
| K2 | Destination · Off |
| K3 | Amount · +0% |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Open musical collection/menu; choose Save Sound explicitly to enter SOUND_SAVE for the named target |

**State coverage:**
- Values/units follow current VA descriptor ranges
- Unused knobs are dim and inert
- Off source/destination is a valid inactive row, not a missing page
- Only engine-supported source/destination enums are selectable
- PRESETS always changes chord sound; keep the explicit editing target unchanged

**Player instructions:**
1. Confirm the sound target.
2. Select section/page.
3. Turn the labelled knobs and save explicitly if desired.

**Related:** SND-ADVANCED-VA, SOUND_SAVE

### SND-SAVED — Sound saved

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** status

Distinguish successful persistence from merely edited or recorded content.

**Entry:** Successful sound write
**Exit:** Brief result → SOUND_PRESETS or originating sound context

| Visible field | Reference value/state |
| --- | --- |
| Saved | Warm Pad 2 |
| Target | Chord |
| State | Saved |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Progressions |
| GLO | SND-ROOT |
| SEL | SOUND_PRESETS |

**State coverage:**
- Show only after the write reports success
- No RAM-only emulator success may be presented as hardware persistence

**Player instructions:**
1. Return to playing.

**Related:** SND-ROOT

### SYS-TOOLS — Tools / System

**Group:** System · **Delivery:** Play — first milestone · **Template:** picker

Keep support and destructive actions outside musical HOME.

**Entry:** SYS-ROOT → Tools/System
**Exit:** GLO → SYS-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Selected | Information |
| Tools | Diagnostics · Backup/Restore |
| Service | Legacy content · Update · Reset |

| Input | Action |
| --- | --- |
| SELECT | Choose Information / Diagnostics / Backup / Legacy / Update / Reset |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- No action occurs simply from highlighting Reset or Update

**Player instructions:**
1. Choose the named tool with SELECT and press SEL.

**Related:** SYS-INFO, SYS-DIAG-OVERVIEW, SYS-BACKUP, SYS-LEGACY, SYS-UPDATE, SYS-RESET-ARM

### SYS-INFO — Information

**Group:** System · **Delivery:** Play — first milestone · **Template:** text

Provide identity and support facts without cluttering the playing screen.

**Entry:** SYS-TOOLS → Information
**Exit:** GLO → SYS-TOOLS; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Firmware | Chord-first design preview |
| Version | Runtime build version here |
| Compatibility | Existing updater family retained |
| USB | Audio + MIDI |

| Input | Action |
| --- | --- |
| SELECT | Additional info page only if required |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Actual version/revision from the running build, no invented preview version
- Safe Mode/fallback build label must be explicit
- Protocol identity and displayed product identity need not be the same string

**Player instructions:**
1. Read the version before reporting a problem.

**Related:** SYS-TOOLS, SYS-USB

### SYS-DIAG-OVERVIEW — Diagnostics · overview

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Retain useful measurements in Tools with their limitations clearly labelled.

**Entry:** SYS-TOOLS → Diagnostics
**Exit:** GLO → SYS-TOOLS; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Device | Not connected in design preview |
| Audio load / peak | Not measured |
| Voices / overload | Not measured |
| USB loss | Not measured |

| Input | Action |
| --- | --- |
| SELECT | No item to enter |
| ALGORITHM | No action |
| K1 | Page · overview |
| K2 | Display · Live |
| K3 | Session · turn to reset |
| K4 | Refresh · 4 Hz |
| HOME | Return to Progressions |
| GLO | Back to Tools |
| SEL | Enter highlighted item |

**State coverage:**
- K1 pages Overview/Timing/Memory/USB
- K2 Live/Hold; holding display does not stop collection
- K3 resets diagnostic session only; never musical data
- K4 2/4/8 Hz; default 4
- Host/emulator timing is labelled; never presented as device CPU
- Fixed linked RAM/POOL are not a dynamic heap meter

**Player instructions:**
1. Use K1 for the required diagnostic page.
2. Use K2 to hold the displayed snapshot, and K3 only to reset session statistics.

**Related:** SYS-TOOLS, SYS-USB

### SYS-DIAG-TIMING — Diagnostics · timing

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Retain useful measurements in Tools with their limitations clearly labelled.

**Entry:** SYS-TOOLS → Diagnostics
**Exit:** GLO → SYS-TOOLS; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Audio work / lag | Not measured |
| Deadline pressure | Not measured |
| Late / stale / shed | Not measured |
| UI service | Not measured |

| Input | Action |
| --- | --- |
| SELECT | No item to enter |
| ALGORITHM | No action |
| K1 | Page · timing |
| K2 | Display · Live |
| K3 | Session · turn to reset |
| K4 | Refresh · 4 Hz |
| HOME | Return to Progressions |
| GLO | Back to Tools |
| SEL | Enter highlighted item |

**State coverage:**
- K1 pages Overview/Timing/Memory/USB
- K2 Live/Hold; holding display does not stop collection
- K3 resets diagnostic session only; never musical data
- K4 2/4/8 Hz; default 4
- Host/emulator timing is labelled; never presented as device CPU
- Fixed linked RAM/POOL are not a dynamic heap meter

**Player instructions:**
1. Use K1 for the required diagnostic page.
2. Use K2 to hold the displayed snapshot, and K3 only to reset session statistics.

**Related:** SYS-TOOLS, SYS-USB

### SYS-DIAG-MEMORY — Diagnostics · memory

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Retain useful measurements in Tools with their limitations clearly labelled.

**Entry:** SYS-TOOLS → Diagnostics
**Exit:** GLO → SYS-TOOLS; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Ordinary RAM | Device linker values |
| POOL | Device linker values |
| XIP / RAM code | Device linker values |
| Stacks | Only instrumented stacks reported |

| Input | Action |
| --- | --- |
| SELECT | No item to enter |
| ALGORITHM | No action |
| K1 | Page · memory |
| K2 | Display · Live |
| K3 | Session · turn to reset |
| K4 | Refresh · 4 Hz |
| HOME | Return to Progressions |
| GLO | Back to Tools |
| SEL | Enter highlighted item |

**State coverage:**
- K1 pages Overview/Timing/Memory/USB
- K2 Live/Hold; holding display does not stop collection
- K3 resets diagnostic session only; never musical data
- K4 2/4/8 Hz; default 4
- Host/emulator timing is labelled; never presented as device CPU
- Fixed linked RAM/POOL are not a dynamic heap meter

**Player instructions:**
1. Use K1 for the required diagnostic page.
2. Use K2 to hold the displayed snapshot, and K3 only to reset session statistics.

**Related:** SYS-TOOLS, SYS-USB

### SYS-DIAG-USB — Diagnostics · USB

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Retain useful measurements in Tools with their limitations clearly labelled.

**Entry:** SYS-TOOLS → Diagnostics
**Exit:** GLO → SYS-TOOLS; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Capture state | Closed |
| Host rate | Not active |
| Under / over / missed | Session deltas |
| Resampler peak | Device measurement required |

| Input | Action |
| --- | --- |
| SELECT | No item to enter |
| ALGORITHM | No action |
| K1 | Page · USB |
| K2 | Display · Live |
| K3 | Session · turn to reset |
| K4 | Refresh · 4 Hz |
| HOME | Return to Progressions |
| GLO | Back to Tools |
| SEL | Enter highlighted item |

**State coverage:**
- K1 pages Overview/Timing/Memory/USB
- K2 Live/Hold; holding display does not stop collection
- K3 resets diagnostic session only; never musical data
- K4 2/4/8 Hz; default 4
- Host/emulator timing is labelled; never presented as device CPU
- Fixed linked RAM/POOL are not a dynamic heap meter

**Player instructions:**
1. Use K1 for the required diagnostic page.
2. Use K2 to hold the displayed snapshot, and K3 only to reset session statistics.

**Related:** SYS-TOOLS, SYS-USB

### SYS-BACKUP — Backup / restore

**Group:** System · **Delivery:** Play — first milestone · **Template:** text

Explain the connected computer route and preserve reliable backup/restore.

**Entry:** SYS-TOOLS → Backup/Restore
**Exit:** GLO → SYS-TOOLS; HOME → Progressions before transfer

| Visible field | Reference value/state |
| --- | --- |
| Backup | Connect USB MIDI to backup tool |
| Restore | Choose compatible backup on computer |
| Playback | Stop before restore |
| Status | Ready |

| Input | Action |
| --- | --- |
| SELECT | Choose Help / Transfer status |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Backup and restore are computer-driven; this screen does not pretend to store a file on-device
- Backup can flush pending edits; make device Busy/Ready status honest
- No computer connected: connection help, not a fake progress bar
- Host reports skipped unsupported objects

**Player instructions:**
1. Connect the USB data cable.
2. Use the documented backup/installer tool on the computer.
3. Save a backup before changing firmware or restoring content.

**Related:** SYS-RESTORE-CONFIRM, SYS-RESTORE-BUSY, SYS-RESTORE-DONE, SYS-LEGACY, SYS-BACKUP-STATUS

### SYS-RESTORE-CONFIRM — Restore incoming backup

**Group:** System · **Delivery:** Play — first milestone · **Template:** confirm

Show named incoming content and consequences before the computer tool commits changes.

**Entry:** Computer backup tool has a validated compatible restore plan
**Exit:** Computer confirms → transfer state; computer cancels → SYS-BACKUP

| Visible field | Reference value/state |
| --- | --- |
| Source | Backup selected on computer |
| Contents | Settings · sounds · patterns |
| Existing content | May be replaced |
| Confirm | Use the computer's Restore action |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| GLO | Cancel on computer first; leave only before transfer |
| HOME | Cancel before transfer only |

**State coverage:**
- Primary confirmation belongs to the computer flow, not a second conflicting on-device promise
- Busy playback/recording is rejected/retried; never silently erase through it
- Unsupported/corrupt backup reports skipped/error content

**Player instructions:**
1. Review the restore plan on the computer.
2. Stop playback/recording.
3. Confirm Restore on the computer.

**Related:** SYS-BACKUP, SYS-RESTORE-BUSY

### SYS-RESTORE-BUSY — Restoring

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Report an actual transfer and recoverable interruption without misleading progress.

**Entry:** Validated restore transfer begins
**Exit:** Success → SYS-RESTORE-DONE; abort/error → SYS-RESTORE-ERROR; previous valid copy retained per interrupted object

| Visible field | Reference value/state |
| --- | --- |
| Operation | Restoring backup |
| Status | Receiving / checking / writing |
| Progress | Only actual known object progress |
| Instruction | Keep the USB cable connected |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | No navigation through a commit |
| GLO | Abort only through the computer tool while supported |

**State coverage:**
- Cable loss / timeout / checksum failure / flash failure / Safe Mode restriction
- Do not show arbitrary percentages
- Settings restored: new RAM settings cannot overwrite restored flash before restart
- Interrupted object commits retain a valid previous copy

**Player instructions:**
1. Keep the connection intact until the computer reports completion.
2. If interrupted, reconnect and follow the tool's recovery result.

**Related:** SYS-RESTORE-DONE, SYS-BACKUP, SYS-RECOVERY, SYS-RESTORE-ERROR

### SYS-RESTORE-DONE — Restore complete · restart

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Make required restart explicit; do not imply restored settings are already active.

**Entry:** All requested compatible objects restored
**Exit:** Restart → SYS-BOOT

| Visible field | Reference value/state |
| --- | --- |
| Restore | Complete |
| Skipped content | Computer report lists any skipped items |
| Next | Restart to load restored data |
| State | Not yet active |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| SEL | Request safe restart when transfer is complete |
| HOME | Do not overwrite restored settings; follow restart route |

**State coverage:**
- Partial restore summary differs from full success
- Restart is scheduled only after the host received its final reply

**Player instructions:**
1. Review the computer's report.
2. Restart as instructed to load the restored content.

**Related:** SYS-BOOT, SYS-BACKUP

### SYS-LEGACY — Legacy content

**Group:** System · **Delivery:** Edit and save — second milestone · **Template:** picker

Retain an explicit route for imported sounds/takes/patterns without dominating new writing.

**Entry:** SYS-TOOLS → Legacy content
**Exit:** GLO → SYS-TOOLS; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Selected | Imported patterns / takes |
| Also | Old loop conversion · DX7 bank receive |
| Optional | Layer compatibility |

| Input | Action |
| --- | --- |
| SELECT | Choose imported content route |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Empty: No imported content; offer back/restore
- Original takes remain fixed pitches
- Conversion never claims exact semantic degrees unless known

**Player instructions:**
1. Restore a compatible backup before looking for imported content.
2. Select the named import/conversion route.

**Related:** SYS-LEGACY-PATTERNS, SYS-LEGACY-LOOP, SYS-LEGACY-DX7, SYS-LEGACY-LAYER

### SYS-LEGACY-PATTERNS — Imported patterns / takes

**Group:** System · **Delivery:** Edit and save — second milestone · **Template:** picker

Expose existing saved performances as fixed-note content and allow explicit conversion/copy.

**Entry:** SYS-LEGACY → Imported patterns/takes
**Exit:** GLO → SYS-LEGACY; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Selected | Pattern A |
| Type | Fixed-note recorded take |
| Key follow | As originally supported |
| Action | Play or Copy into draft |

| Input | Action |
| --- | --- |
| SELECT | Choose a retained pattern/take |
| ALGORITHM | Choose imported pattern/song grouping only where retained |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| PLAY | Play/stop selected imported content through one transport |
| EDIT | Explicit copy/conversion review |

**State coverage:**
- Empty / invalid older version / unsupported Layer content
- Existing Auto takes stay fixed-note after key changes
- Copy does not overwrite the factory idea or source take
- If a retired part cannot be reproduced, list the missing part before conversion

**Player instructions:**
1. Choose an imported take.
2. Press PLAY to hear it.
3. Use an explicit Copy action to develop a draft.

**Related:** SYS-LEGACY, SYS-LEGACY-LAYER

### SYS-LEGACY-LOOP — Old loop conversion

**Group:** System · **Delivery:** Edit and save — second milestone · **Template:** status

Explain the current old-loop-to-pattern conversion without fabricating a second live looper.

**Entry:** SYS-LEGACY → Old loop conversion
**Exit:** GLO → SYS-LEGACY; restart/load result shows named destination

| Visible field | Reference value/state |
| --- | --- |
| Source | Older saved loop |
| Destination | Available legacy pattern |
| Status | Ready to convert on restart |
| Original | Kept until conversion succeeds |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- No old loop found
- No free pattern: preserve old loop and report capacity
- Invalid checksum/version: preserve source and show backup/recovery route
- Success names the resulting pattern; no empty invented looper screen

**Player instructions:**
1. Back up first.
2. Read the named conversion result after loading/restarting.

**Related:** SYS-LEGACY, SYS-BACKUP

### SYS-LEGACY-DX7 — DX7 voice / bank receive

**Group:** System · **Delivery:** Edit and save — second milestone · **Template:** status

Retain the existing external FM6 SysEx route under Advanced/Legacy support.

**Entry:** SYS-LEGACY → DX7 receive
**Exit:** GLO → SYS-LEGACY; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord · FM6 |
| Receive | Ready via MIDI SysEx |
| Single voice | Marks target edited; Save sound keeps it |
| Bank | B1–B32; stop playback first |

| Input | Action |
| --- | --- |
| SELECT | Choose target only before receive |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |
| SAVE | Save received edited voice after a successful receive |

**State coverage:**
- Partial/corrupt/checksum failure produces no success
- Bank receive while playing: Busy; no flash erase through playback
- Safe Mode/flash failure reports restriction
- A complete received bank is not a new unlimited sound pool

**Player instructions:**
1. Stop playback for a bank dump.
2. Send a supported DX7 voice/bank from the computer.
3. Save an edited single voice explicitly.

**Related:** SOUND_SAVE, SYS-LEGACY, SYS-BACKUP

### SYS-LEGACY-LAYER — Layer compatibility

**Group:** System · **Delivery:** Edit and save — second milestone · **Template:** status

Make retained or retired independent Layer behavior explicit for imported content.

**Entry:** SYS-LEGACY → Layer compatibility
**Exit:** GLO → SYS-LEGACY; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Build | Compatibility build |
| Layer | Available for legacy content |
| New writing | Chord + Bass + Drums |
| Target | Legacy Layer sound |

| Input | Action |
| --- | --- |
| SELECT | Choose imported Layer sound while supported |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Open named legacy Layer voice detail |
| ENV | Open SND-LEGACY-LAYER-ENV only with retained, explicitly selected legacy Layer |

**State coverage:**
- Scene-/Layer-free successor: Not included; list affected content and conversion options
- Do not silently drop Layer notes or mislabel them as Bass
- No Layer MIDI/channel/editor entries appear active after retirement

**Player instructions:**
1. Check whether this build retains Layer before importing a four-track piece.

**Related:** SND-LEGACY-LAYER-VOICE, SYS-LEGACY-PATTERNS, SYS-BACKUP, SND-LEGACY-LAYER-ENV

### SND-LEGACY-LAYER-VOICE — Legacy Layer voice

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** params

Retain the current Layer-only Voice/Glide/Priority detail only where that part survives.

**Entry:** SYS-LEGACY-LAYER → voice detail
**Exit:** GLO → SYS-LEGACY-LAYER; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Target | Legacy Layer |
| Voice | Poly |
| Glide | Off |
| Priority | Last |

| Input | Action |
| --- | --- |
| SELECT | Other retained Layer sound pages |
| ALGORITHM | FM6/VA section tabs |
| K1 | Voice · Poly |
| K2 | Glide · Off |
| K3 | Priority · Last |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Voice Poly / Mono / Legato / Unison; Priority Last / Low / High
- Changing voice releases that part safely
- Unavailable in a build that retires Layer; never a blank editable screen

**Player instructions:**
1. Use this page only for a legacy Layer sound.
2. Choose voice and glide deliberately.

**Related:** SYS-LEGACY-LAYER, SND-ADVANCED

### SYS-RESET-ARM — Factory reset

**Group:** System · **Delivery:** Play — first milestone · **Template:** confirm

Preserve the current multi-step destructive reset guard.

**Entry:** SYS-TOOLS → Reset
**Exit:** K4 Reset → SYS-RESET-HOLD; cancel → SYS-TOOLS

| Visible field | Reference value/state |
| --- | --- |
| Erases | User sounds · patterns · settings |
| Keeps | Firmware · recovery · calibration |
| Backup | Back up first |
| Action | Keep |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | Action · Keep |
| SAVE | Only begins hold after Action is explicitly Reset |
| GLO | Cancel |
| HOME | Cancel; Progressions |

**State coverage:**
- Highlighting Reset is not permission to erase
- Safe/restore/busy conditions must be resolved before commit

**Player instructions:**
1. Back up first.
2. Turn K4 to Reset.
3. Read the hold instruction.

**Related:** SYS-RESET-HOLD, SYS-BACKUP

### SYS-RESET-HOLD — Hold SAVE to reset

**Group:** System · **Delivery:** Play — first milestone · **Template:** confirm

Make the second reset step visible and cancelable.

**Entry:** SYS-RESET-ARM K4 Reset
**Exit:** Complete hold → SYS-RESET-CONFIRM; early release/cancel → SYS-RESET-ARM

| Visible field | Reference value/state |
| --- | --- |
| Reset | Armed |
| Instruction | Hold SAVE for 3 seconds |
| Data | User sounds · patterns · settings |
| Cancel | Release early / GLO / HOME |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| SAVE | Hold 3 seconds |
| GLO | Cancel |
| HOME | Cancel |

**State coverage:**
- Countdown while holding
- Release early resets the hold; nothing erased

**Player instructions:**
1. Hold SAVE for the full displayed countdown.
2. Release/cancel if you do not intend to erase.

**Related:** SYS-RESET-CONFIRM, SYS-RESET-ARM

### SYS-RESET-CONFIRM — Reset everything?

**Group:** System · **Delivery:** Play — first milestone · **Template:** confirm

Retain the separate final confirmation after the hold.

**Entry:** Three-second reset hold completed
**Exit:** SAVE → SYS-RESET-BUSY; otherwise SYS-RESET-ARM

| Visible field | Reference value/state |
| --- | --- |
| Final check | Erases all user content |
| Confirm | Press SAVE again |
| Window | 6 seconds |
| Cancel | Any other button or timeout |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| SAVE | Confirm reset once within the window |
| HOME | Cancel |
| GLO | Cancel |

**State coverage:**
- 6-second timeout cancels
- Leaving page or unrelated button cancels
- No persisted erase occurs before this separate action

**Player instructions:**
1. Press SAVE again only to erase.
2. Wait or press another button to cancel.

**Related:** SYS-RESET-BUSY, SYS-RESET-ARM

### SYS-RESET-BUSY — Resetting

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Show the actual erase/restart operation after explicit confirmation.

**Entry:** Final reset confirmation
**Exit:** Restart → SYS-BOOT; failure → SYS-RECOVERY

| Visible field | Reference value/state |
| --- | --- |
| Status | Resetting user content |
| Next | Restarting |
| Instruction | Keep power connected |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |

**State coverage:**
- No musical edits/navigation during the erase
- Firmware/boot/calibration remain preserved

**Player instructions:**
1. Wait for the restart.

**Related:** SYS-BOOT, SYS-RECOVERY

### SYS-UPDATE — Update / recovery

**Group:** System · **Delivery:** Play — first milestone · **Template:** text

Provide a deliberate route to the existing installer and boot recovery.

**Entry:** SYS-TOOLS → Update
**Exit:** GLO → SYS-TOOLS; HOME → Progressions before update mode

| Visible field | Reference value/state |
| --- | --- |
| Before update | Back up user content |
| File | Choose the intended custom .fwsc |
| Action | Use documented installer |
| Recovery shortcut | OCT− + OCT+ + HOME, hold 5 s |

| Input | Action |
| --- | --- |
| SELECT | Choose Help / Enter update mode |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- Choosing a menu row does not flash firmware
- Display exact build/file identity in the computer tool
- Current mapped three-button gesture follows cf_shim.c; do not repeat outdated two-button-only in-app instructions

**Player instructions:**
1. Back up.
2. Select the custom firmware package in the installer.
3. Use the documented update-mode gesture only when instructed.

**Related:** SYS-UPDATE-COUNTDOWN, SYS-UPDATE-ACTIVE, SYS-BACKUP

### SYS-UPDATE-COUNTDOWN — Entering update mode

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Expose the current hold-to-enter countdown and early cancellation.

**Entry:** OCT− + OCT+ + HOME held
**Exit:** Release → prior screen; completed hold → SYS-UPDATE-ACTIVE

| Visible field | Reference value/state |
| --- | --- |
| Action | Entering update mode |
| Countdown | 3 |
| Cancel | Let go before completion |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Part of the held gesture, not a separate confirm |

**State coverage:**
- Do not let a stale hold begun on another screen fire later
- Countdown overlays all normal views

**Player instructions:**
1. Keep holding only to enter update mode.
2. Let go to cancel.

**Related:** SYS-UPDATE-ACTIVE, SYS-UPDATE

### SYS-UPDATE-ACTIVE — Update mode

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Distinguish boot/update mode from a running musical application.

**Entry:** Completed update hold / installer request
**Exit:** Installer-managed restart → SYS-BOOT; recovery follows boot tooling

| Visible field | Reference value/state |
| --- | --- |
| Mode | UPDATE MODE / USB BOOT |
| Next | Continue in the computer installer |
| Instruction | Keep the connection until completion |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |

**State coverage:**
- No HOME action is promised while the app is not running
- Loader/stub availability changes exact mode label
- Interrupted update uses existing recovery; no fictional in-app undo

**Player instructions:**
1. Follow the installer.
2. Wait for its completion and the instrument restart.

**Related:** SYS-BOOT, SYS-RECOVERY

### SYS-BOOT — Startup

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Boot quickly into the progression instrument and make exceptional load states visible.

**Entry:** Power-on / successful update / restore/reset restart
**Exit:** Success → Progressions; guarded failure → SYS-SAFE/SYS-RECOVERY

| Visible field | Reference value/state |
| --- | --- |
| Product | Chord-first FM-1 |
| Status | Loading saved musical state |
| Next | Progressions HOME |
| Playback | Stopped |

| Input | Action |
| --- | --- |
| SELECT | No menu during loading |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Skip optional decorative intro when app is ready |

**State coverage:**
- Normal / first boot defaults / recoverable missing settings / restore pending restart / Safe Mode
- Do not auto-start sound just because an idea was loaded
- Scene-free build uses immediate restrained startup; temporary exploration build can retain skippable old art
- An input that skips art must not be swallowed as a note/button event

**Player instructions:**
1. Wait for HOME.
2. Play or press PLAY deliberately.

**Related:** SYS-SAFE, SYS-RECOVERY

### SYS-SAFE — Safe Mode

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Retain a recoverable boot without loading/saving unsafe user state.

**Entry:** Boot guard / documented recovery entry
**Exit:** Recovery tooling / deliberate restart; normal boot only after resolution

| Visible field | Reference value/state |
| --- | --- |
| Mode | SAFE MODE |
| Content | User settings/sounds not loaded |
| USB | MIDI + console; audio unavailable |
| Next | Backup / repair / update on computer |

| Input | Action |
| --- | --- |
| SELECT | No normal content editing |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| GLO | Limited recovery information |
| HOME | Safe musical/default view only if supported |

**State coverage:**
- No saving or restore-to-normal claim inside safe state
- Default sounds may be available; saved content is not erased by entering Safe Mode
- Actual guard supports later ROM recovery escalation

**Player instructions:**
1. Read the Safe Mode notice.
2. Use the documented computer recovery route.
3. Do not assume missing sounds were deleted.

**Related:** SYS-RECOVERY, SYS-BACKUP, SYS-USB

### SYS-RECOVERY — Recovery help

**Group:** System · **Delivery:** Play — first milestone · **Template:** text

Document real cable/boot/flash failure recovery without inventing a normal-screen fix.

**Entry:** SYS-SAFE / persistent flash error / interrupted update
**Exit:** Back to service route; computer-managed recovery

| Visible field | Reference value/state |
| --- | --- |
| Check | Power · data cable · correct package |
| Backup | Preserve known-good user backup |
| Boot rescue | Use documented boot-stub/ROM route |
| Calibration | Only if controls are mapped wrong |

| Input | Action |
| --- | --- |
| SELECT | Choose relevant help item |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- No automatic erase/install is implied
- ROM/boot-stub gestures differ from the running app; use verified hardware guide
- Persistent storage failure is not a general low-memory popup

**Player instructions:**
1. Keep a known-good firmware and backup.
2. Follow the exact recovery guide for the current boot state.

**Related:** SYS-UPDATE, SYS-CALIBRATION, SYS-BACKUP

### SYS-CALIBRATION — Panel calibration

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Retain hardware control mapping as a service flow, separate from musical setup.

**Entry:** Documented OCT− + OCT+ power-on calibration route
**Exit:** Complete → normal startup; cancel/retry per existing calibration procedure

| Visible field | Reference value/state |
| --- | --- |
| Step | Press the named physical control |
| Prompt | HOME |
| Progress | Next control |
| Result | Saved calibration after completion |

| Input | Action |
| --- | --- |
| SELECT | Only act on the prompted control |
| ALGORITHM | Only act on the prompted control |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Act only when HOME is requested |

**State coverage:**
- No touchscreen/encoder-click input exists
- Repeated/missing control input gets a retry, not a falsely completed mapping
- Calibration storage is not erased by factory content reset

**Player instructions:**
1. Use only when physical controls are mapped incorrectly.
2. Press/turn exactly the control named by the calibration prompt.

**Related:** SYS-BOOT, SYS-RECOVERY

### SND-SAVING — Saving sound

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** status

Show the actual persistence operation without premature success or unsafe cancellation.

**Entry:** Core SOUND_SAVE or SOUND_OVERWRITE_CONFIRM executes a valid Sound-target write
**Exit:** Successful commit → SND-SAVED; failure → SOUND_WRITE_ERROR

| Visible field | Reference value/state |
| --- | --- |
| Saving | Warm Pad 2 |
| Target | Chord |
| Status | Writing / checking |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |

**State coverage:**
- Shown only during actual persistence
- No cancel/navigation promise during a flash commit
- A failed write retains the edit and previous valid stored record

**Player instructions:**
1. Wait for Saved or the reported error.

**Related:** SND-SAVED, SOUND_WRITE_ERROR

### SND-DELETED — User sound deleted

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** status

Report a successful deletion and its resolved references.

**Entry:** Core SOUND_DELETE_CONFIRM executes Delete and the write succeeds
**Exit:** Brief result → SOUND_PRESETS

| Visible field | Reference value/state |
| --- | --- |
| Deleted | Warm Pad 2 |
| References | Updated to stated fallback |
| Capacity | One user slot available |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Progressions |
| GLO | Sound |

**State coverage:**
- Shown only after a successful write
- Factory original returns where a user override had replaced it
- Affected saved-idea/legacy references follow the displayed policy; no dangling IDs

**Player instructions:**
1. Return to Sound or Progressions.

**Related:** SND-ROOT, SOUND_SAVE

### SYS-BACKUP-STATUS — Backing up to computer

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Report a real host-driven backup without implying an on-device backup file.

**Entry:** Computer tool starts LIST/GET backup
**Exit:** Computer reports file completion → SYS-BACKUP; failure/connection loss → explicit retry state

| Visible field | Reference value/state |
| --- | --- |
| Operation | Backing up to computer |
| Status | Reading current content |
| Progress | Actual object progress only |
| File | Saved by the computer tool |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| GLO | Abort/cancel through the computer tool |
| HOME | Leave only when the transfer tool allows |

**State coverage:**
- USB disconnected / transfer active / complete / interrupted
- Flush pending content only through the documented safe path; report Busy rather than causing an unannounced musical interruption
- No fictitious local USB-disk or SD-card destination

**Player instructions:**
1. Keep the cable connected.
2. Check that the computer tool saved the backup file.

**Related:** SYS-BACKUP, SYS-RESTORE-CONFIRM

### SYS-RESTORE-ERROR — Restore not completed

**Group:** System · **Delivery:** Play — first milestone · **Template:** status

Distinguish partial object restoration from a completed whole backup.

**Entry:** Busy / unsupported object / checksum failure / cable loss / flash failure
**Exit:** Computer-guided retry or restart/recovery; no silent overwrite of restored records

| Visible field | Reference value/state |
| --- | --- |
| Result | Restore not completed |
| Reason | Connection interrupted |
| Content | Computer lists completed/skipped objects |
| Next | Reconnect; review report before retry |

| Input | Action |
| --- | --- |
| SELECT | No action |
| ALGORITHM | No action |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| GLO | Return to backup/help after transfer has stopped |
| HOME | Return only when writes/restart lock are resolved |

**State coverage:**
- An interrupted object's prior valid copy remains; other already-completed objects may have changed
- Do not describe a multi-object restore as fully atomic
- Preserve restore locks until a deliberate restart/retry path resolves them
- Safe Mode refuses writes; no unqualified restore success

**Player instructions:**
1. Read the computer report.
2. Reconnect and retry the affected compatible objects, or follow the instructed restart/recovery route.

**Related:** SYS-BACKUP, SYS-RESTORE-BUSY, SYS-RECOVERY

### SND-ADVANCED-VA — Advanced sound · VA

**Group:** Sound · **Delivery:** Edit and save — second milestone · **Template:** picker

Group the retained engine's deep pages without taking note keys away for navigation.

**Entry:** SND-ROOT → Advanced with a VA target
**Exit:** GLO → SND-ROOT; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Target | Chord |
| Engine | VA |
| Section | OSC 1 |
| Page | Oscillator 1 |

| Input | Action |
| --- | --- |
| SELECT | Choose named VA section; SEL enters its page |
| ALGORITHM | Shortcut: choose OSC1–4 / Filter / Envelope / LFO / Mod |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| HOME | Return to Progressions |
| GLO | Back one level |
| SEL | Enter highlighted item |

**State coverage:**
- FM6 and VA retain their real current editor pages
- No deep editor: clear Not available for this engine
- Changing target/engine retargets safely and names it; no stale wrong-part edits
- Note keys remain performance keys in the successor design

**Player instructions:**
1. Open Sound → Advanced.
2. Choose the named section with SELECT then SEL; ALGORITHM is a section shortcut.
3. Inside a parameter page, SELECT chooses its page, ALGORITHM chooses section, and K1–K4 edit the visible values.

**Related:** SND-VA-OSC1, SND-VA-FILTER, SND-VA-ENV1, SND-VA-LFO1, SND-VA-MOD1, SOUND_SAVE

### SND-LEGACY-LAYER-ENV — Envelope · Legacy Layer

**Group:** Sound · **Delivery:** Play — first milestone · **Template:** params

Keep the independent Layer envelope only for imported content in a build that retains it.

**Entry:** SYS-LEGACY-LAYER → Layer envelope
**Exit:** GLO/ENV → SYS-LEGACY-LAYER; HOME → Progressions

| Visible field | Reference value/state |
| --- | --- |
| Target | Legacy Layer |
| Attack | 35 ms |
| Decay / Sustain | 220 ms / 80% |
| Release | 1.2 s |

| Input | Action |
| --- | --- |
| SELECT | No normal target carousel here; Layer is explicitly named |
| ALGORITHM | No action |
| K1 | Attack · 35 ms |
| K2 | Decay · 220 ms |
| K3 | Sustain · 80% |
| K4 | Release · 1.2 s |
| HOME | Return to Progressions |
| GLO | Back to Layer compatibility |
| ENV | Return to Layer compatibility |
| PRESETS | Change chord sound globally; use the explicitly labelled target selector for Bass/Layer |
| SAVE | Open musical collection; choose Save Sound explicitly |

**State coverage:**
- Only exists in a build retaining legacy Layer
- Separate from normal Chord/Bass/Linked ENV targets
- An absent retired Layer is never a blank editable target

**Player instructions:**
1. Open Legacy → Layer compatibility → Layer envelope.
2. Use K1–K4 for the explicitly named Layer sound.

**Related:** SYS-LEGACY-LAYER, SND-ADVANCED, SOUND_SAVE

### DISPLAY-ACCENT — Solid Accent

**Group:** Display · **Delivery:** Play — first milestone · **Template:** params

Preview one readable custom solid accent in the selected palette.

**Entry:** DISPLAY-STYLE → SELECT Accent → SEL
**Exit:** GLO → DISPLAY-STYLE; HOME → Progressions CORE

| Visible field | Reference value/state |
| --- | --- |
| Red | 119 |
| Green | 218 |
| Blue | 203 |
| Preview | #77dacb |

| Input | Action |
| --- | --- |
| SELECT | Choose Theme / Accent / Motion / Legacy scenes |
| ALGORITHM | No action |
| K1 | Red · 119 |
| K2 | Green · 218 |
| K3 | Blue · 203 |
| K4 | Restore · Palette accent |
| HOME | Return Progressions CORE |
| GLO | Return Appearance |
| SEL | Return Appearance; keep the previewed solid accent |

**State coverage:**
- RGB values are bounded to 0–255; Instrument Teal’s default accent is #77dacb.
- Restore returns the selected palette accent without changing the theme, music or Motion preference.
- Validate primary/secondary/focus contrast on the TFT, and persist versioned settings safely outside audio work.

**Player instructions:**
1. Use K1–K3 for red, green and blue.
2. Use K4 to restore the selected palette accent if desired.
3. Return HOME; the layout and music remain unchanged.

**Related:** DISPLAY-STYLE, DISPLAY-MOTION

### CTRL-BANKS — Live Control Banks

**Group:** Controls · **Delivery:** Play — first milestone · **Template:** picker

Explain/select four real control banks without sacrificing CORE, catalogue browsing or musical keys.

**Entry:** SYS-ROOT → Controls → Live control banks
**Exit:** SEL → chosen HOME bank; GLO → CTRL-ROOT; HOME → Progressions CORE

| Visible field | Reference value/state |
| --- | --- |
| Current bank | CORE 1/4 |
| Gesture | Tap HOME on Progressions |
| From elsewhere | HOME returns CORE |
| Validation | Physical gesture test required |

| Input | Action |
| --- | --- |
| SELECT | Choose a bank inside this detail page. SELECT on HOME always remains the complete catalogue. |
| ALGORITHM | No bank binding; retain normal workspace tabs. |
| K1 | — |
| K2 | — |
| K3 | — |
| K4 | — |
| SEL | Open the selected bank on HOME without changing values or transport. |
| GLO | Return Controls |
| HOME | Return Progressions CORE |

**State coverage:**
- Recommended mapping reuses the existing HOME-view gesture; verify physical scanning and timing before release.
- On Progressions HOME cycles CORE → SOUND → TEXTURE → GROOVE. From other screens or secondary scenes HOME returns Progressions CORE.
- No hold chord or encoder press. PRESETS remains global chord sound; ALGORITHM keeps scene/tab roles; OCT−/OCT+ stay musical.
- A bank switch alone changes no sound, value, ownership, capture or transport cursor. Only knob turns edit the labelled parameter.
- SOUND/TEXTURE/GROOVE use audited existing controls; Human/Fill and unsupported prototype macros are not operational parameters.

**Player instructions:**
1. On Progressions tap HOME until the bank name/count shows the desired four controls.
2. Read the bank and values before turning a knob.
3. From elsewhere HOME returns CORE, or choose a bank here with SELECT then SEL.

**Related:** HOME_STAGE, HOME_SOUND, HOME_TEXTURE, HOME_GROOVE, CTRL-ROOT

## Workflow contracts

### WF_PLAY — Find a progression and press PLAY

Hear a musical song immediately.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | HOME_STAGE | Turn SELECT through the complete wrapping list. | Seed key, mode and metre update without sound or an Apply step. |
| 2 | HOME_STAGE | Turn K1 to a compatible groove or Off. | Tempo remains yours. |
| 3 | HOME_STAGE | Press PLAY. | Slot 1 and groove downbeat start together; large chord, degree and ordered rail agree. |
| 4 | HOME_STAGE | Press PLAY again. | Accompaniment stops; independent live HOLD remains. |

### WF_LIVE_CONTROLS — Change the loop while hearing it

Change rhythm, key, mode and tempo from HOME.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | HOME_STAGE | Turn K1 during playback. | New groove appears as pending and begins next bar. |
| 2 | HOME_STAGE | Turn K2 or K3. | Latest complete key/mode request applies at next chord onset; active labels stay truthful. |
| 3 | HOME_STAGE | Turn K4. | Internal tempo changes promptly without resetting slot, bar or drum phase. |
| 4 | HOME_PENDING | Turn SELECT to another song. | Next shows its seed setup; the whole new song begins at the next bar. Browse back to active to cancel. |

### WF_MANUAL — Play your own chord over the groove

Use live roots/HOLD without fighting scheduled chord ownership.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | HOME_STAGE | While the loop runs, press a root; use upper modifiers deliberately. | Live chord and linked bass temporarily take harmony ownership; drums and actual scheduled cursor continue. |
| 2 | HOME_STAGE | Use HOLD, or release the roots. | HOLD sustains live takeover. After its final live root/latch releases, the currently scheduled chord resumes for the remaining slot time. |
| 3 | HOME_STAGE | Press PLAY to stop. | Only accompaniment ownership stops; a live held chord survives. |

### WF_DRUMS_ONLY — Use premade drums for manual playing

Start a groove without automatic chords.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | ACC_LOOP | Press SEQ and turn ALGORITHM to Loop; choose Drums only with K1. | Mode changes on the documented bar boundary. |
| 2 | ACC_GROOVE | Choose Groove tab and select the pattern. | Kit/swing defaults apply without replacing user tempo. |
| 3 | HOME_STAGE | Press HOME, then PLAY and play roots. | One groove clock supports immediate manual chord performance. |

### WF_EDIT — Change one chord and its timing

Edit a semantic slot without changing a saved original.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | EDIT_HARMONY | Press EDIT; read the explicit slot and select another with SELECT if needed. | Factory seed becomes a protected draft; dirty-draft conflicts use an explicit choice. |
| 2 | EDIT_HARMONY | Turn Degree, Quality, Color and Length. | Only this planned chord changes; global Harmony/modifiers stay intact. |
| 3 | EDIT_TIMING | Choose Timing with ALGORITHM; set Bars/Quarter notes, Length, Gate or Tie. | Units and articulation are explicit. |
| 4 | EDIT_VOICING | Choose Voicing; set inversion/register. Use Bass tab for the bass tone. | The resolved notes show the actual planned voicing. |
| 5 | EDIT_HARMONY | Press SEL to audition, or PLAY to hear the phrase. | Intentional audition/plan playback respects live ownership and safe plan boundaries. |

### WF_PHRASE — Extend a phrase and arrange A/B

Represent longer forms with bounded slots and repeats.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | EDIT_PHRASE | Choose Insert/Duplicate and position, then SEL. | Slot is inserted safely up to the 16-slot design limit. |
| 2 | EDIT_PHRASE | Choose metre explicitly, or choose B to make an explicit A copy. | Bar-based durations follow metre; quarter-note lengths remain fixed. |
| 3 | ACC_ARRANGE | Press SEQ, choose Arrange, select A/B and repeats, then Add after and SEL. | A short A/B chain is built with visible order. |
| 4 | ACC_ARRANGE | Press PLAY. | The explicitly selected arrangement plays on the shared clock. |

### WF_LEARN — Understand the chord you hear

See degree, notes, intervals and outside-scale tones.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | LAB_NOTES | Press SEL on HOME. | Now/Last target is explicit; actual chord quality drives the numeral and note spelling. |
| 2 | LAB_INTERVALS | Choose Intervals with ALGORITHM; use K2 to focus notes. | Chord intervals and membership are shown accurately. |
| 3 | LAB_NOTES | Turn SELECT to a numbered planned slot. | Inspector and EDIT focus that slot, separately from current sound. |

### WF_COMPARE — Hear a theory comparison

Compare one musical difference without rewriting the song.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | LAB_COMPARE | Choose Compare; select Triad/Color or Root/Smooth and side A/B. | Browsing remains silent. |
| 2 | LAB_COMPARE_PLAYING | Press SEL. | One-bar deliberate audition uses temporary harmony ownership; groove and clock continue. |
| 3 | LAB_COMPARE_PLAYING | Wait for completion, press SEL to stop, or play a live root. | Return to current scheduled chord for its remaining time; live input takes priority and cancels audition. |

### WF_NEXT — Build a path with offline next chords

Explore suggestions and commit only the chord you choose.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | NEXT_SHORTLIST | Open Lab → Next; inspect three candidates and the visible draft slot. | Offline, context-aware suggestions do not change sound or content. |
| 2 | NEXT_AUDITION | Press SEL to hear a candidate. | Temporary one-bar audition does not alter committed history. |
| 3 | NEXT_INSERTED | Press EDIT to insert into the visible slot. | Candidate replaces that slot with one-step undo and explicit local quality. |
| 4 | EDIT_HARMONY | Continue editing or use SAVE → Undo. | No global Harmony or modifier change is hidden in insertion. |

### WF_MOTION — Turn a held chord into an arp or strum

Change delivery while retaining harmony and groove.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | MOTION_ARP | Press ARP; enable and choose one of the nine existing modes. | Chord notes use arp/strum delivery on the shared tempo. |
| 2 | MOTION_FEEL | Choose Feel; set Gate/Swing and chord-sound Glide. | Expressive controls stay available after Tempo moves to HOME. |
| 3 | HOME_STAGE | Return HOME and play or start the progression. | Large harmonic identity remains stable while note activity can move. |

### WF_BASS_ENVELOPE — Shape bass and envelope

Keep useful sound controls direct.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | BASS_SOUND | Press LFO; browse Bass with SELECT and set level/register. | Bass has an explicit target; PRESETS stays chord sound. |
| 2 | ENV_ENVELOPE | Press ENV and select Chord/Bass/Linked. | Attack, Decay, Sustain and Release apply to the named target. |
| 3 | HOME_STAGE | Return HOME. | Musical changes remain current and are included in future complete-song saving. |

### WF_SOUND — Find, favorite and save a sound

Keep patch management distinct from the primary song workflow.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SOUND_PRESETS | Open GLO → Sound → Presets; choose target and bank, then SELECT. | Sound loads with explicit target and safe musical-boundary policy. |
| 2 | SOUND_PRESETS | Use K3 to favorite the selected sound. | Favorite reference uses its stable sound ID. |
| 3 | SOUND_ACTIONS | Press SEL on Sound Presets, choose Save As New, then SEL. | The explicit sound-only naming route creates a user patch; replacing requires its named confirmation. |
| 4 | IDEA_LIBRARY | Return HOME, press SAVE to open Songs, then SAVE again for current complete-Song saving. | Patch-only persistence is not confused with saving the complete setup. |

### WF_FX — Shape and retain the effect chain

Use only the supported six effects with clear controls.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | FX_REVERB | Press FX and SELECT Reverb. | K1 is On, K2 Type, K3 Decay, K4 Mix. |
| 2 | FX_ORDER | ALGORITHM to Order; SELECT an effect and K1 to move it. | Routing changes explicitly; ALGORITHM remains tab navigation. |
| 3 | HOME_STAGE | Return HOME. | Current chain continues; phase-2 complete-song save retains it. |

### WF_FX_PRESET — Recall or save an effect preset

Swap effects without changing the harmony or groove.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | FX_PRESETS | Choose Presets tab; browse and press SEL to recall. | Only supported effect chain parameters/order/bypass change; Undo is available. |
| 2 | FX_SAVE | Choose Save As New, enter a name and SAVE. | Named effects-only record is saved; the whole song still uses its normal save route. |

### WF_EXTERNAL_CLOCK — Use an external clock deliberately

Keep Receive waiting, loss and resume understandable.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | HOME_STAGE | Choose Receive in System, then return HOME. | Without pulses, Waiting/armed and EXT are shown; local tempo editing is unavailable. |
| 2 | HOME_STAGE | Send external Start. | Slot 1 starts. External Continue instead resumes retained position. |
| 3 | HOME_STAGE | If the clock disappears, read Clock lost. | Only accompaniment stops/releases; cursor and independent live HOLD remain. Pulses alone never restart. |
| 4 | HOME_STAGE | Send an explicit Start/Continue, or choose Internal in System and press local PLAY. | Playback resumes only deliberately. |

### WF_RECORD_FIRST — Record what is playing directly from Progressions

Make and automatically save a new recorded Song without leaving HOME or reviewing a take.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | HOME_STAGE | Choose a progression/groove and press REC while stopped. | HOME shows one-bar count-in; REC cancels it without a Song row. |
| 2 | REC_COUNT_IN | Wait one full bar. | Selected automatic progression and groove begin with capture; no chord suppression. |
| 3 | REC_RECORDING | Change key/tempo/groove or play manual overrides as desired. | The fixed performed notes and timed musical control changes are captured. |
| 4 | REC_FINISHING | Press REC again. | Finishing appears; at next bar accompaniment stops, independent live HOLD remains. |
| 5 | SONG_SAVED | Wait for safe validated save. | A new source-based numbered Recording Song appears as Saved; source original is untouched. |
| 6 | IDEA_LIBRARY | Press SAVE. | Songs opens with the new recording selected; no hidden recordings list or required Keep. |

### WF_RECORD_RUNNING — Capture a running performance

Arm at the next bar without restarting the existing phrase.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | HOME_STAGE | During playback, press REC. | HOME shows Armed; the current progression/groove keep playing. |
| 2 | REC_ARMED | Reach the next downbeat. | Capture begins from the actual phrase position and currently audible state. |
| 3 | REC_RECORDING | Press PLAY for a short end. | Close at actual elapsed time, stop accompaniment promptly, and auto-create a new Song copy without padded silence. |
| 4 | IDEA_LIBRARY | Press SAVE after completion. | See duration, Recording type and true Pending/Saving/Saved status in Songs. |

### WF_RECORD_MANUAL — Record your own chords over drums

Use the same capture method with an intentional accompaniment choice.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | ACC_LOOP | Press SEQ, choose Loop and set Drums only. | Automatic chords are off by your explicit choice, not by REC. |
| 2 | HOME_STAGE | Press HOME and REC. | Normal stopped count-in or running next-bar arm begins. |
| 3 | REC_RECORDING | Play roots/modifiers over the groove; finish with REC or PLAY. | A new Recording Song captures the heard manual performance and accompaniment. |

### WF_RECORD_PENDING — Finish saving a Pending capture

Keep captured content visible if sound/storage/capacity prevents persistence.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SONG_PENDING | Read Pending and its reason after the recording ends. | No false Saved label; the protected capture is in Songs. |
| 2 | IDEA_LIBRARY | Press SAVE to open the selected Pending row. | Type and duration remain visible alongside saved Songs. |
| 3 | IDEA_MENU | Press EDIT to Manage; remove chosen Saved content if storage is full. | Pending and protected current/draft content are excluded from bulk clear. |
| 4 | SONG_PENDING | Stop accompaniment and release live-held notes when the prompt asks. | Existing quiet/storage guards permit the automatic safe write; audible tails are not cut. |
| 5 | SONG_SAVED | Wait for validation. | Pending becomes Saved; another REC may create the next new copy. |

### WF_SONGS_SAVE — Save an editable progression Song

Keep current-save distinct from highlighted-row management.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | IDEA_LIBRARY | Press SAVE from HOME to open Songs, then SAVE to save current. | A new progression opens a prefilled name; the highlighted row is not an overwrite target. |
| 2 | IDEA_NAME | Keep the suggested name or edit it; press SAVE. | A new Progression Song saves directly when safe, with no extra free-slot review. |
| 3 | IDEA_SAVE_OPTIONS | For an edited saved current, choose Save As New or explicit Update. | Original remains unchanged unless named Update is confirmed. |
| 4 | IDEA_OVERWRITE_CONFIRM | If Update was chosen, check the target/count and press SAVE on Update. | Only the named existing Song is updated. |

### WF_SONGS_LOAD — Find and load either Song type

Make row browsing safe and playback source explicit.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | IDEA_LIBRARY | Press SAVE; SELECT highlights a row. | Rows show Progression/Recording type, duration and status; highlighting alone changes no music. |
| 2 | IDEA_LIBRARY | Press SEL on the intended row. | Load applies stopped or queues the complete Saved setup at next bar. |
| 3 | HOME_STAGE | Press HOME and PLAY. | Progression plays semantic chords; Recording plays its fixed note/control/tempo timeline. |
| 4 | IDEA_DETAILS | For a Recording, Manage → Details → EDIT develops its start. | A new semantic Progression draft is editable without rewriting the recorded performance. |

### WF_SONGS_MANAGE — Rename, duplicate or delete one Song

Manage the highlighted row with direct physical controls.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | IDEA_LIBRARY | SELECT the row and press EDIT. | Manage keeps the selected Song name/type visible. |
| 2 | IDEA_MENU | Choose Rename, Duplicate or Delete, then SAVE/SEL. | Rename uses text entry; Duplicate creates a new numbered ID; Delete opens one named counted confirmation. |
| 3 | IDEA_DELETE_CONFIRM | Choose Delete and press SAVE; HOME cancels. | Only one Saved row is removed once safe; loaded audio, protected draft and Pending remain. |
| 4 | IDEA_LIBRARY | Return to Songs. | The exact result is visible; there is no deep take browser. |

### WF_RECORD_TO_CLEAR — Record → Songs → delete or clear recordings

Make capture and cleanup one obvious route.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | REC_RECORDING | REC finishes at next bar, or PLAY ends now. | Accompaniment stops; a new Recording Song auto-saves when safe. |
| 2 | IDEA_LIBRARY | Press SAVE to see it; SELECT the desired row, then EDIT. | Manage acts on the visible highlighted Song. |
| 3 | IDEA_DELETE_CONFIRM | For one item choose Delete, then confirm count 1 with SAVE. | Only that Saved Song is deleted. |
| 4 | SONGS_CLEAR_RECORDINGS_CONFIRM | For a batch choose Clear Recordings in Manage; read the saved count and confirm with SAVE. | One explicit confirmation clears Saved Recording rows only; Pending/draft/current audible copy remain. |
| 5 | SONGS_CLEAR_ALL_CONFIRM | Clear All Songs is the separate all-types option with its own visible count. | HOME cancels either counted confirmation; no silent library-wide deletion. |

### WF_DRAFT — Return to a draft, undo or recover

Keep protected musical edits outside library clear scope.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | IDEA_LIBRARY | Open Songs and press EDIT for Manage. | Return to Draft and Undo remain visible even if the Saved list is empty. |
| 2 | IDEA_DRAFT_RETURN | Choose Return to Draft and execute Return. | Draft becomes current/queued without an Apply step on normal factory browsing. |
| 3 | IDEA_UNDO | Manage → Undo restores one musical gesture. | Saved-row deletion is not disguised as musical undo. |
| 4 | IDEA_RECOVERY | After restart Recover the last valid stopped checkpoint. | Opens stopped; a never-written Pending recording is not falsely claimed recoverable. |

### WF-LIVE-BANKS — Use the four live control banks

Reach existing controls without sacrificing CORE, full catalogue browsing or musical keys.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | HOME_STAGE | Press HOME from another workspace | Progressions opens CORE: Rhythm / Key / Scale / Tempo |
| 2 | HOME_SOUND | Tap HOME while already on Progressions | SOUND: Chord sound / Bass sound / Chord level / Bass level; bank selection changes no values |
| 3 | HOME_TEXTURE | Tap HOME again | TEXTURE: Reverb mix / Delay mix / Chorus mix / Glide |
| 4 | HOME_GROOVE | Tap HOME again | GROOVE: Kit / Drum level / Swing / Click |
| 5 | HOME_STAGE | Tap HOME again | CORE returns. SELECT still browses all songs; PRESETS stays chord sound; OCT keys remain musical. |

### WF_CHORD_LIBRARY — Browse, audition and replace a chord

Choose a named chord from the reference without losing the numbered edit destination or altering global Harmony.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | EDIT_HARMONY | Open EDIT and SELECT the destination slot | A numbered draft slot is focused; the source Song remains untouched. |
| 2 | CHORD_LIBRARY | Turn ALGORITHM to Library and SELECT a candidate | The reference list browses silently; target slot, degree, notes and variant are visible. |
| 3 | CHORD_LIBRARY | Press SEL | A deliberate bounded audition uses the shared note-owner rules; the slot is unchanged. |
| 4 | EDIT_HARMONY | Press EDIT Replace | The named slot receives the candidate, one-step undo is available and any running plan change follows its musical boundary. |

### SYS-WF-HARMONY — Enable simple Harmony

Play full major/minor chords with deliberate modifiers.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SYS-ROOT | Choose Harmony; press SEL | One On/Off control |
| 2 | SYS-HARMONY | Turn K1 On | Auto active; show a direct support message if quantize/scale is unsupported |
| 3 | CTRL-MODIFIERS | Choose a slot; assign modifier/inversion/bass | Assignments change deliberately; HOLD remains separate |

### SYS-WF-DISPLAY — Set accent and motion

Use Instrument Teal or optional Studio/Editorial palettes, with adaptive Motion On or totally Off.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | DISPLAY-STYLE | Choose Instrument Teal, Studio or Editorial | The same type-led layout uses the selected flat palette |
| 2 | DISPLAY-ACCENT | Preview a solid accent or restore its palette default | Chord/degree and focus/status stay readable |
| 3 | DISPLAY-MOTION | Choose On or Off | On adapts to smoothed output/volume and active parts; Off stops all motion/transitions while values update instantly |

### SYS-WF-EXTERNAL — Use external MIDI and clock

Route incoming notes and synchronize without local tempo ambiguity.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SYS-MIDI | Choose Roots and input channel; Clock Receive | Owned incoming notes are safely released when interpretation changes |
| 2 | SYS-CLOCK-WAIT | Start clock on the master device | EXT lock and externally controlled tempo |
| 3 | SYS-CLOCK-LOST | Resolve lost source or explicitly choose Internal | Accompaniment stops and retains position; live HOLD survives. Only explicit Start/Continue or choosing Internal allows a deliberate resume. |

### SYS-WF-CAPTURE — Record USB audio on a computer

Capture the whole mix or deliberate dry stems.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SYS-USB | Connect data cable; select input/rate in the recording app | Host-selected capture state/rate displayed |
| 2 | SYS-DIAG-USB | Inspect errors only if recording has problems | Session deltas and measurement limits remain honest |

### SND-WF-ENVELOPE — Shape chord and bass envelopes

Edit musical Attack/Decay/Sustain/Release with an explicit target.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | ENV_ENVELOPE | Press ENV; SELECT chooses Linked/Chord/Bass | One canonical envelope target and four visible ADSR controls |
| 2 | ENV_ENVELOPE | Edit the named target | Linked affects both; legacy Layer requires its own explicit route |

### SND-WF-ADVANCED — Edit a retained FM6 or VA sound

Reach every retained deep page without making it the main workflow.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SND-ROOT | Choose Advanced sound for named target | Target explicit; PRESETS stays chord sound |
| 2 | SND-ADVANCED | Choose section and parameter page | Four bounded labelled parameters |
| 3 | SND-FM6-OP1-RATE | Edit named parameter | Only named target/parameter changes |
| 4 | SOUND_ACTIONS | Choose Save As New or Replace | Canonical sound-only naming/review route |
| 5 | SOUND_SAVE | SAVE commits New or reviews Replace | Sound-scoped writing/capacity/error outcome |

### SND-WF-FULL — Recover from full sound storage

Keep edits while choosing a valid destination.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SOUND_STORAGE_FULL | Choose Sound actions or cancel | Edits retained; no idea/Song deleted for sound capacity |
| 2 | SOUND_ACTIONS | Choose named user replacement/deletion | Exact Sound target reviewable |
| 3 | SOUND_SAVE | SAVE executes visible New/Replace/Cancel | Named Replace reviewed before a write |

### SYS-WF-BACKUP — Back up and restore safely

Preserve content across the successor firmware.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SYS-BACKUP | Create a backup with the computer tool | Computer retains the file |
| 2 | SYS-RESTORE-CONFIRM | Review content and confirm on computer while stopped | Validated transfer begins |
| 3 | SYS-RESTORE-BUSY | Wait for actual transfer/check/write result | Failure remains recoverable; no fabricated progress |
| 4 | SYS-RESTORE-DONE | Restart after the final reply | Restored data becomes active on startup |

### SYS-WF-RESET — Reset user content deliberately

Erase only after the preserved multi-step guard.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SYS-RESET-ARM | Back up; turn K4 to Reset | Armed, nothing erased |
| 2 | SYS-RESET-HOLD | Hold SAVE for 3 seconds | Separate final question |
| 3 | SYS-RESET-CONFIRM | Press SAVE again within 6 seconds | Erase and restart; other input/timeout cancels |

### SYS-WF-UPDATE — Update or recover firmware

Use the actual compatible installer/recovery paths.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SYS-UPDATE | Back up and choose the intended custom package | Exact source build is known |
| 2 | SYS-UPDATE-COUNTDOWN | Hold the documented running-app gesture or release to cancel | Explicit entry to update mode |
| 3 | SYS-UPDATE-ACTIVE | Complete the computer installer | Restart to startup; no in-app HOME promise in the loader |
| 4 | SYS-BOOT | Verify the running build after restart | Normal HOME or an explicit Safe/Recovery state |

### SYS-WF-LEGACY — Find retained older recordings and sounds

Keep compatibility explicit while the new instrument replaces workstation UI.

| Step | Screen | Physical action | Result |
| --- | --- | --- | --- |
| 1 | SYS-LEGACY | Choose imported content | Named older takes/patterns/loops/routes |
| 2 | SYS-LEGACY-PATTERNS | Play or explicitly copy a take | Fixed-note recording stays fixed and original content is preserved |
| 3 | SYS-LEGACY-LAYER | Check optional part availability | Any unsupported Layer content is disclosed rather than silently dropped |

## Verification before engineering release

- Validate every entry/exit and physical control against the board. Keep main product text free of internal screen IDs and implementation trivia.
- Compare actual sounding notes, degree/quality, planned slot, live input, audition and pending changes independently.
- Exercise empty/full/dirty/saving/error, count-in/record/review, external-clock, no-compatible-groove and recovery routes covered by the screen definitions.
- Verify glyphs and transitions on the native 240×240 FM-1 display, with Motion Off as a stable equivalent.
- Run note release/HOLD, MIDI, USB audio, safe save/restore and boot/update regression checks appropriate to each implemented phase.
- Record exact revision/options and paired ordinary-RAM/POOL/XIP maps; measure audio work/lag, display/input response and USB reliability on hardware.

Canonical sources: `core-screens.json` and `system-screens.json`. Generated outputs: `board-data.json`, `docs/CHORD-FIRST-USER-GUIDE.md`, this handoff and the board's embedded data. Regenerate with `python3 tools/build_chord_first_design.py`.
