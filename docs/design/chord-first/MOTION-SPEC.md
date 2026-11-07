# Motion specification: adaptive prototype

> **Current mascot build:** the owner approved only Devil/Ember and Cat/Plum, with palette tied to character. This supersedes the older theme/scene proposals below. The implemented two-strip HOME, four-level text and controls are documented in [MASCOTS](../../MASCOTS.md). Physical verification remains pending.

Planned successor behavior and implementation guidance, before firmware changes.
The authoritative visual reference is the latest user-supplied
`fm1_adaptive_motion_prototype.html`, preserved byte-for-byte in the repository.[^reference]
Keep its nine 240 × 240 studies, composition, typography, adaptive playback halo
and Bloom vocabulary. The user-selected shipping default is **Instrument Teal**;
Studio and Editorial remain optional themes. The earlier prototype and the
finite-tweens-only / fully static idle policy are superseded.

This is an implementation plan, not implemented firmware, a new synthesis-engine
promise, or measured hardware performance. Browser demonstrations are not audio
measurements. The implementation deltas below bind the chosen appearance to real
musical state without redesigning it.

All distances are **native 240 × 240 LCD pixels**. Durations below are proposed
design targets to verify on the FM-1, not measured device results.

## Palette and fixed composition

Use the latest reference's exact **Instrument Teal** tokens by default:[^palette]

| CSS token | Role | Default value |
| --- | --- | --- |
| `--bg` | Background | `#061b1e` |
| `--fg` | Primary chord, values and labels | `#f2f8f4` |
| `--muted` | Secondary text | `#89a7a5` |
| `--line` | Neutral separator | `#355054` |
| `--subtle` | Subtle surface / inactive step | `#113034` |
| `--accent` | Teal-green accent | `#77dacb` |
| `--onAccent` | Text/mark on the accent | `#092d2c` |

Retain semantic recording/error red from the shared status tokens. Studio dark
and Editorial light are explicit optional themes from the chosen reference,
not additional motion modes. Changing theme does not change musical meaning or
start an animation loop. Do not substitute the former amber Studio default.

The approved static layout owns the coordinates. Its header, current-chord
nameplate, actual degree/quality, paired degree/name phrase rail, and four knob
columns remain fixed. Motion does not push neighboring text, change type size,
reflow the footer, or make essential information disappear while it settles.
Long phrases use an honest position/window; names do not shrink to make room
for animation.

With **Motion On**, retain the prototype's purposeful continuing motion as well
as its finite transitions: a restrained playback halo, real beat/step activity,
and musically driven Bloom/orbit treatment. Primary chord text and all essential
readouts stay anchored. Their readability must not depend on the current halo
phase or intensity. Motion strength adapts automatically; there is no separate
intensity, Quiet, Expressive or Reduced choice.

A stopped transport does not imply silence: live keys, HOLD and audible release
can still feed the adaptive response. Conversely, a running but silent/muted
transport must not produce a full-strength halo. When activity ends, its envelope
settles to the resting composition. A static outline may remain; do not claim
that a stopped or armed source is playing. No synthetic VU or waveform is made
from the intensity value, and no separate decorative BPM is introduced.

## Purposeful transition table

Let `p = clamp((now - start) / duration, 0, 1)`. Use **out-cubic**
`1 - (1 - p)^3` for a small content settle, and **out-quadratic**
`1 - (1 - p)^2` for an emphasis settle. There is no overshoot, spring, bounce or
elastic curve. A fixed-point equivalent is suitable; easing never changes the
musical data itself.

| Event | Target | Preferred presentation |
| --- | ---: | --- |
| Actual sounding chord identity changes | **120 ms**, out-cubic | Replace the old glyphs immediately. New chord glyphs settle from **+5 px vertically to 0**, inside the fixed clipped nameplate. Degree/quality and actual-note metadata commit immediately. Keep full text contrast; no crossfade of two readable chord identities. |
| Confirmed progression position changes | **120 ms**, out-cubic; acceptable tuning range 110–140 ms | Put the indicator at the **actual target slot immediately**. Its short underline settles from 60% to full width/normal emphasis. Never slide the cursor through other chord positions. |
| Knob value changes | **90 ms**, out-quadratic; tuning range 80–100 ms | Show the actual new number/value immediately. Briefly emphasize only that existing footer value/underline, then settle. Do not count through invented intermediate numbers or move its column. |
| Bank label changes | **160 ms**, out-cubic | New bank label enters by at most **10 px horizontally**, clipped **inside its footer bank region**. Direction follows the encoder turn. Header, loaded-sound identity and adjacent controls stay fixed. |
| Workspace changes | **170 ms**, out-cubic; tuning range 160–180 ms | Replace the content and settle the incoming main panel by at most **8 px horizontally**, clipped between the fixed header/footer. Forward/back directions are consistent. Navigation/control ownership changes immediately. |
| List focus changes | **100 ms**, out-quadratic | The exact target row is selected immediately; only its restrained focus treatment settles. Do not sweep a selection band through unselected rows. Scrolling is a clipped window update, not a full-screen pan. |
| Recorded countdown / capturing | **Musical clock**, not a free animation timer | Count and elapsed/bar/beat values reflect real transport events. Use the retained red REC status; avoid gratuitous blinking. |
| Successful save acknowledgement | **240 ms** short, nonblocking acknowledgement | Only after confirmed persistence, show the target name with Saved/check. Pending/Saving states have no success treatment. User input may dismiss the acknowledgement immediately. |
| New saved row arrives in an already visible collection | **180 ms**, out-cubic | After the short acknowledgement, settle the named row by at most **4 px vertically** within the list. The underlying record is available immediately; never delay data availability for choreography. |

### Preferred chord replacement

Use the restrained vertical replacement above. It preserves the modern
typography without needing a generic glyph squeeze/resampling path. The old
identity is removed on the same display update that accepts the new actual
identity; it is not left looking like the active chord for half a transition.
The incoming glyph block has a small presentation offset within its fixed
nameplate, while degree/quality and layout anchors stay fixed.

Repeated attacks of the same chord do **not** restart a name transition.
Arpeggiated notes do not repeatedly replace the parent chord label. If an
intentionally selected audition or live takeover changes what is actually
sounding, update that actual identity, independent of the scheduled phrase
cursor. Silence may retain a dimmed **Last** chord; it must not claim **Now**.

A horizontal squeeze remains an unselected exploration option, not a second
runtime style to implement alongside this one.

## Musical state is authoritative

Use one accompaniment clock for chords, compatible drum patterns, beat/cursor
position and recording boundaries. A visual clock does not play notes, schedule
drums, quantize a take or decide that a queued setup has committed.

- **Now** comes from the accepted sounding-note/ownership state. The phrase
  indicator comes from the player's actual slot. A root/quality match can light
  several repeated chords and is not proof of sequence position.
- Pending idea/groove changes stay explicitly pending until the musical boundary
  actually commits. Animating a label does not make a pending metre or key active.
- Direct key/tempo edits display their actual applied value. If an edit is queued,
  the active value and the pending request remain distinguishable; do not tween a
  number as if it were the running clock's intermediate tempo.
- External Receive shows EXT/Waiting until appropriate input. Lost clock stops
  accompaniment, preserves its last position and independent live HOLD, and
  requires the documented explicit resume. A visual tween never bridges clock
  loss with fake movement.
- Entering Sound, Songs, settings or another workspace does not change PLAY's
  source/owner or cancel musical recording. The new control labels become active
  immediately; no old screen remains interactive during a transition.

## Lists, banks and interrupted input

Bank browsing changes the candidate bank/filter and its footer label. It does
not silently replace the sounding preset unless the actual sound-selection
action requests that change. An empty bank stays a recoverable empty view while
the loaded sound remains intact.

Each animated region has one bounded latest target, not a queue of detents or
note names. A newer event replaces that region's target and cancels obsolete
completion callbacks. Retarget from its current small presentation offset toward
the settled anchor; do not stack offsets, replay every skipped value, or let a
stale callback restore an old label. Coalesce several changes received before a
draw into the newest complete semantic snapshot.

A phrase jump/wrap commits to the exact destination immediately. Its underline
can settle there, but no intervening slot is highlighted as playing. On a list
jump, selection/row number also commits immediately. If a window must scroll,
ensure the true focused row remains named and visible throughout; do not leave a
focus band pointing at a different row's text.

HOME, panic, stop, a new workspace, or Motion Off can cancel finite visual work
immediately. A cancelled tween leaves a complete correct frame, not a partially
erased glyph or an offset pane. Stop ends transport-driven pulses immediately;
independent live HOLD and audible tails can retain their own bounded envelope
response while Motion is On. Panic clears sounding activity and settles the art.

## Recording and safe save sequence

Armed, count-in, capturing, ending-at-boundary, pending write, saved and error are
different states. Use a static outlined/filled REC mark plus a short label to
distinguish them. Count-in changes on the real musical pulse; elapsed and
bar/beat position follow the captured transport. A small clock tick can update
at a real beat, but the whole screen/REC dot must not blink on an unrelated
250 ms timer. No synthetic meter is needed to prove that recording is running.

Ending a capture creates the requested new numbered Song copy while retaining
the original Song. The new row can appear as **Pending** in RAM immediately.
Wait for the safe persistence boundary and validated write before showing
**Saved**. Preserve independent live HOLD, audible tails and the actual pending
capture if the device cannot write yet. Capacity/write errors retain the
recoverable content and never show a check/success frame.

After confirmed success, show the nonblocking 240 ms acknowledgement. If Songs
is already visible, follow it with the 180 ms row arrival. If the user opens
Songs earlier, the record is already there and can receive the same short
arrival without making the user wait. Otherwise keep the current musical
workspace; do not force a collection screen just to show the animation.

If the pending row already exists, change its status and use a restrained
180 ms emphasis there instead of inserting a duplicate. Deduplicate consecutive
save events by stable record identity. At any point a real input can dismiss
presentation feedback without undoing or repeating the completed write.

## One motion setting: On / Off

Expose exactly two user choices:

- **On — adaptive:** the finite transitions above and the prototype's musical
  halo/Bloom response are available. Volume and semantic active-part activity
  determine their strength automatically. Do not expose a manual intensity or
  alternate motion-style selector.
- **Off:** every pulse, orbit, breathing cycle, morph tween, glyph/pane/focus
  translation, bank transition, list arrival, glow cycle and CSS animation or
  transition is disabled. Essential musical values still commit immediately to
  the correct settled frame. A real sequence step or automation value can change
  as data; there is no interpolated visual journey between those states.

Off cancels scheduled visual-only work, settles offsets and removes live effect
phases immediately. Do not keep an effect RAF/timer running invisibly, merely
hide its element, or restart an old animation when returning to On. A browser may
still service its shared musical simulation clock; it must not run a separate
visual loop for an Off or hidden view. On resumes from the current complete
musical state, not a queue of missed beats or previous transitions.

The browser's system reduced-motion preference may enforce effective Off behind
these same two choices. It is an accessibility override, not a third setting or
a second intensity menu. If needed, show a short status such as “Off · system
preference” beside the control. On hardware without that host preference, the
explicit display choice is the authority. Off does not stop sound, recording,
real automation or transport, and must not prevent input/status updates.

## Adaptive intensity from safe metadata

The following is a proposed cheap baseline to tune on the device, not a measured
performance result. Keep a small fixed-size snapshot, published by the owning
musical/audio code and consumed safely by the foreground renderer:

- Normalized output volume `V` in `0..1`, using the actual volume mapping rather
  than assuming a raw knob integer is linear acoustic loudness.
- A semantic active-part mask, typically **Chord, Bass, Drums**. An arpeggiated
  chord remains the Chord part; six chord notes do not become six instruments.
  Count a genuinely independent retained part only once if it is actually
  sounding. A selected, muted, empty or merely armed part does not count. Release
  tails do not add new parts, and effect returns do not inflate this count.
- Optional actual post-gain aggregate level `L` in `0..1`, plus validity. Use an
  existing safe bounded peak/envelope if available. If new instrumentation is
  justified, accumulate only cheap scalar metadata in the audio owner and publish
  it at block boundaries; measure that work. Never run FFTs, copy/scan audio in
  the UI, or let the renderer walk mutable voice arrays or audio buffers.
- Actual accepted note/part attacks and the shared beat/bar phase. Use an event
  sequence/counter or equivalent bounded handoff so a late draw does not replay
  old attacks. Metadata publication must not take a display-held audio lock.

The part owner provides semantic activity; the UI does not infer it by counting
voices or readouts. Use a short **150 ms decrement hold** on the display's part
count to avoid changes between adjacent drum hits or tiny ownership handovers;
increases can take effect immediately. This hold affects art only. Musical part
mute/stop and note releases retain their normal immediate behavior.

A concrete starting calculation, all clamped to `0..1`:

```text
N = number of active semantic part bits, after the short display-only hold
D = min(N, 3) / 3
activityGate = 1 if a part is actually active, else 0
level = L                         if valid post-gain level is available
        V * activityGate          otherwise
raw = level * (0.80 + 0.20 * D)
raw = 0                           if output is muted / volume is zero
tau = 60 ms when raw > I, otherwise 450 ms
alpha = dt / (tau + dt)
I = I + alpha * (raw - I)
```

This is one scalar envelope, with **60 ms attack / 450 ms release** as provisional
time constants, not promised completion durations. `dt` is real monotonic elapsed
time, not an assumed frame count; no exponential, per-voice analysis or per-frame
allocation is required. The post-gain branch does not multiply by `V` again.
Missing, invalid or stale level metadata selects the fallback; it must not leave
a previous loud snapshot driving the art indefinitely. If level metadata is unavailable,
the fallback is explicitly **volume-control / activity based**, not measured
loudness. If only volume is available, it can scale accepted musical attack/beat
cues directly; do not invent a level meter or infer new parts.

For the final drawing envelope, use a small resting gate: enter a visually
active state above **0.04**, leave it below **0.02** after **150 ms**, and settle
values below **0.01** to zero. These are provisional normalized thresholds, not
an audio noise gate. They prevent a near-zero halo from scheduling redraws or
flickering between states. A valid audible FX tail may keep `L` nonzero after
`N` falls to zero; its art can decay naturally without counting FX as an extra
instrument. Attack cues can refresh a bounded short accent, not accumulate more
rings, opacity or overlapping animations on every note.

Map `I` to a small, bounded set of effect parameters: halo opacity/radius range,
Bloom ring emphasis and allowed orbit excursion. For example, begin with halo
peak opacity `0.10 + 0.25*I` while active and radial excursion `3 + 8*I` native
pixels. These are tuning seeds, not a promise to reproduce a browser blur on the
LCD. At rest the accent peak is zero, not a perpetual 0.10 pulse. Keep primary
text and essential footer contrast constant; do not make louder playback brighten
the entire screen. At the same volume, more actual independent parts can add
modest visual weight, without letting polyphony dominate the composition.

A visual limiter caps peak opacity, excursion, effect-layer count and intermediate
frame work. It can clamp `I` or omit a halo frame if the measured UI budget is
exceeded. It must never change the beat clock, shorten/lengthen a bar, quantize
notes, alter gain, or misreport the actual position to make art look smooth.

## Applying the reference across all nine views

Preserve the nine reference views; this motion contract is not a route-removal
plan. Their animation sources differ:

| Reference view | Authoritative state and permitted On response |
| --- | --- |
| Play / Stage | Current chord and actual phrase position commit immediately. Halo strength uses `I`; a beat accent uses actual shared beat phase and an accepted live attack can cause one short accent. A live chord does not fabricate progression travel. |
| Chord library | Focus uses the 100 ms treatment; an audition accent follows the accepted sounding audition. Browsing alone must not act as audio playback. |
| Progression editor | Edit focus and playing cursor are distinct. Timeline ticks follow the actual accompaniment clock, not the selected edit slot. No sweep through falsely playing chords. |
| Live control banks | Values are actual applied/pending values; 90 ms response and 160 ms clipped bank arrival confirm input. Do not animate numbers or imply that a selected bank has changed sound. |
| Groove / Drums | The step marker follows the real drum clock; accepted drum hits may feed the shared envelope. A stored step pattern is not proof a drum hit sounded. Muted/empty/incompatible grooves do not produce hit accents. |
| Harmony suggestions | Focus/audition/insert are explicit events. No automatic cycling through candidates or animated ranking presented as measured probability. |
| Chord morph | The fill/position follows actual musical morph progress or a real control value. Bind the reference's independent CSS morph cycle to actual musical progress. Motion Off shows the current true value immediately and disables visual interpolation, without disabling real musical automation. |
| Generative / Bloom | Retain the ring/orbit/seed study, with amplitude/opacity bounded by `I`. Where the musical generative engine supplies progress, use that; otherwise derive one restrained revolution from an explicit shared **four-bar** phrase phase. Replace the adaptive seconds-based orbit and 2.22 s breath clocks with shared musical phase. Do not claim a generator is running when it is merely armed. |
| Appearance / Settings | Retain theme selection and exactly Motion On / Off. Theme changes may use the finite workspace treatment only while On; Off switches directly. Playback-pulse and parameter response obey the one motion setting rather than creating extra motion choices. |

Temporal phase comes from the same transport beat/bar position everywhere, or
from an accepted live attack's short elapsed-time envelope. There is no
independent motion tempo and no effect that schedules the next musical event.
Use the snapshot's tempo/metre; changing tempo, external Receive waiting/loss,
Start/Continue and phrase resets update the musical phase authority, not a CSS
loop that gradually drifts back into sync. A source without a running beat may
show a held/tail envelope and accepted attack response, but not pretend a free
orbit is the next chord clock.

## Browser preview honesty

The interactive preview may offer **simulated volume** and **simulated active
parts** to demonstrate adaptation. Label them as simulation outside the native
LCD. They are not microphone input, measured output loudness, an audio analyser,
or evidence of FM-1 performance. Its transport clock is a deliberate demo of the
shared musical state; CSS's prototype periods are not the production clock.
All nine views consume the same supplied snapshot. Motion Off and the system
accessibility override must affect every view, including halo, Bloom, morph,
header indicators and theme/parameter transitions.

## Implementation deltas from the supplied adaptive reference

Preserve the selected look and all nine studies. The following are behavior and
engineering adaptations, not permission to replace the visual design:[^behavior]

- **Real inputs and smoothing:** `getAdaptiveEnergy()` currently uses
  `clamp(0.08 + 0.92*(0.58*(volume/100)^0.8 + 0.42*(layers/8)), 0.08, 1)`.
  These are unsmoothed simulated slider values with a permanent 0.08 minimum;
  they do not measure an audio signal. Replace that presentation-only model with
  the safe metadata/envelope contract above, including true quiet/rest behavior.
  Preserve the appearance across the useful intensity range; do not let silence
  masquerade as audible activity merely to satisfy the minimum.
- **Parts, not capacity:** the simulator's **0–8 layers** range illustrates visual
  density. It promises neither eight independent playable instruments nor eight
  extra audio voices, buffers or layers in the FM-1. Use active semantic parts,
  not total polyphony. The nine studies also do not promise new Morph/Bloom audio
  engines; their real musical capabilities and release phases belong to the
  product implementation plan.
- **One timing authority:** the beat halo is already triggered from demo transport
  ticks. Retain its visual intent, but source those ticks from actual musical
  state. CSS morph duration varies from 10 to 3.5 seconds, orbit duration from
  18 to 7 seconds, and Bloom breath repeats every 2.22 seconds independently.
  In firmware, derive their phase from actual morph/generative progress or the
  explicitly shared phrase phase. Energy changes amplitude/emphasis, not tempo.
- **Pause, Off and accessibility:** pause/stop must cancel an in-flight beat
  accent, freeze transport-driven phases at a correct settled state, and preserve
  independent live HOLD/tail response. Off cancels all effect paths, including
  existing Web Animations/CSS transitions and visual-only loops. A change to the
  host reduced-motion preference must apply effective Off immediately, including
  an already running beat accent; it cannot rely only on checking the preference
  at the next beat. Returning to On uses current state without replaying events.
  The static morph value in Off must be the true value, not a hardcoded 56%.

The supplied reference remains unmodified. Its source initially selects Studio;
the latest user instruction selects Instrument Teal as the implementation default.
No firmware flashing, capacity increase or performance result is implied by
importing or planning from this reference.

## Rendering and resource contract

Run easing, text layout and drawing in the foreground, outside audio callbacks.
Input, note releases, transport/MIDI/USB service and the audio deadline take
priority over any visual step. A tween is sampled from monotonic elapsed time;
if drawing is late, skip to the current/final presentation state rather than
queueing catch-up frames.

Cache text layout/glyph data and the settled screen. Redraw only the changed
nameplate, exact phrase-mark region, changed footer cell, incoming pane or list
row. Clear the union of the previous/new bounds so translated glyphs cannot
leave trails. A fixed-size glyph blit/translation does not require a full generic
scalable-font renderer or a second full-screen buffer.

Keep animation state and caches explicitly bounded and measured. Do not allocate
per-frame objects or write flash to record animation progress. Use the chosen
display backend's proper pixel format; the existing indexed CF framebuffer is
not a drop-in destination for RGB565 text calls. Reusing tile transfer gating is
useful, but skipping unchanged composition/hashing is also required.

Any frame-rate/render-work ceiling is provisional until measured against the
actual panel transfer cost and audio workload. Do not promise 30/60 FPS or a CPU,
battery or polyphony gain from this design. When a visual update cannot fit the
measured budget, reduce/skip its intermediate frames while preserving the actual
musical value and timely input handling. Cache any ring/halo geometry and use
bounded precomputed masks or inexpensive outlines before considering blur or a
second full-screen buffer. Rendering nine browser specimens simultaneously is
not a proposed hardware workload: only the visible device view needs its art.

With Motion Off, or an On envelope settled to rest with no active finite
transition, schedule no visual-only redraw. While On is active, update only the
regions whose sampled art/state actually changed. Skip effects for hidden views;
returning to one presents its current complete state, not missed animation.
Do not request visual-only level instrumentation while Off if it can be disabled
safely; existing musical/audio metadata continues for its actual owners.

## Measurement and acceptance

Use matched before/after source/build identities, linker maps and the existing
physical-device diagnostics/workload capture. Report ordinary RAM, POOL, XIP
and any RAM-resident code separately. Host/browser animation timing is not a
measurement of the FM-1.

Verify on the device with long-release six-note chords, linked bass, drums,
effects, and USB capture at both supported rates:

- First musical/display state is correct before any animation completes.
- Repeated chords, fast chord changes, phrase wraps/jumps and rapid encoder turns
  produce no stale names, false intermediate cursor positions or wrong targets.
- Stop, panic, HOME, external-clock loss and note releases remain prompt while a
  transition or save acknowledgement is active.
- Full/partial rendering gives the same final pixels. Off and an inactive,
  settled On view draw/transfer nothing until actual visible data changes. Off
  leaves no effect timer, CSS animation/transition or effect-only RAF running.
- Switching On/Off during every kind of transition, halo, Bloom/orbit and morph
  settles correctly in all nine views. Readouts, actual automation, sequence
  steps and record status still update immediately in Off. A host reduced-motion
  override exercises the same complete Off path, with no third user choice.
- Sweep simulated and device volume through zero, low and high, with zero/one/
  several active parts, held chords, staccato drums, muted parts and release/FX
  tails. Intensity stays bounded, releases calmly and has no near-zero flicker.
  Increasing chord note count alone does not count extra instruments. Compare
  post-gain metadata and fallback paths honestly; invalid level data never
  becomes a synthetic measured-level claim.
- Tempo changes, phrase jumps, odd metre and external clock Waiting/loss/resume
  do not reveal drift between decorative phase and the real cursor/beat. The
  visual limiter never changes the musical clock or sound. Confirm that hidden
  views stop effect work and do not replay old attacks when shown again.
- Bank changes are clipped to the intended footer region; no adjacent value,
  actual sounding-sound label or header slides accidentally.
- Record countdown and cursor events align with the one musical clock; write
  acknowledgements follow actual successful persistence, including interrupted,
  full-capacity and failed writes.
- Input/display service gaps, worst same-half audio work+lag, late/stale/shed
  counters and USB loss are compared under identical workloads. Define final
  release thresholds from the physical baseline; no favorable average hides
  newly missed deadlines or voice shedding.
- The typography is readable at native size during every sampled frame,
  including accidentals, extensions, small footer values and selected list rows.

Engineering sign-off records the measured costs and any chosen reduction in
intermediate visual frames. It does not report untested hardware benefits from
a browser preview.

## Reference notes

[^reference]: Latest supplied file: `fm1_adaptive_motion_prototype.html` (29.8 KB
    in the source listing). Repository copy:
    [instrument-teal/supplied-adaptive-motion-prototype.html](instrument-teal/supplied-adaptive-motion-prototype.html).
    SHA-256: `4b26061f6e0ffd0b524b10bdf8beb76d9e40cd9cbf848fe04eefd03864fe29d8`.
    The earlier `fm1_motion_prototype.html` is historical, not the active reference.

[^palette]: Latest supplied reference, CSS selector
    `body[data-theme="instrument"] .screen`; all seven values above are copied
    exactly. The default-theme change is the user's subsequent explicit choice.

[^behavior]: Latest supplied reference: `getAdaptiveEnergy()`, `updateAdaptiveUI()`,
    `triggerBeat()`, `clearBeatAnimations()`, `updateMotion()`, `renderClock()` and
    CSS `.morph-fill`, `.evolve .seed`, `.evolve .second`, plus the
    `prefers-reduced-motion` rule. These observations describe this source file,
    not measured FM-1 execution.
