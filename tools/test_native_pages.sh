#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/native-pages
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_native_pages_test tests/cf_native_pages_test.c -lm
build/host/cf_native_pages_test --shots build/native-pages
for rate in 44100 48000; do
    for motion in 0 1; do
        build/host/cf_native_pages_test --audio "$rate" "build/native-pages/$rate-$motion.raw" "$motion" > "build/native-pages/$rate-$motion.log"
        tail -1 "build/native-pages/$rate-$motion.log"
    done
    cmp "build/native-pages/$rate-0.raw" "build/native-pages/$rate-1.raw"
done
echo 'PASS: Motion On/Off preserve identical physical audio, six USB channels and MIDI at both rates.'
