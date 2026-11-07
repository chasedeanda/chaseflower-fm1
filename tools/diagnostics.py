#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Read-only development metrics and before/after comparisons. No note-on, reset, flash or install commands.

  python3 tools/diagnostics.py capture before.json --label six-note-pad --seconds 30 --revision COMMIT
  python3 tools/diagnostics.py compare before.json after.json

Reset the session on the device before playing the repeatable workload. JSON records exact integer fields;
the display abbreviates large values. Emulator timing/memory are never treated as device measurements.
"""
import argparse
import datetime
import json
from pathlib import Path
import sys
import time

FIELDS = """uptime_ms session_ms budget_us audio_q8 avg_work_us window_peak_us peak_work_us peak_wall_us
peak_pressure_us peak_lag_us late stale_est shed steals kills voices_chord voices_bass voices_layer voices_drums
voices_tail ui_hz ui_gap_ms core1_state core1_stack_bytes ram_bytes ram_capacity pool_bytes pool_capacity
xip_bytes xip_capacity ram_code_bytes usb_state usb_rate usb_underruns usb_overruns usb_missed rs_peak_us
ring_frames ring_capacity hardware chord_engine bass_engine layer_engine chord_row bass_row layer_row
fx_mask bpm beats halves profile_enabled tick_avg_us part0_avg_us part1_avg_us fx_avg_us master_avg_us wait_avg_us
tick_peak_us part0_peak_us part1_peak_us fx_peak_us master_peak_us wait_peak_us""".split()


def decode(reply):
    if len(reply) < 2 or reply[0] != 1 or reply[1] < len(FIELDS) or len(reply) != 2+reply[1]*5:
        raise ValueError("Unsupported diagnostic reply; install a diagnostics build supporting command 80")
    if any(not 0 <= x < 128 for x in reply):
        raise ValueError("Invalid MIDI data")
    values = []
    for off in range(2, len(reply), 5):
        if reply[off+4] > 15:
            raise ValueError("Invalid 32-bit diagnostic value")
        values.append(sum(reply[off+k] << (7*k) for k in range(5)))
    return dict(zip(FIELDS, values))


def capture(args):
    import fm1_install as fi
    up = fi.Updater(fi.MidiBackend(), args.port)
    try:
        bl, info = fi.open_backup(up)
    except fi.InstallError as e:
        raise ValueError(str(e)) from e
    records = []
    try:
        start = time.monotonic()
        while True:
            records.append(decode(bl.request(80, [], 1.5)))
            if time.monotonic()-start >= args.seconds:
                break
            time.sleep(min(1.0, max(0, args.seconds-(time.monotonic()-start))))
    except fi.InstallError as e:
        raise ValueError(str(e)) from e
    finally:
        bl.link.close()
    report = dict(format="chaseflower-performance", version=1,
                  captured_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  firmware=info[0], revision=args.revision, workload=args.label, samples=records)
    args.output.write_text(json.dumps(report, indent=2)+"\n")
    print(f"Saved {len(records)} samples to {args.output}")


def load(path):
    r = json.loads(path.read_text())
    if r.get("format") != "chaseflower-performance" or r.get("version") != 1 or not r.get("samples"):
        raise ValueError(f"{path}: not a supported performance report")
    for s in r["samples"]:
        if any(k not in s or not isinstance(s[k], int) or not 0 <= s[k] <= 0xFFFFFFFF for k in FIELDS):
            raise ValueError(f"{path}: incomplete or invalid sample")
    return r


def comparison(before, after):
    warnings = []
    if before.get("workload") != after.get("workload"):
        warnings.append("Workload labels differ; use the same playing script before drawing conclusions.")
    for name, r in (("before", before), ("after", after)):
        if any(b["session_ms"] < a["session_ms"] or b["uptime_ms"] < a["uptime_ms"]
               for a,b in zip(r["samples"],r["samples"][1:])):
            raise ValueError(f"{name}: session reset or restart during capture; split the run")
        if any(s["hardware"] != 1 for s in r["samples"]):
            raise ValueError(f"{name}: emulator data is not a device performance measurement")
    # Context checks are visible: never quietly compare different sounds/FX/USB settings.
    context = ("chord_engine", "bass_engine", "layer_engine", "chord_row", "bass_row", "layer_row",
               "fx_mask", "usb_rate", "usb_state", "bpm", "beats", "budget_us")
    if any(before["samples"][-1][k] != after["samples"][-1][k] for k in context):
        warnings.append("Final patch/FX/USB/tempo context differs; inspect the raw reports.")
    lengths=[r["samples"][-1]["session_ms"] for r in (before,after)]
    if abs(lengths[0]-lengths[1]) > max(500,max(lengths)*0.02):
        warnings.append("Session lengths differ. Counter totals need matching run lengths; peaks depend on exposure time.")
    keys = ("budget_us", "peak_work_us", "peak_pressure_us", "peak_lag_us", "shed", "late", "stale_est", "usb_underruns",
            "usb_overruns", "usb_missed", "ram_bytes", "pool_bytes", "xip_bytes")
    rows = []
    for k in keys:
        a, b = [max(s[k] for s in r["samples"]) for r in (before, after)]
        rows.append(dict(metric=k, before=a, after=b, delta=b-a))
    return warnings, rows


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="command", required=True)
    c = sub.add_parser("capture"); c.add_argument("output", type=Path)
    c.add_argument("--label", required=True); c.add_argument("--revision", default="unspecified")
    c.add_argument("--seconds", type=float, default=30); c.add_argument("--port")
    c = sub.add_parser("compare"); c.add_argument("before", type=Path); c.add_argument("after", type=Path)
    args = p.parse_args(argv)
    try:
        if args.command == "capture":
            if not 0 <= args.seconds <= 3600: p.error("--seconds must be between 0 and 3600")
            capture(args)
        else:
            a, b = load(args.before), load(args.after)
            warnings, rows = comparison(a, b)
            print(f"Before: {a.get('firmware')} / {a.get('revision')}")
            print(f"After:  {b.get('firmware')} / {b.get('revision')}")
            for warning in warnings: print("NOTE: "+warning)
            print(f"{'Metric':22} {'Before':>10} {'After':>10} {'Delta':>10}")
            for r in rows: print(f"{r['metric']:22} {r['before']:10} {r['after']:10} {r['delta']:+10}")
    except (ValueError, OSError, RuntimeError) as e:
        print(str(e), file=sys.stderr); return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
