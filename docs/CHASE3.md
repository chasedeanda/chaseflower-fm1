# ChaseFlower 0.6-chase3 — ambient and lo-fi audition release

This release adds **96 chord/layer patches, 24 bass patches and eight drum kits** in eight style banks, three 6/8
grooves, and Chord/Keyboard views on HOME. It also restores eight previously hidden FM factory sounds.
The first build is intentionally broad: keep, tweak or hide sounds after playing them.

## Install this file

Use **`chaseflower-0.6-chase3.fwsc`**, from `releases/0.6-chase3/` or the supplied install ZIP. A `.patch` file is
source code; it cannot be installed directly on the FM-1. The HOME-view patch is already compiled into this firmware.

1. Keep a current backup using the same backup workflow used for chase2. The original November 6 backup remains
   unchanged in this repository. A current backup also captures anything created since then.
2. Connect the FM-1 with the working USB data cable and open the
   [custom-build installer section](https://ficusficusov.github.io/CoralFlower/#own).
3. Under **Another version, or a build of your own**, choose **`chaseflower-0.6-chase3.fwsc`**.
4. Confirm that exact filename is selected before clicking Install. The website's default selection installs
   upstream CoralFlower; reselect the file if you reload the page.
5. Follow the installer's device prompts. After restarting, the splash should say ChaseFlower with version
   **0.6-chase3**.

No tool in this development task flashes your device automatically. The previously working chase2 installer remains
in `releases/0.6-chase2/` if you want to return to it. New sounds are unavailable in chase2; existing old sounds and
their saved selections keep the same positions in chase3.

## Try the HOME views

While already on HOME, tap HOME to cycle **Flower → Chord → Keyboard**. From another screen, HOME returns home.
The view selection is saved across power-off and backup/restore.

- **Chord:** a large chord name, voiced notes with their scale degrees, and bass note(s).
- **Keyboard:** five octaves, chord and bass colors, and note labels. The window follows high/low voicings.
- The views follow live playing, the arpeggiator, and the sequencer's chord display; the last chord remains visible.

Credit: Michael Norris / @norrismw, [CoralFlower issue #8](https://github.com/FicusFicusov/CoralFlower/issues/8).

## Browse the banks

Open **GLO**, then turn **SELECT** to the last page, **Library**.

- **K1:** chord sound bank.
- **K2:** bass sound bank.
- **K3:** layer sound bank.
- **K4:** return all three filters to All.

Return HOME. PRESETS changes the chord sound; SELECT changes the bass sound, within the chosen filters. Pattern and
layer sound selection use the corresponding filters too; a pattern can always choose HOME's sound.

Changing a filter leaves the loaded sound and its edits untouched until you choose another sound. The filters reset
to All at power-on; sound selections themselves are saved as before. An empty User bank leaves the current sound alone.

Each new style bank contains **12 chord/layer choices and three basses**. The matching kits are available on the
rhythm machine and sequencer's KIT control:

| Bank | Character / first audition direction | Matching kit |
| --- | --- | --- |
| Dusty Room | Lo-fi keys, muted tones, dusty textures | Dust |
| Tape Garden | Slow pads, faded strings, gentle pitch/noise movement | Tape |
| Night Jazz | Electric keys, organ and mellow plucks | Brush |
| Soft Focus | Warm pads, choir-like textures and swells | Air |
| Glass Cloud | Bright pads, bell-like sounds and shimmer | Glass |
| Dub Bloom | Dark stabs, organs, resonant tones and subs | Dub |
| Wood & Water | Mallets, plucks and reed-like synths | Wood |
| Chill Circuit | Soft electronic chords, moving pads and bass | Chill |

**All** includes every sound; **Existing** includes the older factory sounds and the eight restored FM voices;
**User** includes saved user sounds. A saved replacement can retain its new factory bank's identity.

The 120 new instruments comprise 32 original ChaseFlower FM voices, 32 unchanged CC0 Aquanode voices, and 56 original
VA designs (32 chord/layer and 24 bass). The kits use existing drum synthesis. These are synth interpretations, rather
than acoustic recordings. Names starting **AQ01–AQ32** identify the imported voices; genre placement is provisional.
The full catalog gives their original bank, slot, name, license and stable sound ID.

Suggested starting points: Dusty EP / Felt Keys in Dusty Room; Tape Bloom / Warped Choir in Tape Garden; Soft Horizon
/ Cloud Choir in Soft Focus. Keep the bank broad until you have tried it with your own playing.

## Play the three 6/8 grooves

1. Press **SEQ** to open the metronome.
2. Turn **SELECT** to select **Rhythm Machine**, then press **SEQ** again.
3. Turn **SELECT** through the rhythms to **Ballad**, **Electro** or **Lo-Fi 6**, each labeled **6/8**.
4. Press PLAY. K1 changes kit, K2 drums volume, K3 tempo and K4 swing. SEL restores the rhythm's own kit/swing.
5. Press EDIT to turn the rhythm into a sequencer pattern you can change.

Each groove is two bars with a restrained variation. Defaults are Brush (Ballad), Chill (Electro) and Dust (Lo-Fi 6).
The suggested tempos under the groove names are recommendations; selecting a groove preserves your actual TEMPO.

TEMPO remains **quarter-note BPM**, matching MIDI clock. In 6/8 the six metronome counts are eighths, accented on
1 and 4. A bar contains three quarter notes / 72 MIDI-clock pulses. For example, TEMPO 90 gives two dotted-quarter
pulses at 60 per minute. This corrects the inherited six-quarter timing for 6/8; existing 6/8 patterns play twice as
fast at the same numeric tempo compared with chase2. Other meters keep their timing. No stored pattern data is rewritten.

## Listen and curate

The audition ZIP contains four-second dry sketches, grouped by bank: three seconds held and one second released for
instruments, or the same 6/8 rhythm for drum-kit comparisons. Notes are C3–G3–B3–D4–E4–A4 for chord/layer sounds and
C2 for bass. Long pad releases extend beyond these short previews on the instrument.

The catalog uses stable IDs such as `CF3-F001`, `CF3-V005` and `CF3-D001`. Record **Keep / Tweak / Hide** beside an ID
in the review CSV. Hiding a sound later should leave its source and identity available in Git.

Factory sound changes preserve your current part effect sends. For a warm ambient starting point, try a moderate
chorus/reverb send and a low delay send; for lo-fi keys, start with less reverb so the attack remains clear. Those
are audition suggestions, not preset changes to your mixer. Level trims are conservative peak-based adjustments,
not a guarantee that every patch has equal perceived loudness.

## USB audio and verification

USB recording retains chase2's six input channels: 1/2 full master, 3/4 dry chord and 5/6 dry bass, at 44.1 or 48 kHz.
The [USB guide](USB-AUDIO.md) still applies, including the GarageBand/Spark setup.

Host verification covers the complete new library, USB capture during rendering, saved sound and blob round trips,
old sound-list positions with user banks/replacements, view persistence, internal and MIDI 6/8 clocks, sequencer
conversion, full FX/layer stress, AddressSanitizer, the existing suites and the real JieLi firmware build. The
verification report records the final build sizes and checksums. Representative original sounds produce identical
physical-output words in chase2 and chase3.

This candidate still needs your FM-1 listening and performance check. Hardware USB timing and CPU margin cannot be
established by the host emulator. Start with the three grooves and the first three sound banks, then explore the rest.
