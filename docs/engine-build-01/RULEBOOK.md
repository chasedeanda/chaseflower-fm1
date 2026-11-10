# CORD Engine — Musical Rulebook v1.0
**Product specification · 10 November 2026 · Research/design owner: ChatGPT + CORD project owner · Implementation owner: Codex**

**Sources / integration baseline:** [`chasedeanda/cord-fm1`](https://github.com/chasedeanda/cord-fm1), default-branch tree `519f7aa2747f6eb1447210974d85e9066342af2d`. The eight Signature patches are being implemented separately and are **not assumed to be on this branch yet**. The companion [`CORD_ENGINE_Scenes_v1.json`](CORD_ENGINE_Scenes_v1.json) is the authoritative scene-data appendix; it was independently validated for 16 bars, note range and grid format.

## 1. Product definition

**CORD Engine is a continuously looping, 16-bar original backing-track instrument that can be shaped in real time.** It creates a playable harmonic bed, links bass and a compatible drum groove, moves through a repeatable musical arc, and lets the musician intervene without losing the beat. It is not a linear song player, shuffle button, fixed backing-track audio recording, or unlimited randomizer.

**Single most important invariant:** after bar 16, the engine proceeds immediately to bar 1 without stopping the transport. The underlying harmony and arrangement design stay recognizable. At **Variation = 0**, repeating cycles are musically identical, aside from a natural decay/reverb tail crossing the boundary. Other Variation values add bounded changes to articulation/drums, **not arbitrary replacement chords**. A deliberate Regenerate command creates a related new scene state.

**User's action loop:** choose scene → PLAY → hear the 16-bar arrangement → turn Energy / Movement / Variation → optional Fill / Drop / Back → Regenerate selectively → lock anything worth keeping → play over it indefinitely. CORD always responds to live harmony input; it never forces the performer to wait for the arrangement before taking over.

### Explicit non-goals for v1

- No microphone/audio listening or automatic generative music model.
- No copied song arrangement, MIDI transcription, identifiable melody, sampling, or artist endorsement; references are research prompts, **all included progressions and drum grids are original**.
- No new audio DSP engine, effect algorithm, granular/reverse buffer, sample importer, or extra audio track in the first pilot.
- No claims of independently interlocking two-part marimba counterpoint or separately sequenced synth leads until a dedicated additional layer track is implemented.
- No automatic tempo/key/meter jumps on a repeated cycle.
- No writes to flash, allocations, harmony theory computation or randomization in the audio ISR.

## 2. What the existing repo really supports

| Verified current component | Implication for CORD Engine |
|---|---|
| `firmware/src/cf_progression.h`: `CF_P_MAX = 16` | One Engine loop can use **eight two-bar chord slots** (within the 16-slot capacity), instead of a new general sequencer. |
| `firmware/src/cf_accomp.c`: tick-based repeating slots at `SQ_PPQN=96`; 4/4 bar = 384 ticks; latest-value setup/edit mailboxes | Keep the current tick clock and safe bar/chord-boundary handoffs. Add a small Engine controller above it. |
| `firmware/src/cf_accomp_ui.c`: authored plan includes exact note masks, note arrays (up to six), gate/tie, bass and groove | Use explicit source masks, fixed MIDI note arrays and an Engine scene type, not loose scale-degree guesses. |
| `firmware/src/cf_drum.c`: stable groove IDs, 2-bar groove loops with per-lane hit grids; existing `DreamPop` **ID 56** and `NeoSoul` **ID 61** | Engine chooses current groove **by stable ID, never by array index**. Groove phase follows the same transport. |
| `firmware/src/cf_perform.c`: bar-queued Fill, Drop and Back | Reuse the existing performance effect. A manual action always takes priority over any scheduled Engine action. |
| `firmware/src/cf_variation.c`: seeded, main-loop-only variation calculations | Seed the Engine's *own* deterministic PRNG and avoid modifying another subsystem's random stream or rewiring global Variations. |
| `docs/FX-PRESETS.md`: 13 named effect recipes; smooth recall | Change existing recipe or conservative macro mix; do not invent a new DSP effect. |
| `docs/SONGS-LIBRARY.md`: existing 16-slot semantic phrase and six saved idea objects | A simple **Save Snapshot** with eight two-bar chord slots can reuse the current Song pathway, but must be labeled as a static snapshot rather than a saved evolving Engine rule-set. |

**Critical design limit:** the current basic accompaniment schedules **chord sound, linked bass and drums**. It does **not** independently generate a second melodic instrument. A scene's existing arpeggiator can animate the chord part, but cannot simultaneously provide a sustained chord bed *and* a separately generated synth lead without a later multi-part feature. This is why Engine Build 01 delivers the arrangement loop and one scene before any advanced layer expansion.

## 3. The immutable 16-bar form

Every v1 scene is in **4/4**, 16 bars long. The chord spine is four original chords; each lasts two bars, the 8-bar phrase repeats twice. Implement the pilot as **eight chord slots × 768 ticks**, not 16 slots × 384 ticks: bars are still counted separately as 16 and no second-bar chord reattack is required. Stage behavior changes between the two phrases, which makes the **16-bar experience** the primary unit rather than just an eight-bar chord loop.

| Bars | Stage | Required behavior | Important guard |
|---|---|---|---|
| **1–4** | **INTRO / Space** | Establish scene timbre and two chords, leave room for live playing. No surprise fills. | Do not start with a huge drum arrangement in ambient/dream-pop scenes. |
| **5–8** | **GROOVE / Foundation** | Bring in linked bass and/or drums according to scene profile; keep existing tempo. | Changes occur at first sample of bar 5, not partway through bar 4. |
| **9–12** | **LIFT / Energy** | Fullest version of the scene. If Movement is high and the scene allows it, chord arp can replace held chord articulation. | Never exceed voice/CPU budgets. |
| **13–16** | **RETURN / Breathing room** | Reduce rhythmic density, make bars 15–16 a turnaround or open space. | Bar 16 → 1 must not glitch, retrigger transport, or create a phantom extra bar. |

At **default Energy**, use the scene-specific stage gains in `phase_profiles` from the JSON (percent *relative to each part's user mix*). The pilot Heavenly Gaze values are **INTRO 82/0/0**, **GROOVE 90/66/68**, **LIFT 95/85/88**, **RETURN bars 13–14 90/65/55, bars 15–16 80/0/0** in **chord/bass/drums** order. These are arrangement gain multipliers, not absolute hardware output levels; they must not overwrite saved sound or player mix settings.

For Energy dial `E` with scene default `E0`, apply `chord_mult = min(1, (stageChord/100) × (0.6 + 0.4 × E/E0))` and `bass/drum_mult = min(1, (stageGain/100) × E/E0)`. Clamp to legal range. Therefore Energy 0 still retains a quiet chord bed, but removes bass/drums; initial default dial values produce the exact authored stage profile. Smooth gain targets over 50 ms or the existing appropriate safe fading mechanism. Never change BPM, key, chord identity or FX algorithm as a side effect of Energy.

**Bar identity:** internal `bar_index=0..15`; `cycle_count` increments at 15→0; for 4/4 a bar is `4×96 =384` quarter-note ticks. Restart PLAY from bar 1. On an external MIDI clock, follow the existing clock implementation: never start a separate timer or make up time after clock loss.

## 4. Four live Engine dials and existing performance dials

The new **HOME → Engine** page is separate from the existing **HOME → Core** (Rhythm, Key, Scale, Tempo) and **HOME → Perform** (Fill, Drop, Back, Tempo). Do not repurpose or remove either existing bank.

| Engine dial | Exact control contract | When it takes effect |
|---|---|---|
| **K1 ENERGY, 0–100** | Stage-level scaling per formula above. Default from scene. Never randomizes harmony or tempo. | Gain ramps begin at next audio control tick and smooth; orchestration enable/disable is quantized to next bar. |
| **K2 MOVEMENT, 0–100** | Sets *musical articulation/activity*; **not** the existing mascot visual **Motion On/Off**. See policy map below. | Next chord onset/bar as required by policy; preserve note ownership. |
| **K3 VARIATION, 0–100** | Controls bounded embellishments **between** repeating cycles; not chaos and not automatic key changes. | Changes apply at the next bar; variation choices for a bar are decided once before its start. |
| **K4 REGENERATE / UNDO** | One clockwise detent requests one new related combination from the authored allowed list. One counter-clockwise detent restores the previous combination. Does not alter the current scene ID, BPM or 16-bar phase. | Sound/FX/groove at next bar; new harmonic variant at next **two-bar chord boundary**. UI shows Pending until committed. |

**GLO → Engine Locks:** K1 Harmony, K2 Sound (chord + linked bass), K3 Groove (groove + kit + swing), K4 FX. Turning either direction toggles a lock exactly once per detent. All initially off. Locks govern **Regenerate only**, not stage mix shaping, clock/transport or deliberate new scene load. A direct user edit in the corresponding existing screen implicitly engages that category's lock for the current scene; show the lock indicator. Unlock manually to include it in future regeneration. A scene change resets all four locks after the new scene commits.

**Scene navigation:** while HOME → Engine, SELECT browses named scenes without changing the current playback; SEL requests the highlighted scene. When stopped, load immediately; when playing, schedule at the **next bar-1 boundary** (no partial 16-bar fragments), show Next/Queued, and allow another scene choice to replace the pending one. PLAY retains the existing transport toggle semantics. HOME continues cycling pages. PRESETS from other pages remains the existing sound selector.

**Regenerate must not create 157-sound fatigue in a new disguise.** All candidate lists are curated and finite. Exclude the currently selected value when more than one alternative exists. If every category is locked, show **Everything locked**, do not advance the seed. Maintain a one-step Undo snapshot that includes the four selected categories but **not** manual Fill/Drop/Back state.

### Movement mapping by authored scene policy

| Policy | 0–34 | 35–64 | 65–100 |
|---|---|---|---|
| `held` (Heavenly Gaze) | Sustained chords; no new note attacks | Sustained with existing FX movement only | During bars 9–12 only: arpeggiated chord at 1/8-note rate, returning to held at bar 13. Never layer two independent chord voices. |
| `intimate` / `ambient` | Long notes | Long notes with subtle existing FX modulation | No automatic arp: preserve piano/ambient identity. |
| `syncopated` | One chord onset/bar | Three onset grid indices `[0,6,10]` per bar | Same indices; tighten gate from 55% to 38% of each onset interval. |
| `stab` | `[0,8]` sixteenth-step indices per bar | `[0,3,7,10]` | `[0,2,6,10,14]` |
| `offbeat` | `[2,10]` | `[2,6,10,14]` | Same four offbeat attacks, stronger but bounded velocity accents. |
| `arp_eighth` | Quarter-note chord arp | Eighth-note chord arp | Eighth-note arp with simple gate variation; **not a second marimba player**. |
| `quarter_stabs` | `[0,8]` | `[0,4,8,12]` | `[0,4,6,8,12,14]` |

For non-held attacks: step grid is 16 equal sixteenth subdivisions of a 4/4 bar, velocity default 95, gate length = 45% of spacing to next attack unless otherwise specified. Each scene's `chord_articulation` selects **exactly one** row above. These more complex policies are **future scene implementation**, not part of Build 01; the pilot only needs `held` with the single existing arpeggiator handoff. Do not silently claim the engine already supports intra-bar retriggers.

### Variation mapping

- **0–24**: the **same** 16 bars repeat, exactly (except normal non-looped effect tails). No embellishments.
- **25–49**: on odd-numbered repeat cycles only, remove or add **one quiet closed-hat event** in bar 7 **if drums are active**, chosen from the existing closed-hat lane and never doubling a crash. No changes to chord roots, drum groove ID or bass notes.
- **50–74**: above plus an optional one-bar **Fill at bar 12** if Energy ≥50 and not manually dropped. Selection is deterministic from scene seed + cycle_count; never double-schedule an existing manual fill.
- **75–100**: above plus optional bar-8 fill at Energy ≥70 and one extra restrained hi-hat change in bar 11; **no random harmony replacements, no drastic tempo jumps, no random FX order**.

Each scene's base beat and sixteen bar positions remain fixed. Calculations and seed updates happen outside the audio ISR. Cycle 0 is the exact authored arrangement; variation decorations begin in cycle 1. For pilot, if an existing drum API cannot surgically insert/remove a single hat, leave the unimplemented decorative subdivision disabled and report it—do **not** approximate by swapping to an unrelated genre groove.

## 5. Manual performance always wins

- **Fill:** queues one bar of the existing Fill implementation, then returns to the original groove. If an automatic fill would occur on the same boundary, suppress the automatic fill.
- **Drop:** latch drum mute at the next bar until the player invokes Back. The Engine's planned Build/Lift phases must not undo it, even at bar 1 of a new cycle.
- **Back:** at next bar, restore the current Engine stage's expected drum level (not a stale level from the moment of Drop).
- **Live chord input:** immediately take control according to `cf_accomp.c`'s current behavior; engine progression continues in background; on release resume the chord that **should be active at that exact bar/slot**. Live-note ownership and HOLD have priority over automated retriggers.
- **Manual key changes:** transpose the authored MIDI voicings and bass notes by the **nearest signed semitone delta** between original scene tonic and newly selected tonic (allowed deltas `-6..+5`), preserving intervals, extensions and sorted note order. Apply at next chord onset, and auto-lock Harmony against regeneration. Do not restore original key on loop boundary.
- **Manual scale changes:** in Build01, preserve the pilot root-degree order **I–V–vi–IV** but replace each chord with the existing Auto Harmony **basic triad for the new selected scale**; **remove the authored sevenths/ninths/suspensions while an alternate scale is active**, instead of inventing accidentals. Switching back to pilot's **E Ionian** restores the authored extended voicings, transposed for the current key. Quantize at chord onset and auto-lock Harmony. (This is an explicit safe trade-off for v1; richer scale-aware extensions can be a later authored feature.)
- **Manual tempo changes:** preserve exact bar phase, no restart; never snap back to the scene's suggested BPM at bar 1. A different scene explicitly loaded by SEL may install its own initial tonic/scale/BPM at the new cycle boundary.
- **External MIDI clock loss / receive stop:** keep the existing safe release/pause behavior, don't freewheel silently.
- **User saved sound edits:** neither a stage transition nor a new loop may overwrite a manually edited patch.

Priority at each bar boundary: **safety/stop → live held notes → explicit player action (Back/Drop/Fill) → pending scene load → explicit regeneration → authored stage automation → cycle decoration**. Exactly one beat/transport owner. Latest compatible user action wins.

## 6. Eight artist-referenced scene recipes (research, not transcriptions)

These are **CORD's original progressions, suggested tempos and rhythm assignments** selected as musical interpretations. They are **not verified song keys, BPMs or harmonic transcriptions**. The quoted song is the production/timbral reference. Actual fixed MIDI voicings, sixteen bar chord-to-slot index map, stage profiles, and stage defaults are in the companion JSON. Factory sound IDs must be resolved by exact name after the Signature 8 agent merges; no guessed numeric index.

| Scene | Specific recording reference | CORD chord sound | Original four-chord spine (two bars each, repeated) | BPM | Groove, stable ID | Bass | FX |
|---|---|---|---|---:|---|---|---|
| **Felt Night** | Nils Frahm — **Keep** (*Felt*) | EMBER KEYS | Cmaj9 → Am9 → Fmaj9 → G6add9 | 72 | Off | Off | Velvet |
| **Heavenly Gaze** | Cocteau Twins — **Heaven or Las Vegas** | GHOST CHOIR | Eadd9 → Bsus4 → C#m7 → Amaj9 | 104 | **DreamPop 56** | TAPE SUB | Wide Drift |
| **Butterfly Club** | Herbie Hancock — **Butterfly** (*Thrust*) | NOIR TINES | Fm9 → Bbm9 → Eb13 → Abmaj9 | 86 | **NeoSoul 61** | ROUND FINGER | Silver Plate |
| **Apollo Drift** | Brian Eno — **An Ending (Ascent)** (*Apollo*) | APOLLO VEIL | Dmaj9 → Bm11 → Gmaj9 → Aadd9 | 68 | Off | Off | Deep Space |
| **Plastic Matter** | SOPHIE — **Faceshopping** | FROSTWIRE | C#m(add9) → Amaj7 → Emaj9 → Bsus4 | 150 | **Hyperpop 34** | DARK PULSE | Bite |
| **Hot Milk Club** | Jackie Mittoo & Soul Vendors — **Hot Milk** | OFFBEAT DUB | G7 → Cmaj7 → Dm7 → G9 | 82 | **new Rockstdy 64** | DUB SUB | Slap |
| **Interlock Wood** | Steve Reich — **Mallet Quartet**, I. Fast | CEDAR MALLET | C6add9 → Dm9 → Em7 → Gsus4add9 | 112 | Off | Off | Clean |
| **Neon Start** | The Cars — **Let's Go** | RAZOR PULSE | Aadd9 → F#m7 → Dmaj9 → E7add9 | 126 | **PopRock 29** | CLEAN MONO | Bite |

**Important source-based artistic differences:** Frahm's *Felt* is explicitly documented as damped, close-miked piano; Guthrie details stereo detuning, chorus and clocked echoes on “Heaven or Las Vegas”; Hancock's *Thrust* album credits Fender Rhodes; Eno's “An Ending” has uncertain exact synth hardware, so we're borrowing its restrained atmosphere only; SOPHIE is documented for synthesizing tactile and metallic-seeming textures, not a proven ring-mod patch; the Jackie Mittoo reference is a Studio One instrumental with organ/rocksteady context; Reich says marimbas interlock and vibraphones carry melody; the Cars' Prophet-5 hard-sync riff is well documented. Sources are below.

**Do not confuse source references with claims of reproduction.** Scenes contain no artist MIDI, recording, sample or copyrighted melodies. Names on the device are original CORD scene names; artist/song attribution belongs in optional information/help and docs as *inspired by*, not counterfeit branding.

**Scope notes:** `Rockstdy 64` is a **new original groove** whose exact 2×16-step drum grid is defined in JSON; it is not currently in the firmware. An independent two-voice Steve Reich-like canon and the Cars lead+chord two-part arrangement are **not** in the first Engine build. Add these only when a later multi-part spec authorizes the required code.

## 7. Next agent handoff: Build 01 / one finished vertical slice

**Build 01 = ENGINE CORE + ONE SHIPPING-QUALITY PILOT SCENE.** No additional audio engine or artist research. Ship only **Heavenly Gaze**, referenced to Cocteau Twins' “Heaven or Las Vegas.” Make the 16-bar loop, stage automation, four Engine dials, Fill/Drop/Back integration, locks and deterministic Regenerate work end-to-end. Stub other authored scenes as research metadata only (not visible/playable until implemented). The companion `CORD_ENGINE_Codex_Handoff_01.md` is the executable scope/acceptance contract.

### Pilot scene — fixed notes, clock and behaviors

- Scene ID **2**, internal key `HEAVENLY_GAZE`, 104 BPM, E major, 4/4; on initial load **Energy 55, Movement 42, Variation 25**; no locks.
- Chord slots: bars 1–2 **Eadd9** MIDI `[52,56,59,66]`; 3–4 **Bsus4** `[47,52,54,59]`; 5–6 **C#m7** `[49,52,56,59]`; 7–8 **Amaj9** `[45,49,52,56,59]`; bars 9–16 repeat same sequence. On first cycle, exactly **eight** chord-change onsets at bars 1,3,5,7,9,11,13,15. Each chord slot is 768 ticks long, so its second bar naturally continues the same chord; do not add a separate tied slot or reattack. Don't retrigger the same chord at bar 2,4,6… unless Movement changes the articulation policy.
- Sound **GHOST CHOIR** from Signature 8 (resolve the post-merge factory identity; do not invent a placeholder patch); bass **TAPE SUB** existing VA; DreamPop stable groove ID **56**, Air kit; FX **Wide Drift**.
- Phase gains in §3. In bars 9–12, Movement ≥65 may activate the **existing CORD chord arpeggiator** instead of holding sustained chords (1/8-note rate, one octave, gate 70%, no latch); deactivate at bar 13 or when Movement drops below65, returning to current sounding chord. No separately sequenced second pad.
- Automatic fill: bar 12 at Energy≥50 and Variation≥50; bar 8 additionally at Energy≥70 and Variation≥75. Automatic one-bar fill never extends the player's manual fill. At the pilot default Variation25, no automatic fills.
- Regenerate allowed choices and **three explicit harmonic sets** are fixed in companion JSON `regenerate_candidates` for scene 2. Start at candidate index 0 (author's original). For every unlocked category, advance to the next candidate index modulo its list length; **do not use randomness in Build 01 regeneration**. Harmony, Sound (chord+bass advance together by their parallel lists), Groove, FX are independent indices. For Sound with 3 candidates, pair GHOST CHOIR/TAPE SUB, BREATH PAD/WARM TRIANGL, TAPE STRINGS/DEEP SINE; for Groove alternate DreamPop ID56 and Pop ID1; for FX cycle Wide Drift, Deep Space, Orbit. This is reproducible, auditable, and eliminates decisions/parameter inventing.
- **Undo** restores the prior four candidate indices at the same respective quantized boundaries and preserves stage playhead, current Energy/Movement/Variation dials and Fill/Drop/Back state. One level, not infinite history.
- On an authored scene selection while playing, new scene queues until cycle boundary. Build01 only has one playable scene; SELECT displays it and SEL reloads it only if stopped. Do not expose unfinished eight-scene menu entries.

### Acceptance matrix for Build 01

1. Clock: after 64 bars (four full loops) bar sequence must be exactly `1..16 × 4`; no missing/duplicate bar; tempo 104 or external clock unchanged across cycles.
2. Chords: eight correct Eadd9/Bsus4/C#m7/Amaj9 onsets per 16 bars; chords held across pair boundary; no stuck voices over bars16→1.
3. Orchestration: stage multipliers and bass/drum enabled states match JSON; E0 mutes bass/drums and keeps quiet chord; E100 remains bounded and no clipping or unsupported voice stealing.
4. Live: change Energy at bar6, Movement at bar10, Variation at bar12; exactly the prescribed next-boundary behavior; editing UI does not reset progression.
5. Existing Perform: Fill lasts exactly one bar; Drop persists for more than one 16-bar cycle; Back returns at next bar with correct current stage gain.
6. Regenerate: four unlocked categories step through exact documented lists; locks prevent changes; manually edited category auto-locks; Undo works; no seed/state pollution of existing Variations.
7. Playback: PLAY/STOP, MIDI clock, live roots/HOLD, save Song, restoration/old factory indices, sound patches and FX behavior all regress cleanly.
8. Audio: bounded levels, DC/clipping/stuck-note tests, dynamic CPU under original budget; no work/allocation/flash from audio callback; renderer and MIDI reflect actual emitted notes, not just screen state.
9. UX: new Engine page retains Core's Rhythm/Key/Scale/Tempo and Perform's Fill/Drop/Back/Tempo; old visual Motion On/Off unaffected; Engine page shows Scene name, bar `XX/16`, stage name, pending state, locks and current chord.
10. Hardware: real FM-1 acceptance required before release; no claim of hardware verification based solely on emulator tests.

## 8. Subsequent handoffs (fixed sequence)

- **Build 01 — Engine Core + Heavenly Gaze** (**next**): 16-bar loop, phase scheduler, scene page, live dials, locks, Regenerate and interoperability. One pilot first limits rework and avoids collisions with the Signature 8 patch agent.
- **Build 02 — Seven remaining scenes + articulation**: consume the seven explicitly authored scene records; add exact `Rockstdy` original groove ID64 and required staccato/offbeat trigger-grid policies; don't add unresearched alternatives. Verify per-scene voice budgets. This is **after** Build01.
- **Build 03 — Independent companion part & true evolving composition**: add independently clocked melodic/pad layer on part 2 with safe note ownership (to make a Reich-style interlocking second line and a Cars-style synced lead over chords possible); develop authored 16-bar fills and layer orchestration, not endless randomly generated solos.
- **Build 04 — Persistent Engine songs**: versioned scene/seed/locks/three dials/16-bar-stage snapshots, restore after boot, compatibility + backup/restore migration. **Save Snapshot** in Build01 is not falsely presented as full Engine persistence.
- **Build 05 — Essentials browsing & curated regeneration expansions**: integrate repo-audited duplicate reduction and further artist-targeted alternatives without expanding the primary library uncontrollably.

**Do not parallel-write** `cf_accomp.c`, `cf_sound.c`, `eng_va.c`, `eng_fm6.c` and the new Signature 8 integration until the current patch agent's branch is available. The next agent should branch from a stable, merged base and modify only the Engine-owned files plus minimal wiring/tests.

## 9. Sources, provenance and limits

### Repository sources
- [`firmware/src/cf_progression.h`](https://github.com/chasedeanda/cord-fm1/blob/main/firmware/src/cf_progression.h): sixteen-slot maximum.
- [`firmware/src/cf_accomp.c`](https://github.com/chasedeanda/cord-fm1/blob/main/firmware/src/cf_accomp.c): transport clock, repeat, plan mailboxes and note ownership.
- [`firmware/src/cf_accomp_ui.c`](https://github.com/chasedeanda/cord-fm1/blob/main/firmware/src/cf_accomp_ui.c): chord plan and safe edits.
- [`firmware/src/cf_drum.c`](https://github.com/chasedeanda/cord-fm1/blob/main/firmware/src/cf_drum.c): groove IDs / kits.
- [`firmware/src/cf_perform.c`](https://github.com/chasedeanda/cord-fm1/blob/main/firmware/src/cf_perform.c): Fill/Drop/Back.
- [`firmware/src/cf_variation.c`](https://github.com/chasedeanda/cord-fm1/blob/main/firmware/src/cf_variation.c): deterministic seeded foreground variation.
- [`docs/FX-PRESETS.md`](https://github.com/chasedeanda/cord-fm1/blob/main/docs/FX-PRESETS.md): approved effect bank.
- [`docs/SONGS-LIBRARY.md`](https://github.com/chasedeanda/cord-fm1/blob/main/docs/SONGS-LIBRARY.md): legacy Song persistence limits.

### Musical references (no audio analysis performed by ChatGPT)
1. Nils Frahm, *Felt* (incl. “Keep”): https://www.nilsfrahm.com/works/felt/ — documented felt damping and very close microphones.
2. Cocteau Twins / Robin Guthrie, “Heaven or Las Vegas”: https://cocteautwins.com/cocteau-twins-equipment-and-sounds.html — documented ±10-cent detuning, chorus and clocked delay; https://reverb.com/news/the-drum-machines-of-the-cocteau-twins — MPC60/SP1200 drum-making context.
3. Herbie Hancock, *Thrust* incl. “Butterfly”: https://www.herbiehancock.com/music/discography/album/thrust/ — Fender Rhodes album credits.
4. Brian Eno, “An Ending (Ascent)”: https://www.musicradar.com/news/the-40-greatest-synth-sounds-of-all-time-no-28-brian-eno-an-ending-ascent — atmospheric approach; hardware attribution uncertain.
5. SOPHIE, “Faceshopping”: https://blog.scienceandmediamuseum.org.uk/sophie-music-and-legacy/ — tactile, synthetic metallic timbres; **no specific ring-mod claim**.
6. Jackie Mittoo & Soul Vendors, “Hot Milk”: https://jamaicamusicatlas.com/entity/riddim/hot-milk-riddim and https://www.reggaerecord.com/en/content/featured_artist.php?id=669 — Studio One / organ / rocksteady provenance.
7. Steve Reich, *Mallet Quartet* I. Fast: https://stevereich.com/composition/mallet-quartet/ — marimba canon/harmonic background vs vibraphone melody.
8. The Cars, “Let's Go”: https://midi.org/dave-smith-and-sequential-circuits — Prophet oscillator-sync techniques documented, not a copied riff.

**All scene chords, note arrays, BPM choices, bar automation numbers, drum grids, and patch combinations are *CORD design decisions*, not presented as recorded facts about the reference songs.**
