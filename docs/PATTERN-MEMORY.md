# What patterns and undo cost

“Patterns” here means the recorder/sequencer's eight A–H slots. Each holds up to
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

## Agreed smaller recording target

The user clarified the intended workflow after this audit: select a progression
and its rhythm/tempo, press REC, play the chords by hand over that beat, then
keep the resulting loop. Keep at most **three individual recordings**, each
playable and clearable. A two-bar take is a representative example; duration
should remain selectable. A multi-pattern song arranger and recorder Undo/Redo
are not needed. This supersedes the earlier recommendation to retain recorder
undo. Variation's separate small previous-result record is outside this cut.

With the existing event/pattern capacity, three pattern records and the current
48-byte header would occupy 7,656 bytes, compared with today's 20,336. Removing
the 5,152-byte recorder undo allocation as well gives a **17,832-byte gross
ordinary-RAM reduction**. This is a sizing target, not a measured replacement
build: compatibility staging, take cancellation and revised controls still need
to be accounted for. There is no reason to sacrifice voices or audible FX first.

The current code still has eight patterns and Undo/Redo. The scene-removal PR
leaves them intact. Scenes use the separate **POOL** region; removing their
graphics does not turn the pattern allocation into free ordinary RAM.

Before reducing the runtime slot count, separate it from the legacy on-flash and
backup count: simply changing SQ_NPAT also changes the store checksum/layout,
pattern sound records, song indices and backup lengths. Older recordings D–H
and arrangements must remain recoverable without being silently remapped into
A–C. Current take cancellation also uses the pre-edit snapshot; replace that
behavior deliberately when removing the undo buffer.

REC on progression HOME currently does not hand off the selected catalogue groove
to the recorder: accompaniment blocks REC, and the existing HOME recording path
uses the Rhythm Machine selection. The smaller recorder must explicitly carry
the selected groove, kit, swing, meter and tempo into a manual chord take and
release automatic chord ownership. Save/restart, individual clear, full-slot
behavior, count-in/cancel and exact recorded Auto pitches need regression coverage.
