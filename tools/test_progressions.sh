#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/progression-tests
cc -std=c99 -O2 -Wall -Wextra -Werror -Ifirmware/src -o build/host/cf_progression_test \
    tests/cf_progression_test.c firmware/src/cf_progression.c firmware/src/cf_harmony.c \
    firmware/src/cf_chord.c firmware/src/cf_theory.c
build/host/cf_progression_test
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
    -o build/host/cf_suggestion_test tests/cf_suggestion_test.c -lm
build/host/cf_suggestion_test
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
    -o build/host/cf_led_guide_test tests/cf_led_guide_test.c -lm
build/host/cf_led_guide_test
for rate in 44100 48000; do
    for browse in 0 1; do
        build/host/cf_suggestion_test --audio "$browse" "$rate" "build/progression-tests/$rate-$browse.raw" \
            > "build/progression-tests/$rate-$browse.log"
        tail -1 "build/progression-tests/$rate-$browse.log"
    done
    cmp "build/progression-tests/$rate-0.raw" "build/progression-tests/$rate-1.raw"
done
echo 'PASS: complete catalog, automatic setup, HOME controls, live highlighting and identical audio with/without drawing at both rates.'
