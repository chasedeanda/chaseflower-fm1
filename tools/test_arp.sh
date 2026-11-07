#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/arp-tests
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_arp_test tests/cf_arp_test.c -lm
build/host/cf_arp_test --captures build/arp-tests
for rate in 44100 48000; do
    for drawing in 0 1; do
        build/host/cf_arp_test --audio "$rate" "build/arp-tests/$rate-$drawing.raw" "$drawing" > "build/arp-tests/$rate-$drawing.log"
        tail -1 "build/arp-tests/$rate-$drawing.log"
    done
    cmp "build/arp-tests/$rate-0.raw" "build/arp-tests/$rate-1.raw"
done
echo 'PASS: all 13 modes, native ARP roll, latch/release, progression takeover and identical physical/USB/MIDI streams while drawing at both capture rates.'
