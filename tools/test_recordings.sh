#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
    -o build/host/cf_recording_test tests/cf_recording_test.c -lm
build/host/cf_recording_test
cc -std=gnu11 -O2 -w -DST_RECORDINGS -Ifirmware/src -o build/host/st_record_cut_test tests/st_log_test.c
build/host/st_record_cut_test
cc -std=gnu11 -O2 -w -o build/host/recording_flash_test tests/recording_flash_test.c
build/host/recording_flash_test

node web/test_recording_backup.mjs
