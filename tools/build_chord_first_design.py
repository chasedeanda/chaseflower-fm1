#!/usr/bin/env python3
"""Validate and publish the planned chord-first screen/workflow documentation.

This describes a successor interface. It does not build or alter firmware.
Core and system inventories are the shared source for the board and player guide.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DESIGN = ROOT / "docs/design/chord-first"
PHASES = {
    1: "Play — first milestone",
    2: "Edit and save — second milestone",
    3: "Learn — third milestone",
    4: "Suggest — fourth milestone",
}
CONTROLS = [
    ("MASTER", "Master volume", "Always controls output volume."),
    ("SELECT", "Browse every progression", "Select an item/slot in the current workspace."),
    ("PRESETS", "Chord sound", "Keep sound browsing accessible; deep editing is under GLO → Sound."),
    ("ALGORITHM", "Progressions / Bass / Bonfire / Nothing", "Choose a visible workspace tab; secondary scenes remain optional."),
    ("K1", "Rhythm / groove", "Edit the parameter labelled K1 on the current screen."),
    ("K2", "Key", "Edit the parameter labelled K2 on the current screen."),
    ("K3", "Scale / mode", "Edit the parameter labelled K3 on the current screen."),
    ("K4", "Tempo", "Edit the parameter labelled K4 on the current screen. Glide lives in detail."),
    ("HOME", "Cycle Core / Sound / Texture / Groove / Perform / Keyboard", "From another screen return to Progressions Core; while on Progressions, cycle the six HOME pages."),
    ("SEL", "Inspect the sounding chord", "Enter the highlighted item; follow explicit on-screen audition labels."),
    ("SEQ", "Accompaniment", "Groove, loop, arrangement and click details."),
    ("EDIT", "Edit the selected progression slot", "Keep the numbered edit focus separate from the sounding chord."),
    ("SAVE", "Songs: save current / browse saves", "Naming SAVE commits New or reviews Replace; a labelled SAVE confirms the selected explicit action."),
    ("PLAY", "Start/stop accompaniment", "One transport across workspaces; an explicitly labelled audition is temporary."),
    ("REC", "Record directly from Progressions", "Count in or arm at the next bar; finish into a new Song, with Pending/Saving/Saved truthfully shown."),
    ("ARP", "Chord motion", "Arpeggiator and performance-motion details."),
    ("LFO", "Bass", "Retains a direct route to bass rather than hiding it behind an advanced editor."),
    ("ENV", "Envelope", "Shape the current sound; advanced synthesis remains available in Sound detail."),
    ("FX", "Effects", "Presets and individual effect settings."),
    ("GLO", "Grouped settings", "Harmony, Sound, Controls, Display, MIDI/USB and System; back within settings."),
    ("OCT− / OCT+", "Performance octave", "Keep existing panic/recovery gestures distinct and documented in System."),
    ("Lower note keys", "Play roots", "Live input takes priority according to the explicit note-ownership contract."),
    ("Upper chord keys", "Deliberate modifiers and HOLD", "Major/minor/extension choices remain explicit; no automatic Rich/Dreamy layers."),
]
CONTRACTS = [
    "The large chord and its scale degree describe actual sounding harmony. The phrase rail describes planned slots and the real transport cursor; repeated pitch matches do not infer sequence position.",
    "Each planned chord pairs a scale-degree numeral above its chord/root name. Major uses uppercase, minor lowercase. A sounding-degree/quality label remains visible beside the large chord. Key transposition preserves degree; mode/quality changes update it.",
    "HOME K1–K4 are Rhythm, Key, Scale, Tempo. SELECT is one complete catalogue; metadata never hides ideas. Browsing supplies starting key/mode/metre without an Apply step.",
    "One accompaniment clock owns progression timing and premade grooves. Running groove/setup changes land at defined musical boundaries; tempo preserves phase. Active and pending values must be labelled separately.",
    "Live roots/HOLD have priority over temporary intentional audition, which has priority over scheduled harmony. The groove and transport cursor continue. Return to the current scheduled chord rather than restarting the phrase. The inventory specifies recording exceptions and cancellation.",
    "Harmony is one On/Off choice. On builds the agreed complete major/minor chords; deliberate 7 follows quality, including Emaj7 → Bmaj7 → D♯m7 in E major. Learning labels actual notes and outside-scale tones accurately.",
    "Harmony On/Off controls assistance for live root-key playing. Explicit Progression PLAY still plays its arranged chords when that assistance is Off, without switching the setting on. A recorded Take retains its fixed notes.",
    "Songs is the one user-facing collection for saved progressions and recordings. Factory progression browsing remains on HOME. Editable Songs use semantic chord/degree/duration data; recorded Songs retain captured performance events and timing. Existing recorded Auto takes keep their fixed-note meaning and format.",
    "REC is direct from HOME. It captures the scheduled progression plus deliberate live/groove/key/tempo changes rather than unexpectedly muting the arranged chords. Ending a capture creates a new numbered Song copy; no mandatory Review/Keep step. Pending, Saving and Saved are different states, and flash writes wait for an actually safe quiet boundary without cutting independent HOLD or audible tails.",
    "Songs supports selected-row Load, Rename, Duplicate and Delete, plus counted Clear Recordings/Clear All actions. Bulk clear excludes factory templates, the protected working draft and Pending captures. A Pending capture can be discarded only through its own explicit selected action.",
    "The selected current HOME appearance is the owner-approved soft mascot reference; the earlier adaptive prototype remains preserved as design history. The approved mascot build uses Devil/Ember by default and Cat/Plum as its only alternative. Character and theme are coupled; a separate theme/accent selector is not present. Preserve the large chord, paired degrees, thin separators, four text controls and the supplied prototype's restrained halo/Bloom visual vocabulary; earlier Coral Bloom, Aurora and Studio Rack are historical alternatives.",
    "Motion has exactly two user choices: On (adaptive) and Off. On responds automatically to output level/volume and semantic active-instrument activity using a smoothed bounded envelope, with musical motion tied to the shared clock. There is no manual Quiet/Expressive/intensity/speed selector. Off cancels every pulse, orbit, morph travel and tween; truthful chord, value, playback and record/save data still update instantly. The supplied loudness/layer sliders are simulation inputs, not new firmware settings or evidence of eight instrument parts. Measure display, input, memory and audio/USB timing on the FM-1.",
    "HOME from another screen returns to Progressions Core. Further HOME taps cycle Core/Sound/Texture/Groove/Perform/Keyboard without changing any parameter; SELECT still browses the complete catalogue, PRESETS still selects chord sound, ALGORITHM keeps its scene/workspace role and OCT± stays musical. The five musical pages show four text control columns; Keyboard instead uses the full display for the key guide and disables all four knobs.",
    "HOME Perform is Fill / Drop drums / Bring back / Tempo. Moments commit at bar boundaries; Fill lasts one bar and returns automatically, Drop leaves chords and bass running, Back restores groove phase with a downbeat bloom. Motion Off keeps static truth. V1 is live-only; Breakdown and separate multi-bar builds are deferred.",
    "HOME Keyboard / Learn is page 6/6 after Perform. It shows the current physical key assignments, root notes/degrees, normal live/latched states and the last pressed key’s full name and explanation. K1–K4 are inactive; the full screen belongs to the guide. HOME wraps to Core. Key/scale/OCT and Manual/Auto/reassigned modifiers update from their existing owners.",
    "No image, label or example value in this board is a physical-device measurement. Fonts, partial redraws, storage limits and audio/USB timing need linked-build and FM-1 verification before release.",
    "Advanced FM6/VA pages enumerate retained capabilities and repeatable control templates. Keep a usable Advanced Sound route while cosmetic migration proceeds; rewriting every advanced page is not a gate for the first audible progression-playback milestone.",
]
QUICK_START = [
    "Open HOME. Turn SELECT to explore the complete progression catalogue; each idea supplies its starting key, mode and time signature.",
    "Press PLAY to hear the selected progression. Press PLAY again to stop.",
    "Turn K1 for a compatible drum groove or Drums Off, K2 to transpose, K3 for the mode and K4 for tempo. PRESETS changes the chord sound; MASTER changes volume.",
    "Play the lower root keys and deliberately choose upper-key modifiers. The large chord and degree show what you hear; the phrase rail keeps the planned order visible.",
    "As the later milestones arrive, use EDIT to develop a progression, REC to capture it directly, SAVE to open Songs, SEL to learn from the sound and the Next view to explore continuations.",
]


def clean(value: object) -> str:
    return str(value).replace("|", "\\|").replace("\n", " ")


def load_inventory() -> dict:
    sources = [DESIGN / "core-screens.json", DESIGN / "system-screens.json"]
    data = {"screens": [], "workflows": []}
    for path in sources:
        part = json.loads(path.read_text())
        data["screens"].extend(part["screens"])
        data["workflows"].extend(part["workflows"])
    screen_ids = [s["id"] for s in data["screens"]]
    if len(screen_ids) != len(set(screen_ids)):
        raise ValueError("Duplicate screen IDs")
    workflow_ids = [w["id"] for w in data["workflows"]]
    if len(workflow_ids) != len(set(workflow_ids)):
        raise ValueError("Duplicate workflow IDs")
    known = set(screen_ids)
    required = ("id", "group", "title", "phase", "kind", "purpose", "entry", "fields", "knobs", "select", "algorithm", "buttons", "exit", "states", "playerSteps", "related")
    kinds = {"chord", "picker", "params", "timeline", "confirm", "status", "text"}
    for screen in data["screens"]:
        for key in required:
            if key not in screen:
                raise ValueError(f"Missing {key}: {screen['id']}")
        if screen["phase"] not in PHASES or screen["kind"] not in kinds:
            raise ValueError(f"Invalid phase/kind: {screen['id']}")
        if len(screen["knobs"]) != 4:
            raise ValueError(f"Four knob definitions required: {screen['id']}")
        for target in screen["related"]:
            if target not in known:
                raise ValueError(f"Unknown related screen {target}: {screen['id']}")
        if not screen["entry"] or not screen["exit"]:
            raise ValueError(f"Missing entry/exit: {screen['id']}")
    for flow in data["workflows"]:
        if not flow["steps"] or flow["phase"] not in PHASES:
            raise ValueError(f"Empty/invalid workflow: {flow['id']}")
        for step in flow["steps"]:
            if step["screen"] not in known:
                raise ValueError(f"Unknown workflow screen {step['screen']}: {flow['id']}")
            if not step["action"] or not step["result"]:
                raise ValueError(f"Missing action/result: {flow['id']}")
    data.update({
        "title": "Chord-first FM-1 — screen and workflow inventory",
        "status": "Planned behavior and coverage; selected visual reference is the supplied adaptive prototype with the current Devil/Ember and Cat/Plum mascot selection. Earlier inventory layout sketches are not the selected screen design or released firmware.",
        "version": "0.4",
        "date": "2026-11-07",
        "phases": [{"id": k, "label": v} for k, v in PHASES.items()],
        "controls": [{"control": c, "home": h, "elsewhere": e, "inside": e} for c, h, e in CONTROLS],
        "quickStart": QUICK_START,
        "contracts": CONTRACTS,
        "issues": {"tracker": 1, "playback": 43, "visuals": 44, "home": 45, "editing": 46, "learning": 47, "suggestions": 48, "themes": 49, "performance": 50, "trimming": 51},
        "defaultAppearance": {"character": "Devil", "theme": "Ember", "motion": "On (adaptive)", "motionChoices": ["On (adaptive)", "Off"], "background": "#150F0C", "foreground": "#FFF4E5", "secondary": "#D0BFB1", "separator": "#5D4537", "subtle": "#241813", "accent": "#FFA55B", "onAccent": "#28170D"},
    })
    data["implementation"] = json.loads((DESIGN / "implementation-evidence.json").read_text())
    return data


def guide(data: dict) -> str:
    by_id = {s["id"]: s for s in data["screens"]}

    def player_text(value: object) -> str:
        text = str(value)
        for key in sorted(by_id, key=len, reverse=True):
            text = text.replace(key, by_id[key]["title"])
        return text

    lines = [
        "# Playing the chord-first FM-1", "",
        "> **Planned successor interface.** This is the player documentation for the design board. These controls and later-stage features are not all available in the currently released firmware.", "",
        "The [implementation plan](design/chord-first/IMPLEMENTATION-PLAN.md) uses the supplied [adaptive prototype](design/chord-first/instrument-teal/prototype.html), with **Devil/Ember** as the current default and **Cat/Plum** as the only alternative and **Motion On (adaptive) / Off**.", "",
        "## Start here", "",
    ]
    lines += ["> " + data["implementation"]["summary"], ""]
    lines += [f"{i}. {x}" for i, x in enumerate(data["quickStart"], 1)]
    lines += ["", "## Read the main screen", "",
        "The large chord is the sound you are hearing. Its degree/quality line might read **vi · minor**. In F major, the planned phrase can show **IV over Bb**, **V over C**, **vi over Dm**, and **I over F**. Uppercase numerals mean major; lowercase means minor. A slot counter such as 03/04 is the transport position, not the scale degree.", "",
        "The notes line shows actual chord tones. The phrase cue shows planned chords; its cursor continues while you temporarily play over a loop. Pending changes are labelled until they become active. Devil/Ember is the default; Cat selects Plum. The two bottom HOME strips use one horizontal rule and three thin control dividers, with the bank cue in the header. Display → Motion has only On and Off: On automatically adapts its response to loudness and active instruments; Off stops all visual animation while musical values, chord identity and playback/record/save status continue to update instantly. The supplied nine-screen adaptive prototype defines the visual direction; several study controls and musical engines are not implemented features.", "",
        "## Delivery order", "",
    ]
    lines += [f"- **{label}.**" for label in PHASES.values()]
    lines += ["", "## Physical controls", "", "| Control | On HOME | Elsewhere |", "| --- | --- | --- |"]
    lines += [f"| {clean(c)} | {clean(h)} | {clean(e)} |" for c, h, e in CONTROLS]
    lines += ["", "## Step-by-step workflows", ""]
    for flow in data["workflows"]:
        lines += [f"### {flow['title']}", "", f"**Availability:** {PHASES[flow['phase']]}. {flow['goal']}", ""]
        for i, step in enumerate(flow["steps"], 1):
            lines.append(f"{i}. **{player_text(step['action'])}** — {player_text(step['result'])} ({by_id[step['screen']]['title']}).")
        lines.append("")
    lines += ["## Screen reference", "", "Every entry below is a planned screen or a named state/tab of a shared screen. Advanced parameter pages reuse the same visual and control template; retained functions can remain behind Advanced Sound while the new appearance is migrated. Every advanced page does not need to be rebuilt before the first playback milestone.", ""]
    groups = list(dict.fromkeys(s["group"] for s in data["screens"]))
    for group in groups:
        lines += [f"### {group}", "", "| Screen | Milestone | Open it | What it does |", "| --- | --- | --- | --- |"]
        for s in data["screens"]:
            if s["group"] == group:
                lines.append(f"| {clean(s['title'])} | {s['phase']} | {clean(player_text(s['entry']))} | {clean(player_text(s['purpose']))} |")
        lines.append("")
    lines += ["## What stays predictable", ""]
    lines += [f"- {x}" for x in CONTRACTS if not x.startswith("No image") and not x.startswith("Advanced FM6")]
    lines += ["", "## Design and release reference", "", "The [screen/workflow inventory](design/chord-first/design-board.html), [engineering handoff](design/chord-first/ENGINEERING-HANDOFF.md) and [GitHub roadmap](https://github.com/chasedeanda/chaseflower-fm1/issues/1) describe the same planned controls. The inventory's earlier screen sketches document coverage; the current mascot build and MASCOTS guide define the selected appearance. Use the release-specific README/manual for behavior already shipped on the device.", ""]
    return "\n".join(lines)


def engineering(data: dict) -> str:
    lines = ["# Chord-first FM-1 engineering handoff", "", "**Status:** complete planned-interface inventory for review; not an implementation or hardware validation claim.", "", "Start with the [implementation plan](IMPLEMENTATION-PLAN.md), [historical adaptive study](instrument-teal/prototype.html) and [motion specification](MOTION-SPEC.md).", "", f"Design version {data['version']}, {data['date']}. {len(data['screens'])} screen/state definitions and {len(data['workflows'])} workflows share canonical JSON with the board and player guide.", "", "## Product contracts", ""]
    lines[2:2] = [data["implementation"]["summary"], ""]
    lines += [f"{i}. {x}" for i, x in enumerate(CONTRACTS, 1)]
    lines += ["", "## Delivery", ""] + [f"- {v}." for v in PHASES.values()]
    lines += ["", "Playback, HOME and the modern visual system are the first work. Optional accent modes, pruning and a comprehensive optimization project do not block initial playback. Measure linked memory and FM-1/USB timing; derive safe capacities rather than inventing limits.", "", "## Screen contracts", ""]
    for s in data["screens"]:
        lines += [f"### {s['id']} — {s['title']}", "", f"**Group:** {s['group']} · **Delivery:** {PHASES[s['phase']]} · **Template:** {s['kind']}", "", s["purpose"], "", f"**Entry:** {s['entry']}", f"**Exit:** {s['exit']}", "", "| Visible field | Reference value/state |", "| --- | --- |"]
        lines += [f"| {clean(f['label'])} | {clean(f['value'])} |" for f in s["fields"]]
        lines += ["", "| Input | Action |", "| --- | --- |", f"| SELECT | {clean(s['select'])} |", f"| ALGORITHM | {clean(s['algorithm'])} |"]
        lines += [f"| K{i} | {clean(x)} |" for i, x in enumerate(s["knobs"], 1)]
        lines += [f"| {clean(k)} | {clean(v)} |" for k, v in s["buttons"].items()]
        lines += ["", "**State coverage:**"] + [f"- {x}" for x in s["states"]]
        if s["playerSteps"]:
            lines += ["", "**Player instructions:**"] + [f"{i}. {x}" for i, x in enumerate(s["playerSteps"], 1)]
        lines += ["", "**Related:** " + (", ".join(s["related"]) or "HOME via physical HOME"), ""]
    lines += ["## Workflow contracts", ""]
    for w in data["workflows"]:
        lines += [f"### {w['id']} — {w['title']}", "", w["goal"], "", "| Step | Screen | Physical action | Result |", "| --- | --- | --- | --- |"]
        lines += [f"| {i} | {clean(x['screen'])} | {clean(x['action'])} | {clean(x['result'])} |" for i, x in enumerate(w["steps"], 1)]
        lines.append("")
    lines += ["## Verification before engineering release", "", "- Validate every entry/exit and physical control against the board. Keep main product text free of internal screen IDs and implementation trivia.", "- Compare actual sounding notes, degree/quality, planned slot, live input, audition and pending changes independently.", "- Exercise empty/full/dirty/saving/error, count-in/record/review, external-clock, no-compatible-groove and recovery routes covered by the screen definitions.", "- Verify glyphs and transitions on the native 240×240 FM-1 display, with Motion Off as a stable equivalent.", "- Run note release/HOLD, MIDI, USB audio, safe save/restore and boot/update regression checks appropriate to each implemented phase.", "- Record exact revision/options and paired ordinary-RAM/POOL/XIP maps; measure audio work/lag, display/input response and USB reliability on hardware.", "", "Canonical sources: `core-screens.json` and `system-screens.json`. Generated outputs: `board-data.json`, `docs/CHORD-FIRST-USER-GUIDE.md`, this handoff and the board's embedded data. Regenerate with `python3 tools/build_chord_first_design.py`.", ""]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--board-fragment", type=Path)
    args = parser.parse_args()
    data = load_inventory()
    payload = json.dumps(data, ensure_ascii=False, indent=2) + "\n"
    (DESIGN / "board-data.json").write_text(payload)
    (ROOT / "docs/CHORD-FIRST-USER-GUIDE.md").write_text(guide(data))
    (DESIGN / "ENGINEERING-HANDOFF.md").write_text(engineering(data))
    rollout = {"status": "All entries planned; no implementation or physical validation inferred from a mockup", "defaultAppearance": data["defaultAppearance"], "screens": [{"id": s["id"], "title": s["title"], "group": s["group"], "sharedTemplate": s["kind"], "musicalMilestone": s["phase"], "designStatus": "Planned — migrate layout to selected prototype", "implementationStatus": "Not assessed by this design task", "deviceVerification": "Required"} for s in data["screens"]]}
    rollout["status"] = "Engineering preview progress; physical verification remains pending"
    for entry in rollout["screens"]:
        entry.update(data["implementation"]["screens"].get(entry["id"], {}))
    (DESIGN / "screen-rollout.json").write_text(json.dumps(rollout, ensure_ascii=False, indent=2) + "\n")
    if args.board_fragment:
        path = args.board_fragment
        fragment = path.read_text()
        start_tag = '<script type="application/json" id="fm1-board-data">'
        import html
        end_tag = "</script>"
        value = payload.replace("</", "<\\/")
        if start_tag not in fragment:
            start_tag, end_tag = html.escape(start_tag, quote=True), html.escape(end_tag, quote=True)
            value = html.escape(value, quote=True)
        start = fragment.index(start_tag) + len(start_tag)
        end = fragment.index(end_tag, start)
        fragment = fragment[:start] + "\n" + value + fragment[end:]
        path.write_text(fragment)
    print(json.dumps({"screens": len(data["screens"]), "workflows": len(data["workflows"]), "groups": len(set(s['group'] for s in data['screens'])), "status": "validated"}))


if __name__ == "__main__":
    main()
