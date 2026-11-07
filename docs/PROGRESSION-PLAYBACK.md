# Progression playback (issue #43)

Development source. Physical FM-1 acceptance remains required before release.

From Progressions HOME, PLAY begins the selected idea at slot 1. Every catalogue
slot plays and the complete idea loops (up to eight slots). The catalogue currently
supplies no durations: each slot lasts one bar. The prepared plan supports integer
bar durations. The default is Chords + Drums, with linked bass following the normal
Bass On/Off, octave and octaver controls. Drums Off permits chords alone; Click is
available in every meter, including meters without a compatible groove. Drums only
is explicit under GLO → Accompany. Choices remain session state.

HOME: K1 Rhythm, K2 Key, K3 Scale, K4 Tempo; PRESETS retains the chord sound.
Glide is under GLO → Accompany K4 and remains an edit of that sound. SELECT is the
complete wrapping list. Stopped browsing changes setup silently. Running browsing
queues the complete next idea, installs its tonic/mode/meter/compatible groove at
the next bar, and starts its first slot. Active values remain visible until then.
Knobs keep editing the active idea, independently of a pending idea. The latest
request wins. Groove/mode changes arrive at a bar, key/scale/modifier voicings at
an onset. Tempo changes preserve phase. A whole idea switch takes priority over
edits queued for the previous active idea.

The Now name uses actual sounding accompaniment notes unless the live keyboard is
playing, in which case it shows the live chord. The slot number and underline are
the playback cursor; matching repeated roots do not determine playback position.

Harmony On/Off continues to govern the live keyboard. Explicit catalogue playback
constructs full major/minor chords; with Auto On it also uses the deliberately
selected modifier slots, inversion and bass degree. Selecting 7 in E major produces
Emaj7, Bmaj7 and D♯m7 on degrees I, V and VII. No harmony styles return. Existing
recorded Auto takes retain their fixed MIDI notes and unchanged 22-byte events.

MIDI Clock In is explicit: external BPM is shown with EXT, local tempo edits are
disabled, and missing pulses leave position unchanged (no internal fallback).
START restarts slot 1; STOP releases accompaniment notes and pauses; CONTINUE
resumes position. Quarter-note BPM is retained in 6/8: 120 BPM is a 1.5-second bar.
Meter changes preserve quarter-note phase, even inside a pulse batch/audio block.
PLAY stops accompaniment from any screen. Recording requires stopping accompaniment
first; no implicit pattern allocation or recording happens here.

## Runtime and persistence

The foreground prepares a validated 148-byte plan: degree, duration, interval mask,
voiced MIDI notes, linked bass and complete setup. Two IRQ-protected latest-value
mailboxes separate pending ideas from edits to the active idea. Audio owns the
active copy and consumes publications at bounded boundaries. Plan construction,
strings, catalogue searches and voicing remain outside rendering. PLAY commands use
the existing event queue; queued button intent preserves rapid start/stop behavior.

One 96-PPQN accompaniment source advances chord/bass, groove, click, arp/MIDI grid
and displayed cursor. It consumes internal quarter-note time or MIDI pulses, and
handles 6/8 in half-quarter units. It does not independently start the legacy rhythm
and sequencer clocks. The mutually exclusive SQ note owner and its existing release,
MIDI, bass and Glide helpers are reused. Shared live notes are not retriggered or
released by accompaniment; Glide yields while live notes own the relevant part.

No playback plan writes patterns, drum preferences or flash. Automatically applied
key/scale/quantize/meter are excluded from the settings image persisted by the
ordinary settings poll. Other explicit settings/sound edits retain their normal
persistence. No saved record formats change.

`sh tools/test_accomp.sh` checks every catalogue idea across twelve tonics and
seven modes, all slots/loops, full chords, queued/latest requests, repeated-root
cursor, tempo phase, 6/8 and meter transitions, MIDI input/transport, rapid PLAY,
live HOLD ownership, unchanged patterns/flash, and physical-output comparisons with
USB capture/drawing at 44.1/48 kHz. Existing Auto tests cover fixed take compatibility.

Physical release gates: controls and screen readability, long releases and Glide,
all drum meters, external MIDI clock loss/restart, independent HOLD during repeated
stop/restart, and simultaneous USB capture on an FM-1. Host tests cannot certify
these hardware checks.

## Navigation across 100 ideas

Hold GLO on Progressions HOME to open Browse (a short GLO press keeps its normal
behavior). K1 jumps by ten catalogue numbers; K2 by one: #1 → #71 → #73 takes
seven and two turns. Numbers retain catalogue identity across every order. K3 sets
Catalog, case-insensitive A–Z, Shuffle, or Favorites; SELECT follows that order on
HOME and Browse. Favorites is a short wrapping list, while Catalog/A–Z/Shuffle
always include the complete library. Direct jumps work even outside a favorites list.
A shuffle is one Fisher–Yates permutation, so all ideas appear once per traversal;
choosing Shuffle again makes a fresh permutation. Browsing does not select a new
order or save flash by itself. Running direct/random jumps use the same atomic
next-bar setup queue as normal browsing.

K4 on Browse sets Favorite Yes/No. On Progressions HOME, hold SAVE for 700 ms to
toggle the visible active idea, once per hold; a short press opens sound saving.
SAVE on Browse stars its shown target. A star beside the catalogue number marks
favorites. Favorites use stable catalogue IDs and survive restart. Only an explicit
favorite/order edit dirties preferences, saved by a small logged CPB1 record in the
unused CZ pair 0xB0000/0xB1000. Existing object IDs and settings/take layouts remain
unchanged. Backup object 70 includes preferences; restore validates its checksum
and defers applying it until restart, like other preference objects.

## Validation on current main

Host checks pass: accompaniment/navigation (112,545 assertions), AddressSanitizer,
Auto, progressions, queued variations, diagnostics, visuals, USB, FX presets,
Chase3 and drum bank. Backup/restore passes 155 checks, including favorites and
malformed-record rejection; browser/Python installer archive checks pass.
Accompaniment physical output is identical with capture/drawing on/off at both
USB rates. No physical FM-1 validation has been performed.

The complete vendor-toolchain link uses 437,940 / 573,440 bytes of XIP and
97,516 / 98,304 bytes of ordinary RAM (conservative symbol accounting), with
333,852 / 344,064 bytes of pool. The 148-byte plan plus active state and both
mailboxes occupy 476 bytes. Browser order, favorites and hold state are separate.
Existing firmware link safety checks pass, including at least 8 KiB pool spare.
