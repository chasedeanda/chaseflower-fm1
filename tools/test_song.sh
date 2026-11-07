#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -std=c11 -O2 -Wall -Wextra -Ifirmware/src -o build/host/cf_song_test tests/cf_song_test.c firmware/src/cf_song.c firmware/src/cf_progression.c firmware/src/cf_harmony.c firmware/src/cf_theory.c firmware/src/cf_chord.c
build/host/cf_song_test
