# Native-screen visual system

> **Current mascot build:** the owner approved only Devil/Ember and Cat/Plum, with palette tied to character. This supersedes the older theme/scene proposals below. The implemented two-strip HOME, four-level text and controls are documented in [MASCOTS](../../MASCOTS.md). Physical verification remains pending.

Planned successor design. All sizes below are **native LCD pixels**, not enlarged
desktop pixels. The screen is 240 × 240. Inspect at actual size before release.

## Direction

The owner's **`fm1_adaptive_motion_prototype.html` is the selected visual/motion base**.
Retain its complete nine-screen study: Play / Stage, Chord Library, Progression
Editor, Live Control Banks, Groove / Drums, Harmony Suggestions, Chord Morph,
Generative / Bloom, and Appearance / Settings. The earlier static nine-screen
image remains a typography/layout reference. Do not reduce the approved motion
prototype to a static mock-up or discard Morph/Bloom because of an older brief.

The chord is the hero. Use aligned text, spacing, thin separators and four text
control columns with a visible bank indicator. Keep chord/degree and transport
anchors stable. The musical clock, changing steps, transitions and parameter
responses should feel alive without unrelated ambient decoration.

**Instrument Teal is the default palette**, matching the latest supplied HTML:

| Token | Value |
| --- | --- |
| Background | `#061b1e` |
| Primary / foreground | `#f2f8f4` |
| Secondary / muted | `#89a7a5` |
| Line | `#355054` |
| Subtle | `#113034` |
| Accent | `#77dacb` |
| On accent | `#092d2c` |

**Studio** (dark) and **Editorial** (warm light) are optional palette expressions
of this same interface, not different layouts or instruments. A custom solid
accent can replace the palette accent if contrast remains readable. Preserve the
owner's supplied adaptive-motion HTML byte-for-byte as the reference; implementation
and default-theme adaptations belong in separate working copies.

Avoid unrelated floating-card chrome, dials, glass, glow and ambient gradients.
Keep the prototype's functional step cells, parameter bars, drum steps, moving
marks and Morph/Bloom studies. Put the degree **above** the chord name in both the hero and planned
phrase, following the earlier explicit degree-label requirement even where the
reference image puts a numeral below a name.

The board's descriptions, complete parameter definitions and player instructions
belong outside the simulated LCD. They are documentation, not copy to squeeze
onto the instrument.

## Type and density

| Role | Target size | Rule |
| --- | ---: | --- |
| Main chord hero | 72–104 | Compose symbols clearly; keep the typographic anchor fixed |
| Main value/count/time | 32–64 | One dominant value or action per screen |
| Main menu/song row | 18–22 | Three generous rows; scroll rather than shrink |
| Degree and planned chord name | 15–17 | Degree above name; both remain readable |
| Primary status/page cue | 16–18 | Use `3/8`, short state words or position dots |
| Knob value | 14–16 | Short units and names; detailed ranges live in the guide |
| Secondary musical label | 14 minimum | Rhythm, Key, Scale, Tempo; no full instructions |
| Rare diagnostic/advanced detail | 12 minimum | Use the shared reference template; no tiny essential musical text |

Avoid sentences such as “Sounds 1 through 4 of 8.” Use a large short position
cue or dots. A list has a visible selected item and an obvious next/previous
direction. Overflow scrolls through a window; it does not squeeze the font.

## Screen families

Different tasks use meaningful content, not different decorative skins. The
selected Instrument Teal/Studio/Editorial palette applies across workspaces; red REC
and destructive/success states remain explicit through labels as well as color.

| Family | Visual form | Icon vocabulary | Accent role |
| --- | --- | --- | --- |
| Play / Progressions | Large chord hero, degree above names, paired phrase cue, four control columns | Optional music/play cue | Palette accent |
| Groove | Groove name and flat step lanes with the actual playhead | Optional drum/grid cue | Palette accent |
| Songs | Large name rows, short duration/type, thin separators, selected-row emphasis | Optional music-file/recording cue | Palette accent |
| Record | HOME record overlay, large count-in/time, clear REC dot and bar/beat position | Circle, timer, square | Semantic red |
| Learn | Chord/name/degree rows or a factual note/interval diagram | Optional piano/book cue | Palette accent |
| Next | Candidate chord rows with degree and a short musical reason | Optional direction cue | Palette accent |
| Sound / envelope | Large preset/value and functional envelope/wave shape; text controls | Optional wave cue | Palette accent |
| Effects | Flat chain/routing labels and one focused effect/value | Optional link cue | Palette accent |
| Settings | Generous label rows with thin separators | Optional settings cue | Palette accent |
| Confirm / result | One clear status symbol, target name and action | Check, trash, alert | Success / destructive status |

Use a consistent supplied icon set. Pair icons and colors with readable labels;
no action or state depends on color alone. Do not place a new unlabelled icon
where a physical control's meaning would become ambiguous.

Use one restrained accent rather than a rainbow of workspace skins. Custom
accent choices remain solid. Studio and Editorial preserve the same spacing, control hierarchy, bank
behavior and degree placement as Instrument Teal.

## Four physical controls, explicit banks

CORE always means **Rhythm / Key / Scale / Tempo**. Additional banks expose audited
existing controls, with the bank name, `1/4` position or four dots always visible:

| Bank | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| CORE | Rhythm | Key | Scale | Tempo |
| SOUND | Chord sound | Bass sound | Chord level | Bass level |
| TEXTURE | Reverb mix | Delay mix | Chorus mix | Glide |
| GROOVE | Kit | Drum level | Swing | Click |

Do not copy unimplemented Human/Fill percentages or a new harmony-color macro
from the reference. A bank switch changes only the visible control assignment;
it must not change values, choose a sound, move the cursor or affect transport.
PRESETS still controls chord sound globally. Effect bypass and metronome-running
semantics remain visible and follow the existing engine controls.

**Recommended gesture, requiring physical verification:** while already on
Progressions, tap HOME to cycle CORE → SOUND → TEXTURE → GROOVE. From any other
screen or secondary scene, HOME returns to Progressions CORE. This reuses the
existing HOME-view cycle gesture. SELECT remains the complete catalogue on HOME,
ALGORITHM retains its normal scene/tab role, and OCT−/OCT+ remain musical. Do not
introduce a new hold chord, a push-encoder interaction or root-key navigation.
Keep a discoverable bank explanation/selection in Controls detail.

## Motion language

The user-facing preference is **Motion: On / Off**, exactly two choices, as in
the latest supplied adaptive-motion prototype. Earlier Quiet/Expressive/Reduced
selectors are superseded; there is no independent Intensity, Speed or Calm
preference.

- **On is adaptive:** derive a bounded response from volume/output level and the
  number of active instrument parts. Smooth it so small level changes and
  percussion transients do not make the display jump between styles. More output
  and more active instruments can produce a stronger response; quiet/sparse
  playing stays restrained. Tune the internal response curve during validation,
  not through another user motion dial.
- **Musical source:** drive timing from the shared clock and actual resolved
  playing/ownership state. Use a real output envelope and active-part snapshot
  when implemented; mark prototype-supplied levels as simulations. The demo’s up-to-eight layers are not
  a hardware instrument/voice-capacity promise. Do not pretend
  that hardware FFT, velocity sensing or these measurements already exist.
- **Playback:** preserve the prototype's beat accents, moving steps/playhead,
  actual chord transitions and functional parameter/bank/focus responses.
  Harmonic labels stay anchored and do not falsely change identity every beat.
- **REC / Saved:** count-in, elapsed/bar state and recording feedback remain
  clear on HOME. A Saved response follows durable acknowledgment; Pending/Saving
  stays distinct regardless of animation timing.
- **Off is total:** stop every animation and transition, including CSS pulses,
  fades, interpolation, halo, Morph/Bloom motion, focus/parameter and saved
  transitions. Musical values, notes, degree, playhead and recording/storage
  status still update immediately to the correct settled state.
- **OS accessibility:** an operating-system reduced-motion preference may force
  effective Off privately. It must not create a third Reduced user choice.

Coalesce rapid events, reuse settled layers and redraw only affected regions.
Measure and cap cadence, pixel work, foreground latency, scratch memory and flash
on the FM-1 with demanding audio and USB capture. Keep graphics out of audio
callbacks. Do not promise FPS, CPU savings or polyphony from a browser study.

Chord Morph and Generative / Bloom are retained visual studies. The prototype
does not establish implemented morph/generative DSP, unsupported sound macros,
recording persistence or a release commitment for every pictured capability.
The audited bank mapping above governs real controls; proposed study controls
must be labelled until separately designed, implemented and verified.

## Songs and recording

Songs is the one user-facing collection for saved progressions and recordings.
Factory progression browsing remains on HOME. A recorded Song is identified by
its recording icon/type and short duration; a progression Song has its own type.

REC is available directly from the progression screen. Record the performance
the player hears, including the selected progression and deliberate live changes,
without unexpectedly muting the scheduled chords. Drums-only accompaniment lets
the player record their own chord performance using the same REC flow.

Ending a capture creates a new numbered Song copy. Show Saving/Pending/Saved
honestly, using a safe storage boundary that preserves live HOLD and audible
tails. Every new capture appears in Songs; no hidden take list or mandatory
review-and-keep sequence is required. The original saved Song stays intact.

Song management must make Load, Rename, Duplicate, Delete, Clear Recordings and
Clear All Songs discoverable. Clearing saved records excludes factory templates,
the protected working draft and a pending capture unless explicitly selected for
discard. Confirm the scope/count once, then show the result without extra dialogs.

## Reference versus implementation

The animation studies, board thumbnails and example numbers are design artifacts.
They are not device captures or evidence of completed playback/storage. The
canonical inventories define control and state behavior; the generated board,
player guide and engineering handoff must stay aligned with them.

## Historical explorations

Coral Bloom (branching note flowers), Aurora (luminous musical fields) and Studio
Rack (DAW/guitar-amp forms) explored the earlier expressive-motion request. They
are retained as design history and are **not selected** for this interface.
Gradient, glow, organism and larger reactive-scene proposals from those studies
are not active Display pages. Chord Stage likewise remains an earlier reference. The latest supplied
adaptive-motion HTML now governs the nine-screen study, including its own Morph
and Generative / Bloom views; those are not a selection of the earlier Coral
Bloom skin.
