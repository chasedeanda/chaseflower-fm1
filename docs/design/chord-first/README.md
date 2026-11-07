# Instrument Teal interface handoff

**Start with [AGENT-START.md](AGENT-START.md).**

The selected visual reference is the supplied adaptive motion prototype, with
**Instrument Teal as the default** and **Motion On (adaptive) / Off**.

- [Implementation plan and delivery gates](IMPLEMENTATION-PLAN.md)
- [Selected prototype: Instrument Teal default](instrument-teal/prototype.html)
- [Untouched supplied prototype](instrument-teal/supplied-adaptive-motion-prototype.html)
- [Visual system](VISUAL-SYSTEM.md) and [motion specification](MOTION-SPEC.md)
- [Complete engineering handoff](ENGINEERING-HANDOFF.md)
- [Screen/workflow inventory](design-board.html)
- [Core screen definitions](core-screens.json) and [system definitions](system-screens.json)
- [Implementation status checklist](screen-rollout.json)
- [Player guide](../../CHORD-FIRST-USER-GUIDE.md)
- [Product vision and retained musical decisions](../../CHORD-FIRST-VISION.md)

The inventory covers **205 screen/state definitions and 37 workflows**. Its older
layout sketches explain behavior and coverage; the selected adaptive prototype
defines appearance. The supplied HTML demonstrates motion without an audio
engine. Morph/Bloom and several sample parameters are studies, not evidence of
completed firmware capabilities. The plan specifies the real control mappings,
direct recording/Songs workflow, safe persistence and physical acceptance gates.

Regenerate and validate the documentation with Python's standard library:

```sh
python3 tools/build_chord_first_design.py --board-fragment docs/design/chord-first/functional-board.fragment.html
```

`design-board.html` is the included standalone review export. The canonical JSON
and generator remain the source for behavior and player instructions. Re-export
the standalone review after editing the fragment/data; do not hand-edit generated
contracts in multiple places.

This package contains documentation and browser references. It does not include
the local workspace's unrelated in-flight firmware changes. Bring the handoff
onto the current engineering integration branch before implementing an issue;
recheck current integration instead of assuming this planning snapshot remains the latest code.
