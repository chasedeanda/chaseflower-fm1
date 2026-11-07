# Spotify cover expansion: first 27 snippets

The development catalog now includes **27 additional named song snippets**, IDs
**101–127**, bringing the explorer to **127 ideas**. The existing 100 entries,
including the four Paramore sections, retain their order and IDs. This document
records the source and simplification for each new entry; the [explorer guide](../PROGRESSION-EXPLORER.md)
is the player-facing catalog.

## Complete source coverage and preferences

The user first supplied six annual Spotify playlist links, then supplied a
[combined playlist](https://open.spotify.com/playlist/1eSgJXraV9A5uJCenjzVtf),
**Your Top Songs 2020 (2)**, with duplicates intentionally preserved as a
preference signal. On November 7, 2026, every position **1–601** in that combined
playlist was read through the Spotify desktop app's accessibility rows.

Excluding every row crediting **Dance Gavin Dance** or **Dwellings**, including
collaborations, removes **11 occurrences**. The remaining **590 eligible
occurrences** form **503 casefold-exact song-title/full-artist-credit groups**.
The complete raw list retains duplicates and excluded rows for auditability.

An occurrence count is the number of matching rows in this supplied playlist,
not a listening playcount or proof that a song appeared in that many different
years. Grouping uses the casefold-exact displayed title and complete artist
credits, without fuzzy matching, credit reordering or cross-version merging.
Live, acoustic and remix versions remain separate whenever their title or
credits differ. Equal counts have equal preference weight; source position only
breaks ties consistently.

The first bank balances that repeat signal with recognizable sections and
source-supported chords that fit the existing degree format. Higher-repeat
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

Triad reductions omit named sevenths, ninths, suspensions, guitar voicings,
inversions and slash-bass movement where noted. No melody, lyrics or full song
arrangement is stored. **Pluto Projector is an explicitly simplified cover
adaptation**, rather than a transcription of the original verse.

All 27 entries use **4/4 as a suggested practice setup**. This is not a claim
that the source recording or each cited arrangement has that meter. Repeated
cards, such as the two-chord phrases in Forever and The Shade, show a repeated
phrase; they do not store chord durations or establish automatic song timing.

## Implemented bank and sources

The full title and credits are retained here even when the device uses a shorter
display name. TSSF means The Story So Far; Rex means Rex Orange County in device
context labels.

| ID | Song / artist | Selected section | Seed chords | Proposed encoding | Evidence and qualification |
| --- | --- | --- | --- | --- | --- |
| 101 | Daylily — Movements | Chorus phrase | G–D–Em–D | D major: IV–I–ii–I | Strong chart-supported triad seed, reducing Gmaj7. [Reviewed section chart](https://www.cifraclub.com/movements/daylily/), [corroborating arrangement](https://vchords.com/en/chords/movements/daylily-5838). |
| 102 | Full Circle — Movements | Opening verse phrase | A–E–F♯m | E major: IV–I–ii | Selected opening verse agrees in [this chart](https://www.guitartabsexplorer.com/movements/full-circle-chords) and [chart version 1](https://www.chords-and-tabs.net/song/name/movements-full-circle). A conflicting capo-4 acoustic chorus arrangement was not used. The three slots do not describe the whole verse. |
| 103 | Kept — Movements | Chorus phrase | Am–F–Am–C–Am–F–Am | A minor: i–VI–i–III–i–VI–i | Seven-change phrase explicitly labeled chorus in the [section chart](https://www.chords-and-tabs.net/song/name/movements-kept). Chart-based seed; rhythm and guitar articulation are player-controlled. |
| 104 | Keep This Up — The Story So Far | Verse phrase | E–C♯m–E–C♯m–A | E major: I–vi–I–vi–IV | Strong verse seed. [Reviewed section chart](https://www.cifraclub.com.br/the-story-so-far/keep-this-up/), [capo-2 arrangement](https://guitartuna.com/chords/keep-this-up-the-story-so-far-easy-guitar-chords-673edb6402172726c7f7976a). The latter uses D/Bm/G shapes, agreeing in concert pitch. Later F♯-major material is outside the chosen phrase. |
| 105 | Proper Dose — The Story So Far | First verse phrase | E♭–A♭–E♭–A♭–Fm–A♭ | E♭ major: I–IV–I–IV–ii–IV | [Chart](https://www.e-chords.com/keyboards/the-story-so-far/proper-dose) explicitly labels the first verse. Later B-major borrowed chords and the intro's E♭/G inversion are outside this passage. |
| 106 | Overstepping — Belmont | Verse loop | G♭–B♭m–A♭ | D♭ major: IV–vi–V | Chart-based verse seed from the [section arrangement](https://akkordov.net/akkordi/belmont/648520-overstepping). Keeps the short harmonic loop, omitting guitar riffs and voicing detail. |
| 107 | Country Girl — Belmont | Chorus loop | A–C♯m–E–F♯m | E major: IV–vi–I–ii | [Section chart](https://www.e-chords.com/chords/belmont/country-girl) supports the selected chorus loop. Triad seed, with performance timing left to the player. |
| 108 | Placeholder — The Story So Far | Chorus phrase | G–Bm–G–Bm–A | D major: IV–vi–IV–vi–V | [Chord chart](https://www.guitartabsexplorer.com/de/story-so-far/placeholder-chords), [guitar-tab reference](https://www.guitartabs.cc/tabs/t/the_story_so_far/placeholder_tab.html). Retains the selected harmonic order while omitting the guitar arrangement. |
| 109 | Bags — Clairo | Verse loop | G♭–B♭m–A♭ | D♭ major: IV–vi–V | [Chord chart](https://www.cifraclub.com/clairo/bags/) supports the loop. The [artist's Song Exploder interview](https://songexploder.net/transcripts/clairo-transcript.pdf) supplies arrangement context; it is not an independent exact chord transcription. |
| 110 | Sofia — Clairo | Chorus loop | Dm–G–C–F | C major: ii–V–I–IV | Explicit reduction of Dm7–G7–Cmaj7–Fmaj7. [Chart](https://lamucal.com/chords/clairo/sofia-1662765), [section/key analysis](https://www.hooktheory.com/theorytab/view/clairo/sofia). |
| 111 | Pretty Girl — Clairo | Chorus loop | Dm–C–Em–Dm | C major: ii–I–iii–ii | [Section chart](https://www.cifraclub.com/clairo/pretty-girl/), [analysis](https://www.hooktheory.com/theorytab/view/clairo/pretty-girl). A short chord seed rather than the full instrumental or vocal arrangement. |
| 112 | It's Not The Same Anymore — Rex Orange County | Chorus phrase | F–C–Gm–Dm | F major: I–V–ii–vi | Device name: **Not The Same**. [Chart](https://www.cifraclub.com/rex-orange-county/its-not-the-same-anymore/) supports the selected phrase. Slash-bass movement is dropped in this reduction. |
| 113 | THE SHADE — Rex Orange County | Chorus phrase | B♭–F–B♭–F | F major: IV–I–IV–I | Device name: **The Shade**. [Section chart](https://www.e-chords.com/chords/rex-orange-county/the-shade) supports this repeated two-chord phrase; major-seventh colors are omitted. Repeated cards do not store durations. |
| 114 | Pluto Projector — Rex Orange County | Simplified verse adaptation | C–Em–F–G | C major: I–iii–IV–V | Follows a [simplified cover by grentperez](https://www.youtube.com/watch?v=MUK9Ki5GLQ8). The device explicitly says **Rex: simple verse**. The [fuller arrangement](https://www.chords-and-tabs.net/song/name/rex-orange-county-pluto-projector-4) contains F♯ half-diminished and borrowed Fm material; this seed adapts that harmony and does not claim an exact original verse. |
| 115 | bloody valentine — mgk | Chorus loop | D–A–F♯m–F♯m | A major: IV–I–vi–vi | Strong simplified-triad seed. [Chart](https://www.guitartabsexplorer.com/machine-gun-kelly/bloody-valentine-chords), [corroborating arrangement](https://chordier.com/bloody-valentine-chords/). |
| 116 | my ex's best friend — mgk, blackbear | Chorus loop | D♭–Fm–E♭ | F minor: VI–i–VII | Three chord changes; no fourth chord is invented. [Section chart](https://vchords.com/en/chords/mgk-%28machine-gun-kelly%29/my-exs-best-friend-7307), [F-minor analysis](https://www.hooktheory.com/theorytab/view/machine-gun-kelly/my-exs-best-friend-(ft-blackbear)). |
| 117 | I Think I'm OKAY — mgk, YUNGBLUD, Travis Barker | Opening chorus phrase | C–Em–D–C–G–D | G major: IV–vi–V–IV–I–V | Six-slot excerpt. The full chorus exceeds eight changes. [Section chart](https://www.guitartabsexplorer.com/machine-gun-kelly/i-think-im-okay-chords). |
| 118 | I.F.L.Y. — Bazzi | Simplified verse / main loop | Em–A–D–G | D major: ii–V–I–IV | Explicit reduction of Em9–A13–Dmaj9–Gmaj7. [Verse chart](https://ukutabs.com/b/bazzi/i-f-l-y/), [GuitarTuna arrangement](https://guitartuna.com/chords/ifly-bazzi-easy-guitar-chords-61d369d454f3d7e4799c9c88). Strumming advice does not establish the recording's meter. |
| 119 | Intentions — Justin Bieber, Quavo | Chorus phrase | A–A–F♯m–D–F♯m–D | A major: I–I–vi–IV–vi–IV | Six-slot phrase explicitly labeled chorus. [Chart](https://www.cifraclub.com/justin-bieber/intentions/), [tutorial using capo 2](https://www.youtube.com/watch?v=3z_S_dotGlQ). |
| 120 | Hurt — Oliver Tree | Opening pre-chorus phrase | Am–C–G–F | A minor: i–III–VII–VI | This specific phrase precedes the main refrain. [Chart](https://www.cifraclub.com.br/oliver-tree/hurt/), [corroborating chart](https://www.wikisongbook.com/oliver-tree/hurt), [section/key analysis](https://www.hooktheory.com/theorytab/view/oliver-tree/hurt). |
| 121 | Habitual — Justin Bieber | Chorus phrase | A–B–E | E major: IV–V–I | Creator-posted tutorial identifies the chorus. [Tutorial](https://www.youtube.com/watch?v=Q0AcAXYelP8), [chart/key](https://www.cifraclub.com/justin-bieber/habitual/). |
| 122 | Confirmation — Justin Bieber | Verse / main loop | Fm–E♭–D♭–A♭–B♭m–Fm | F minor: i–VII–VI–III–iv–i | Concert F minor. Em shapes require capo 1; copies omitting the capo can mislead. [Creator tutorial](https://www.youtube.com/watch?v=pN0B68MgE6I), [capo-1 transcription](https://www.guitar-link.com/artist/J/Justin%20Bieber/Confirmation/1). |
| 123 | Changes — Justin Bieber | Simplified verse phrase | F–Em–Dm | C major: IV–iii–ii | Reduction of Fmaj7–Em7–Dm7. The verse was chosen because chorus arrangements disagree about an added Cmaj7. [Transcription](https://www.guitar-link.com/artist/J/Justin%20Bieber/Changes/1), [published original-key metadata](https://www.musicnotes.com/sheetmusic/justin-bieber/changes/MN0207625). |
| 124 | Forever — Justin Bieber, Post Malone, Clever | Opening chorus phrase | C♯m–E–C♯m–E | C♯ minor: i–III–i–III | Two repetitions of a two-chord phrase. [Section chart](https://ukutabs.com/j/justin-bieber/forever-feat-post-malone-clever/), [original-key reference](https://www.ilchitarristasullaspiaggia.it/2020/02/forever-justin-bieber-feat-post-malone-chords.html). Repeated cards do not store durations. |
| 125 | Yummy — Justin Bieber | Simplified opening chorus phrase | Gm–Am–Gm–Am | F major: ii–iii–ii–iii | Reduction of alternating Gm7/Am7. F major is a useful encoding; it does not settle the recording's tonal center against relative D minor. [No-capo chart](https://www.cifraclub.com/justin-bieber/yummy/), [corroborating arrangement](https://www.e-chords.com/en/chords/justin-bieber/yummy). |
| 126 | concert for aliens — mgk | Chorus / post-chorus loop | B–E–F♯–G♯m | B major: I–IV–V–vi | Chart-based four-chord seed. Main chart has a low user rating and a suspicious bridge typo; FM-1 listening should confirm this selected chorus. [Section chart](https://www.guitartabsexplorer.com/machine-gun-kelly/concert-for-aliens-chords), [recording-linked chord reference](https://chordu.com/chords-tabs-machine-gun-kelly-concert-for-aliens-official-lyric-video--id_5PTx1QrJ3PE). |
| 127 | Sick and Tired — iann dior, mgk, Travis Barker | Chorus / main loop | G♯m–E–B–C♯m | G♯ minor: i–VI–III–iv | Chart uses a transposed Em–C–G–Am arrangement; recording-linked references support concert G♯ minor. FM-1 listening should confirm sounding pitch. [Section chart](https://www.guitartabsexplorer.com/iann-dior/sick-and-tired-chords), [concert-pitch reference](https://chordify.net/pl/chords/ianndior-iann-dior-sick-and-tired-feat-machine-gun-kelly-travis-barker). |

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
| AMAZING — Rex Orange County | Borrowed minor-iv harmony needs to be retained or explicitly adapted; a fixed diatonic-major reduction would change it. |
| Loving Is Easy — Rex Orange County, Benny Sings | Resolve the A♭/B♭ material and exact selected phrase before assigning one fixed key/mode. |
| Cut My Hair — Mounika., Cavetown | [Chart](https://ukutabs.com/m/mounika/cut-my-hair-feat-cavetown/) changes E♭ major to E♭ minor in the verse and also uses C major. A single fixed diatonic chord quality cannot capture this mixture. Preserve those changes rather than substituting easier chords. |
| Alien Boy — Oliver Tree | [Guitar chart](https://www.chords-and-tabs.net/song/name/oliver-tree-alien-boy) uses Fmaj7–Em7–Am with capo 1, implying concert G♭maj7–Fm7–B♭m. A triad reduction fits B♭ minor VI–v–i, but [the analysis](https://www.hooktheory.com/theorytab/view/oliver-tree/alien-boy) includes richer harmony and bass detail. Check the exact selected passage before promoting it. |
| Cash Machine — Oliver Tree | Public arrangements disagree on chord qualities and sometimes roots. [One arrangement](https://www.recifra.com/oliver-tree/cash-machine/?hl=en) gives F♯m–A–C♯m–D for the opening chorus, while [the section analysis](https://www.hooktheory.com/theorytab/view/oliver-tree/cash-machine) flags secondary/borrowed harmony. Needs a recording check. |

The main refrain of **Hurt** is a separate future snippet: Am–C–F–**E major**.
Natural minor would incorrectly produce Em. The runtime already supports a
harmonic-minor scale, and its policy of choosing quality from the scale third
while normalizing fifths can produce these four seed triads in A harmonic minor.
Adding that mode to the catalog needs explicit mode-coverage and label checks.
The G-natural root in the implemented pre-chorus is outside A harmonic minor,
so the two sections should keep distinct setups.

## Implementation and verification scope

The additions append IDs 101–127 and preserve the earlier bank. Catalog data
stores up to eight degrees, one tonic, one mode and a suggested meter. The
current bank uses the existing scale-derived major/minor quality and perfect
fifths. Per-slot extensions, inversions, bass overrides and durations are not
stored. The new entries use the existing accompaniment engine with its one-bar-per-slot
default; no original song timing or complete arrangement is encoded.

Independent expected-chord fixtures cover every appended entry, including the
short three-slot loops and repeated two-chord phrases. On November 7, 2026,
`sh tools/test_progressions.sh` passed the catalog and UI checks, with identical
audio output at 44.1 and 48 kHz. These host checks establish implementation
consistency. FM-1 listening remains useful for the weaker-source entries,
sounding keys, phrasing and the explicit adaptations.

This work does not publish a release package, flash a device or modify the
Spotify source playlists. Full source coverage, preference accounting and
harmonic evidence are retained so the next bank can continue from this list.
