# Recording memory: before and after

The original recorder/sequencer reserved eight A–H slots. Each holds up to
eight bars with chord events, bass, an independent LAYER synth and drums. These
are notes and timing instructions, not recorded audio files. A song can arrange
up to 32 pattern entries.

The 100 browseable progression ideas are a separate catalogue of constant tables
and strings in flash. They are not the source of this 25 KB RAM allocation.
The new progression player and its two bounded mailboxes use 476 bytes in the
measured device build; it does not copy ideas into the eight recorder slots.

| What is reserved | RAM bytes | What it gives you |
| --- | ---: | --- |
| Recorded note/chord events | 16,896 | 96 events per pattern, shared by chords/bass/LAYER, across eight patterns |
| Drum-hit maps | 2,816 | Eleven drum lanes, eight bars per pattern |
| Pattern settings | 576 | Track lengths/divisions, kit choices, swing, mute, key/scale and other settings |
| Song and storage header | 48 | Pattern order and checked store metadata |
| **Pattern store** | **20,336** | All eight patterns available in RAM |
| Undo/pre-edit snapshots | 5,152 | Two copies of the current pattern/song plus bookkeeping |
| **Combined** | **25,488** | **About 26% of the 96 KB ordinary-RAM region** |

The memory is reserved even when slots are empty. The synth patches and audio
buffers have separate allocations. Removing LAYER alone does not remove one
third of the pattern store: its 96 event slots are shared with chords and bass.

Undo is one level, with redo. One snapshot remembers the state before editing;
the other is the saved undo state. It can undo a recording or an edit, including
an accidental clear. It does not keep a long history. Its two 2,536-byte pattern
copies account for 5,072 bytes; two song copies and flags/timing account for the
other 80 bytes. This is the sequencer's undo; Variation has a separate, much
smaller previous-result record.

## Implemented three-recording model

The current default holds three recent recordings, newest first. REC uses the
progression's selected rhythm/kit/swing and tempo while you play the chords.
Successful new takes replace the oldest by default; GLO → Recordings → AUTO DEL
Off protects a full set until you clear a slot. Cancelled takes keep all previous
recordings. Recorder Undo/Redo and the song-arrangement tab are omitted.
[RECORDINGS.md](RECORDINGS.md) explains the controls and save/export workflow.

The live store is now **7,656 bytes**, down from 20,336. The **5,152-byte recorder
undo allocation is gone**. Ordering, per-take tempo/meter, preferences and a few
control fields use a little additional ordinary RAM. The measured net ordinary
RAM reduction is **17,728 bytes**, increasing headroom from **1,256 to 18,984**.
This is from real JieLi device links with progression playback retained.

Cancellation still needs to protect an overwritten oldest take until the new one
succeeds. A **2,584-byte transient cancel guard** lives in POOL, alongside a
**3,840-byte foreground flash/migration buffer**. They add 6,424 POOL bytes; the
scene-free build has 28,660 POOL bytes free after this change. These are separate
from ordinary RAM and do not recreate edit/redo history.

| Region | Before recorder change | After | Change |
| --- | ---: | ---: | ---: |
| Ordinary RAM | 97,048 | 79,320 | **−17,728** |
| POOL | 308,980 | 315,404 | +6,424 |
| XIP | 419,652 | 421,640 | +1,988 |
| RAM code | 3,016 | 3,052 | +36 |

Full inputs, compiler flags, section/symbol inventories and the linked baseline
identity are in [the measured report](../benchmarks/resources/recordings/report.md).
No voices, sound engines, audible FX tails, USB or MIDI channels were removed.
These static memory measurements make no runtime-speed or polyphony claim.

The new storage model separates three live slots from the eight-pattern archive.
It streams old CFQ5/CFQ6 records without reserving all eight in ordinary RAM and
saves each new take independently. The old pattern/song objects stay untouched.
Settings retain their 188-byte layout; recorder policy has a separate checked
record. Backup/restore includes both archives and active recordings. Power-cut,
legacy import, cancellation, age ordering, protected-full and per-take tempo/meter
specs cover the replacement.
