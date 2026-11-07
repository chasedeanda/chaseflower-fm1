#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/performance-tests
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
   -o build/host/cf_perform_test tests/cf_perform_test.c -lm
if ! build/host/cf_perform_test "$@" > build/performance-tests/results.log; then
    cat build/performance-tests/results.log
    exit 1
fi
tail -1 build/performance-tests/results.log
