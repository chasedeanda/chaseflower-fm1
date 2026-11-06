# ChaseFlower — first boot candidate

Personal firmware for Chase's M-VAVE FM-1, based on CoralFlower commit
`272cad9e85a444d879d72598807b7ca01a098d6d`.

This first candidate changes the animated startup title to **ChaseFlower** and the USB manufacturer/product names to
**ChaseFlower / ChaseFlower FM-1**. The reserved audio input name is **ChaseFlower In**. USB audio remains disabled
until the next milestone: this build should appear as MIDI and a console, just as the upstream firmware does.

The INFO version stays `CoralFlower 0.6-chase1` because the existing browser installer and backup tools identify the
firmware family by that prefix. The startup subtitle shows the version without that family prefix. The package identity
stays `FM-1_930`. Source filenames, copyright notices, boot stub, loader, protected flash layout, saved-data formats,
sounds, effects, sequencer and rhythm patterns retain their upstream behavior.

## Build on an Apple Silicon Mac

Docker Desktop must be running. The existing build system executes the JieLi compiler in Linux x86-64 containers;
the compiler is not a native macOS executable.

```sh
python3 -m venv ../work/venv
../work/venv/bin/pip install Pillow==11.1.0 fonttools==4.66.1
sh tools/get_toolchain.sh ../work/jieli
PYTHON="$PWD/../work/venv/bin/python" \
JIELI_TOOLCHAIN="$PWD/../work/jieli/toolchain" \
./build.sh --release 0.6-chase1
```

The build writes `build/release-0.6-chase1/coralflower-0.6-chase1.fwsc`, its application image, checksums and licenses.
The delivered copy is named `chaseflower-0.6-chase1.fwsc`; renaming the package file does not change its identity or
contents. The existing manual GitHub Actions release workflow is another build route, on a repository containing
these changes. A manual workflow run produces an artifact without publishing a release.

The first-boot package and its checksum are also checked into `releases/0.6-chase1/`. Generated build directories,
the downloaded compiler and local Python environment stay outside source control. The personal repository is
<https://github.com/chasedeanda/chaseflower-fm1>; the original CoralFlower repository is retained as Git's `upstream`
remote. GitHub Pages publication is manual in this fork; ordinary source pushes run checks without publishing a site.

See [the verification report](CHASEFLOWER-VERIFICATION.md) for measured build sizes and test results.

## Install manually

1. Save a backup of your current sounds and settings. Keep the installer's downloaded backup somewhere safe.
2. Open <https://ficusficusov.github.io/CoralFlower/> in Chrome or Edge on the Mac.
3. Connect the FM-1 directly with a data-capable USB cable and provide reliable power.
4. Choose the local `.fwsc` file in the installer's **Choose a .fwsc file** control. Confirm that the chosen filename
   is `chaseflower-0.6-chase1.fwsc` before starting. Installing the site's default release would install upstream instead.
5. Use the installer's update flow, keep the cable connected and page open, and wait for the restart.
6. Confirm the **ChaseFlower** startup title, then the normal HOME screen. Play several notes and chords, test bass,
   effects and the sequencer, and confirm existing sounds/settings survived. Check **ChaseFlower FM-1** in MIDI Studio.

This candidate has no USB audio device yet, so GarageBand digital audio and Aggregate Device testing belong to the
USB-audio milestone. Physical boot, MIDI enumeration and audio quality must be checked on the FM-1; a successful
build or emulator run cannot establish them.

## Recovery

- If the instrument still runs, use the same installer and a known-good CoralFlower `.fwsc` to return to upstream.
- To enter update mode from the running instrument, hold **OCT- + OCT+ + HOME** for five seconds. Releasing during
  the countdown cancels it.
- The unchanged version-3 boot stub supports rescue by holding **OCT- alone at power-on**. With a staged update
  loader, this enters update mode; without one it falls back to the chip's ROM USB boot mode. Reconnect to the installer
  if it recognizes the update loader. ROM USB boot mode requires the appropriate recovery procedure, not ordinary
  MIDI enumeration. The upstream installer links the [FM-1 recovery tool](https://github.com/Quixotic7/MvaveFM1Unbricker).
- The upstream README also describes returning to official M-VAVE V15 through the installer. Keep the backup before
  changing firmware families; do not assume every firmware uses compatible saved data.

## Next milestones

1. Confirm this candidate boots and plays on Chase's FM-1.
2. Connect the inherited UAC1 capture to CoralFlower's actual final mix; preserve MIDI and six channels where practical.
   Start with 44.1 kHz PCM16, then assess 48 kHz, endpoint scheduling and CPU/memory headroom. Test the Spark 2 and
   GarageBand setup on the Mac after USB capture works.
3. Discuss personal instrument changes, beginning with a 6/8 rhythm. The current source has 6/8-related timing/UI
   handling, but comments say 6/8 presets were removed; inspect the desired pattern and compound-meter accents before
   implementing it. Do not substitute six quarter-note beats for two groups of three eighth notes.

No firmware is flashed automatically.
