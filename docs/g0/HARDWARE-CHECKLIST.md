# G0 — five-minute hardware audition (pending)

Use the G0 test build after reviewing the PR. Keep a guitar connected and, if available, USB audio/MIDI capture running. This checklist has **not** been performed on hardware. Record build hash, actual BPM/key, audio/MIDI capture, pass/fail and symptoms. Do not merge until the audition and required device measurements are reviewed.

| Time | Play and check |
| --- | --- |
| 0:00–0:35 | With your chosen key/tempo, load the Heavenly Gaze pilot and PLAY. Verify those choices remain. Hear a quiet pulse in bar 1 and bass by bar 5; follow the current chord and XX/16. Check the first 16→1 transition without a gap or restart. |
| 0:35–1:15 | Sweep Energy low/mid/high and Space dry/wet while playing guitar. Hear the response begin within 250 ms without clicks or runaway feedback; the user's mix and FX baseline must stay intact. Turning Energy back up from zero must restore the pulse immediately on its scheduled hits. |
| 1:15–2:00 | Set Feel 0, 35 and 100 at different stages. Verify `Next chord`/`Next` appears and clears on the next two-bar chord onset: held → half-note gated chords → eighth-note ARP. Check bars outside Lift, including Intro and Return. |
| 2:00–3:00 | On ARP, turn Play It Auto → On → Off → Auto. On must play scheduled ARP regardless of Feel; Off must hold/gate chords. Change rate, mode, octave, gate and swing; hear and see the next-onset result. Try the independent Live keys switch and hold/release a keyboard chord through an onset. No doubled owners or hanging notes. |
| 3:00–3:45 | On Perform, trigger Fill, Drop and Back. Drop must survive a full 16-bar boundary until Back. Move through Sound/Texture/Groove/Core/ARP during playback; preserve tempo/key and continuity. |
| 3:45–4:30 | Turn K4 clockwise twice while queued: the latest whole take must wait for bar 1. Turn back once: restore the preceding committed combination. Manually change sound/FX/groove, then request another take; pinned categories must remain yours. Use existing GLO locks to unpin if needed. |
| 4:30–5:00 | Listen through another boundary, then STOP. Confirm no stuck notes, clock jump, USB glitch or unexplained reset. Log whether this is useful and enjoyable to play guitar over; host tests do not answer that. |

Separately compare the level-matched Ghost Choir and Breath Pad dry/wet files. Both factory patches remain unchanged; record the user's preference without treating host RMS or peak as a quality ranking.

For the resource gate, record hardware callback/CPU worst case, stack high-water and voice shedding using the existing diagnostics. These require actual physical measurements; linked RAM/flash totals cannot substitute. Existing SAVE is a static Song snapshot. REC retains manual-take ownership. Do not expect generative recording/export.

Results: **pending hardware review**. G1 may start only after G0 passes hardware review and is merged by the user.
