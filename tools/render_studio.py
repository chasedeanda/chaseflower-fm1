#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render paired Studio takes with the real host firmware."""
import array
import csv
import hashlib
import json
import subprocess
import sys
import zipfile
import wave
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "build/studio/listening-pass"
ENGINE = ROOT / "build/host/cf_studio_test"
SAMPLE_RATE = 44100
SEEDS = (107, 218, 497, 618, 719, 844, 1021, 1154, 1549, 1890)


def render(name, seed, regen=0, usb=0, rate=48000, bars=8, wav=True, genre=0):
    prefix = OUTPUT / name
    result = subprocess.run(
        [str(ENGINE), "--render", str(seed), str(int(regen)), str(prefix), str(rate), str(usb), str(bars), str(genre)],
        cwd=ROOT, capture_output=True, text=True,
    )
    prefix.with_suffix(".log").write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError(result.stdout + result.stderr)
    raw = prefix.with_suffix(".raw")
    data = raw.read_bytes()
    raw.unlink()
    samples = array.array("i")
    samples.frombytes(data)
    if sys.byteorder != "little":
        samples.byteswap()
    peak = max(abs(value >> 8) for value in samples)
    assert peak < 32767, f"{name} clipped ({peak})"
    if wav:
        pcm = array.array("h", (value >> 8 for value in samples))
        if sys.byteorder != "little":
            pcm.byteswap()
        with wave.open(str(prefix.with_suffix(".wav")), "wb") as audio:
            audio.setparams((2, 2, SAMPLE_RATE, 0, "NONE", "not compressed"))
            audio.writeframes(pcm.tobytes())
    with prefix.with_suffix(".csv").open(newline="") as trace:
        events = list(csv.DictReader(trace))
    fields = dict(pair.split("=") for pair in next(line for line in result.stdout.splitlines()
                                                   if line.startswith("seed=")).split())
    return events, {**{key: int(value) for key, value in fields.items()}, "peak": peak,
                    "pcm": hashlib.sha256(data).hexdigest()}


def main():
    OUTPUT.mkdir(parents=True, exist_ok=True)
    pairs = []
    original_events = {}
    for i, seed in enumerate(SEEDS, 1):
        original, first = render(f"{i:02}-original", seed)
        original_events[seed] = original
        changed, second = render(f"{i:02}-bass-regen", seed, regen=2)
        assert first["progression"] == second["progression"]
        assert first["clocks"] == second["clocks"]
        assert first["frames"] == second["frames"]
        for part in ("0", "1", "2", "3"):
            before = [event for event in original if event["part"] == part]
            after = [event for event in changed if event["part"] == part]
            assert before and after, f"Seed {seed} missing part {part}"
            assert (before == after) == (part != "1"), f"Seed {seed} unexpected change on part {part}"
        pairs.append({"original": first, "bass_regen": second})
        print(f"Pair {i:02}  seed {seed}  progression {first['progression']}", flush=True)
    for part, label in ((0, "chords"), (2, "layer"), (3, "drums")):
        changed, _ = render(f"03-{label}-regen", SEEDS[2], regen=part + 1)
        for channel in range(4):
            before = [event for event in original_events[SEEDS[2]] if int(event["part"]) == channel]
            after = [event for event in changed if int(event["part"]) == channel]
            assert (before == after) == (part != channel), f"{label} regeneration changed wrong part"
    for rate in (44100, 48000):
        idle = render(f"usb-{rate}-idle", SEEDS[0], rate=rate, bars=4, wav=False)
        active = render(f"usb-{rate}-active", SEEDS[0], usb=1, rate=rate, bars=4, wav=False)
        assert idle == active, f"USB capture altered physical PCM or MIDI events at {rate}"
    _, long_take = render("long-form", SEEDS[2], bars=24)
    _, rolls = render("dream-pop-three-rolls", 100, regen=5, bars=32)
    genres = {}
    for genre, label in enumerate(("dream-pop", "indie", "new-wave", "pop-punk", "lo-fi", "ambient")):
        _, genres[label] = render(label, 100, bars=16 if genre==5 else 8, genre=genre)
    with zipfile.ZipFile(OUTPUT / "genre-demos.zip", "w", compression=zipfile.ZIP_DEFLATED) as pack:
        for label in genres:
            pack.write(OUTPUT / f"{label}.wav", f"{label}.wav")
    (OUTPUT / "listening.json").write_text(json.dumps({"pairs": pairs, "long_take": long_take, "rolls": rolls, "genres": genres}, indent=2) + "\n")
    print("All four part-regeneration paths isolate their own events. USB PCM and MIDI agree at both rates.")


if __name__ == "__main__":
    main()
