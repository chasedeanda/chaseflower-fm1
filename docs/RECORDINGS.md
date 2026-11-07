# Three recent recordings

On Progressions HOME, choose an idea, rhythm and tempo. Press **REC**. After one
bar of count-in, play the chords yourself over the selected drum beat. Automatic
progression chords stop so your playing owns the take. The selected kit, swing,
meter and tempo carry into the recording.

Press **REC** again to finish in whole bars; a take can be one to eight bars and
ends automatically at eight. Hold REC to cancel the current new take. Cancellation
keeps your previous recordings, including when all three slots were occupied.

The default keeps the **latest three** completed takes. The newest is always
**1**, the previous 1 becomes 2, and the previous 2 becomes 3. A successful fourth
take replaces the oldest. An empty or cancelled take does not push anything out.
The ordering survives a restart without copying all three patterns around RAM.

Press SEQ, then SEQ again to open **Recordings** (when the metronome's destination
is Recordings). Turn ALGORITHM to choose 1, 2 or 3; PLAY starts/stops the selected
take. SAVE returns from a track editor to this list. The existing track editors
remain available, but the song-arrangement tab is omitted.

Recording data saves automatically after playback stops and the instrument has
been quiet briefly. Stop playback before powering off. No separate SAVE gesture
is required to keep a completed take. The brief quiet interval keeps flash-sector
erases out of live playback.

## Keep recordings until cleared

In **GLO → Recordings**, turn **AUTO DEL** Off. A full set then refuses another
take and tells you to clear a recording first. With AUTO DEL On (the default),
completed new takes replace the oldest automatically.

To clear one manually, stop playback, select it in Recordings and **hold SAVE for
three seconds**. Releasing early or selecting a different recording cancels the
clear gesture. Clearing has no Undo. Holding REC cancels a new take; it does not
undo earlier recording or editing changes. Variation retains its separate small
previous-result control.

## GarageBand and backups

Play a take and record its USB audio or MIDI output into GarageBand. The firmware
backup files preserve device data; they are not GarageBand projects, WAV files or
standard MIDI files. Direct file export is separate from this recorder change.

Use the updated web or Python installer to back up the new recordings. An older
installer may not recognize their new backup IDs. Legacy A–H pattern/song objects
remain archived and exportable; A–C initially populate the three recent slots.
D–H and the old arrangement are never silently mapped into the three live slots.
An older firmware plus the original backup can recover them. Existing D–H sound
references and tweaked-sound records are retained rather than reclaimed.

## Developer notes

The runtime pattern count is three; the legacy archive count remains eight. New
`CTK1` records are 2,556 bytes each and include a checked version/slot, sequence
number, tempo, meter and unchanged 2,536-byte pattern payload. Logical age is
separate from physical slot IDs. New backup IDs 71–73 hold the three physical
slots; 74 holds the checked recording policy. Modern restore clients apply old
archive chunks before active takes regardless of JSON object order.

Each recording uses one existing storage A/B transaction in sectors E6000–EBFFF;
policy uses EC000–EDFFF. These allocations are beyond updater staging E0000–E4FFF
and below globals FC000–FEFFF. Firmware/update-loader addresses and all legacy
object addresses are unchanged. Backup LIST/PUT wait during a live replacement;
unfinished data is not saved over an earlier take. Unknown recording versions are
kept unavailable until explicitly cleared, rather than silently resurrecting an
older archived take.

`sh tools/test_recordings.sh` runs the focused recording, storage power-cut,
flash-boundary and web/Python restore-order specs. The full feature regression
suite runs in CI. Physical device listening/input/timing remains a release check.
