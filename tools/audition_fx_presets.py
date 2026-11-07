#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render every factory FX chain on VA and FM chords, with WAVs and level CSV.

Run tools/test_fx_presets.sh first to build the host renderer. No device access.
"""
import argparse
import csv
from pathlib import Path
import subprocess


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=root / "build/fx-auditions")
    args = parser.parse_args()
    renderer = root / "build/host/cf_fx_presets_test"
    if not renderer.exists():
        parser.error("build the renderer with sh tools/test_fx_presets.sh first")
    output = args.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)
    result = subprocess.check_output([str(renderer), "--count"], text=True)
    count = int(next(s.split(",")[1] for s in result.splitlines() if s.startswith("FX_COUNT,")))
    rows = []
    for fm in (0, 1):
        for index in range(count):
            wav = output / f"{index + 1:02d}-{'fm' if fm else 'va'}.wav"
            result = subprocess.check_output(
                [str(renderer), "--audition", str(index), str(fm), str(wav)], text=True
            )
            line = next(s for s in result.splitlines() if s.startswith("FX_LEVEL,"))
            rows.append(next(csv.reader([line]))[1:])
    with (output / "levels.csv").open("w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["number", "sound", "preset", "peak_dbfs", "rms_dbfs", "clipped_samples"])
        writer.writerows(rows)
    for sound in ("VA POLY KEYS", "FM PAD"):
        group = [r for r in rows if r[1] == sound]
        rms = [float(r[4]) for r in group]
        print(f"{sound}: RMS {min(rms):.2f}..{max(rms):.2f} dBFS; "
              f"{sum(int(r[5]) for r in group)} clipped samples")
    print(f"PASS: {len(rows)} factory/voice auditions; WAVs and levels.csv in {output}")


if __name__ == "__main__":
    main()
