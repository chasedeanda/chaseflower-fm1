#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
    -o build/host/cf_accomp_test tests/cf_accomp_test.c -lm
build/host/cf_accomp_test
mkdir -p build/accomp-tests
for rate in 44100 48000; do
    for capture in 0 1; do
        build/host/cf_accomp_test --audio "$capture" "$rate" "build/accomp-tests/$rate-$capture.raw" \
            > "build/accomp-tests/$rate-$capture.log"
        tail -1 "build/accomp-tests/$rate-$capture.log"
    done
    cmp "build/accomp-tests/$rate-0.raw" "build/accomp-tests/$rate-1.raw"
done
echo 'PASS: accompaniment transport and identical physical output with USB capture/drawing at both rates.'
