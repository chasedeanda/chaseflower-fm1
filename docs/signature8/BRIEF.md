# CORD Signature 8 — Artist-referenced research and fixed implementation handoff

**Research owner:** ChatGPT + CORD owner. **Implementation owner:** Codex.  
**Repository:** https://github.com/chasedeanda/cord-fm1  
**Reference branch inspected:** `main` (2026-11-10).  
**Scope:** Eight artist-referenced **original** factory sound prototypes, no additional synthesis engines, no library expansion beyond these eight in this phase, no auto-generated research tasks for Codex.

## Decisions already made — do not re-decide

1. Develop **exactly eight prototypes**, in the existing eight thematic banks, in the order and with the names below. Keep this as a reversible preview rather than replacing old sounds in place.
2. **Do not** ask Codex to research artists, select genres or decide which sounds deserve to exist. These decisions are in this document.
3. Program each sound with the exact starting patch defined below, using the existing `op`/`voice`/`pack` helpers for FM6 and `OSC`/`FLT`/`ENV`/`LFO`/`M_` macros for VA. These are **fixed initial sound-design specifications**, not alleged artist settings. If a preset cannot compile due to an engine constraint, preserve the closest expressible version and report the precise conflict; do not substitute a new creative sound.
4. Factory sound IDs, storage compatibility, and existing patch bytes are immutable. Append the new entries under new stable indices and expose them **only in a pilot / curated Signature 8 view** without changing saved global indices. Leave old entries addressable in the full library. The repository already uses append-only FM indices and browse filters; work within those invariants.
5. Do not recreate or incorporate commercial recordings, samples, trademarks as preset names, or proprietary patches. Artist names appear only in research metadata/documentation, not the device's preset labels.
6. Initial test is **technical**: dry synthesis from the real CORD engine, clipping, stuck notes, pitch/register, CPU/voice budget, file identities, and reproducibility. Neither Codex nor the owner needs to perform a 157-preset subjective review to start.
7. Don't release firmware or introduce another feature before delivering the eight patch implementations and the reproducible test report.

## Definitive eight-song board

| # | Bank | New device name | Engine | Specific recording and target | Exact source preset to distinguish from |
|---|---|---|---|---|---|
| 1 | Dusty Room | **EMBER KEYS** | FM6 | **Nils Frahm — “Keep”** (*Felt*, 2011). The intimate, felt-damped piano body with quietly softened attack—not the creaking-floorboard recording itself. | `CF3-F002 FELT KEYS`, `CF3-F018 HOLLOW TINES` |
| 2 | Tape Garden | **GHOST CHOIR** | VA | **Cocteau Twins — “Heaven or Las Vegas”** (1990). The wide, harmonically blurry sustained guitar/voice-like layers, particularly Robin Guthrie's clean, detuned guitar texture. | `CF3-F010 WARPED CHOIR`, `CF3-F034 ICE CHOIR`, inherited `BREATH PAD` |
| 3 | Night Jazz | **NOIR TINES** | FM6 | **Herbie Hancock — “Butterfly”** (*Thrust*, 1974). Fender Rhodes body and articulating tines, adapted to classic six-op FM with independent velocity response. | `CF3-F001 DUSTY EP`, `CF3-F017 VELVET EP`, inherited `DX TINE` |
| 4 | Soft Focus | **APOLLO VEIL** | VA | **Brian Eno — “An Ending (Ascent)”** (*Apollo: Atmospheres and Soundtracks*, 1983). Soft, sustained, low-motion harmonic veil, not a dramatic modulation showcase. | inherited `AURORA`, `CF3-F026 CLOUD CHOIR`, `CF3-V023 GENTLE SWELL` |
| 5 | Glass Cloud | **FROSTWIRE** | VA | **SOPHIE — “Faceshopping”** (*OIL OF EVERY PEARL'S UN-INSIDES*, 2018). A pitched, elastic metallic synthetic strike drawn from the recording's hyperreal abrasive sound-design language. | `CF3-F033 GLASS RAIN`, `CF3-F036 AIR BELLS`, existing `BELL BASS` ring-mod VA |
| 6 | Dub Bloom | **OFFBEAT DUB** | VA | **Jackie Mittoo & The Soul Vendors — “Hot Milk”** (1968). The compact Hammond-style keyboard articulation and offbeat Jamaican rhythm vocabulary. **Not** an attempt to recreate the whole studio recording. | `CF3-F044 MUTED SKANK`, `CF3-F043 ROUND ORGAN`, inherited `ORGANISH` |
| 7 | Wood & Water | **CEDAR MALLET** | FM6 | **Steve Reich — “Mallet Quartet,” I. Fast** (2009). The woody interlocking marimba attack, distinct from the sustained vibraphones. | `CF3-F049 WOOD MALLET`, `CF3-F052 BAMBOO BELL`, inherited `DX MARIMBA` |
| 8 | Chill Circuit | **RAZOR PULSE** | VA | **The Cars — “Let's Go”** (*Candy-O*, 1979). Greg Hawkes' nasal Prophet-5 hard-sync attack and falling harmonic sweep. | `CF3-F059 GENTLE SYNC`, inherited VA `HOLLOW` (hard sync) and `SYNC BASS` |

### Evidence level and exact musical-to-synthesis translation

**01 Frahm:** The artist's [Felt page](https://www.nilsfrahm.com/works/felt/) explicitly says thick felt damped piano strings, light playing, and close microphones; “Keep” is on the tracklist. **Translation:** transient-suppressed FM piano with warm body. **Do not claim** that Frahm used a DX7.

**02 Guthrie:** In [his own technical account](https://cocteautwins.com/cocteau-twins-equipment-and-sounds.html), Guthrie describes clean PRS guitar through a Marshall preamp, Lexicon pitch spread at +10/-10 cents, Dimension D, and tempo-synced TC2290 echo on “Heaven or Las Vegas.” **Translation:** a synthesized, detuned, slowly moving harmonic choir with modest stereo spread; optional existing `Wide Drift` FX preview. **Do not claim** CORD recreates his guitar tone or effects exactly.

**03 Hancock:** [Hancock's official *Thrust* discography](https://www.herbiehancock.com/music/discography/album/thrust/) lists the song and credits his Fender Rhodes. Yamaha's [FM 101 Part Five](https://yamahasynth.com/learn/synth-programming/fm-101-part-five-expanding-your-sound/) documents a three-pair FM electric piano with a 14:1 short upper-tine modulator, velocity-to-modulation depth and detuned carriers. **Translation:** a responsive CORD-specific three-pair six-op voice. Yamaha's published numbers are **FM-X and must not be passed off as DX7-compatible raw rate values**; the CORD values below are explicitly an engineering starting program.

**04 Eno:** [MusicRadar's recording analysis](https://www.musicradar.com/news/the-40-greatest-synth-sounds-of-all-time-no-28-brian-eno-an-ending-ascent) and [Eno's interview](https://www.muzines.co.uk/articles/brian-eno/7300) support the ambient-synth context, while the specific patch/instrument layering on this song remains uncertain. **Translation:** sparse, stable, slowly changing VA pad, intentionally **not** another four-oscillator AURORA clone.

**05 SOPHIE:** [Pitchfork's musician interviews](https://pitchfork.com/features/article/how-sophies-music-inspired-a-generation-of-underground-artists/) and [Vogue's SOPHIE interview](https://www.vogue.com/article/sophie-face-shopping-album-british-producer-beauty-standards-red-hair) document the contrasting abrasive synthetic sound and physical/rubbery/metallic sound vocabulary. [Sound on Sound](https://www.soundonsound.com/techniques/synth-school-part-2) explains ring modulation's sum/difference sidebands and metallic results. **Translation:** original CORD ring-mod metallic strike. **Do not assert** SOPHIE used ring modulation on this track; the exact technique is unverified.

**06 Mittoo:** [AllMusic's “Hot Milk” review](https://www.allmusic.com/song/hot-milk-mt0031754811) identifies the organ and piano timbres; [Jamaican Music Atlas](https://jamaicamusicatlas.com/story/keys-behind-the-riddim) explains short offbeat organ skank and piano bubble; [the riddim archive](https://riddim-id.com/riddims/209/hot-milk) identifies the 1968 recording. **Translation:** fast, midrange-heavy VA additive organ chord stab, no long reverb.

**07 Reich:** [Reich's own composition notes](https://stevereich.com/composition/mallet-quartet/) confirm two five-octave marimbas and two vibraphones, and that the fast outer movements interlock marimba backgrounds while vibes carry melody. **Translation:** woody, short FM marimba-like pluck suitable for interlocking notes, **not** a generic vibraphone bell.

**08 Cars:** [MusicRadar's classic-preset analysis](https://www.musicradar.com/news/the-10-best-synth-presets-ever-used-and-where-to-hear-them) identifies Greg Hawkes' Prophet-5 oscillator-sync on “Let's Go”; [Sequential's Take 5 guide](https://sequential.com/wp-content/uploads/2025/07/Take-5-Users-Guide-v2_2.pdf) describes a modern recreation. **Translation:** fast downward sync-harmonic sweep on CORD's existing OSC2 hard-sync and ENV3→OSC2 pitch route. **Do not reuse** commercial factory-preset bytes.

## Fixed synthesis recipes for the prototype

All values below are raw CORD patch API values (where this file uses engine macros) or `tools/gen_fm6_patches.py` helper values (where this file uses `op`/`voice`). They are not claimed to be historical artist settings. If a patch has to be clipped or normalized for hardware safety, that **technical** adjustment is permitted, must be logged and must not change algorithm/oscillator identity.

### FM6 presets — implement with `op`, `voice`, `pack` from `tools/gen_fm6_patches.py`

**EMBER KEYS** — algorithm 5 (three modulator-carrier pairs), feedback 0, transpose neutral 24, no pitch LFO. The program emphasizes body carriers and suppresses modulator attack.

```python
EMBER_KEYS = voice('EMBER KEYS', 5, [
    op(r=(99,42,25,60), l=(99,68,0,0), ol=91, fc=1, kvs=2, rs=3),
    op(r=(99,79,46,45), l=(68,0,0,0), ol=24, fc=1, kvs=4, rs=3),
    op(r=(99,48,27,61), l=(99,63,0,0), ol=57, fc=2, det=8, kvs=2, rs=2),
    op(r=(99,78,43,44), l=(72,0,0,0), ol=17, fc=2, kvs=4, rs=2),
    op(r=(99,51,25,59), l=(99,57,0,0), ol=34, fc=1, det=6, kvs=1),
    op(r=(99,82,45,42), l=(62,0,0,0), ol=10, fc=3, kvs=2),
], fb=0, lfs=22, lfd=0, lpmd=0, lpms=0, trnsp=24)
```

**NOIR TINES** — algorithm 5, feedback 3, and specifically a short 14:1 tine modulator (`OP6` into `OP5`) whose brightness is highly velocity-sensitive. This differs from the soft attack of Ember Keys.

```python
NOIR_TINES = voice('NOIR TINES', 5, [
    op(r=(99,31,24,65), l=(99,82,0,0), ol=95, fc=1, kvs=2, rs=3),
    op(r=(99,55,35,55), l=(99,66,0,0), ol=66, fc=1, kvs=5, rs=3),
    op(r=(99,34,26,63), l=(99,78,0,0), ol=82, fc=1, det=8, kvs=2, rs=2),
    op(r=(99,59,38,55), l=(99,65,0,0), ol=58, fc=2, kvs=6, rs=2),
    op(r=(99,43,28,58), l=(99,66,0,0), ol=66, fc=1, det=6, kvs=2),
    op(r=(99,87,65,42), l=(99,0,0,0), ol=55, fc=14, kvs=7, rs=3),
], fb=3, lfs=28, lfd=20, lpmd=0, lpms=0, trnsp=24)
```

**CEDAR MALLET** — algorithm 4 (two serially modulated tonal branches, unlike the three parallel-pair template that dominates the custom FM library). Woody lower-pitched body, a short brighter bar-strike branch, no sustain. Do not enable pitch LFO.

```python
CEDAR_MALLET = voice('CEDAR MALT', 4, [
    op(r=(99,51,38,47), l=(99,0,0,0), ol=95, fc=1, kvs=3, rs=3),
    op(r=(99,78,48,44), l=(99,0,0,0), ol=48, fc=4, kvs=5, rs=3),
    op(r=(99,87,60,36), l=(80,0,0,0), ol=24, fc=2, kvs=5, rs=2),
    op(r=(99,72,46,42), l=(92,0,0,0), ol=45, fc=2, kvs=3, rs=3),
    op(r=(99,90,53,35), l=(90,0,0,0), ol=32, fc=4, kvs=5, rs=3),
    op(r=(99,91,60,30), l=(67,0,0,0), ol=15, fc=2, kvs=4, rs=2),
], fb=0, lfs=20, lfd=0, lpmd=0, lpms=0, trnsp=24)
```

### VA presets — implement directly in the generated patch source, using the existing macros in `firmware/src/eng_va.c`

Names below are display labels (12 characters maximum). Use these **exact** starting definitions rather than choosing replacement oscillator configurations.

**GHOST CHOIR** — wide, breath-supported detuned harmonic layer, not a formant/vowel sweep:

```c
static const uint8_t SIG_GHOST_CHOIR[] = {
    OSC(0, PWM, 58, 0, -7), MODE(0, MORPH), O_(0, SHAPE), 57,
    OSC(1, TRI, 51, 0, 7), OSC(2, SIN, 25, 12, 0),
    MODE(3, NOISE), O_(3, WAVE), VN_WHITE, O_(3, LEVEL), 9, O_(3, KTRK), 0,
    FLT(LP, 70, 12, 5), VA_FTYPE, 7, VA_FSPREAD, 62,
    ENV1(91, 94, 115, 104), ENV2(93, 90, 70, 100), VA_VEL, 45,
    LFO(0, 22, 0, 127, 0), LFO(1, 31, 1, 127, 0), LFO(2, 18, 0, 127, 0),
    M_(0, VS_LFO1, VD_SHP1, 12), M_(1, VS_LFO1 + 1, VD_SPREAD, 18),
    M_(2, VS_LFO1 + 2, VD_CUT, 5), 0xFF
};
```

**APOLLO VEIL** — deliberately sparse: two pitched oscillators, NO random pitch movement, NO four-oscillator swirl; two slow LFOs affect spectrum/spread, very long attack/release. This is the technical distinction from inherited `AURORA`.

```c
static const uint8_t SIG_APOLLO_VEIL[] = {
    OSC(0, SAW, 57, 0, -2), MODE(0, MORPH), O_(0, SHAPE), 23,
    OSC(1, SIN, 42, 0, 2), FLT(LP, 57, 6, 3),
    VA_FTYPE, 5, VA_FSPREAD, 26, VA_VEL, 30,
    ENV1(105, 100, 122, 111), ENV2(103, 103, 86, 110),
    LFO(0, 16, 0, 127, 0), LFO(1, 25, 1, 127, 0),
    M_(0, VS_LFO1, VD_SHP1, 9), M_(1, VS_LFO1 + 1, VD_SPREAD, 12),
    M_(2, VS_LFO1 + 1, VD_FTYPE, 3), 0xFF
};
```

**FROSTWIRE** — pitched fundamental plus ring-produced inharmonic sidebands, short strike, band-pass shape. SOPHIE is the **aesthetic reference**, and ring modulation is our own documented technique.

```c
static const uint8_t SIG_FROSTWIRE[] = {
    OSC(0, SIN, 35, 0, 0), OSC(2, SIN, 0, 17, 0),
    OSC(3, SIN, 76, 0, 8), VA_RING4, 1,
    FLT(BP, 93, 14, 0), VA_FSPREAD, 16,
    ENV1(1, 79, 0, 82), ENV2(0, 67, 0, 72), VA_VEL, 90,
    LFO(0, 26, 4, 127, 0),
    M_(0, VS_ENV1 + 1, VD_LVL1 + 3, 13),
    M_(1, VS_LFO1, VD_CUT, 4), 0xFF
};
```

**OFFBEAT DUB** — four discrete organ partials (root, octave, fifth-above-octave, two octaves) with short, controlled envelope; no movement necessary. This is not the same as a sustained drawbar pad.

```c
static const uint8_t SIG_OFFBEAT_DUB[] = {
    OSC(0, SIN, 73, 0, 0), OSC(1, SIN, 44, 12, 0),
    OSC(2, SIN, 26, 19, 0), OSC(3, SIN, 13, 24, 0),
    FLT(LP, 92, 0, 0), VA_VEL, 24,
    ENV1(1, 68, 42, 28), 0xFF
};
```

**RAZOR PULSE** — two saw oscillators, existing hard sync OSC2, fast ENV3 sweep of OSC2 pitch, secondary velocity-to-cutoff; compact rhythmic lead, no added distortion by default.

```c
static const uint8_t SIG_RAZOR_PULSE[] = {
    OSC(0, SAW, 24, 0, 0), OSC(1, SAW, 86, 12, 0),
    VA_SYNC2, 1, FLT(LP, 92, 12, 0), VA_DRIVE, 8,
    ENV1(1, 72, 87, 38), ENV3(0, 79, 0, 34), VA_VEL, 68,
    M_(0, VS_ENV1 + 2, VD_PIT1 + 1, 34),
    M_(1, VS_VEL, VD_CUT, 10), 0xFF
};
```

## Explicit integration instructions

- **Bank 1** EMBER KEYS, **Bank 2** GHOST CHOIR, **Bank 3** NOIR TINES, **Bank 4** APOLLO VEIL, **Bank 5** FROSTWIRE, **Bank 6** OFFBEAT DUB, **Bank 7** CEDAR MALLET, **Bank 8** RAZOR PULSE.
- These are **new factory programs**, added after existing IDs; do not repurpose `CF3-Fxxx`/`CF3-Vxxx` identifiers.
- Provide a dedicated browsable `Signature 8` **pseudo-filter** in the current sound-bank/filter architecture (membership is the eight new factory IDs). Each new sound also retains its assigned one-of-eight style-bank association; the filter must not consume or repurpose one of those bank IDs. The full existing library remains available. Preserve saved global sound indices, and avoid changing storage schema unless strictly required; implement lookup filtering without reordering persisted entries.
- Every new sound gets neutral transpose (FM6 `trnsp=24`, VA no global/transposed pitch). Pitch correction of the old Aquanode bank is a separate follow-up, not a reason to modify this task.
- Preview the existing effects **without baking them into the sound**: EMBER=Velvet; GHOST=Wide Drift; NOIR=Silver Plate; APOLLO=Deep Space; FROSTWIRE=Phase Sweep; OFFBEAT=Slap; CEDAR=Silver Plate; RAZOR=Bite. Keep these as optional preset examples; dry sound remains the default.
- The render/test fixture is fixed: **C4 velocity 45, 80, 115 for all**; Cmaj9 (`C4 E4 G4 B4 D5`) 2-second sustain for chord-capable programs; **A3–C4–E4–G4** evenly spaced 250 ms notes for plucks, stabs, and leads; **C3** for low-register coverage and **C5** for high-register coverage; 3-second sustain + 2-second release on sustained textures. For mallet and percussive presets render the entire natural decay. Reverb bypassed for dry tests.
- Render eight dry files and eight with their fixed optional FX examples, plus a baseline dry version of each named comparison sound. Label WAVs with names and source revision.
- Automated tests: no clipping, DC runaway, stuck voices, unbounded release, invalid patch values, dropped drum timing, or changed old program bytes; report measured peak/RMS, active voices, CPU estimate, and any rendering regression. This is engineering validation—not proof of similarity to the artist recording.
- If a patch does not reach the desired effect, report the mismatch (e.g., missing mod route, lack of amplitude, inadequate spectral decay); **do not silently invent a different sound**. Return the technical failure to the research team for a revised specification.

## Existing duplicates to remove from *primary* discovery once Signature 8 is built

These entries remain present at their original indices for legacy content:

- `CF3-F017 VELVET EP`: source-structurally duplicates `CF3-F001 DUSTY EP` except for LFO speed.
- `CF3-F018 HOLLOW TINES`: equivalent core to `CF3-F002 FELT KEYS` except LFO speed.
- `CF3-F020 SOFT VIBRA`, `CF3-F036 AIR BELLS`, `CF3-F052 BAMBOO BELL`: same core as `CF3-F004 MUTED BELL` except LFO speed.
- `CF3-F034 ICE CHOIR`: core duplicate of `CF3-F010 WARPED CHOIR`.
- `CF3-F035 CRYSTAL VEIL`: core duplicate of `CF3-F011 DUSK STRINGS`.
- `CF3-F043 ROUND ORGAN` and `CF3-F042 HOLLOW CHORD`: 3-member core cluster with `CF3-F019 LOUNGE ORGAN`, with `HOLLOW CHORD` and `ROUND ORGAN` strictly identical at synthesis-program-byte level.
- `CF3-F057 SOFT PWM`: core duplicate of `CF3-F009 TAPE BLOOM`.
- `CF3-F049 WOOD MALLET`: core duplicate of `CF3-F033 GLASS RAIN` (hide from primary only when CEDAR MALLET is present).
- `CF3-F060 MUTED ARP`: core duplicate of `CF3-F044 MUTED SKANK`.

These groupings are direct comparisons of the packed FM6 programs (`[0:118]`), ignoring only LFO speed byte 112, and preserve name bytes 118–127 separately.

### Aquanode preservation and normalization — explicit, separate future task

Source audit confirms 34 imported, byte-distinct programs using 14 algorithms. Keep source bytes untouched. Five programs are stored **+24 semitones**: `AQ02 DX FAST`, `AQ03 DX FAST`, `AQ25 DX 5`, `AQ26 DX 6`, `AQ29 DX 7`. Four are stored **+12 to +14 semitones**: `AQ04 DX CRAC`, `AQ08 DX FADE`, `AQ09 DX FLUT`, `AQ32 DX 10`. Do **not** lower all 34 sounds: 12 have a negative offset and 7 are neutral. Proposed separate implementation: expose an adjusted-register **curated alias** for high-transpose patches while keeping original SysEx bytes and original factory entry intact. Do not include this task in Signature 8 coding.

## Finish line

Codex delivers **eight exact new programs**, a reversible `Signature 8` collection, deterministic test recordings, source-code changes, and a technical report. **No artist research, no exploration of alternative concepts, no unrequested ninth patch, no new engines, no release**, no manual listening assignment to the owner.
