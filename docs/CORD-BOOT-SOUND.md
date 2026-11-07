# CORD boot sound

The boot signature is the selected first-pass warm Dm7 chime: D3, F3, A3 and C4
enter at 0, 65, 130 and 195 ms, then settle to silence by 1.1 seconds. Soft attacks,
brief second-harmonic brightness and a narrow stereo spread give the four notes a
soft electric-key character. The fixed sound matches the illustrative Dm7 in the
CORD piano-key logo; it does not follow or alter the user's current key or sound.

[Listen to the actual firmware render](audio/cord-boot-chime.wav).

Use **GLO → Display → K3 BOOT SND**. On previews it. Off cancels a pending start or
fades the active sound. The existing saved preference is retained, independently
of Motion; SAFE mode stays quiet. MASTER controls physical output through the
existing audio path. The sound is mixed after FX and before MASTER, so changing
the synth sound or effects does not recolor it. USB's complete stereo mix carries
the chime; chord/bass stems and recorded/MIDI note events remain independent.

The audio owner handles a bounded start/stop mailbox. There are four fixed
oscillators and no allocation, flash access or new scheduling clock in the mixer.
Each sample advances its envelopes and onset time, independent of block length.
The complete sound has a finite cutoff and smooth final fade. A new instrument
note cancels a start that has not been mixed yet; an active sound or overload
request fades it within 256 samples (5.8 ms at 44.1 kHz). Inactive rendering adds
nothing to an existing mix.

The preview is a full-MASTER physical-output render at 44.1 kHz with 0.2 seconds
of silence after the signature. It peaks at -17.86 dBFS (no clipped samples), with
RMS -32.64 dBFS across the complete file. SHA256:
`9ffb7cd9848f8696f5247381dbf4c909af6b87622c936a6c9fc5494796dbe722`.
The regression keeps that selected render byte-identical.

Tests cover duration and silent tail, headroom, exact output across different
block sizes, inactive mix preservation, both priority fade sources, queued-start
cancellation, UI audition/Off, saved On/Off, Motion independence, SAFE, MASTER
mute, no MIDI/flash/recording mutation and clean USB stems. With a first key queued
before the first audio block, physical output, six USB channels and MIDI are
byte-identical with or without a pending chime at both 44.1/48 kHz capture rates.
FX, USB, native-page and scene-profile regressions also pass.

[Paired JieLi link audit](../benchmarks/resources/cord-boot-chime/report.md) records
the exact committed source, flags, inputs and ELF. Compared with 0.6-chase9's
same-toolchain runtime, ordinary RAM falls by 416 bytes and XIP by 1,792 bytes;
POOL and RAM code stay unchanged. These static measurements do not establish
hardware CPU or deadline margins. Listening on the physical speaker/headphones
and startup/input timing remain device checks.

```sh
sh tools/test_boot_sound.sh
sh tools/test_cord_boot.sh
sh tools/test_fx_presets.sh
sh tools/test_usb.sh
```

The test writes `build/boot-sound/cord-chime.wav` from the actual firmware audio
path. It does not flash a device.
