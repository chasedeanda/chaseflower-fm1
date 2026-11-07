#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/fx-tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src -o build/host/cf_fx_preset_test tests/cf_fx_preset_test.c firmware/src/cf_fx_preset.c
build/host/cf_fx_preset_test
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_fx_presets_test tests/cf_fx_presets_test.c -lm
build/host/cf_fx_presets_test
cc -std=gnu11 -O2 -Wall -Wno-unused-function -Ifirmware/src -o build/host/st_fx_log_test tests/st_log_test.c
build/host/st_fx_log_test
for hz in 44100 48000; do
    for capture in 0 1; do
        build/host/cf_fx_presets_test --render "$capture" "$hz" "build/fx-tests/$hz-$capture.raw" > "build/fx-tests/$hz-$capture.log"
        tail -1 "build/fx-tests/$hz-$capture.log"
    done
    cmp "build/fx-tests/$hz-0.raw" "build/fx-tests/$hz-1.raw"
done
build/host/cf_fx_presets_test --shots build/fx-tests > build/fx-tests/shots.log
