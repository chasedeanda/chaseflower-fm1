#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Host-only tests, including word-for-word audio comparisons while changing scenes. No flashing.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/visuals-tests
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
   -o build/host/cf_visuals_test tests/cf_visuals_test.c -lm
build/host/cf_visuals_test > build/visuals-tests/state-draw.log
tail -1 build/visuals-tests/state-draw.log
build/host/cf_visuals_test --audio 0 build/visuals-tests/steady.raw > build/visuals-tests/steady.log
build/host/cf_visuals_test --audio 1 build/visuals-tests/switching.raw > build/visuals-tests/switching.log
build/host/cf_visuals_test --audio 2 build/visuals-tests/off.raw > build/visuals-tests/off.log
cmp build/visuals-tests/steady.raw build/visuals-tests/switching.raw
cmp build/visuals-tests/steady.raw build/visuals-tests/off.raw
tail -1 build/visuals-tests/switching.log
echo 'PASS: scene migration, persistence, Off bypass, HOME/FX/sequencer controls, boot exit/skip and identical physical/USB audio.'
