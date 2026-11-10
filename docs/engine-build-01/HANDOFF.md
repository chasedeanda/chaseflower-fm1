# Codex handoff — CORD Engine Build 01 (implementation only)

Repository: https://github.com/chasedeanda/cord-fm1

## Your input artifacts

Read **both** before changing code:
- `CORD_ENGINE_Musical_Rulebook_v1.md` — complete product, timing, behavior and integration contract.
- `CORD_ENGINE_Scenes_v1.json` — authored scene IDs, exact pitches, tempo, groove ID, FX, stage profiles and the pilot's finite regeneration alternatives.

**Do not conduct artist research, choose presets, choose notes, choose musical behavior, change FX design or improvise arrangements.** Research/design decisions are made; implementation must follow the artifacts. Report any mismatch between the documented engine APIs and current merged branch. Do not invent a musical fallback.

## Branch and collision rules

The Signature 8 sound agent is separately implementing eight new factory patches. Start **after** its changes have merged to a stable base. `GHOST CHOIR` must resolve to that new patch's actual factory identity. Never hardcode a guessed factory index. Do not alter `gen_chase_library.py`, its generated sound programs, `eng_va.c`/`eng_fm6.c` patch definitions, Aquanode bytes or FX algorithms. Do not release or flash hardware.

## Exact scope

1. Introduce a dedicated Engine scene constant/symbol `CF_P_ENGINE = 253` without changing existing `CF_P_DRAFT=254` and `CF_P_NONE=255`; adapt plan validation, setup/repeat and UI interactions to allow this distinct scene source, but do not overwrite the current user's edit draft.
2. Foreground-build one `cpa_plan_t` for **8 chord slots × 768 ticks** (16 bars total), key E, 4/4 at 104 BPM; use `HEAVENLY_GAZE` ID2 as the only exposed scene, and read its 16-bar chord-index map + 4 exact note arrays from JSON. Every two-bar slot is one chord (slot 1/2/3/4/5/6/7/8 maps to chord 0/1/2/3/0/1/2/3). Do not retrigger at the intermediate bar. After slot 8, loop to slot 1 without transport stop/reset.
3. Load **GHOST CHOIR** on chord part, existing **TAPE SUB** linked bass part, groove **DreamPop stable ID56** and its associated Air kit; approved global FX recipe **Wide Drift**.
4. Add a small, bounded Engine bar-phase controller, with stage attenuation using the JSON `dreampop` five segments. Implement Energy scaling and smooth gain changes without overwriting current user mix or patch bytes. No new DSP, large ISR operations, new allocations or flash writes.
5. Add HOME → Engine page *in addition to* existing HOME → Core and HOME → Perform. Scene name, stage, bar `XX/16`, chord, K1 Energy/K2 Movement/K3 Variation/K4 Regenerate/Undo, Pending state, and four locks from GLO → Engine Locks. Preserve all current button behavior outside this new page. Distinguish musical Movement from visual Motion On/Off.
6. Implement Movement policy `held` exactly: under 65, hold chords; 65 or more, chord arp on **only bars9–12**, 1/8 note, one octave, 70% gate, no latch; return to held chord on bar13 or immediately after Movement goes below65 at a safe chord boundary. No second independent layer.
7. Implement Variation levels exactly as rulebook; no chords changed automatically. On first pass a missing safe per-lane hat override must be reported rather than replaced by arbitrary groove switching. For Build01, base arrangement, full repeat, and (where possible) two designed fills take precedence over decorative hat edits.
8. Regenerate is **round-robin** by four independent category indices from JSON: Harmony variations 0..2, Sound pairs 0..2, Grooves (DreamPop ID56, Pop ID1), FX (Wide Drift, Deep Space, Orbit). Advancing only unlocked categories, latest compatible request wins, Undo one step. Harmony commits at next two-bar chord boundary; other changes at next bar. Never change key, BPM, stage or live Fill/Drop state during regeneration.
9. Preserve Core controls: changing Key transposes pilot MIDI notes by nearest signed semitone delta `-6..+5`; changing Scale keeps root-degree order `[0,4,5,3]` and uses existing basic diatonic triads only (remove authored extensions while scale differs from E Ionian; restore them on E Ionian). Neither change resets tempo/cycle. Both auto-lock Harmony. Tempo adjustments preserve phase. Reuse existing Fill/Drop/Back priority and live chord takeover; manual Drop must persist across successive cycles until Back. Song saving must at minimum produce a **flat 16-bar snapshot** through existing Song infrastructure; do not claim to persist evolving Engine controls or runtime seed in this build.
10. Provide docs and host tests for four 16-bar loops, note/tie correctness, phase boundaries, live interaction, Groove ID mapping, manual Drop persistence, Regenerate/Undo/locks, no clipping, no clock drift, no audio callback allocations, old saves/factory indices intact. Report static and dynamic CPU/RAM changes.

## Forbidden scope

- Do not expose or implement the other seven scenes yet.
- Do not add a new drum groove, new FX, new synthesis engine, generative melody, recorder, granular buffer, multi-part arpeggiator or full Engine flash format.
- Do not arbitrarily modify MIDI behavior, global Core control mapping, visual mascot Motion toggle or the library catalog.
- Do not claim real hardware verification from emulator results.

## Deliverables

Implementation commit or PR, a diff summary, tests/logs, a deterministic 64-bar performance trace (bar, slot, stage, MIDI note set, chord/bass/drum gain and active locks), and a short known-limits list. Stop and report an incompatibility rather than guessing musical intent.

This is the **immediate next coding handoff once Signature 8 integration is stable**. Then the research owner can authorize Build 02 for the seven additional scene records and specialized articulation.
