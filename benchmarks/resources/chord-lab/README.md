# Chord Lab linked resources

Paired real JieLi builds of unchanged main `abd37afab562555d4e8a58a66d18b488cce594f9`
and that base plus the Chord Lab implementation. `baseline/report.json` records the
clean base; `report.json` records the source hashes, local changes, build options and
explicit comparison. These are linked allocations, not runtime timing measurements.

| Region | Main bytes | Chord Lab bytes | Change | Remaining |
| --- | ---: | ---: | ---: | ---: |
| Ordinary RAM | 83,064 | 86,232 | +3,168 | 12,072 |
| POOL | 315,404 | 315,404 | 0 | 28,660 |
| XIP | 489,360 | 500,336 | +10,976 | 73,104 |
| RAM code | 3,092 | 3,092 | 0 | 21,484 |
| NOINIT | 216 | 216 | 0 | 15,448 |

Both use the same compiler, default app flags, size pass, generated fonts, sdk and
source-base revision. Audited app ELF SHA256:
`68d616958fecbb9f912d30dd6e120778f929c1661d3cc4cf480edfcd95678963`.
Baseline ELF SHA256:
`51b84cc4180a5032b8dafa989f0f3d1bf0f2131431c3fbd447938a85f0e25e2e`.

Reproduce in an isolated implementation checkout with Pillow 11.1.0, fonttools
4.66.1 and the JieLi Linux toolchain (Docker on this Mac):

```sh
JIELI_TOOLCHAIN=/path/to/jieli/toolchain /path/to/python tools/audit_resources.py \
  --build --output build/resource-audit/chord-lab
python3 tools/audit_resources.py --inputs build/resource-audit/chord-lab \
  --compare benchmarks/resources/chord-lab/baseline/report.json \
  --output build/resource-audit/chord-lab
```

The build, loader/package checks and ARM syntax passed. Host snapshots prove that
an unchanged Lab view performs no new composition/hash/LCD transfer. Playback audio
is identical while browsing/rendering versus no browsing, at both USB capture rates.
Physical CPU/input/display timings, audio tails and USB stress on the FM-1 remain
pending. No runtime speed or polyphony improvement is inferred from these figures.
