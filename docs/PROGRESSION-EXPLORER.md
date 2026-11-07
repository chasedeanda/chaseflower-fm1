# Progression explorer

Development source; not included in the existing 0.6-chase5 package.

## Controls

The development build opens directly to Progressions and excludes decorative
scenes and the fish intro. ALGORITHM returns to the main Progressions view.
The optional exploration build (`CF_SCENES=1`) retains the cycle
**Progressions → Bass → Bonfire → Nothing**. See [scene-free details](SCENE-FREE.md).
HOME still cycles the main, Chord, Keyboard and Diagnostics views.

- **SELECT** browses all **130 ideas**, wrapping at both ends. Tags and the current
  scale never hide an idea. Selecting sets its starting key, scale and meter.
  Hold **GLO** for Browse: **K1 jumps ten**, **K2 jumps one**, **K3 chooses
  Catalog / A–Z / Shuffle / Favorites**, **K4 stars the idea**. #73 takes seven
  K1 turns and two K2 turns from #1. SELECT follows the chosen order; direct number
  jumps always use the stable catalogue number, regardless of order.
- Hold **SAVE** on Progressions for 0.7 seconds to add/remove the visible idea
  from Favorites. A short SAVE press still opens sound saving. Favorites and the
  chosen order survive power-off and are included in backups. Shuffle visits all
  130 once per traversal; choosing Shuffle again makes a new random order.
- **PLAY** starts the selected idea at its first slot, with chords, linked bass
  (when enabled) and the selected compatible groove. PLAY again stops accompaniment,
  including after navigating to another screen; independent live HOLD remains.
- **K1 RHYTHM** cycles **Off → Click → compatible grooves** without changing BPM.
  During playback the latest choice arrives at the next bar.
- **K2 KEY**, **K3 SCALE**, **K4 TEMPO** retain the active values while changes wait.
  Key/mode arrive at the next chord; tempo changes preserve phase. External MIDI
  clock shows **EXT** and disables the local tempo control.
- **SELECT while playing** queues the whole idea for the next bar, then begins its
  first slot. **Next** is separate from the active title/key/mode/meter.
- **GLO → Accompany** offers **Chords + Drums / Drums only**, rhythm, tempo and
  the chord sound's Glide. **PRESETS** still selects the chord sound.
- The playback cursor is an underline and slot number, independent of the chord
  match highlights (a repeated chord can light several matching cards).
- Browsed musical setup and accompaniment choices are session state; browsing/playback
  do not save them to flash or modify patterns. Explicit favorite/order changes save
  a separate small preference record. See [playback details](PROGRESSION-PLAYBACK.md).

## Catalog and sources

The library spans Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian and
Locrian, plus **2/4, 3/4, 4/4, 5/4, 6/8 and 7/4**. It includes pop, rock,
country, folk, jazz/soul seeds, funk, reggae, ambient and cinematic ideas, with
pop punk, songwriter, emo and math rock represented as requested. Genre/mood
labels describe writing prompts; they do not change patches or add notes.

Four earlier entries use short sections of two Paramore songs:

- **Still Into You**: chorus phrase IV–V–vi–I in F; the verse preserves
  the chart’s power chords F5–F5–D5–D5.
  Reference: [the Alsta school chart](https://www.rvn.se/globalassets/folkhogskolorna/alsta/still-into-you.pdf).
- **The Only Exception**: opening verse B–B–F♯m6–Emaj7 and chorus
  B–F♯m6–Emaj7. The B Mixolydian root map is retained, with explicit minor-sixth
  and major-seventh source chords. References: [licensed first-page preview](https://www.musicnotes.com/sheetmusic/paramore/the-only-exception/MN0084459),
  [full section chart](https://www.cifraclub.com/paramore/the-only-exception/).

The added bank contains 30 song snippets selected from the user's combined Spotify
top-song playlists and later requests. Repeated songs were retained as preference evidence; Dance
Gavin Dance, Dwellings, and mgk / Machine Gun Kelly (including features) are excluded. The additions include Movements, The
Story So Far, Belmont, Clairo, Rex Orange County, Bazzi, Justin Bieber, Oliver
Tree and Morgan Wallen. The [eight Morgan Wallen sections](research/MORGAN-WALLEN-COVER-SNIPPETS.md)
include written slash bass and seventh chords; Cover Me Up uses 6/8. See the [source and section notes](research/SPOTIFY-COVER-SHORTLIST.md)
for full titles, charts, source details and songs held for further checking.

Source song sections preserve their written chord qualities, sevenths, extensions
and chromatic passing harmony. They are not reduced to fit the degree catalog.
Pluto Projector includes F♯m7♭5 and G7sus4; Sofia keeps dominant G7 distinct from
Cmaj7 and Fmaj7. Harmony On uses these source chords for manual keys as well as
PLAY; upper buttons remain explicit performance overrides. The suggested meters
and one-bar-per-slot practice playback do not claim original recorded timing.

The table shows browser position and stable ID separately. Removed artist IDs are
not reused; saved favorites for remaining IDs retain their identity.

[Minnix Music's progression overview](https://minnixmusic.wordpress.com/2024/09/11/the-100-chord-progressions-every-musician-must-know/)
and [Hooktheory's popular progressions](https://www.hooktheory.com/theorytab/popular-chord-progressions)
informed the wider range. Jazz/blues/classical entries marked as roots or triad
seeds retain the simplified Auto rules. They do not reproduce every dominant
seventh, borrowed chord, diminished chord or bass inversion in those sources.
Locrian and other altered modes also retain normal major/minor fifths as requested.

| # | ID | Idea | Context | Starting key/mode | Meter | Chords |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | 1 | Open Road | Pop punk | E Ionian (Major) | 4/4 | I–V–vi–IV |
| 2 | 2 | Homeward | Pop punk | D Ionian (Major) | 4/4 | vi–IV–I–V |
| 3 | 3 | Front Row | Pop punk | A Ionian (Major) | 4/4 | I–IV–vi–V |
| 4 | 4 | Summer Drive | Pop punk | E Ionian (Major) | 4/4 | IV–I–V–vi |
| 5 | 5 | Last Chorus | Pop punk | D Ionian (Major) | 4/4 | IV–V–I–vi |
| 6 | 6 | Old Friends | Pop punk | G Ionian (Major) | 4/4 | I–vi–IV–V |
| 7 | 7 | Basement Show | Pop punk | B Ionian (Major) | 4/4 | vi–V–IV–V |
| 8 | 8 | Long Way Home | Pop punk | A Ionian (Major) | 4/4 | I–IV–ii–V |
| 9 | 9 | After the Show | Pop punk | E Ionian (Major) | 4/4 | I–iii–IV–V |
| 10 | 10 | Still Into You | Paramore: chorus | F Ionian (Major) | 4/4 | IV–V–vi–I |
| 11 | 11 | Porch Light | Songwriter | G Ionian (Major) | 4/4 | I–V–IV–I |
| 12 | 12 | Empty Room | Songwriter | C Ionian (Major) | 6/8 | I–iii–vi–IV |
| 13 | 13 | Handwritten | Songwriter | D Ionian (Major) | 4/4 | vi–iii–IV–I |
| 14 | 14 | Slow Morning | Songwriter | G Ionian (Major) | 6/8 | IV–I–ii–V |
| 15 | 15 | Postcard | Songwriter | C Ionian (Major) | 4/4 | I–vi–ii–V |
| 16 | 16 | Coming Home | Songwriter | E Ionian (Major) | 4/4 | IV–V–iii–vi |
| 17 | 17 | Late Train | Songwriter | A Aeolian (Minor) | 6/8 | i–VII–VI–VII |
| 18 | 18 | Window Seat | Songwriter | E Aeolian (Minor) | 4/4 | i–iv–VII–III |
| 19 | 19 | Quiet Ending | Songwriter | D Ionian (Major) | 6/8 | I–IV–I–V |
| 20 | 20 | The Only Exception | Paramore: verse | B Mixolydian | 6/8 | I–I–v6–IVmaj7 |
| 21 | 21 | Night Drive | Emo | F# Aeolian (Minor) | 4/4 | i–VI–III–VII |
| 22 | 22 | Heavy Weather | Emo | C# Aeolian (Minor) | 4/4 | i–VII–VI–iv |
| 23 | 23 | Half Awake | Emo | E Aeolian (Minor) | 4/4 | VI–III–VII–i |
| 24 | 24 | Slow Return | Emo | B Aeolian (Minor) | 6/8 | i–VI–VII–i |
| 25 | 25 | Undertow | Emo | D Aeolian (Minor) | 4/4 | i–iv–VI–VII |
| 26 | 26 | Unsent | Emo | A Aeolian (Minor) | 6/8 | VI–iv–i–VII |
| 27 | 27 | Fading Light | Emo | F# Aeolian (Minor) | 4/4 | i–III–VII–iv |
| 28 | 28 | Holding On | Emo | D Ionian (Major) | 4/4 | IV–vi–V–I |
| 29 | 29 | Small Hours | Emo | E Ionian (Major) | 4/4 | ii–IV–vi–V |
| 30 | 30 | Last Light | Emo | A Ionian (Major) | 6/8 | vi–iii–IV–ii |
| 31 | 31 | Glass Steps | Math rock | E Ionian (Major) | 4/4 | IV–iii–vi–ii |
| 32 | 32 | Open Strings | Math rock | D Ionian (Major) | 4/4 | I–ii–IV–iii |
| 33 | 33 | Sunroom | Math rock | A Ionian (Major) | 4/4 | ii–iii–IV–I |
| 34 | 34 | Side Streets | Math rock | E Ionian (Major) | 5/4 | vi–ii–V–iii |
| 35 | 35 | Five Windows | Math rock | D Ionian (Major) | 5/4 | IV–ii–iii–vi |
| 36 | 36 | Uneven Ground | Math rock | D Dorian | 7/4 | i–IV–VII–IV |
| 37 | 37 | Bright Corners | Instrumental | C Lydian | 4/4 | I–II–iii–II |
| 38 | 38 | Floating | Instrumental | D Lydian | 5/4 | I–II–V–I |
| 39 | 39 | Mosaic | Math rock | E Dorian | 7/4 | i–IV–ii–VII |
| 40 | 40 | Return Path | Instrumental | A Ionian (Major) | 4/4 | iii–vi–ii–IV |
| 41 | 41 | Low Lantern | Modal / Phrygian | E Phrygian | 4/4 | i–II–vii–i |
| 42 | 42 | Close Quarters | Modal / Phrygian | D Phrygian | 2/4 | i–II–iv–i |
| 43 | 43 | Stone Spiral | Modal / Phrygian | F# Phrygian | 3/4 | i–vii–VI–II |
| 44 | 44 | Desert Glass | Modal / Phrygian | A Phrygian | 6/8 | i–iv–II–i |
| 45 | 45 | Fault Line | Modal / Locrian | B Locrian | 4/4 | i–II–V–iv |
| 46 | 46 | Narrow Bridge | Modal / Locrian | F# Locrian | 5/4 | i–iv–II–vii |
| 47 | 47 | Crooked Waltz | Modal / Locrian | E Locrian | 3/4 | i–II–VI–i |
| 48 | 48 | Shadow Steps | Modal / Locrian | D Locrian | 7/4 | i–vii–V–II |
| 49 | 49 | Two Step | Modal / Dorian | A Dorian | 2/4 | i–IV–ii–i |
| 50 | 50 | Dorian Waltz | Modal / Dorian | D Dorian | 3/4 | i–IV–VII–i |
| 51 | 51 | Wide Sky | Modal / Mixolydian | G Mixolydian | 4/4 | I–VII–IV–I |
| 52 | 52 | Wandering Waltz | Modal / Mixolydian | D Mixolydian | 3/4 | I–IV–VII–I |
| 53 | 53 | Prism Waltz | Modal / Lydian | E Lydian | 3/4 | I–II–V–iii |
| 54 | 54 | Seven Skylights | Modal / Lydian | A Lydian | 7/4 | I–II–iii–V |
| 55 | 55 | Still Into You | Paramore: verse | F Ionian (Major) | 4/4 | F5–F5–D5–D5 |
| 56 | 56 | Only Exception | Paramore: chorus | B Mixolydian | 6/8 | I–v6–IVmaj7 |
| 57 | 57 | Three Chords | Pop / Country | C Ionian (Major) | 4/4 | I–IV–V |
| 58 | 58 | Rock Circle | Rock / Blues roots | E Ionian (Major) | 4/4 | I–IV–V–IV |
| 59 | 59 | Rock Return | Rock | A Ionian (Major) | 4/4 | I–V–IV–V |
| 60 | 60 | Back Porch | Country | G Ionian (Major) | 2/4 | I–V–I–IV |
| 61 | 61 | Bluegrass Turn | Bluegrass | D Ionian (Major) | 2/4 | IV–I–IV–V |
| 62 | 62 | Folk Waltz | Folk | G Ionian (Major) | 3/4 | I–IV–V–I |
| 63 | 63 | Country Road | Country | A Ionian (Major) | 6/8 | I–IV–vi–IV |
| 64 | 64 | River Bend | Americana | D Ionian (Major) | 3/4 | IV–V–IV–I |
| 65 | 65 | Two Five One | Jazz: triad seed | C Ionian (Major) | 4/4 | ii–V–I |
| 66 | 66 | Turnaround | Jazz: triad seed | F Ionian (Major) | 4/4 | vi–ii–V–I |
| 67 | 67 | Passing Through | Jazz: triad seed | Bb Ionian (Major) | 4/4 | iii–vi–ii–V |
| 68 | 68 | Warm Keys | Soul | Eb Ionian (Major) | 6/8 | I–ii–iii–vi |
| 69 | 69 | Four Way Soul | Soul | Ab Ionian (Major) | 4/4 | IV–iii–ii–I |
| 70 | 70 | Midnight Soul | R&B | C Aeolian (Minor) | 4/4 | i–iv–VII–III |
| 71 | 71 | Disco Floor | Disco | E Dorian | 4/4 | i–IV–i–VII |
| 72 | 72 | Night Funk | Funk | D Dorian | 4/4 | i–ii–IV–i |
| 73 | 73 | Dub Horizon | Dub / Reggae | A Aeolian (Minor) | 4/4 | i–VII–iv–i |
| 74 | 74 | Island Light | Reggae | A Mixolydian | 4/4 | I–IV–I–VII |
| 75 | 75 | Soft Focus | Ambient | C Ionian (Major) | 6/8 | I–iii–ii–IV |
| 76 | 76 | Drifting Signal | Ambient | G Lydian | 6/8 | I–V–II–I |
| 77 | 77 | Canyon Echo | Cinematic | D Dorian | 6/8 | i–VII–IV–ii |
| 78 | 78 | Moonlit Circle | Cinematic | E Aeolian (Minor) | 3/4 | i–iv–VII–III |
| 79 | 79 | Canon | Classical: root seed | D Ionian (Major) | 4/4 | I–V–vi–iii–IV–I–IV–V |
| 80 | 80 | Classic Cadence | Classical: triads | C Ionian (Major) | 3/4 | I–IV–I–V–I |
| 81 | 81 | Adapted Fifths | Classical / Modal | F Ionian (Major) | 7/4 | I–IV–vii–iii–vi–ii–V–I |
| 82 | 82 | Minor Cadence | Folk / Natural minor | D Aeolian (Minor) | 6/8 | i–iv–v–i |
| 83 | 83 | Phrygian Dance | Modal / Phrygian | E Phrygian | 2/4 | i–II–i–vii |
| 84 | 84 | Dark Waltz | Modal / Phrygian | D Phrygian | 3/4 | i–iv–vii–II |
| 85 | 85 | Copper Steps | Modal / Phrygian | C Phrygian | 5/4 | i–VI–II–iv |
| 86 | 86 | Seven Torches | Modal / Phrygian | F Phrygian | 7/4 | i–II–iv–vii |
| 87 | 87 | Half Light | Modal / Locrian | C Locrian | 6/8 | i–iv–VI–II |
| 88 | 88 | Unsteady | Modal / Locrian | F# Locrian | 2/4 | i–II–iv–V |
| 89 | 89 | Edges | Modal / Locrian | B Locrian | 5/4 | i–V–VI–iv |
| 90 | 90 | Seven Shadows | Modal / Locrian | A Locrian | 7/4 | i–vii–VI–II |
| 91 | 91 | Lydian Sprint | Modal / Lydian | D Lydian | 2/4 | I–II–V–II |
| 92 | 92 | Long Arc | Modal / Lydian | F Lydian | 4/4 | I–iii–II–V |
| 93 | 93 | Silver Current | Modal / Lydian | G Lydian | 6/8 | I–II–vi–V |
| 94 | 94 | Wide Angle | Modal / Mixolydian | C Mixolydian | 5/4 | I–VII–IV–v |
| 95 | 95 | Off Beat | Modal / Mixolydian | E Mixolydian | 7/4 | I–IV–v–VII |
| 96 | 96 | Coastal Dance | Modal / Mixolydian | D Mixolydian | 2/4 | I–VII–I–IV |
| 97 | 97 | Dorian Five | Modal / Dorian | C Dorian | 5/4 | i–IV–ii–VII |
| 98 | 98 | Seven Current | Modal / Dorian | B Dorian | 7/4 | i–ii–IV–vi |
| 99 | 99 | Minor Two | Folk / Natural minor | D Aeolian (Minor) | 2/4 | i–VII–VI–v |
| 100 | 100 | Minor Five | Cinematic | F Aeolian (Minor) | 5/4 | i–III–VI–iv |
| 101 | 101 | Daylily | Movements: chorus | D Ionian (Major) | 4/4 | IVmaj7–I–ii–I |
| 102 | 102 | Full Circle | Movements: verse | E Ionian (Major) | 4/4 | IV–I–ii |
| 103 | 103 | Kept | Movements: chorus | A Aeolian (Minor) | 4/4 | i–VI–i–III–i–VI–i |
| 104 | 104 | Keep This Up | The Story So Far: verse | E Ionian (Major) | 4/4 | I–vi–I–vi–IV |
| 105 | 105 | Proper Dose | The Story So Far: verse | E♭ Ionian (Major) | 4/4 | I–IV–I–IV–ii–IV |
| 106 | 106 | Overstepping | Belmont: verse | D♭ Ionian (Major) | 4/4 | IV–vi–V |
| 107 | 107 | Country Girl | Belmont: chorus | E Ionian (Major) | 4/4 | IV–vi–I–ii |
| 108 | 108 | Placeholder | The Story So Far: chorus | D Ionian (Major) | 4/4 | IV–vi–IV–vi–V |
| 109 | 109 | Bags | Clairo: verse | D♭ Ionian (Major) | 4/4 | IV–vi–V |
| 110 | 110 | Sofia | Clairo: chorus | C Ionian (Major) | 4/4 | ii7–V7–Imaj7–IVmaj7 |
| 111 | 111 | Pretty Girl | Clairo: chorus | C Ionian (Major) | 4/4 | ii7–Imaj7–iii7–ii7 |
| 112 | 112 | Not The Same | Rex Orange County: chorus | F Ionian (Major) | 4/4 | I–V–ii–vi |
| 113 | 113 | The Shade | Rex Orange County: chorus | F Ionian (Major) | 4/4 | IVmaj7–Imaj7–IVmaj7–Imaj7 |
| 114 | 114 | Pluto Projector | Rex Orange County: verse | C Ionian (Major) | 4/4 | I–iii7–IVmaj7–♯ivø7–V7–V7sus4–V7 |
| 115 | 118 | I.F.L.Y. | Bazzi: verse | D Ionian (Major) | 4/4 | ii9–V13–Imaj9–IVmaj7 |
| 116 | 119 | Intentions | Justin Bieber, Quavo: chorus | A Ionian (Major) | 4/4 | I–I–vi–IV–vi–IV |
| 117 | 120 | Hurt | Oliver Tree: pre-chorus phrase | A Aeolian (Minor) | 4/4 | i–III–VII–VI |
| 118 | 121 | Habitual | Justin Bieber: chorus | E Ionian (Major) | 4/4 | IV–V–I |
| 119 | 122 | Confirmation | Justin Bieber: verse | F Aeolian (Minor) | 4/4 | i–VII–VI–III–iv–i |
| 120 | 123 | Changes | Justin Bieber: verse | C Ionian (Major) | 4/4 | IVmaj7–iii7–ii7 |
| 121 | 124 | Forever | Justin Bieber, Post Malone, Clever: chorus phrase | C♯ Aeolian (Minor) | 4/4 | i–III–i–III |
| 122 | 125 | Yummy | Justin Bieber: chorus | F Ionian (Major) | 4/4 | ii7–iii7–ii7–iii7 |
| 123 | 128 | Last Night | Morgan Wallen: chorus opening | Eb Aeolian (Minor) | 4/4 | VI–VII–i–III |
| 124 | 129 | Cover Me Up | Morgan Wallen: chorus ending | D Ionian (Major) | 6/8 | ii–IV–vi–I–IV–I |
| 125 | 130 | One Thing At A Time | Morgan Wallen: verse phrase | G Ionian (Major) | 4/4 | IV–V–I–ii7 |
| 126 | 131 | Everything I Love | Morgan Wallen: pre-chorus | Ab Ionian (Major) | 4/4 | vi–IV–I |
| 127 | 132 | I Can't Love You... | Ella Langley, Morgan Wallen: opening chorus | Bb Aeolian (Minor) | 4/4 | VImaj7–VII–i |
| 128 | 133 | Thought You Should | Morgan Wallen: chorus loop | F# Ionian (Major) | 4/4 | I–V/3–ii7 |
| 129 | 134 | Cowgirls | Morgan Wallen, ERNEST: chorus loop | F Aeolian (Minor) | 4/4 | i–VI–III–VII |
| 130 | 135 | What I Want | Morgan Wallen, Tate McRae: chorus loop | B Dorian | 4/4 | i–VII–IV |

V/3 denotes the major chord over its third: C♯/E♯ in Thought You Should Know.
That title and I Can't Love You Anymore are shortened on the device.

## Compatibility and verification

Browser selection is one RAM byte. Catalog and labels are read-only firmware
constants. The browser now holds up to 255 entries/IDs and 32 bytes of favorite
bits, so entries above the old 128 limit remain reachable in every order.
Browser preference v3 is 44 bytes in the existing object/sectors. It reads the
28-byte v1/v2 records, preserving their favorites and appearance; migration
happens in RAM and writes only after an explicit edit. Backup restore accepts
both lengths and validates their version/checksum. No audio buffer, framebuffer
or heap is added. Legacy settings stay 188 bytes, pattern events 22 bytes and CHH1
preferences 38 bytes. Main-screen choice uses existing settings bits: bit 3
Bonfire, bit 4 Nothing, bit 5 Bass, all clear Progressions. Hints and detailed
HOME views keep their bits. Earlier clear/Bass records open Progressions;
explicit Bonfire/Nothing records retain their selection.

A 32-bit ARM comparison compile of the original 100-entry bank measured 4,108
bytes of catalog/label constants and approximately 600 bytes of lookup/matching
code. This earlier estimate excludes the added source chord definitions and retained song descriptors; it is
not a final JieLi device link or a hardware performance measurement.

`sh tools/test_progressions.sh` verifies every catalog entry, all seven modes,
all keys, all meters, selection wrapping, actual root mapping, automatic setup,
K1 groove changes, pending-meter replacement, live/HOLD/sequencer highlights and
recording guards. Two deterministic renders run identical browsing inputs with
and without screen drawing; physical output and all six USB channels must match
at 44.1 and 48 kHz. The scene, Auto, rhythm-bank, library, USB and diagnostics
regressions cover the affected shared paths.

FM-1 listening, timing and the final combined linked memory report remain
release checks. No release package or flashing is included in this change.
