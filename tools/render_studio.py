#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render paired Studio takes with the real host firmware."""
import argparse
import array
import csv
import hashlib
import json
import math
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


def render(name, seed, regen=0, usb=0, rate=48000, bars=8, wav=True, genre=0, output=OUTPUT):
    prefix = output / name
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


def dbfs(power):
    return 10 * math.log10(max(power / 32768**2, 1e-12))


def pcm32_to_wav(path):
    samples = array.array("i")
    samples.frombytes(path.read_bytes())
    if sys.byteorder != "little":
        samples.byteswap()
    peak = max(abs(sample >> 8) for sample in samples)
    assert 100 < peak < 32767, f"{path.name} peak {peak}"
    pcm = array.array("h", (sample >> 8 for sample in samples))
    if sys.byteorder != "little":
        pcm.byteswap()
    with wave.open(str(path.with_suffix(".wav")), "wb") as audio:
        audio.setparams((2, 2, SAMPLE_RATE, 0, "NONE", "not compressed"))
        audio.writeframes(pcm.tobytes())
    path.unlink()
    return pcm, peak



def bass_harmonics_db(samples):
    # The bass fixture holds MIDI 36. Sum bins within 4% of each harmonic
    # so detuning and the pitch modulation sidebands stay in the comparison.
    start, size = int(1.6 * SAMPLE_RATE), 32768
    window = [(samples[(start + i) * 2] + samples[(start + i) * 2 + 1]) * (1 - math.cos(math.tau * i / size))
              for i in range(size)]
    fundamental_bin = 440 * 2**((36 - 69) / 12) * size / SAMPLE_RATE
    power = []
    for harmonic in range(1, 9):
        center = fundamental_bin * harmonic
        harmonic_power = 0.0
        for bin_index in range(math.ceil(.96 * center), math.floor(1.04 * center) + 1):
            coefficient = 2 * math.cos(math.tau * bin_index / size)
            previous = older = 0.0
            for sample in window:
                older, previous = previous, sample + coefficient * previous - older
            harmonic_power += previous**2 + older**2 - coefficient * previous * older
        power.append(harmonic_power)
    return 10 * math.log10(sum(power[1:]) / power[0])


def solo(prefix, genre, part, original):
    raw = prefix.with_suffix(".raw")
    result = subprocess.run(
        [str(ENGINE), "--render-voice", str(genre), str(part), str(int(original)), str(raw)],
        cwd=ROOT, capture_output=True, text=True,
    )
    if result.returncode:
        raise RuntimeError(result.stdout + result.stderr)
    voice, name = next(line for line in result.stdout.splitlines() if line.startswith("voice=")).split(" name=")
    samples, peak = pcm32_to_wav(raw)
    if sys.byteorder != "little":
        samples.byteswap()

    def mean_square(start, end):
        section = samples[int(start * SAMPLE_RATE) * 2:int(end * SAMPLE_RATE) * 2]
        return sum(sample * sample for sample in section) / len(section)

    attack = dbfs(mean_square(.025, .125))
    hold = dbfs(mean_square(1.6, 2.4))
    release = dbfs(mean_square(2.6, 3.0))
    # First-order high-pass energy relative to the same mono window.
    # Use a lower cutoff to resolve changes to bass harmonics.
    cutoff = 200 if part == 1 else 1000
    alpha = 1 - math.exp(-2 * math.pi * cutoff / SAMPLE_RATE)
    low = high_power = full_power = 0
    for i in range(int(.025 * SAMPLE_RATE) * 2, int(.6 * SAMPLE_RATE) * 2, 2):
        mono = (samples[i] + samples[i + 1]) * .5
        low += alpha * (mono - low)
        high_power += (mono - low)**2
        full_power += mono**2
    sustain_windows = [dbfs(mean_square(i / 20, (i + 1) / 20)) for i in range(16, 46)]
    result = {"factory_id": int(voice.removeprefix("voice=")), "name": name,
            "peak_dbfs": dbfs(peak**2), "held_dbfs": hold,
            "attack_vs_hold_db": attack - hold, "release_vs_hold_db": release - hold,
            "upper_energy_db": 10 * math.log10(high_power / full_power),
            "sustain_range_db": max(sustain_windows) - min(sustain_windows),
            "wav_sha256": hashlib.sha256(prefix.with_suffix(".wav").read_bytes()).hexdigest()}
    if part == 1:
        result["bass_harmonics_db"] = bass_harmonics_db(samples)
    return result


def render_originals():
    output = ROOT / "build/studio/originals"
    output.mkdir(parents=True, exist_ok=True)
    genres = ("dream-pop", "indie", "new-wave", "pop-punk", "lo-fi", "ambient")
    evidence = {"solo": [], "band": []}
    files = []
    for genre, label in enumerate(genres):
        before, old = render(f"{label}-band-featured", 102, regen=8, bars=4, genre=genre, output=output)
        after, new = render(f"{label}-band-originals", 102, regen=9, bars=4, genre=genre, output=output)
        assert before == after and before, f"{label} changed the MIDI note schedule"
        assert {row["part"] for row in before} == {"0", "1", "2", "3"}, f"{label} missing a band part"
        for key in ("progression", "bpm", "clocks", "frames"):
            assert old[key] == new[key], f"{label} changed {key}"
        assert new["pcm"] != old["pcm"], f"{label} new band sounds match the old PCM"
        assert not (new["sheds"] or new["voice_kills"]), f"{label} band overloaded or ran out of voices"
        evidence["band"].append({"genre": label, "identical_note_events": len(before),
                                 "featured": old, "originals": new})
        for version in ("featured", "originals"):
            files.extend((f"{label}-band-{version}.wav", f"{label}-band-{version}.csv"))
        for part, role in enumerate(("chords", "bass", "layer")):
            stem = f"{label}-{role}"
            featured = solo(output / f"{stem}-featured", genre, part, False)
            original = solo(output / f"{stem}-original", genre, part, True)
            assert original["wav_sha256"] != featured["wav_sha256"], stem
            assert abs(original["peak_dbfs"] - featured["peak_dbfs"]) <= 7, f"{stem} has a level jump"
            features = ("attack_vs_hold_db", "release_vs_hold_db", "upper_energy_db", "sustain_range_db")
            if part == 1:
                features += ("bass_harmonics_db",)
            contrast = max(abs(original[key] - featured[key]) for key in features)
            assert contrast >= 1.5, f"{stem} has little measured envelope or spectral contrast"
            evidence["solo"].append({"genre": label, "role": role, "featured": featured, "original": original})
            files.extend((f"{stem}-featured.wav", f"{stem}-original.wav"))
        print(f"{label}  {len(before)} identical MIDI note events  {new['sheds']} host sheds", flush=True)
    (output / "evidence.json").write_text(json.dumps(evidence, indent=2) + "\n")
    files.append("evidence.json")
    with zipfile.ZipFile(output / "cord-studio-originals.zip", "w", compression=zipfile.ZIP_DEFLATED) as pack:
        for name in files:
            pack.write(output / name, name)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--originals", action="store_true", help="compare the original genre trios with the featured sounds")
    args = parser.parse_args()
    if args.originals:
        render_originals()
    else:
        main()
