# Chase FM-1

Chase's personal firmware for the **M-VAVE FM-1**, built on [CoralFlower](https://github.com/FicusFicusov/CoralFlower).

**Release 0.6-chase4** combines the swimming bass and bonfire visuals, bass-jump boot screen and Diagnostics
dashboard with the ambient/lo-fi sound banks, sequencer and six-channel USB recording.

**[Download the firmware (.fwsc)](https://github.com/chasedeanda/chaseflower-fm1/releases/download/v0.6-chase4/coralflower-0.6-chase4.fwsc)** ·
[Release notes](docs/releases/0.6-chase4.md) · [All releases](https://github.com/chasedeanda/chaseflower-fm1/releases)

<p align="center"><img src="docs/images/chase-bass.gif" width="480" alt="Chase FM-1 HOME: a swimming bass with body motion and a small school of baitfish"></p>

<table>
  <tr>
    <td align="center"><img src="docs/images/chase-bonfire.gif" width="320" alt="Bonfire: a triangular golden flame and rising embers"><br>Bonfire</td>
    <td align="center"><img src="docs/images/chase-boot.gif" width="320" alt="Chase FM-1 boot: the bass jumps from a moonlit lake and swims completely off screen"><br>Chase FM-1 startup</td>
  </tr>
</table>

These animations come from the firmware renderer. The boot preview holds its last frame for viewing; the
instrument's intro runs for 2.4 seconds and any input skips it.

## Play and explore

- **Top Lock, always.** Upper keys choose a chord type; lower keys play roots. The chosen chord stays latched until
  you select another one. Your saved Top chord-button definitions are retained.
- **Bass or Bonfire.** Turn **ALGORITHM** on HOME to change scenes without interrupting a held chord or playback.
  Bass is the default and your choice is saved. ALGORITHM still orders effects in FX and selects patterns/songs
  in Sequencer.
- **Four HOME views.** Tap HOME while already there to cycle **Scene → Chord → Keyboard → Diagnostics**. From
  another screen, HOME returns to your selected view.
- **Chords, bass, arpeggios and grooves.** Choose from 27 scales, chord extensions and inversions, a separate bass
  voice, nine arpeggiator patterns, eight sequencer patterns and a song arrangement. Three additional grooves use 6/8.
- **Ambient and lo-fi banks.** The chase3 library adds 96 chord/layer sounds, 24 bass sounds and eight drum kits in
  eight style banks, including 32 CC0 Aquanode voices. Existing saved sound positions and user sounds are preserved.
- **Six effects.** Filter, drive, chorus, phaser, delay and reverb, in the order you choose.
- **USB audio and MIDI.** Record six-channel USB capture at 44.1 or 48 kHz while USB MIDI remains available.
  See the [GarageBand and Spark setup](docs/USB-AUDIO.md).
- **Diagnostics.** Monitor audio workload, UI cadence, linked memory and USB status. Configure the dashboard from
  **GLO → Diagnostics**. [Read the measurements guide](docs/DIAGNOSTICS.md) before interpreting timing numbers.

[Scenes and controls](docs/VISUALS.md) · [Sound banks and 6/8](docs/CHASE3.md) ·
[Diagnostics](docs/DIAGNOSTICS.md) · [USB recording](docs/USB-AUDIO.md)

**Next build:** [Auto harmony and smart modifiers](docs/AUTO-HARMONY.md) are available in source via
**GLO → Harmony**. They are not included in the existing 0.6-chase4 download above.

## Install this release

1. Download **`coralflower-0.6-chase4.fwsc`** from the [GitHub release](https://github.com/chasedeanda/chaseflower-fm1/releases/tag/v0.6-chase4).
   This is the installable file; the separate app `.bin` and GitHub source archives are not installer packages.
2. Save a current backup of your sounds, settings and patterns. The installer can back up compatible CoralFlower,
   ChoralRoot and Felucca data. Keep the backup somewhere safe.
3. Open the [upstream installer's custom-build section](https://ficusficusov.github.io/CoralFlower/#own) in Chrome
   or Edge on a computer. Connect the FM-1 directly with a data-capable USB cable.
4. Expand **Another version, or a build of your own**, click **Choose a .fwsc file**, and select your download.
   Check that **`coralflower-0.6-chase4.fwsc`** is selected before pressing Install. The installer's default is
   upstream CoralFlower, so select this custom file first.
5. Install from that custom-build section and keep the cable connected and page open until the restart.
6. Confirm the **Chase FM-1** bass-jump startup and **`CoralFlower 0.6-chase4`** in INFO. The INFO family, package
   identity `FM-1_930` and USB names remain compatible with the existing installer and recording setup.

The release includes checksums and licence files. See [release notes](docs/releases/0.6-chase4.md) for validation
and recovery details. Host and firmware-build checks pass; the combined release's hardware appearance and timing
still need to be checked on the FM-1.

## Build and verify

Generate headers with Python 3, Pillow 11.1.0 and fontTools 4.66.1, then run the feature checks:

```sh
python3 tools/build.py --gen-only
sh tools/test_visuals.sh
sh tools/test_diagnostics.sh
sh tools/test_chase3.sh
sh tools/test_usb.sh
```

Build with the JieLi Linux x86-64 toolchain (Docker on Apple Silicon):

```sh
./build.sh --release 0.6-chase4
```

See [Mac build setup](docs/CHASEFLOWER.md#build-on-an-apple-silicon-mac) for the toolchain and Python environment.
A fresh checkout generates fonts, icons, tables and samples into `build/gen/`. GitHub Actions runs the complete
host checks; the release workflow builds firmware from its tagged source revision.

The inherited [CoralFlower browser demo](https://ficusficusov.github.io/CoralFlower/play/) runs upstream firmware;
it does not include all Chase FM-1 changes.

## Credits and licence

CoralFlower is a fork of [ChoralRoot FM-1](https://github.com/Quixotic7/ChoralRootFM1) by Quixotic7, which is itself
a fork of [Felucca](https://github.com/hugelton/Felucca) by Leo Kuroshita (Hügelton Instruments). The FM engine comes
from [Melodee](https://github.com/keremimo/melodee) by Kerem Kilic, and the screens use
[Departure Mono](https://departuremono.com), a font by Helena Zhang. Thank you all.

CoralFlower is free software under the GPL-3.0. `LICENSE` and `LICENSING.md` have the details.

M-VAVE and FM-1 are trademarks of their respective owners. Orchid is a product of Telepathic Instruments. CoralFlower
is not affiliated with M-VAVE, Cuvave or Telepathic Instruments.

## Feedback

Track Chase FM-1 changes in [this repository's issues](https://github.com/chasedeanda/chaseflower-fm1/issues).
Earlier ChaseFlower notes and plans remain in `docs/` as historical records.
