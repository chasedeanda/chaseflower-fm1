#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_editor_test tests/cf_editor_test.c -lm
build/host/cf_editor_test
cc -std=gnu11 -O2 -w -DST_DRAFT -Ifirmware/src -o build/host/st_draft_cut_test tests/st_log_test.c
build/host/st_draft_cut_test
