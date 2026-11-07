# Chase FM-1 drum-loop refresh

Research and source refresh: November 6, 2026. **38 original two-bar grooves: all 26 existing patterns reworked,
plus 12 additions.** Lo-fi, hip-hop, trap and gaze now lead the rhythm browser. The remaining club styles and
other metres follow. This is a source/audition candidate, not a published firmware release.

## Listen first

Run from the repository root:

```sh
python3 tools/render_drum_bank.py
```

Open `build/drum-bank/index.html` in a browser. It has a style filter, a search box, individual loop players,
the step grids, WAV downloads and measured levels. `build/drum-bank/chase-fm1-drum-auditions.zip` contains the
offline gallery and every WAV. Extract the ZIP before opening the gallery. No accounts or network are needed.

`build/drum-bank/sampler.wav` plays one phrase each of **Lo-fi → NeoSoul → Trap → Plugg → NuGaze → DreamPop →
LoHouse → Garage**, with a short silence between grooves. These are useful first auditions across the requested
styles; musical preference still needs an ear check.

The previews come from the real firmware DSP, at each groove's suggested BPM, with effects off and the original
relative levels. Each individual WAV contains two repetitions of its two-bar phrase. They are not normalized.
On the instrument, TEMPO remains the user's global setting: the BPM label is a suggestion, not an automatic
change when selecting a groove. Use the suggested BPM to hear the intended pace.

## Research and curation

The references were selected for relevant musical direction and concrete production detail. A current sample
catalogue or chart helps identify active styles; it does not prove a loop will fit every piece of music. The
patterns here are original arrangements of common genre vocabulary, not transcriptions or embedded recordings.

| Direction | Reference | What informed the new bank |
| --- | --- | --- |
| Current nu-gaze | [Noise Honey — before the colors leave…nu gaze](https://splice.com/sounds/packs/noise-honey/before-the-colors-leave-nu-gaze/samples), listed on [Noise Honey's trending chart](https://splice.com/sounds/labels/noise-honey/charts) when researched | A live-kit direction, punchy kick/snare, tom movement and cymbal contrast rather than another electronic four-on-the-floor preset. The pack catalogue includes room-oriented toms and processed drum grooves. |
| Shoegaze and indie | [Drumdrops — Shoegaze](https://splice.com/sounds/packs/drumdrops/drumdrops-shoegaze/samples) and [Splice's shoegaze overview](https://splice.com/blog/what-is-shoegaze-music/) | Separate driving gaze, soft dream-pop and spacious slowcore choices. Splice distinguishes revival shoegaze from the heavier Deftones/Whirr/Wisp nu-gaze direction. These firmware grooves are rhythm starting points for those textures. |
| Dusty lo-fi house | [Attack — Lo-Fi House](https://www.attackmagazine.com/technique/beat-dissected/lo-fi-house/) | Rounded drums, a warm kit, offbeat hats and restrained syncopation. The tutorial references DJ Boring, DJ Seinfeld and Ross From Friends, at 118 BPM. Our swing values use the FM-1's own scale. |
| Lo-fi/rave crossover | [Attack — JamesJamesJames-style Lo-Fi House & Breaks](https://www.attackmagazine.com/technique/beat-dissected/lo-fi-house-breaks-fusion-in-the-style-of-jamesjamesjames/) | Warm dance drums alongside broken-beat choices. Its use of a layered break is a listening reference; no break recording is included here. |
| Garage and two-step | [Attack — UK Garage](https://www.attackmagazine.com/technique/beat-dissected/uk-garage/) | Broken kicks, backbeat snares and swung hat/rim responses, with a faster four-on-the-floor SpeedG alternative. |
| Trap | [Native Instruments — How to make a trap beat](https://www.native-instruments.com/blogs/blog/how-to-make-a-trap-beat) | Half-time snare placement, space between kicks and short hat bursts. Separate Trap, CloudTrap and Rage patterns give different density and kit choices. |
| Plugg / pluggnb | [Splice — Pluggnb Drum Kit](https://splice.com/sounds/collections/splice/cddxc3bm-v-q-uf31elmlta9028/samples) and [Splice / Swoope — How to Make Pluggn'B](https://www.youtube.com/watch?v=CxolhsiH7jw) | A sparse 808-based pocket, clap, small rim responses and space for melodic chords. |
| Lo-fi and hip-hop | [Ableton — Making Lo-Fi Hip Hop on Push](https://www.ableton.com/en/blog/beat-generation-making-lo-fi-hip-hop-on-the-new-push/) | An organic, percussion-led direction. Here, swing, lane balance, syncopation and two-bar variation provide movement within the existing sequencer format. |

No third-party audio, pack files, MIDI downloads or session files were imported. The firmware uses its existing
synthesized kits. The referenced commercial packs remain external listening/production references.

## The bank

All phrases are two bars. **New** marks the twelve additions. Display names fit the existing nine-character limit.

| Groove | Suggested BPM | Kit | Character |
| --- | ---: | --- | --- |
| Lo-fi | 78 | Dust | Dusty swing, quiet rim answers |
| HipHop | 92 | X Lo-Fi | Boom-bap, kick pickups, hat variation |
| NeoSoul **new** | 86 | Brush | Rim pocket, broken hats, room for chords |
| TripHop **new** | 82 | Tape | Slow backbeat, low-tom turn |
| Trap **new** | 142 | 808 | Half-time snare and short hat bursts |
| CloudTrap **new** | 136 | Chill | Airy clap, open second-bar downbeat |
| Plugg **new** | 144 | 808 | Sparse bounce, clap and rim responses |
| Rage **new** | 150 | Crush | Driven half-time drums, denser hats |
| NuGaze **new** | 126 | X Live | Driving backbeat, cymbal lift, tom turnaround |
| DreamPop **new** | 104 | Air | Soft drum-machine pulse |
| Slowcore **new** | 76 | X Live | Space, quarter-note hats, floor-tom turn |
| LoHouse **new** | 118 | Dust | Warm house, offbeat hats, shuffled toms |
| DeepHouse **new** | 122 | X Deep | Restrained kick/clap, syncopated toms |
| House | 126 | 909 | Four-on-the-floor and a kick pickup |
| Garage | 132 | X Club | Swung two-step, broken kicks, rim answers |
| SpeedG **new** | 138 | X Club | Faster four-on-the-floor garage |
| Pop | 108 | Tape | Bedroom-pop backbeat |
| Funk | 106 | Tight | Syncopated kicks, rim chatter, hat opening |
| Techno | 134 | X Tech | Steady kick, tom/rim response |
| Break | 132 | X Break | Broken kicks and extra snare hits |
| Jungle | 164 | X Break | Displaced snares and chopped hats |
| DnB | 174 | X Tech | Two-step, firm backbeat, rim detail |
| Halftime | 140 | Dub | Dubby space and sparse percussion |
| Drill | 142 | X Tech | Loping hats, displaced second-bar snare |
| Grime | 140 | Tight | Angular kick/rim, sparse half-time |
| Jersey | 140 | X Club | Five-kick figure, clap backbeat |
| Hyperpop | 152 | Crush | Driven backbeat and short hat bursts |
| Breakcore | 180 | X Break | Busy broken snares with hat gaps |
| Amapiano | 112 | X Deep | Low/mid tom dialogue over a steady pulse |
| Baile | 130 | 808 | Tom-led rhythm, second-bar pickup |
| Dembow | 96 | X Club | Dembow snare with quiet rim answers |
| PopRock | 122 | X Live | Live backbeat and short tom fill |
| Ballad, 6/8 | 66 | Brush | Soft rim pulse and tom pickup |
| Electro, 6/8 | 92 | Dub | Electronic pulse and bell response |
| Lo-Fi 6, 6/8 | 72 | Dust | Dusty compound groove |
| Tamborzao, 2/4 | 130 | 808 | Tom-led short phrase |
| MathRock, 5/4 | 140 | X Live | Two-bar kick/snare variation |
| MathRock, 7/4 | 130 | X Live | Late snare pickup and tom turn |

## Compatibility and limits

- Every existing rhythm keeps its original ID. Saved selections and KIT/SWING overrides still resolve by ID
  after the browser order changes. New IDs are 51–62; retired IDs 33, 37, 43 and 44 remain unused.
- Existing saved sequencer patterns are untouched. EDIT/REC copies the new groove's complete hits, lane levels,
  kit and swing into an editable pattern, including the correct rotation when recording starts in bar two.
- Existing user KIT/SWING overrides take precedence over these defaults. On the rhythm page, **SEL** resets the
  selected groove's overrides if you want to audition its new intended kit and swing.
- The current drum grid is sixteenths in 4/4, 2/4 and 6/8, and eighths in 5/4 and 7/4. It has a level per lane,
  not per hit. The trap patterns use sixteenth bursts, not true 32nd/triplet rolls. No player or storage changes
  were needed for this bank.
- NuGaze and Slowcore use the existing synthesized X Live kit, not acoustic recordings. Amapiano uses a low-tom
  approximation of a log drum. Bass, chord texture, effects and mix processing are still part of achieving a full
  genre sound; the loops supply the rhythmic starting point.
- For 6/8, BPM stays quarter-note based: a bar lasts three quarter notes, grouped as two dotted-quarter pulses.

## Verification

```sh
sh tools/test_drum_bank.sh
sh tools/test_chase3.sh
clang -target armv7m-none-eabi -Os -fno-builtin -w \
  -Ifirmware/hal -Ifirmware/src -Ibuild/gen -fsyntax-only firmware/src/coralflower.c
```

The dedicated bank check passes **147,107 checks**, covering valid names/IDs/kits/grids/levels, actual two-bar
variation, no simultaneous closed/open hats, saved-ID lookup and overrides, all rotated sequencer copies, and
MIDI note-on timing against the score over two cycles in both players. Every groove is also rendered through the
real DSP with no clipped samples. The dry render peaks range from roughly -21.0 to -11.8 dBFS, preserving room
for live chords and bass. These measurements establish rendering integrity, not subjective musical quality or
physical-device performance. The library/6/8 integration suite and firmware syntax check pass as well.
