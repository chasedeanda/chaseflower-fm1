# Semantic phrase and protected draft core

This is the first Milestone C engineering slice, based on merged accompaniment
PR #63 (`3a1a88957519ec3796ee72b427074c518bc4c500`). It supplies a pure, bounded
model for the native editor and Songs integration. It does not yet expose editor
controls, save a recovery checkpoint, replace recording policy or change PLAY.

## Model boundary

`cf_song_t` owns up to sixteen semantic slots, a 24-character name, the stable
factory source ID, key/mode/metre/tempo and stable rhythm/kit/swing metadata.
Factory cloning preserves source interval masks, chromatic alterations and slash
bass; generic seeds follow their degree's mode. Originals are never modified.
The current catalogue remains eight-slot data and is not resized by this slice.

Slots support Follow-mode or explicit quality, deliberate color, chromatic root,
inversion/Smooth, chord/bass register, bass Follow/Off, bar or quarter-note units,
gate and tie metadata. Bars range 1–8; quarter-note length is stored as 1–128
quarter fractions (0.25–32 quarter notes). The resolver returns existing-engine
96-PPQN ticks: in 6/8 one eighth note occupies 96 ticks, so the same absolute
quarter duration occupies twice as many ticks. Gate/tie are returned metadata;
the current accompaniment scheduler does not consume them yet.

The resolver reuses the established harmony/voicing logic, bounds MIDI notes,
clamps unavailable inversions and coalesces octave duplicates. Smooth uses the
provided previous scheduled chord. It never reads or writes physical keys,
latched modifiers, transport, hardware, storage or global Harmony. Live major/
minor seventh behavior is unchanged. Deliberately selected major seventh chords
remain possible outside a diatonic reference.

Insertion, duplication and removal name an explicit slot. Capacity refuses an
extra slot without overwriting existing content; the final slot cannot be
removed. `cf_song_draft_begin` refuses to replace a dirty protected draft.
Complete successful changes create one undo step; invalid/no-op changes preserve
it. Undo restores the previous complete phrase and leaves the draft unsaved.
Alias-safe operations also support a caller choosing its current/previous image.

## Durable format boundary

The **344-byte CSG1 v1** codec has explicit little-endian fields, printable name,
fixed sixteen-slot capacity, reserved bytes and an FNV checksum. It serializes no
pointers, native padding or inactive slot contents. Decode checks magic, version,
size, checksum, reserved bytes and all active semantic bounds into a temporary
before replacing the destination. Unknown/corrupt/truncated input leaves it
unchanged. Structural rhythm/kit metadata remains opaque to this pure layer;
the eventual device adapter must resolve stable IDs and validate its available
bank/metre before scheduling. This codec is not a new flash object or backup ID.

Offsets: header/version/size 0–7; name 8–31; source ID 32–33; BPM 34–35; rhythm ID
36–37; key/mode/metre/count/kit/swing/mode/repeat 38–45; reserved 46–51; sixteen
18-byte slots 52–339; checksum 340–343. Each slot encodes mask, degree, alteration,
inversion, register, bars, bass interval/register/Off, quality, color, gate, tie,
unit and quarter fractions in that order. Signed fields use explicit int8 bytes.
No legacy settings, recording, sound or preference record changes.

## Verification

`sh tools/test_song.sh` passes **62,449 checks**, including every catalogue chord,
all keys, extreme registers/inversions, Smooth, bar/quarter timing, protected draft
and alias cases, source-mask/bass preservation, sixteen-slot capacity and failed
mutation boundaries. Every truncated record and every single-bit corruption is
rejected; forged invalid semantic/reserved/inactive fields are rejected even
with a recomputed checksum. The same suite passes AddressSanitizer and
UndefinedBehaviorSanitizer. The 730 workspace and 153 native HOME checks pass.

Both real and host firmware units include the module; ARM syntax and a real
JieLi link/check pass. No static draft/session object is allocated yet. The
[device audit](../../../benchmarks/resources/semantic-draft-core/report.md)
records exact committed inputs and compares the unchanged source of merged #63.
Physical checks and native editor/playback/storage integration remain pending.
