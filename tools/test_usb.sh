#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Host tests only: real firmware DSP/USB code, mocked USB register access. No device is opened or flashed.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/usb-tests
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src \
   -o build/host/cf_usb_test tests/cf_usb_test.c -lm
CF_USB_DIAG=build/usb-tests/diagnostics.syx build/host/cf_usb_test > build/usb-tests/protocol.log
tail -7 build/usb-tests/protocol.log
# Each process starts a fresh, deterministic instrument. Capture must not change a single physical output word.
for rate in 44100 48000; do
for part in 0 1 2 3 4 5 6; do
    for clean in 0 1; do
        for fx in 0 1; do
            name="$rate-$part-$clean-$fx"
            build/host/cf_usb_test --render "$part" "$clean" "$fx" 0 "build/usb-tests/$name-off.raw" "$rate" > "build/usb-tests/$name-off.log"
            build/host/cf_usb_test --render "$part" "$clean" "$fx" 1 "build/usb-tests/$name-on.raw" "$rate" > "build/usb-tests/$name-on.log"
            cmp "build/usb-tests/$name-off.raw" "build/usb-tests/$name-on.raw"
            tail -2 "build/usb-tests/$name-on.log"
        done
    done
done
done
python3 tests/test_usb_tools.py
echo 'PASS: protocol/packet/resampler/drift checks and 56 render comparisons (USB active vs idle, both rates).'
