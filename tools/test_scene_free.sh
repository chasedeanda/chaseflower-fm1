#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/scene-free-tests
for scenes in 0 1; do
    cc -std=gnu11 -O1 -w -DCF_SCENES="$scenes" -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
        -o "build/host/cf_scene_free_$scenes" tests/cf_scene_free_test.c -lm
    "build/host/cf_scene_free_$scenes"
    for rate in 44100 48000; do
        "build/host/cf_scene_free_$scenes" --audio "$rate" "build/scene-free-tests/$rate-$scenes.raw" \
            > "build/scene-free-tests/$rate-$scenes.log"
        tail -1 "build/scene-free-tests/$rate-$scenes.log"
    done
done
for rate in 44100 48000; do
    cmp "build/scene-free-tests/$rate-0.raw" "build/scene-free-tests/$rate-1.raw"
done
echo 'PASS: scene-free/legacy profiles preserve identical physical audio, six USB channels and MIDI at both rates.'
