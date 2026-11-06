# FM-1 update plan

## Goals

1. Replace the flower and coral visuals with a bonfire and a swimming bass (fish).
2. Plan recording improvements after defining the desired recording workflow.

## Starting point

- Assumed base: CoralFlower for the M-VAVE FM-1; confirm the installed version before implementation.
- This workspace currently has no firmware source code.
- Upstream source: https://github.com/FicusFicusov/CoralFlower
- Upstream documents a flower in the Top key layout and coral in the Side layout, with growth driven by played chords.
- Custom visuals are planned as firmware rendering changes; a user-uploadable image feature has not been established.

## Update 1: custom visuals

Requested subjects: a bonfire and a swimming bass.

Proposed mapping, pending preference:

- Flower / Top layout → Bonfire.
- Coral / Side layout → Swimming bass.

Animation ideas to review:

- Bonfire: flickering flames above logs, with occasional rising embers. Played notes could briefly increase flame height or add sparks.
- Bass: a recognizable fish silhouette with a moving tail and fins, gentle swimming motion, and optional bubbles. Played notes could trigger a tail flick or a short burst of movement.
- Decide whether both scenes move quietly at idle or animate only while playing.

Implementation sequence:

1. Obtain the intended firmware source and establish a working baseline build.
2. Inspect the renderer, display constraints, animation timing, and existing musical inputs to the visuals.
3. Preview both designs at the actual screen resolution.
4. Implement the scenes and their responses to music.
5. Verify layout switching, readable musical information, smooth animation, and uninterrupted audio in the emulator.
6. Build a firmware package for hardware verification.

## Update 2: recording

Scope is not yet defined. First determine whether the goal is recording notes into patterns, looping/overdubbing performances, capturing audio, or exporting recordings to a computer.

Then define the desired start/stop controls, count-in, recording length, playback behavior, and save/export workflow. Check the existing firmware and hardware capabilities before committing to an implementation.

## Next decisions

- Confirm the firmware/version being customized.
- Review the proposed visual mapping and animation behavior.
- Describe the desired recording change.
