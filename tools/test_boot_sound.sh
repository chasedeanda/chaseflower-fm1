#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/boot-sound
cc -std=gnu11 -O1 -w -Itools/emu/web/compat -Ibuild/gen -Ifirmware/src -Itests -o build/host/cf_boot_sound_test tests/cf_boot_sound_test.c -lm
build/host/cf_boot_sound_test
for rate in 44100 48000; do
    for pending in 0 1; do
        build/host/cf_boot_sound_test --render "$rate" "build/boot-sound/$rate-first-key-$pending.raw" "$pending" 1
    done
    cmp "build/boot-sound/$rate-first-key-0.raw" "build/boot-sound/$rate-first-key-1.raw"
done
build/host/cf_boot_sound_test --render 44100 build/boot-sound/cord-chime.pcm 1 0
build/host/cf_boot_sound_test --render 44100 build/boot-sound/muted.pcm 1 0 0
python3 - <<'PY'
import wave, array, math, json, hashlib
from pathlib import Path
p=Path('build/boot-sound'); raw=(p/'cord-chime.pcm').read_bytes()
with wave.open(str(p/'cord-chime.wav'),'wb') as f:
    f.setnchannels(2); f.setsampwidth(2); f.setframerate(44100); f.writeframes(raw)
# Louder rising-strum development variant: finite tail and output headroom.
pcm=array.array('h',raw); peak=max(abs(v) for v in pcm); rms=math.sqrt(sum(v*v for v in pcm)/len(pcm))
assert 2000<peak<26000
assert not any(pcm[44100*23//10*2:])
print(json.dumps({'seconds':len(pcm)/88200,'peak_dbfs':round(20*math.log10(peak/32768),2),'rms_dbfs':round(20*math.log10(rms/32768),2)}))
PY
echo 'PASS: first keys match physical output, six USB channels and MIDI with/without a queued chime at both rates.'
