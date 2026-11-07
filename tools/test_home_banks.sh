#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/home-banks
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_home_banks_test tests/cf_home_banks_test.c -lm
build/host/cf_home_banks_test
build/host/cf_home_banks_test 0 build/home-banks/steady.raw
build/host/cf_home_banks_test 1 build/home-banks/cycling.raw
cmp build/home-banks/steady.raw build/home-banks/cycling.raw
echo 'PASS: identical audio with HOME bank cycling and drawing.'
