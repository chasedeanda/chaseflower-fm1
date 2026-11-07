# Phase 4: simple variations

Initial increment from `main` at `b0193f4`, November 6, 2026. The user asked to
keep this simple, so the interface is one **GLO → Variation** page.

| Control | Action |
| --- | --- |
| K1 Context | Harmony, Arp, Drums, FX, Sound, Scale |
| K2 Amount | Subtle, Musical, Chaos |
| K3 Generate | Turn right for a variation |
| K4 Undo | Turn right to swap the previous and current result |
| SAVE | Existing sound/FX save workflow; other settings keep automatically |

GLO remembers the page. Entering GLO from ARP, FX, the drum machine, a sound
editor or HOME selects the corresponding context. The sound context follows the
part being edited. Each action changes only its selected component. Tempo,
volume, MIDI setup, saved presets and recorded patterns stay under their existing
controls. Undo is one level and lasts for the session.

Subtle makes small adjustments. Musical uses playable, constrained settings.
Chaos allows wider variation while retaining valid ranges and bounded feedback.
The arp's generated settings remain separate from its Random note-order mode;
generation never switches its On/Off or Latch settings.

Harmony changes the selected idea without starting playback or changing the live key/scale.
Subtle moves one idea in the complete list, Musical prefers the current mode, and
Chaos can choose any of the 100 ideas. There are no tag filters. See HOME
Progressions for the selected idea and root-key hints. Sound varies the existing engine's amplitude envelope. Scale preserves the
key and Quantize setting, and keeps seven-note scales when Auto Harmony is on.

FX uses the existing recall fade and preserves bypass. Drums keep the selected
metre and tempo, switching at the next bar when a rhythm is already playing.
Musical/Chaos select a different groove near the current tempo and keep its
intended kit and swing. Subtle keeps the groove/kit and moves swing by at most
two points. Chaos no longer chooses arbitrary kits or extreme swing offsets.
Arp, scale and sound changes requested during held notes or sequencer playback
queue once and apply automatically when safe. An independent drum groove can
continue. FX tails and an enabled-but-stopped metronome do not block settings;
sound changes wait for that part's release-stage voices. K3 shows **Queued** and
K4 becomes **Cancel** while waiting. Leaving the page or changing context/amount
cancels the request. Undo can queue too. No second Generate turn is needed.
No variation writes recorded sequence data or runs generation in the audio ISR.

SAVE opens the existing FX management page or saves a sound as **New** by default.
Arp, drum and scale settings use their existing automatic persistence. Suggestions
remain session-only. There are no new flash records or backup formats.

This is a useful subset of roadmap issues [#18](https://github.com/chasedeanda/chaseflower-fm1/issues/18)
and [#19](https://github.com/chasedeanda/chaseflower-fm1/issues/19), not completion
of both issues. Component-lock controls, a seed/repeat interface, separate
variation slots, progression generation/handoff and coordinated recipes are
deferred. The internal generator is deterministic for a given input and seed.

Run `sh tools/test_variations.sh` for generation bounds, deterministic fixtures,
component isolation, exact FM/VA envelope undo, original drum override restoration,
existing save routing, next-bar switching, queued/canceled live edits, and bit-identical
physical/six-channel USB audio at 44.1/48 kHz. `tests/variations.txt` previews the
real emulator controls; `tests/variations-queued.txt` covers waiting, automatic
application and cancellation. Existing host suites and device-target syntax checks are
regression gates. Device listening, physical controls, linked memory/timing and
sustained GarageBand recording remain required before a firmware release.

Original increment validation: all nine feature suites pass, including 132,432
variation checks and the physical/USB audio comparisons; backup and storage
power-cut suites pass, and AddressSanitizer/UBSan report no errors. The real JieLi
build produces a valid development package and passes package/register checks.
Linked RAM is 96,776/98,304 bytes and pool usage is 333,852/344,064 bytes, so device
stack and timing validation remain necessary. This development package has not
been flashed or published.

Follow-up after device feedback: queue/apply/cancel, HOLD/Latch, recording count-in,
manual edits while waiting, and groove tempo/kit/swing tests pass (154,528 checks).
All nine feature suites pass; waiting preserves bit-identical physical/six-channel
USB audio at both rates. Sanitizers and the real JieLi build pass. Three generated
100 BPM grooves render at a -13.4 dBFS peak with no clipping; musical suitability
still needs listening on the FM-1.
