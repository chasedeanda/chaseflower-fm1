# CORD Studio controls

Turn **SELECT** on an overview to switch between Studio, Sound, Record and Help. On the Studio overview, turn **ALGORITHM** to highlight any amp or pedal. Highlighting an amp selects the keyboard instrument, which follows you between workspaces and appears in the colored **KEYS** footer. Highlighting a pedal leaves the keyboard instrument in place. Press **SEL** to zoom in or out. **HOME** returns to the Studio overview.

## Studio

**PLAY** starts or stops the band. The four color-coded musician amps come first, followed by the Harmony, Shaping, Genre and ARP pedals. The header shows the key, scale, meter, tempo and current chord as a Roman numeral. It follows a held manual chord while you play one.

### Genre

Highlight the Genre pedal with **ALGORITHM** or **SEQ**, press **SEL** to open it, and turn **K1** or **PRESETS** to choose a genre. **PRESETS** also works from the Studio overview. Genre changes the harmonic and rhythmic vocabulary of the whole band, including the two playing styles available for each musician. A genre change queues a new take for the next bar during playback. Your current tempo continues while the band plays. The matching sounds are available separately through **Sound → EDIT Load palette**.

| Genre | Playing character |
| --- | --- |
| Dream Pop | Sustained or pulsing chords, melodic bass, a soft beat and sparse upper figures |
| Indie | Chopped or ringing chords, offbeat bass, room drums and a chime line |
| New Wave | Short chord stabs, octave bass, a machine beat and stepped synth figures |
| Pop Punk | Driving chords and bass, a backbeat and a high answering hook |
| Lo-Fi | Soft keys, a late bass pocket, half-time drums and short melodic fragments |
| Ambient Soundscape | Slow harmony, pedal bass, sparse percussion and evolving swells |

### Harmony and Shaping

Highlight Harmony or Shaping with **ALGORITHM** or **SEQ** and press **SEL** to open its four dials. Harmony is also available within any zoomed Studio musician by turning **SELECT**. Key, tempo, scale and meter can still be changed from the overview with a musician highlighted.

| Page | Knob | What it changes |
| --- | --- | --- |
| Harmony | K1 Key | Transposes the band to a new tonic. During playback the change lands at the next chord. |
| Harmony | K2 Tempo | Adjusts the internal tempo. When following an external MIDI clock, the source controls the tempo. |
| Harmony | K3 Scale | Changes the scale used by harmony and the generator. During playback it lands at the next chord. |
| Harmony | K4 Meter | Chooses 3/4, 4/4 or 6/8. During playback it lands at the next bar. |
| Shaping | K1 Space | Scales the audible chorus, delay and reverb together. Raise the relevant send on Sound → Sends if a part is dry. |
| Shaping | K2 Tempo | The same tempo control as on Harmony. |
| Shaping | K3 Energy | Raises or lowers the effective Activity of all four musicians around their current settings. 50% is neutral. Phrases gain or shed events and can change accents within the current take. |
| Shaping | K4 Evolve | Sets how often one musician automatically varies its phrase. 0% turns it off. 1–49% gives the band one change every 8 bars, 50–79% every 4 bars and 80–100% every 2 bars. Ambient doubles those intervals. Muted musicians and musicians with Activity below 35% are skipped. |

Press **EDIT** on the Studio overview to generate a new band take for the next bar. Zoom into a musician and press **EDIT** to regenerate only that part on the next sixteenth-note grid point, or eighth-note grid point in 6/8.

### Musician panel

| Knob | What it changes |
| --- | --- |
| K1 Style | Chooses between two ways of playing for the selected musician and genre. For example, Indie Bass offers Offbeat and Answer. |
| K2 Activity | Sets how busy that musician plays. The effect depends on the style and can include extra notes, accents and percussion hits. |
| K3 Level | Sets the musician’s mix level. This is the same level on Sound → Tone and in the GLO mixer. |
| K4 Sustain on Chords | Sets the envelope level while a chord is held. Sound → Envelope → Release sets how long it rings after note-off. |
| K4 Touch on Bass and Layer | Sets the velocity of generated notes. 50% leaves the programmed velocity as played. |
| K4 Punch on Drums | Sets the velocity of generated drum hits. |

Style and Activity changes take effect on the next playable grid point. Continuous knobs take single steps when turned slowly and five-unit steps when turned quickly.

### GLO mixer

Press **GLO** to open or close the mixer. K1–K4 set the levels of Chords, Bass, Drums and Layer in that order. Hold upper keys 1–4 for momentary mute or 5–8 for momentary solo. **EDIT** latches mute and **SEL** latches solo on the selected musician. On the mixer or zoomed Studio Drums panel, turn **PRESETS** left for a fill or right to drop or return the drums at the next bar. **SAVE** from the mixer opens Global settings.

## Sound

On the overview, **K1 Library** selects the short Genre list or All sounds. **K2 Preset** browses the selected musician’s tones or kits. With Genre selected, press **EDIT** to load the complete genre palette. While playing, the pitched sounds change immediately and the drum kit changes at the next bar. The palette keeps the current levels, pan, sends and shared effects settings.

Zoom into a musician and turn **SELECT** through its named pages. The page counter shows where you are.

| Page | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| Tone | Preset or drum kit | Mix level | First synth macro, such as filter cutoff on VA tones | Pan |
| Envelope | Attack, the time to reach full level | Decay, the time to fall toward Sustain | Sustain, the held level | Release, the time to fade after note-off |
| Sends | Drive amount | Chorus send | Delay send | Reverb send |
| FX Presets | Browse shared FX presets | | | |

Drums has Tone, Sends and FX Presets. **PRESETS** also browses the sound or kit. Sends shows a preview of the selected musician’s drive curve, chorus sweep, delay taps and reverb tail. Each graph responds to that musician’s knob. From the Sound overview, press **FX** to jump to FX Presets. Turn **K1** or **PRESETS** to highlight a preset and press **SEL** to apply it to the band. Use **SELECT** to turn to another Sound page. **SAVE** opens preset management, and pressing **FX** again opens the shared EQ and effects editor. **LFO** from a zoomed pitched musician opens its detailed synth editor.

## Arpeggiator

Select Chords or Layer, highlight the ARP pedal with **SEQ**, and press **SEL** to zoom in. The **ARP** button opens the same panel directly. On the Pattern page, **K1 Arp For** chooses Off, Keys or Band. Keys arpeggiates the notes you hold. Band arpeggiates that musician’s generated chord tones with the transport running. **K2 Mode** selects the note order or strum mode, **K3 Octave** sets the range, and **K4 Rate** sets the clock division or the spacing in milliseconds for a strum.

Turn **SELECT** to the timing page. **K1 Gate** sets how much of each step a note occupies, **K2 Swing** shifts alternating steps, **K3 Latch** keeps the arpeggio going after key release, and **K4 Glide** sets the Chords glide time.

## Record and Help

**Record** shows the four generated phrases and the playhead. **SEL** opens a two-bar view of the highlighted musician. **REC** starts or finishes a manual take of up to eight bars. In the Saved takes panel, turn **K1** to select one of the three slots and hold **SAVE** for three seconds to clear it. Release early to cancel. Finish recording or stop saved-take playback before clearing. Use USB audio capture to record the full generated mix as audio.

**Help** contains Start here, Harmony, Playing and Capture. Turn **ALGORITHM** to select a topic and **SEL** to open it. The Harmony lesson shows the role of the note you play. **K1** switches between Auto and Manual harmony. **EDIT** opens Chord Lab.
