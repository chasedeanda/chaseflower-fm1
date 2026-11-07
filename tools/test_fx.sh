#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/fx-tests
python3 - <<'VERIFY'
import sys
from pathlib import Path
sys.path.insert(0, 'tools')
from gen_fx_presets import generate
assert Path('firmware/src/cf_fx_factory.c').read_text()==generate(), 'Factory bank differs from approved auditions'
VERIFY
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
   -o build/host/cf_fx_test tests/cf_fx_test.c -lm
if ! build/host/cf_fx_test > build/fx-tests/controls.log; then cat build/fx-tests/controls.log; exit 1; fi
tail -1 build/fx-tests/controls.log
build/host/cf_fx_test --audio 0 build/fx-tests/steady.raw > build/fx-tests/steady.log
build/host/cf_fx_test --audio 1 build/fx-tests/drawing.raw > build/fx-tests/drawing.log
cmp build/fx-tests/steady.raw build/fx-tests/drawing.raw
echo 'PASS: 13 recipes, recall/browse/view isolation, bounded anchored macros, 188-byte settings, Motion Off and identical physical/USB audio while drawing.'
