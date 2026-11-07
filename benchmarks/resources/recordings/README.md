# Three rotating recordings: device measurements

Final compiled firmware: e60c4eb5345622e9c8833b2025a0869347b1d78a.
Default CF_SCENES=0 with progression playback retained. The comparison is the
previous scene-free build at 1da70f0, recorded in scene-profiles/scene-free.
Unrelated rendering/USB/synth/FX settings remain enabled. The compiler flags,
startup and linker identity, dependency hashes, ELF hash and complete sized
symbol list are captured in the accompanying files. Later documentation/CI-only
work does not change the compiled firmware inputs.

- Ordinary RAM: 97,048 -> 79,320 bytes, **17,728 reclaimed**; headroom 18,984.
- POOL: 308,980 -> 315,404; headroom 28,660. Added 3,840-byte foreground I/O
  staging and a 2,584-byte temporary cancellation guard; no edit/redo history.
- XIP: 419,652 -> 421,624 (+1,972); RAM code: 3,016 -> 3,052 (+36).
- NOINIT unchanged at 216 bytes.

The 20,336-byte live eight-pattern store becomes 7,656 bytes. The 5,152-byte
recorder undo object is absent. Ordering, tempo/meter, policy and backup manifest
fields account for the difference between gross store/history savings and the
net ordinary-RAM result. Legacy eight-pattern archives stay in flash, streamed
through bounded staging instead of loaded into ordinary RAM. CPU/input-latency,
polyphony and physical audio improvements are not inferred from static sizes.

ELF, package and full build log remain in ignored build/resource-audit/recordings.
See docs/RECORDINGS.md and docs/PATTERN-MEMORY.md for behavior and migration.
