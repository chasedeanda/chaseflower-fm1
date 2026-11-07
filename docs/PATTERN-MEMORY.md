# What patterns and undo cost

“Patterns” here means the recorder/sequencer's eight A–H slots. Each holds up to
eight bars with chord events, bass, an independent LAYER synth and drums. These
are notes and timing instructions, not recorded audio files. A song can arrange
up to 32 pattern entries.

The 100 browseable progression ideas are a separate catalogue of constant tables
and strings in flash. They are not the source of this 25 KB RAM allocation.

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

The existing scheme is useful when recording and arranging multi-part loops.
For the chord-first instrument, I recommend keeping **live chord recording and
one-step undo**, then replacing the general workstation container with a
measured chord/timing/draft model. Existing takes must remain recoverable and
fixed-note Auto recordings must keep their actual pitches.

That replacement could reduce overhead, but it still needs memory for recordings,
a working draft and undo. Keeping all current event capacity in its present
layout already requires 16,896 bytes. Loading fewer slots into working memory,
using a compact encoding or a different undo strategy requires a tested storage
and migration design before quoting net savings.

This scene-removal change leaves patterns, recording, song capacity and undo
intact. Scenes use the separate **POOL** region; removing their graphics does not
turn the pattern allocation into free ordinary RAM.
