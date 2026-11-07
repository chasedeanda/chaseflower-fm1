#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/auto-tests
cc -std=c99 -O2 -Wall -Wextra -Werror -Ifirmware/src -o build/host/cf_harmony_test \
    tests/cf_harmony_test.c firmware/src/cf_harmony.c firmware/src/cf_chord.c firmware/src/cf_theory.c
build/host/cf_harmony_test
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
    -o build/host/cf_auto_test tests/cf_auto_test.c -lm
build/host/cf_auto_test
