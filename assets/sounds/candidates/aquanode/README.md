# Aquanode original banks — audition candidates

These are unchanged original DX7 banks from [Aquanode Soundscapes](https://github.com/aquanodemusic/aquanode-soundscapes), downloaded 2026-11-06 at revision `f0e59287c9991038f688c5e7457db77117638124`.

The creator identifies banks named Aquanode as their own originals in [the Dexed notes](https://github.com/aquanodemusic/aquanode-soundscapes/tree/f0e59287c9991038f688c5e7457db77117638124/dexed). The source repository publishes them under CC0-1.0; its license is preserved as `LICENSE-CC0.txt`. Credit: Aquanode / aquanodemusic. No Infinite Cycles remake banks are included.

Included: Aquanode Presets 1, 2, 4 and Aquanode Aquazone Dexed Presets. Each is a 4,104-byte, 32-slot DX7 bulk dump. All four pass length, header, MIDI data-byte and checksum checks. See `SHA256SUMS` and `source-audit.json` (including original slot names).

Aquanode Presets 3 was inspected but is **not included**: its payload has 19 bytes above 127, despite a passing checksum. It needs separate diagnosis; the original file was not silently changed.

The included files have 122 non-default slots and 80 unique parameter sets, comparing the first 118 voice bytes without the name. They have not been auditioned through ChaseFlower or integrated into its firmware. Names do not establish sound quality or genre.

Use a DX7-compatible software instrument such as [Dexed](https://asb2m10.github.io/dexed/) to explore them on a computer. ChaseFlower's direct import workflow still needs integration and testing; these files are **not firmware installers**. Do not pass a .syx file to the firmware installer.

See `docs/CHASEFLOWER-NEXT-RELEASE.md` for the integration and audition plan. This folder is not read by the firmware build.
