#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Pure host tests. No device access or flashing.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/chase3-tests
python3 tools/gen_chase_library.py --check
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests \
   -o build/host/cf_chase3_test tests/cf_chase3_test.c -lm
build/host/cf_chase3_test > build/chase3-tests/integration.log
tail -9 build/chase3-tests/integration.log
cc -std=gnu11 -O2 -w -DCHASE_LIBRARY=1 -Ibuild/gen -Ifirmware/src \
   -o build/host/chase3_va_test tests/cr_va_test.c -lm
build/host/chase3_va_test > build/chase3-tests/va.log
tail -1 build/chase3-tests/va.log
