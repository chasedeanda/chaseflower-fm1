#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/engine
python3 tools/gen_engine_scene.py --check
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests tests/cf_engine_test.c -o build/host/cf_engine_test -lm
build/host/cf_engine_test build/engine/baseline-64.csv build/engine/performance-64.csv build/engine/midi-baseline-64.csv build/engine/midi-performance-64.csv
for rate in 44100 48000; do
    for capture in 0 1; do
        build/host/cf_engine_test --usb "$capture" "$rate" "build/engine/usb-$rate-$capture.raw" > "build/engine/usb-$rate-$capture.log"
        tail -2 "build/engine/usb-$rate-$capture.log"
    done
    cmp "build/engine/usb-$rate-0.raw" "build/engine/usb-$rate-1.raw"
done
echo 'PASS: four-cycle Engine physical output identical with USB capture active/idle at both rates.'
