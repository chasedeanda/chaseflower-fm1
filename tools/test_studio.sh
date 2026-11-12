#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/studio/ui
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests tests/cf_studio_test.c -o build/host/cf_studio_test -lm
build/host/cf_studio_test
if [ "${1:-}" = "--listening" ]; then
    python3 tools/render_studio.py
fi
