# Spotify cover expansion: source chord preservation

The development catalog now includes **22 additional named song snippets**, IDs
**101–127** (with removed artist IDs left unused), bringing the explorer to **122 ideas**. The existing 100 entries,
including the four Paramore sections, retain their order and IDs. The earlier
Paramore sections are also audited below. This document
records the source and preserved harmony for each entry; the [explorer guide](../PROGRESSION-EXPLORER.md)
is the player-facing catalog.

## Complete source coverage and preferences

The user first supplied six annual Spotify playlist links, then supplied a
[combined playlist](https://open.spotify.com/playlist/1eSgJXraV9A5uJCenjzVtf),
**Your Top Songs 2020 (2)**, with duplicates intentionally preserved as a
preference signal. On November 7, 2026, every position **1–601** in that combined
playlist was read through the Spotify desktop app's accessibility rows.

Excluding every row crediting **Dance Gavin Dance**, **Dwellings**, or **mgk / Machine Gun Kelly**, including
collaborations, removes **20 occurrences**. The remaining **581 eligible
occurrences** form **495 casefold-exact song-title/full-artist-credit groups**.
The complete raw list retains duplicates and excluded rows for auditability.

An occurrence count is the number of matching rows in this supplied playlist,
not a listening playcount or proof that a song appeared in that many different
years. Grouping uses the casefold-exact displayed title and complete artist
credits, without fuzzy matching, credit reordering or cross-version merging.
Live, acoustic and remix versions remain separate whenever their title or
credits differ. Equal counts have equal preference weight; source position only
breaks ties consistently.

The first bank balances that repeat signal with recognizable sections and
source-supported harmony for the selected sections. Higher-repeat
songs with unresolved harmony remain on the research list below. Songs that
share a progression keep their own names and section labels: for example,
Overstepping and Bags both use IV–vi–V in the proposed D♭-major setup.

- [Complete 601-position native UI snapshot](spotify-cover-native-snapshot.json).
- [Cleaned complete occurrence list](spotify-cover-combined-tracks.json).
- [Eligible preference ranking](spotify-cover-preferences.json).
- [Listening summary and repeat counts](SPOTIFY-COVER-LISTENING-SUMMARY.md).

Only positions 1–100 have Spotify track URIs independently matched against the
public embed data. No URI was invented for positions 101–601. A grouped display
identity does not prove that repeated rows have an identical recording URI.
The earlier [public-access extraction](spotify-cover-source-tracks.json) of
[Your Top Songs 2020](https://open.spotify.com/playlist/37i9dQZF1ELXfKO0nfaeKP)
remains as a historical first-100-track/access audit; it is superseded by the complete
combined snapshot for coverage and preferences. Initial public failures on the
other five annual URLs no longer limit this combined-list source.

## What these entries represent

Each entry is a short **harmonic seed** for exploration and accompaniment, with a proposed
starting key and mode. Charts generally describe community arrangements, and
confidence concerns the selected passage rather than every chord in the song.
The degree encodings below are inferred from the cited chords and checked
against the actual catalog content; a chart's key label alone does not establish
its exact harmony.

Written chord qualities and colors are retained. Daylily uses Gmaj7; Sofia uses
Dm7–G7–Cmaj7–Fmaj7, keeping the dominant seventh distinct from major sevenths.
The factory format stores per-slot source masks and chromatic alterations.
Pluto Projector now uses the cited original seven-change opening verse phrase,
including F♯m7♭5 and G7sus4, rather than the earlier cover adaptation.

**Selection policy:** do not simplify unless a demonstrated FM-1 limitation makes
the source harmony impossible. A catalog-format limitation calls for a format
change. All restored chords fit the existing six-note accompaniment voice limit;
none needs a note removed. Uncertain harmony stays on the research list instead
of being replaced by an easy progression. See [policy](cover-selection-policy.json).

All 22 entries use **4/4 as a suggested practice setup**. This is not a claim
that the source recording or each cited arrangement has that meter. Repeated
cards, such as the two-chord phrases in Forever and The Shade, show a repeated
phrase; they do not store chord durations or establish automatic song timing.

## Implemented bank and sources

The full title and credits are retained here even when the device uses a shorter
display name. TSSF means The Story So Far; Rex means Rex Orange County in device
context labels.

| ID | Song / artist | Selected section | Source chords | Proposed encoding | Evidence and qualification |
| --- | --- | --- | --- | --- | --- |
| 101 | Daylily — Movements | Chorus phrase | Gmaj7–D–Em–D | D major: IVmaj7–I–ii–I | Preserves the charted Gmaj7. No major-seventh reduction. [Reviewed chorus chart](https://www.cifraclub.com/movements/daylily/). |
| 102 | Full Circle — Movements | Opening verse phrase | A–E–F♯m | E major: IV–I–ii | Selected opening verse agrees in [this chart](https://www.guitartabsexplorer.com/movements/full-circle-chords) and [chart version 1](https://www.chords-and-tabs.net/song/name/movements-full-circle). A conflicting capo-4 acoustic chorus arrangement was not used. The three slots do not describe the whole verse. |
| 103 | Kept — Movements | Chorus phrase | Am–F–Am–C–Am–F–Am | A minor: i–VI–i–III–i–VI–i | Seven-change phrase explicitly labeled chorus in the [section chart](https://www.chords-and-tabs.net/song/name/movements-kept). Chart-based seed; rhythm and guitar articulation are player-controlled. |
| 104 | Keep This Up — The Story So Far | Verse phrase | E–C♯m–E–C♯m–A | E major: I–vi–I–vi–IV | Strong verse seed. [Reviewed section chart](https://www.cifraclub.com.br/the-story-so-far/keep-this-up/), [capo-2 arrangement](https://guitartuna.com/chords/keep-this-up-the-story-so-far-easy-guitar-chords-673edb6402172726c7f7976a). The latter uses D/Bm/G shapes, agreeing in concert pitch. Later F♯-major material is outside the chosen phrase. |
| 105 | Proper Dose — The Story So Far | First verse phrase | E♭–A♭–E♭–A♭–Fm–A♭ | E♭ major: I–IV–I–IV–ii–IV | [Chart](https://www.e-chords.com/keyboards/the-story-so-far/proper-dose) explicitly labels the first verse. Later B-major borrowed chords and the intro's E♭/G inversion are outside this passage. |
| 106 | Overstepping — Belmont | Verse loop | G♭–B♭m–A♭ | D♭ major: IV–vi–V | Chart-based verse seed from the [section arrangement](https://akkordov.net/akkordi/belmont/648520-overstepping). Keeps the short harmonic loop, omitting guitar riffs and voicing detail. |
| 107 | Country Girl — Belmont | Chorus loop | A–C♯m–E–F♯m | E major: IV–vi–I–ii | [Section chart](https://www.e-chords.com/chords/belmont/country-girl) supports the selected chorus loop. The cited chorus chord names are retained; performance timing is player-controlled. |
| 108 | Placeholder — The Story So Far | Chorus phrase | G–Bm–G–Bm–A | D major: IV–vi–IV–vi–V | [Chord chart](https://www.guitartabsexplorer.com/de/story-so-far/placeholder-chords), [guitar-tab reference](https://www.guitartabs.cc/tabs/t/the_story_so_far/placeholder_tab.html). Retains the selected harmonic order while omitting the guitar arrangement. |
| 109 | Bags — Clairo | Verse loop | G♭–B♭m–A♭ | D♭ major: IV–vi–V | [Chord chart](https://www.cifraclub.com/clairo/bags/) supports the loop. The [artist's Song Exploder interview](https://songexploder.net/transcripts/clairo-transcript.pdf) supplies arrangement context; it is not an independent exact chord transcription. |
| 110 | Sofia — Clairo | Main loop | Dm7–G7–Cmaj7–Fmaj7 | C major: ii7–V7–Imaj7–IVmaj7 | Preserves all four seventh chords, including dominant G7. [Hook chart](https://busk.town/songs/sofia), [recording reference](https://chordify.net/nl/chords/clairo-sofia). |
| 111 | Pretty Girl — Clairo | Chorus loop | Dm7–Cmaj7–Em7–Dm7 | C major: ii7–Imaj7–iii7–ii7 | Preserves the charted sevenths. [Full chord version](https://www.cifraclub.com/clairo/pretty-girl/smztssg.html), [UkuTabs](https://ukutabs.com/c/clairo/pretty-girl/). |
| 112 | It's Not The Same Anymore — Rex Orange County | Chorus phrase | F–C–Gm–Dm | F major: I–V–ii–vi | Device name: **Not The Same**. [Chart](https://www.cifraclub.com/rex-orange-county/its-not-the-same-anymore/) supports the selected phrase. The cited chorus writes plain C; no unverified slash bass is inferred from another section. |
| 113 | THE SHADE — Rex Orange County | Chorus phrase | B♭maj7–Fmaj7–B♭maj7–Fmaj7 | F major: IVmaj7–Imaj7–IVmaj7–Imaj7 | Preserves the chorus major sevenths. [Section chart](https://www.e-chords.com/chords/rex-orange-county/the-shade). |
| 114 | Pluto Projector — Rex Orange County | Opening verse phrase | C–Em7–Fmaj7–F♯m7♭5–G7–G7sus4–G7 | C major: I–iii7–IVmaj7–♯ivø7–V7–V7sus4–V7 | Opening verse, including the passing half-diminished chord and the dominant suspension/resolution. [Full source](https://www.chords-and-tabs.net/song/name/rex-orange-county-pluto-projector-4). |
| 118 | I.F.L.Y. — Bazzi | Verse / main loop | Em9–A13–Dmaj9–Gmaj7 | D major: ii9–V13–Imaj9–IVmaj7 | Preserves ninths, dominant thirteenth and major sevenths from the chart labeled Official. [GuitarTuna](https://guitartuna.com/chords/ifly-bazzi-easy-guitar-chords-61d369d454f3d7e4799c9c88). Other public charts use different chord labels; this entry follows this identified version. |
| 119 | Intentions — Justin Bieber, Quavo | Chorus phrase | A–A–F♯m–D–F♯m–D | A major: I–I–vi–IV–vi–IV | Six-slot phrase explicitly labeled chorus. [Chart](https://www.cifraclub.com/justin-bieber/intentions/), [tutorial using capo 2](https://www.youtube.com/watch?v=3z_S_dotGlQ). |
| 120 | Hurt — Oliver Tree | Opening pre-chorus phrase | Am–C–G–F | A minor: i–III–VII–VI | This specific phrase precedes the main refrain. [Chart](https://www.cifraclub.com.br/oliver-tree/hurt/), [corroborating chart](https://www.wikisongbook.com/oliver-tree/hurt), [section/key analysis](https://www.hooktheory.com/theorytab/view/oliver-tree/hurt). |
| 121 | Habitual — Justin Bieber | Chorus phrase | A–B–E | E major: IV–V–I | Creator-posted tutorial identifies the chorus. [Tutorial](https://www.youtube.com/watch?v=Q0AcAXYelP8), [chart/key](https://www.cifraclub.com/justin-bieber/habitual/). |
| 122 | Confirmation — Justin Bieber | Verse / main loop | Fm–E♭–D♭–A♭–B♭m–Fm | F minor: i–VII–VI–III–iv–i | Concert F minor. Em shapes require capo 1; copies omitting the capo can mislead. [Creator tutorial](https://www.youtube.com/watch?v=pN0B68MgE6I), [capo-1 transcription](https://www.guitar-link.com/artist/J/Justin%20Bieber/Confirmation/1). |
| 123 | Changes — Justin Bieber | Verse phrase | Fmaj7–Em7–Dm7 | C major: IVmaj7–iii7–ii7 | Preserves all three seventh chords in the verse. [Transcription](https://www.guitar-link.com/artist/J/Justin%20Bieber/Changes/1), [keyboard chart](https://www.e-chords.com/keyboards/justin-bieber/changes). |
| 124 | Forever — Justin Bieber, Post Malone, Clever | Opening chorus phrase | C♯m–E–C♯m–E | C♯ minor: i–III–i–III | Two repetitions of a two-chord phrase. [Section chart](https://ukutabs.com/j/justin-bieber/forever-feat-post-malone-clever/), [original-key reference](https://www.ilchitarristasullaspiaggia.it/2020/02/forever-justin-bieber-feat-post-malone-chords.html). Repeated cards do not store durations. |
| 125 | Yummy — Justin Bieber | Opening chorus phrase | Gm7–Am7–Gm7–Am7 | F major: ii7–iii7–ii7–iii7 | Preserves the opening alternating minor sevenths. [Chorus chart](https://www.cifraclub.com/justin-bieber/yummy/). |

## Earlier Paramore source corrections

The rule applies to the earlier named covers too. **Still Into You**’s verse
(ID 55) preserves F5–F5–D5–D5 from the [school chart](https://www.rvn.se/globalassets/folkhogskolorna/alsta/still-into-you.pdf),
without adding major/minor thirds to its power chords. Its selected chorus
(ID 10) keeps B♭–C–Dm–F as written.

**The Only Exception**’s opening verse (ID 20) uses B–B–F♯m6–Emaj7, verified
against the [licensed first-page preview](https://www.musicnotes.com/sheetmusic/paramore/the-only-exception/MN0084459).
Its chorus (ID 56) uses B–F♯m6–Emaj7, corroborated by the [full section chart](https://www.cifraclub.com/paramore/the-only-exception/).
The previous I–v–IV triads lost both the sixth and major seventh. These sections
now use explicit source masks. The older StrumPatterns guitar arrangement uses
custom high-string tuning and unusual generated symbols; it is no longer used
to infer generic triads or the opening card order.

## Further research and holds

The repeat counts below are source-playlist occurrences, following the exact
identity policy above. An unimplemented high-repeat song is still a priority;
it has not been replaced by an invented easy progression.

| Song / artist | Why it remains outside the first bank |
| --- | --- |
| Third Degree — Movements (4 occurrences) | [E-Chords](https://www.e-chords.com/chords/movements/third-degree) gives Em7–Em11–G, while [Cifra Club](https://www.cifraclub.com/movements/third-degree/) gives Bm–A7–G and an inconsistent key field. Resolve roots, voicing and selected section against the recording. |
| Colorblind — Movements (3) | The [detailed chart](https://www.guitartabsexplorer.com/movements/colorblind-chords) uses F♯m9–Bm11 and Bm11/D; [another analysis](https://songparts.com/songs/movements/colorblind) favors D-major upper structures. Omitting the extensions and D bass may change the song's harmonic identity. |
| Quicksand — The Story So Far (3) | [Acoustic capo chart](https://www.chords-and-tabs.net/song/name/the-story-so-far-quicksand) and [standard-tuning guitar tab](https://www.guitartabsexplorer.com/story-so-far/quicksand-tab) disagree about the selected harmony. Check the original passage and tuning before choosing triads. |
| Bowser's Castle — Belmont (3); Hollowed Out — Belmont (3) | Original-recording and acoustic arrangements need to be separated before selecting a source-supported snippet. Their repeat weight keeps them high on the next research pass. |
| AMAZING — Rex Orange County | Borrowed minor-iv harmony needs to be verified and retained; a fixed diatonic-major reduction would change it. |
| Loving Is Easy — Rex Orange County, Benny Sings | Resolve the A♭/B♭ material and exact selected phrase before assigning one fixed key/mode. |
| Cut My Hair — Mounika., Cavetown | [Chart](https://ukutabs.com/m/mounika/cut-my-hair-feat-cavetown/) changes E♭ major to E♭ minor in the verse and also uses C major. A single fixed diatonic chord quality cannot capture this mixture. Preserve those changes rather than substituting easier chords. |
| Alien Boy — Oliver Tree | [Guitar chart](https://www.chords-and-tabs.net/song/name/oliver-tree-alien-boy) uses Fmaj7–Em7–Am with capo 1, implying concert G♭maj7–Fm7–B♭m. Its complete chord voicings must be retained; [the analysis](https://www.hooktheory.com/theorytab/view/oliver-tree/alien-boy) includes richer harmony and bass detail. Check the exact selected passage before promoting it. |
| Cash Machine — Oliver Tree | Public arrangements disagree on chord qualities and sometimes roots. [One arrangement](https://www.recifra.com/oliver-tree/cash-machine/?hl=en) gives F♯m–A–C♯m–D for the opening chorus, while [the section analysis](https://www.hooktheory.com/theorytab/view/oliver-tree/cash-machine) flags secondary/borrowed harmony. Needs a recording check. |

The main refrain of **Hurt** is a separate future snippet: Am–C–F–**E major**.
Natural minor would incorrectly produce Em. The runtime already supports a
harmonic-minor scale, and its policy of choosing quality from the scale third
while normalizing fifths can produce these four seed triads in A harmonic minor.
Adding that mode to the catalog needs explicit mode-coverage and label checks.
The G-natural root in the implemented pre-chorus is outside A harmonic minor,
so the two sections should keep distinct setups.

## Implementation and verification scope

The 22 retained additions preserve their stable IDs. IDs 115–117 and 126–127
are removed because they credit mgk; those IDs are not reused for other songs. Catalog data
stores up to eight degrees, one tonic, one mode and a suggested meter. The
generic bank retains scale-derived triads. Source sections additionally retain
per-slot masks, altered roots and optional slash bass, without changing saved
settings or event formats. Original song durations are not stored. The new entries use the existing accompaniment engine with its one-bar-per-slot
default; no original song timing or complete arrangement is encoded.

Independent expected-chord fixtures exercise the full source intervals through
physical-key events, source chord cards and actual accompaniment notes in all 12
keys. The generic Auto engine remains unchanged for entries without source data.
Manual source harmony follows the selected song with Harmony On; explicit chord
buttons still let the player change it. When a root has multiple source voicings,
manual playing starts with the first occurrence (for example G7 in Pluto Projector);
use the Sus4 button for G7sus4. PLAY performs every stored slot in order.
A chromatic root absent from the current seven-note key map shows “key --”; PLAY
still sounds its actual pitch instead of substituting a diatonic root.

The entire 22-entry selection was audited against its cited sections. Eight entries
had documented reductions and now retain their written sevenths, extensions or
passing harmony. The other fourteen retain the chord qualities in their cited
section charts. This is source-chart verification, not a claim of a studio-recording
transcription by ear. Melody, guitar articulation and original timing remain outside
these short chord sections. No source chord is deliberately reduced to a triad.

No release package or device flash is produced by this source correction.
