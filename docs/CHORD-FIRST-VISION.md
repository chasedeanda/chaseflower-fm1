# A chord-first FM-1

Design proposal, November 6, 2026. This records the direction discussed in the
current chat and the Progressions discussion in
the earlier progression and harmony design discussion.
It proposes future behavior; it is not a firmware implementation or release note.
The linked chat was still implementing its latest progression changes when read.

## The instrument's purpose

Find a progression, understand it by playing it, add a groove, develop the idea,
and save it so it sounds the same when recalled.

The primary saved object should be a musical idea: progression, chord timing,
key/mode, chord voicings, groove, tempo, chord/bass sounds, motion and effects.
Sound design remains available, but the first screen and the shortest paths
should serve playing and writing chords.

## Latest direction and priority

The follow-up decision is explicit: **press PLAY and hear the selected
progression, with drum-pattern, key and tempo changes while it runs, comes next**.
Then deliver chord editing, then learning, then simple offline suggestions.
Saving/editing/teaching must not delay the first useful playback milestone.

The user selected **`fm1_adaptive_motion_prototype.html` as the visual/motion base**,
building on the quiet, typography-first nine-screen image. Preserve all nine
prototype screens, including Chord Morph and Generative / Bloom studies. Keep
the large chord hero, aligned text, thin separators and four physical control
columns with a visible bank indicator; the shared clock and functional motion
should feel alive, not unrelated or loud.

**Instrument Teal is the default**, using the latest supplied HTML's exact
palette: background `#061b1e`, foreground `#f2f8f4`, muted `#89a7a5`, line `#355054`,
subtle `#113034`, accent `#77dacb`, on-accent `#092d2c`. Studio and Editorial are
optional palettes of the same layout. Custom accent remains solid. The earlier
Coral Bloom, Aurora and Studio Rack explorations are historical, not selected;
the supplied prototype's own Morph/Bloom studies remain in the approved nine.
Preserve the supplied adaptive-motion HTML byte-for-byte; working adaptations
and the default Instrument Teal choice belong in separate copies.

The CORE bank's HOME controls are **K1 Rhythm / K2 Key / K3 Scale / K4 Tempo**.
Tempo's promotion reflects the new playback priority; Glide remains available
in TEXTURE and chord/motion detail. The earlier K4 Glide assignment is historical.

The owner's latest adaptive-motion HTML now governs the visual study. Preserve
its exactly two-choice **Motion: On / Off** preference: On responds to smoothed volume/output
level and the number of active instruments; Off stops all animation/transitions.
There are no separate Quiet, Expressive, Reduced, Calm, Intensity or Speed motion
settings. An OS reduced-motion preference may privately force effective Off.

Retaining all nine studies does not silently implement every pictured musical
capability or unsupported control. Preserve the established Harmony rules,
deliberate modifiers and truthful recording/save contracts. Morph/Bloom are visual
studies until separately designed, implemented and verified as musical features.

The next design requirement is a **complete screen/workflow board before
engineering**, with the same definitions serving the README/player guide. Cover
entry/exit routes, every physical control, retained advanced pages, ordinary and
important empty/error/confirmation states, and all planned delivery phases.
Keep user instructions and engineering state contracts aligned from canonical
screen/workflow data. Label this as planned successor behavior until released.

On preserves the prototype's beat accents, moving steps, chord transitions and
functional parameter/bank/focus/recording responses. Their strength follows one
smoothed, bounded output/active-instrument response, not another user style
selector. Musical anchors stay legible. Off leaves the complete correct settled
screen: current chord/degree, notes, cursor, controls and record/save state still
update instantly. Prototype-supplied levels and the up-to-eight layer demo are simulations, not
hardware measurements or a promise of eight device parts/engines/tracks/voices.

## Decisions carried forward from the other chat

These are established user preferences from the prior discussion. The selected direction supersedes the grid and exploratory skins. CORE has
direct tempo; Glide remains in TEXTURE/detail.

- Progressions is the main/default HOME screen. ALGORITHM on HOME cycles
  **Progressions → Bass → Bonfire → Nothing**. Keep animations available behind
  Progressions for now; eventual removal is possible, not yet decided.
- SELECT browses one complete wrapping list of ideas. A key, scale or tag must
  not hide entries. Genre, mood, artist and song-section labels describe ideas;
  they are not mandatory filtering steps.
- Browsing automatically changes the idea's starting key and scale. There is
  no separate Use/Apply action. Show and apply the idea's time signature too.
- HOME K1 selects the rhythm-machine preset without leaving Progressions.
  In CORE, K2 is Key, K3 is Scale and K4 is Tempo. PRESETS stays global chord sound.
- Keep **Now**, showing what is actually sounding. Light every matching chord
  card as the user plays; repeated instances of the same chord can all match.
  Extensions and inversions should not obscure a matching root/major-minor
  identity. Do not mistake a pitch match for proof of the player's position
  in the progression.
- The catalogue should be exploratory: all seven modes and every supported
  metre, including **2/4, 3/4, 4/4, 5/4, 6/8 and 7/4**. These are supported
  metres, not a promise of every possible time signature.
- Keep the library broad, rather than restricting it to the user's favorite
  genres. Include pop, rock, jazz, folk, blues and modal exploration, with strong
  representation for pop punk, singer-songwriter, emo and math rock. The named
  touchstones are The Story So Far, Movements, Paramore, Polyphia, Intervals
  and CHON.
- Include identifiable, sourced song **sections**, alongside original writing
  prompts. The user particularly liked the Paramore examples. Label artist,
  song and section, and distinguish a simplified harmonic seed from a faithful
  transcription. A whole song may have several distinct sections.
- The catalogue discussion started with a proposed 40 curated ideas; the user
  asked about 100. The later expansion was headed toward 56, covering the seven
  modes and supported metres. **100 is a growth direction, not an agreed quota**.
  Prefer distinct, useful ideas to repeated entries added to reach a count.
- Keep Harmony simple: **Off** leaves normal playing alone; **On** builds the
  complete major/minor chords requested by the user. Seventh selection follows
  chord quality. Preserve **Emaj7 → Bmaj7 → D♯m7** in E major with 7 selected.
- Do not restore automatic Rich/Dreamy/Custom harmony layers, silently add
  extensions, or make an exploratory tag change the harmony rules. Modifiers
  remain deliberate choices on the chord buttons.
- Include a next-chord suggestion workflow inspired by the user's
  [Hooktheory Trends reference](https://www.hooktheory.com/trends). Suggestions
  should help the player build a path rather than only choose a complete preset.

The older `PHASE3-PLAN.md` describes the original filtered GLO-page design and
retired harmony styles. Where it differs, the newer user decisions above govern
this proposal.

## The most valuable additions

### Hear the selected progression

On Progressions, **PLAY starts the selected progression immediately**, beginning
at slot 1; PLAY again stops it and releases accompaniment-owned notes. The default
is chord playback with the selected compatible groove, including a Drums Off
choice so the progression can play alone. Offer **Drums** and **Chords + Drums**
as explicit accompaniment choices. Manual
playing over premade drums remains immediate. Automatic chord playback adds
per-slot durations, a visible playback cursor and synchronized drums, so a
progression can be heard and developed before recording a take.

Use one musical clock and one transport for that accompaniment. Viewing a
different screen must not change what PLAY controls. Browsing does not start
sound unexpectedly. When accompaniment is running, a newly selected idea can
be queued automatically at a bar boundary with **Next** visible; it needs no
confirmation. Key, scale, metre and compatible groove must switch together.
In manual/stopped use, browsing updates the setup directly, as requested.
During a queued transition, keep active setup values visible and identify the
pending idea separately. Do not show a pending key as if it already described
the sounding chords. HOME knob changes target the active idea until the switch.

Proposed live-change policy: groove changes at the next bar; key/scale changes
at the next chord onset; tempo changes promptly while retaining musical phase.
Coalesce rapid turns to the latest complete request. Under external MIDI clock,
show external tempo and make local tempo editing unavailable. Keep the current
quarter-note tempo convention for 6/8; a 6/8 bar at 120 quarter-note BPM lasts
1.5 seconds. Use the actual player's slot cursor, separately from root/chord
matches. Preserve live HOLD and independently owned notes when transport stops.

### Make the progression editable and transposable

The source has since expanded to support up to eight catalogue slots. The new
display should show a compact phrase cue/window, with the active position clear;
it must not hardcode a four-chord playback limit. Each
slot has a scale degree/root, explicit chord quality and modifier choices, a
duration, inversion and bass note. Keep the main view readable while opening one
slot for detail. Give the new data model a bounded variable-length phrase rather
than inheriting a permanent four-chord limit; longer song sections and forms
such as twelve-bar blues must retain their timing/repeats. Show longer phrases
in windows, with a visible position. A/B sections can follow without showing
eight or sixteen tiny cards at once.

Store the editable harmony as degrees/chord choices, with an explicit fixed-root
override when needed. Current Auto recordings capture fixed notes; changing key
does not turn those recordings into transposable progressions. Preserve existing
takes and add semantic progression storage as a new layer.
Include one-step undo and a recoverable working draft with the first editing
version: browsing a factory idea must not discard the user's unsaved progression.
Returning to the draft should be an explicit visible action, without adding an
Apply step to normal browsing.

### Teach the sound the player is hearing

SEL initially inspects **Now**, or its last sounding chord during silence; label
that target. In the inspector, SELECT can switch to a numbered progression slot.
EDIT uses the last explicitly focused slot. Without one, it can use a unique
matching card, otherwise default to slot 1; repeated chords never establish a
sequence position by themselves. The editor always names its slot. For example:

```text
Bmaj7       degree 5 in E major
B D♯ F♯ A♯
1 3 5 7
A♯: outside E major
```

The established Auto rule intentionally differs from strict diatonic stacking.
The screen must name actual notes and actual chord quality; it must not label
Bmaj7 as a diatonic V7 or imply that a forced perfect fifth belongs to every
mode. This matters especially for Locrian ideas. Keep Auto's agreed behavior;
make teaching accurate through explicit labels and comparisons. Exact song
reproduction can later use deliberate chord overrides rather than changing the
global Auto rule.

Add short audible comparisons: triad versus the selected extension, root position
versus Smooth inversion, or the original ending versus one substitution. One
comparison at a time is more useful here than paragraphs of theory.

### Suggest what could come next

[Hooktheory Trends](https://www.hooktheory.com/trends) explores next-chord
probabilities from its song database and identifies matching songs/sections.
Its [published API documentation](https://www.hooktheory.com/api/trends/docs)
describes both next-chord and matching-song endpoints. This is a promising
reference for the interaction, not evidence that the firmware already contains
or can redistribute those statistics.

Put **Next** beside the chord inspector, not on the HOME groove knob. Use the
last few committed chord changes, the active key/mode and actual chord quality
as context. Ignore arpeggiated note events, repeated triggers and mere inversion
changes when building the harmonic history.

Show three candidates at once, each with the chord name, numeral/root-key hint,
and one short reason such as Return home, Continue, or Contrast. SELECT explores
candidates. SEL explicitly auditions the selected candidate; EDIT inserts it
into the selected draft slot. Automatically refresh recommendations as the player
develops the progression, but never automatically replace or play their next
chord just because a recommendation changed.

Begin with a compact offline table/rules built from the curated library, with
ranking that can consider sequence context, voice leading and a route back to
the tonic. Label a rule-based ranking as suggestions. If displaying a measured
frequency, identify its corpus: frequency in our small library is not a
Hooktheory-wide probability. Popularity means common in a dataset, not a guarantee
that a chord is the best artistic choice. Give less familiar continuations room
alongside common ones.

Retain the simple Auto rules. An explicitly auditioned/inserted suggestion can
carry its own chord quality; it should not quietly alter global Harmony or
latched modifiers. Treat modes with little example data as theory-guided
fallbacks rather than pretending that all modes have equally strong statistics.
Later, a computer-side curation tool could use an authorized data source to
prepare richer offline tables; live network access need not be part of playing.

### Let drums support the harmony

K1 remains the fast groove control. SEQ opens groove detail: kit, level, swing,
and supported variation/fill controls. The idea's metre is authoritative;
K1 browses compatible grooves. A missing groove should have an explicit
Drums Off/Click fallback rather than silently switching a 3/4 idea into 4/4.
An explicit rhythm-library view can expose the whole drum bank when desired.

Preserve the user's tempo while browsing grooves; show a suggested BPM
separately. Do not silently replace an intentionally chosen groove every time
the user explores a different progression. Restore a compatible last choice for
a new metre, and show the resulting groove on HOME.

The current bank contains synthesized drum patterns. Importing audio loop files
would be a separate feature with storage and tempo-handling requirements, not
something to assume the pattern browser already does.

### Save the complete musical idea

SAVE should default to the whole idea. Recall restores the progression, slot
edits, timing, groove, sounds and relevant performance settings together. Keep
sound-only saving available as an explicit choice. Use a separate versioned
record and stable catalogue/groove IDs; preserve existing sounds and patterns.

Later additions worth pursuing are A/B sections, duplicate-and-change-one-chord
and an alternate ending. Undo accompanies the first editable draft.
Favorites/recent ideas may be useful supplementary
views, but they must never replace access to the complete list.

## A UI for 240 × 240 pixels

Use the selected type-led Instrument Teal interface. The large actual chord is the hero;
compose root, quality and extension clearly and keep the degree above it. Use
flat color, aligned text, thin separators and restrained selection marks. The
same geometry supports Studio and Editorial palettes. User/player prose
belongs in detail or the guide, not on the 240 × 240 screen.

- Top: quiet idea/section identity and compact transport/metre information.
- Center: the large actual sounding chord, with root/quality/extension composed
  clearly and a quiet note row if space permits.
- Preserve the old screen's scale-degree information in both contexts: the
  sounding chord has a compact degree/quality line (for example **vi · minor**
  beside Dm), and each planned chord has its numeral above its note/chord name.
  In F major, that reads **IV / Bb, V / C, vi / Dm, I / F**. Uppercase is major;
  lowercase is minor, following the actual resolved quality. Notes can remain
  on a separate quiet line; they do not replace scale-degree information.
  The arranged seventh-color demo is **IV/B♭maj7 → V/C7 → vi/Dm7 → I/Fmaj7**;
  C7 is C E G B♭. This explicit planned V7 does not change the live modifier
  contract **Emaj7 → Bmaj7 → D♯m7**, where 7 deliberately follows major/minor
  quality and may introduce an outside-scale tone.
- A restrained ordered progression rail uses those paired degree/name labels
  to show position and the next chord. Key transposition changes names while
  preserving degrees; changing mode can change both names and numeral case.
  Longer phrases use a visible window/position; do not shrink eight labels to fit.
- Bottom: four text control columns, with the bank name and count/dots visible.
  CORE is **Rhythm / Key / Scale / Tempo**, in K1–K4 order.

Keep planned slot names, actual sounding notes, live matches, playback position
and edit focus distinct. Repeated matching chords do not identify sequence
position. The chord anchor is authoritative for actual sound; the rail explains
where the automatic progression is, or what is planned during manual playing.
Put extended metadata and root-key guidance in browse/inspect detail as needed.

The chosen motion prototype supplies the musical behavior: beat/playhead,
actual chord transitions, parameter/bank/focus feedback and the retained
Morph/Bloom studies. On adapts strength to smoothed output/volume and active
instrument count. Use actual playing/ownership and a measured output envelope
when available; do not invent hardware audio/velocity/FFT data. Off stops every
animation and transition while musical/status changes update directly. Do not
add another motion intensity, speed or style control.

Accent preferences belong in Display detail rather than taking a HOME knob.
Let Instrument Teal/Studio/Editorial and a custom solid accent preview directly, using
the same layout. Gradients, glow and reactive color fields are not active style
options. Motion Off settles event transitions while musical/status values update.
Preserve contrast and non-color position/focus cues, persist checked/versioned
preferences safely, and measure idle/reactive display costs before shipping.

From another screen or secondary scene, HOME returns to Progressions CORE.
While already on Progressions, tap HOME to cycle CORE → SOUND → TEXTURE → GROOVE.
This recommended reuse of the existing HOME-view cycle requires physical-device
verification. ALGORITHM retains its normal scene/tab role; SELECT remains the
complete catalogue on HOME and chooses items in detail. OCT−/OCT+ remain musical.
There is no hold chord, touch interaction or assumed encoder press.

The additional audited banks are SOUND: Chord sound / Bass sound / Chord level /
Bass level; TEXTURE: Reverb mix / Delay mix / Chorus mix / Glide; GROOVE: Kit /
Drum level / Swing / Click. PRESETS remains global chord sound. Switching banks
alone changes no sound, parameter value, note ownership or transport state.
Human/Fill and new harmony-color controls from a reference image are not treated
as implemented parameters. Controls detail explains the bank cycle explicitly.

## Proposed button destinations

This is a new navigation proposal, not a claim about the current controls.

| Physical button | Destination or action |
| --- | --- |
| HOME | From elsewhere: Progressions CORE; on Progressions: cycle the four live banks |
| SEL | Inspect chord: Now, Next suggestions, notes/intervals, voicing comparison |
| SEQ | Accompaniment: Groove, Loop, Arrange, Click |
| ARP | Chord motion: existing arp first; pulse/strum later |
| EDIT | Edit the progression's selected chord and timing |
| SAVE | Open Songs; save/recall progressions and recordings through explicit actions |
| PLAY | Start/stop the selected accompaniment mode from every workspace |
| REC | Record directly on HOME; every completed non-empty capture creates a new Song copy |
| FX | Effects, beginning with presets and useful musical controls |
| ENV | Sound envelope controls |
| LFO | Bass; retain a direct musical route despite the engraved LFO label |
| GLO | Harmony On/Off, MIDI/USB, Controls, Display, System |

MASTER remains volume. PRESETS remains chord sound. Bass sound/mix remains
available directly from LFO and in Accompaniment/Sound detail. Advanced synth
parameters stay reachable through **GLO → Sound → Advanced**, including deep LFO
and FM6/VA editing. Modifier assignment is **GLO → Controls → Modifiers** after
SEL becomes Chord Lab. They need not occupy the primary writing
workflow. Existing button remapping would require a complete navigation audit.

Within a workspace, SELECT chooses a slot/list row, ALGORITHM chooses a visible
tab, and the four knobs edit labelled parameters. SEL enters a highlighted
submenu item; the workspace's own button returns to its parent, and HOME always
returns to Progressions. Keep common musical actions within one or two screens.
Use long-presses only as optional shortcuts, not the sole route to a feature.

An example chord edit has **Degree / Quality / Color / Length**. Its Voicing tab
has **Inversion / Register / Spread / Bass**. These are local per-chord edits;
they do not reintroduce global automatic harmony styles. Modifiers can be
explicitly captured from the physical buttons into the selected slot.

## Delivery order

1. **PLAY the selected progression.** One synchronized chord/drum transport,
   clean start/stop, all catalogue slots/metres, live groove/key/tempo changes,
   Drums Off, visible actual chord and playback position. Preserve full SELECT
   browsing, automatic key/scale/metre and the agreed simple Harmony rules.
   Do not require copying into a sequencer or recording a take first. Establish
   the selected typographic HOME, four live banks and the supplied prototype's
   adaptive On / settled Off motion alongside this useful flow.
2. **Edit and save musical ideas.** Slot degree, chord quality/color, duration
   and voicing, plus recoverable draft/undo and complete idea save/recall.
3. **Learning.** Actual notes/intervals, accurate scale membership and audible
   voicing/chord comparisons, revealed on demand.
4. **Simple offline suggestions.** Next-chord candidates from a compact curated
   table/rules; deliberate audition/insertion. Broader song-linked statistics
   and advanced generation can follow. Grow useful catalogue breadth throughout.

The current rhythm, pattern and song players are mutually exclusive, so stage 1
needs transport integration rather than starting two independent players. Reuse
the existing chord/drum pattern tracks where practical. Avoid new work in the
audio callback for catalogue browsing or theory rendering. Verify the final
workflow on the physical screen and controls, with sustained notes, recording,
metre changes and USB audio running.

Keep broader generative systems, additional automatic harmony styles and unrelated
new DSP behind this sequence. The selected quiet interface belongs to the initial
UI. The central milestone is that
a progression, its groove and its saved identity feel like one instrument.

### Next playback milestone: acceptance criteria

- From Progressions HOME, one PLAY press starts slot 1 with its selected groove;
  a second press stops and releases accompaniment-owned notes.
- Every catalogue slot plays in order and loops, including eight-slot entries.
  Start with one bar per slot unless explicit catalogue timing is supplied.
- K1 changes the groove, K2 transposes, K3 changes mode, and K4 changes tempo
  without leaving HOME, losing phase or starting independent transports.
- Groove Off still allows chord playback. Changing grooves preserves tempo.
  Metre changes use a compatible groove/fallback and a coherent queued setup.
- All seven modes, twelve tonic positions and supported metres retain the
  established complete major/minor and deliberate-modifier behavior.
- Rapid changes coalesce safely; displayed active/pending values match playback.
- Stop/panic/restart do not leave notes hanging or erase independent live HOLD.
- Existing recorded takes retain fixed-note playback and their storage format.
  Playing/browsing causes no pattern mutation or flash save.
- Verify host event/MIDI timing and physical-device response with drums, long
  chord releases and USB capture. Inspect modern glyphs/tweens on the FM-1.

The first implementation should prepare a compact degree/duration plan outside
the audio renderer and publish it through the bounded event handoff. The plan
and groove consume one accompaniment tick source. Reuse existing note ownership,
MIDI and bass helpers; do not manufacture playback by simulating physical keys.

## Modern rendering without importing an entire UI

The local `cr_anim.c` already has small integer ease-out/spring helpers, and
`cr_draw.c` demonstrates a screen model with an unchanged-state fast path.
Borrow those ideas and the chord typography, while keeping this product's own
navigation and progression ownership.

The current CF framebuffer is 8-bit palette indices aliasing the RGB565 canvas
storage; existing `cr_text()` cannot simply be called into it. A purposeful new
renderer can draw a small set of modern components into RGB565 strips using the
existing canvas allocation. An incremental alternative is fixed-size antialiased
glyphs with indexed coverage ramps. Keep font/scratch and graphical-region
integration explicit for the selected typography-first interface.

Do not add the entire generic CR text renderer without measuring its budget:
the local implementation has 18,440 bytes of glyph/resampling/Huffman pool
scratch on the target, plus approximately 30 KB of font flash. Those are source
costs, not the necessary cost of a minimal modern chord view. The build currently
generates CF pixel fonts rather than `cr_fonts.h`. A focused font pipeline can
include only the needed glyphs/sizes. If the selected direction uses name squeeze,
constrain that resampling to its own rectangle. The reference's ring table is
currently const flash, not RAM. Do not add a decorative scene allocation.

Run motion in foreground rendering with bounded frames and input pumping; keep
string work, typography and graphics out of audio callbacks. Measure and cap
cadence, dirty regions, pixel work, scratch memory and foreground latency with
demanding audio and USB capture. Reuse settled layers and stop unnecessary redraw
loops, especially at idle and in Off. A browser study establishes no FPS, CPU or
polyphony promise.

## What to trim for a substantially new firmware

Treat this as a new musical instrument built on the dependable hardware/audio
foundation. Change its default workflow, navigation, saved musical object,
accompaniment ownership and visual language together. Keep the device drivers,
installer/recovery support, backups, note-release safeguards and USB recording
paths that make it usable.

| Area | Proposed treatment | Likely benefit |
| --- | --- | --- |
| Tag/key/scale gates and retired Auto styles | Remove from the product model, as already requested | Predictable browsing and harmony; small state/code simplification |
| Multiple performance layouts | Standardize the supported experience on Top Lock, root keys, modifiers and HOLD | Less interaction/state complexity; little inherent DSP gain |
| Bass/Bonfire and decorative boot | Keep secondary during exploration, then consider compiling all scene/boot graphics out of the new firmware | Real graphics-buffer and code savings; reduced UI work when scenes would otherwise run |
| Separate metronome/rhythm/pattern/step-editor entry flows | Replace with one Accompaniment workspace and transport | Large workflow improvement; resource gains require replacing redundant state/processing too |
| General four-track workstation features | First product focuses on chords, linked bass and drums; defer independent LAYER, mixed-kit lane editing and DAW-like grids | Simpler scheduler/storage; potential RAM and active synth-CPU savings |
| Full FM6/VA sound editor | Put in Advanced Sound or omit its UI after reviewing useful musical macros | Fewer screens/code paths; presets still have their normal DSP cost |
| Large patch library | Curate useful chord/bass/kit banks; avoid deleting presets just for speed | Mainly flash/catalogue savings, not less work for an unchanged playing sound |
| Other scales | Keep the seven modes central; put other useful scales in Advanced | Better selection; scale tables themselves are small and cheap |
| Diagnostic dashboard | Move to Tools/System; retain lightweight measurement and export | Clear musical HOME without losing evidence for optimization |
| Old format conversion | Retire after a documented migration path | Less compatibility code/buffer use; not a normal-playback CPU gain |
| USB audio/MIDI, updater, recovery, backup and guards | Keep | Recording, connectivity and reliable operation remain part of the instrument |

The existing source already compiles many old engines out: slicer/sample/grain,
FM4, CZ, QUAD and several other Felucca engines are disabled in `coralflower.c`.
The old `ui.c`, `cr_ui.c`, `cr_engine.c`, `cr_draw.c` and `chord.c` are absent
from the active compilation dependency closure. Deleting these can clean the
repository but does not make this binary faster. Several `cr_*` components
remain active, including effects, banks, MIDI and safety/core support. Audit
dependencies rather than deleting a historical filename prefix.

## Where performance can actually improve

1. **Compose only changed screen regions.** `cf_fb.c` redraws the whole 240×240
   picture for a UI frame, hashes 225 tiles, and sends changed tiles. Keep that
   existing transfer optimization, then add model/region invalidation so settled
   content avoids composition and hashing. Budget actual chord, parameter,
   transport and adaptive On motion explicitly; Off has no animation loop.
   Cache chord names, note spelling and candidate rankings
   until their musical inputs change. This targets UI CPU and display traffic;
   do not describe it as a measured DSP speedup.
2. **Compile out scene resources when retiring them.** `fl_px` is 120×70 bytes
   and `scene_body_px` is 80×48 bytes: **12,240 bytes** of dedicated POOL graphics
   buffers, before other scene state/code. Both the scene and decorative boot
   uses must be removed. These are candidate gross buffer savings, not a measured
   net linker delta. Normal Progressions already skips scene animation/rendering;
   hiding the artwork alone does not recover these static allocations.
3. **Consider removing the independent LAYER synth part.** The active build
   allocates three melodic engine parts and mixes four parts including drums.
   A chord+bass+drums product could reduce engine/voice state, and avoid LAYER's
   rendering when it would be active. Keep chord polyphony, optional linked bass
   and drums. Measure the linked changes and audition affected saved content
   before making this a product cut.
   Also audit the general sequencer allocation: eight `sq_pat_t` records occupy
   **20,288 bytes of ordinary RAM** (8 × the asserted 2,536-byte record), before
   container metadata. A compact chord/degree/duration model is a meaningful
   redesign target. It will still need its own memory and recording support, so
   this is an existing allocation to reduce, not 20,288 bytes promised free.
4. **Profile effects, release tails and USB in realistic combinations.** Measure
   chord changes with long releases, bass and drums, then chorus/delay/reverb and
   USB at both supported capture rates. Improve actual bypass/tail scheduling
   where it is costly. Several effects already bypass when disabled, and USB
   taps already run only while the host captures, so preserve those existing
   gates. Reverb's conservative 15-second idle path is a specific candidate for
   more accurate silence detection without truncating an audible tail.
   Retain useful musical space and sound character; deleting
   an unused patch or editor page does not reduce the DSP cost of the same sound.
5. **Keep suggestion and UI work outside audio deadlines.** Store compact
   progression/transition tables in flash. Compute theory labels, candidate
   ordering and save-menu presentation in the foreground; send bounded musical
   events to the shared clock. No network call, graph traversal or string
   formatting should be part of rendering an audio block.

Use the existing diagnostics with repeatable physical-device workloads, paired
before/after linker maps and the same notes, sounds, effects and USB settings.
Check audio work/lag, deadline/USB counters, input/display response and distinct
ordinary-RAM/POOL/XIP regions. Current documentation contains older build sizes,
not a measurement of the combined in-progress redesign. Do not promise a CPU
percentage or polyphony increase before measuring. Compiled-in second-core
support also does not mean a second worker is currently active or that enabling
it is automatically an improvement.

The full successor firmware should deliver a recognizable new instrument:
Progressions HOME, one accompaniment engine, editable/transposable chord ideas,
premade drums, complete idea saving, useful Next suggestions, a modern new UI,
and a measured resource budget. Its next milestone is specifically immediate
progression playback with live groove/key/tempo changes; editing, learning and
suggestions follow in that order. Source cleanup and rebranding accompany that
behavioral change; the proven low-level core can remain.

## Research references supplied by the user

- [Hooktheory Trends](https://www.hooktheory.com/trends): the next-chord/path
  exploration interaction and matching song sections.
- [Hooktheory Popular Progressions](https://www.hooktheory.com/theorytab/popular-chord-progressions):
  a broad progression reference with related songs and several levels of harmony.
- [Minnix Music's progression article](https://minnixmusic.wordpress.com/2024/09/11/the-100-chord-progressions-every-musician-must-know/):
  a broad curation starting point. Verify each formula, chord quality and song
  attribution against the actual intended behavior before adding an entry.
