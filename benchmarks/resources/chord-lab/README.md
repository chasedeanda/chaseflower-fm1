# Chord Lab linked resources

Paired real JieLi builds of main `09403c9aecb96afed05a63f6938e17c226dd6ee7` and the
integrated Chord Lab implementation. The checked native-pages baseline was verified
against every current-main dependency hash before reuse. `report.json` records the
compiled merge of Chord Lab parent `94bcf51` with main, source hashes, conflict resolution
and explicit byte deltas. These are linked allocations, not runtime timing measurements.

| Region | Main bytes | Chord Lab bytes | Change | Remaining |
| --- | ---: | ---: | ---: | ---: |
| Ordinary RAM | 83,880 | 87,064 | +3,184 | 11,240 |
| POOL | 315,404 | 315,404 | +0 | 28,660 |
| XIP | 442,320 | 452,640 | +10,320 | 120,800 |
| RAM code | 3,092 | 3,092 | +0 | 21,484 |
| NOINIT | 216 | 216 | +0 | 15,448 |

Audited app ELF SHA256:
`664e98bfaab90d60d3fa25f85a56ec783e93035ca23b9c33ae4d645b679dcacb`.
Baseline ELF SHA256:
`a9cd32c51530c4ff850300d739b632dca7ea36127a819304776ec17ba04b413f`.

Both use the same compiler, default app flags, size pass, generated fonts and SDK.
Reproduce with Pillow 11.1.0, fonttools 4.66.1 and the JieLi Linux toolchain:

```sh
JIELI_TOOLCHAIN=/path/to/jieli/toolchain /path/to/python tools/audit_resources.py \
  --build --compare benchmarks/resources/chord-lab/baseline/report.json \
  --output build/resource-audit/chord-lab
```

The device build, package checks, native-screen/motion, Lab, editor and accompaniment
checks pass. Browsing/rendering versus no browsing preserves identical playback audio
at both USB capture rates. Physical CPU/input/display timing, tails and USB stress
remain pending. No hardware speed or polyphony improvement is inferred.
