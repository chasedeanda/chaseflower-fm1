#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_mascot_test tests/cf_mascot_test.c -lm
build/host/cf_mascot_test
