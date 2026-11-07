#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/lab-tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src -o build/host/cf_lab_test tests/cf_lab_test.c firmware/src/cf_lab.c firmware/src/cf_progression.c firmware/src/cf_chord.c firmware/src/cf_harmony.c firmware/src/cf_theory.c
build/host/cf_lab_test
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_lab_ui_test tests/cf_lab_ui_test.c -lm
for rate in 44100 48000; do
    build/host/cf_lab_ui_test "$rate"
    for browse in 0 1; do
        build/host/cf_lab_ui_test "$rate" "$browse" "build/lab-tests/$rate-$browse.raw"
    done
    cmp "build/lab-tests/$rate-0.raw" "build/lab-tests/$rate-1.raw"
done
echo "PASS: identical audio while browsing all Lab tabs at both USB capture rates."
