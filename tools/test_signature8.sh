#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/signature8
python3 tools/gen_signature8.py --check
python3 tools/gen_chase_library.py --check
cc -std=gnu11 -O2 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests tests/cf_signature8_test.c -o build/host/cf_signature8_test -lm
build/host/cf_signature8_test
build/host/cf_signature8_test --inventory build/signature8/legacy.bin
python3 -c 'import hashlib,pathlib; p=pathlib.Path("build/signature8/legacy.bin"); expected=pathlib.Path("assets/sounds/signature8-legacy.sha256").read_text().split()[0]; assert hashlib.sha256(p.read_bytes()).hexdigest()==expected, "legacy programs or global indices changed"'
for hz in 44100 48000; do
    for capture in 0 1; do
        build/host/cf_signature8_test --usb "$capture" "$hz" "build/signature8/usb-$hz-$capture.raw" > "build/signature8/usb-$hz-$capture.log"
        tail -1 "build/signature8/usb-$hz-$capture.log"
    done
    cmp "build/signature8/usb-$hz-0.raw" "build/signature8/usb-$hz-1.raw"
done
