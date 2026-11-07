#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Host-only score/copy/compatibility checks and real-engine renders. Optional output folder for WAVs.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/drum-bank-tests
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
   -o build/host/cf_drum_bank_test tests/cf_drum_bank_test.c -lm
if [ "$#" -gt 0 ]; then
    mkdir -p "$1"
    set -- --render "$1"
else
    set --
fi
if ! build/host/cf_drum_bank_test "$@" > build/drum-bank-tests/results.log; then
    cat build/drum-bank-tests/results.log
    exit 1
fi
tail -39 build/drum-bank-tests/results.log
