#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/variation-tests
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
    -o build/host/cf_variation_test tests/cf_variation_test.c -lm
build/host/cf_variation_test
for rate in 44100 48000; do
    for attempt in 0 1; do
        build/host/cf_variation_test --audio "$attempt" "$rate" "build/variation-tests/$rate-$attempt.raw" \
            > "build/variation-tests/$rate-$attempt.log"
        tail -1 "build/variation-tests/$rate-$attempt.log"
    done
    cmp "build/variation-tests/$rate-0.raw" "build/variation-tests/$rate-1.raw"
done
echo 'PASS: rejected variations preserve identical physical and six-channel USB audio at both rates.'
