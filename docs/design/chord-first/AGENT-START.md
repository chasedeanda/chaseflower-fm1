# Agent start: chord-first FM-1

Start implementation from the published plan on **`codex/chord-first-roadmap-01a1b40b`**. This publication changes documentation and browser references only. Its refreshed source base includes `main` at `29c3ba6e8fe87c33f94c8c139157460568ce89b3`, where [progression playback PR #53](https://github.com/chasedeanda/chaseflower-fm1/pull/53) has landed. Inspect and reuse that implementation; the milestones below define completion/verification, not a request to build a second playback engine. Recheck the current integration reference before source edits because other work can advance.

## Read first

1. [Implementation plan](IMPLEMENTATION-PLAN.md), especially milestones A/B and the protected behavior.
2. [Selected Instrument Teal prototype](instrument-teal/prototype.html) and [untouched supplied reference](instrument-teal/supplied-adaptive-motion-prototype.html).
3. [Motion specification](MOTION-SPEC.md) and [visual system](VISUAL-SYSTEM.md).
4. [Core screen contracts](core-screens.json), [system contracts](system-screens.json) and [rollout mapping](screen-rollout.json).
5. [Player guide](../../CHORD-FIRST-USER-GUIDE.md) and [engineering handoff](ENGINEERING-HANDOFF.md).

**Selected direction:** Instrument Teal by default; large actual chord, visible sounding degree, planned numerals above chord names, thin separators and four text controls. Motion has exactly **On (adaptive) / Off**. On follows safe real output/activity metadata and musical events; Off cancels all visual animation while values, transport and recording remain truthful. The supplied loudness/layers sliders are preview inputs, not new device controls. Morph/Bloom are visual references, not approval to implement new musical engines or ship fake Grain/Shimmer/Freeze controls.

## Obtain the plan and a safe source checkout

Read the published branch without disturbing another working checkout:

```sh
git fetch origin
git log -1 --oneline origin/codex/chord-first-roadmap-01a1b40b
git show origin/codex/chord-first-roadmap-01a1b40b:docs/design/chord-first/IMPLEMENTATION-PLAN.md
```

Before firmware edits, identify the **current integration reference and its in-flight source changes** with the integration owner. Existing Auto Harmony, progression, groove and UI work must be preserved. Read applicable repository instructions and inspect status/diffs. Do not reset, clean, stash, replace or commit another task's working changes as part of setup.

Create or reuse an isolated implementation checkout from that agreed current integration snapshot, using the available worktree tooling. Bring in the reviewed docs-only publication commit there, or reconcile the documentation branch with that integration snapshot before coding. Record both the resulting source commit and design commit. Do not assume the publication branch remains the latest source and later overwrite newer engine/UI work to resolve conflicts. Documentation review, ownership planning and read-only baseline inspection can begin immediately while the source handoff is resolved.

## Start with A, then the first audible B

**Milestone A — one real HOME:** establish a reproducible linked/audio/input baseline; implement the shared native-screen primitives, bounded glyphs, snapshot/invalidation model, Instrument Teal and versioned On/Off appearance settings. Show actual live chord/notes, paired degrees and Core controls on the device. Keep reliable live modifiers, HOLD, MIDI/USB and release handling.

**A exit:** native 240 × 240 HOME is readable, reflects actual notes, preserves live playing and settles completely with Motion Off. Report device checks still pending as pending; browser appearance is not device verification.

**Milestone B — first useful PLAY:** play the selected progression and premade groove from slot 1 through one accompaniment clock. Complete Core/live banks and Groove/Loop/Mix/Click using real supported parameters. Preserve phase on tempo changes; apply key/mode/groove changes at their defined boundaries; distinguish active setup, queued setup, cursor, matches and edit focus.

**B exit:** select → PLAY → change groove/key/tempo → stop works audibly without stuck notes, independent clock drift or an input stall. Harmony Off still permits explicit progression playback. Verify supported metres, every catalogue slot, external-clock waiting/loss/resume, live HOLD and applicable USB/MIDI regressions. Full Songs, learning, every advanced page and broad optimization do not delay this vertical slice.

## Divide work by issue and file ownership

| Lane | Issues | Scope / integration boundary |
| --- | --- | --- |
| Foundation and first playback | [#43](https://github.com/chasedeanda/chaseflower-fm1/issues/43), [#44](https://github.com/chasedeanda/chaseflower-fm1/issues/44), [#45](https://github.com/chasedeanda/chaseflower-fm1/issues/45), [#49](https://github.com/chasedeanda/chaseflower-fm1/issues/49), [#50](https://github.com/chasedeanda/chaseflower-fm1/issues/50) | Shared accompaniment, renderer, HOME/navigation, appearance persistence and measurement. Agree snapshot/event APIs first; work in separately owned modules/fixtures where possible. |
| Editor, recording and Songs | [#46](https://github.com/chasedeanda/chaseflower-fm1/issues/46) | Follow the playback ownership/clock contract. Direct HOME REC captures what is heard; new numbered Song copy; truthful Pending/Saving/Saved; explicit selected-row management and counted clear. Storage/quiet boundaries and migration need their own evidence. |
| Learning | [#47](https://github.com/chasedeanda/chaseflower-fm1/issues/47) | After usable playback/edit targets: actual notes/intervals, accurate membership and deliberate comparisons. Preserve Emaj7 → Bmaj7 → D♯m7 under the agreed live 7 rule. |
| Offline Next | [#48](https://github.com/chasedeanda/chaseflower-fm1/issues/48) | After editor/learning focus and audition contracts: compact offline shortlist, explicit audition/insertion and undo. No network/statistics dependency. |

Use **one common integration lane** for edits to `cf_ui.c`, `cf_screens.c`, `cf_views.c`, `cf_state.h` and `coralflower.c`. The same coordination applies to real-time glue (`cf_out.c`, `cf_play.c`, `cf_seqp.c`, `cf_drum.c`) and persistence (`cf_settings.c`, `cf_backup.c`, `storage.c`). These are shared ownership boundaries, not separate concurrent assignments. Claim an explicit file set before editing; bundle small common-file changes for the integrator rather than letting several agents rewrite the same screen/router/unit. Keep unrelated source work intact when resolving conflicts.

## Completion evidence

- Record source/design revisions, build options, meaningful host checks and separate linked RAM/POOL/flash measurements.
- Exercise the complete musical flow, rapid changes, note releases, Motion On→Off, pending setup and actual storage boundaries relevant to the change.
- Keep existing Advanced Sound, system, backup/update/recovery and legacy-content routes usable during cosmetic migration. Retheme them through shared families; they are not a gate for first audible PLAY.
- Update canonical screen/workflow state and regenerate the board/guide together. Track Planned, Designed, Implemented and Device-verified separately.
- Include real native firmware captures and hardware/audio/USB findings where available; identify outstanding physical checks without claiming they passed.

This handoff starts engineering. It does not itself modify firmware, start implementation agents, flash hardware or authorize replacing other in-flight work.
