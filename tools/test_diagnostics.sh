#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/diagnostics
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
   -o build/host/cf_diagnostics_test tests/cf_diagnostics_test.c -lm
build/host/cf_diagnostics_test build/diagnostics/metrics.syx
python3 tests/test_diagnostics.py
