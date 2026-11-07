# Paired scene profiles

Captured from revision `1da70f0cbc97c20d473a10bf18d12c790ba30d21`, which integrates
progression playback/browser main `29c3ba6` with scene removal. Only CF_SCENES
changes between the two device compilations: 1 for exploration, 0 for scene-free.
Shared compiler dependencies, build recipe, toolchain, Python packages, linker
and startup hashes match. Source-status differences are later documentation and
host-test edits; compiled firmware and build inputs match.

Both folders contain the full compiler dependency/macro captures, ELF identity,
section sizes and sized-symbol inventory. The scene-free report compares against
the matching exploration report. The exploration report also records its delta
from the historical 5bfd500 baseline; that delta includes new playback features
and is not the cost of scenes.

Scene removal frees 464 bytes of ordinary RAM, 24,872 POOL bytes and 18,240 XIP
bytes. `sq_st` (20,336) and `sqh` (5,152) are unchanged in both profiles. No runtime
speed or latency gain is inferred. See [the explanation](../../../docs/SCENE-FREE.md).

ELFs, build logs and firmware packages remain in the ignored local build folders.
