#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/g1
python3 tools/gen_engine_scene.py --check
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests tests/cf_g1_test.c -o build/host/cf_g1_test -lm
build/host/cf_g1_test
python3 tools/render_g1.py
