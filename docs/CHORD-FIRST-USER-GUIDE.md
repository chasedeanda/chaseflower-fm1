# Playing the chord-first FM-1

> **Planned successor interface.** This is the player documentation for the design board. These controls and later-stage features are not all available in the currently released firmware.

The [implementation plan](design/chord-first/IMPLEMENTATION-PLAN.md) uses the supplied [adaptive prototype](design/chord-first/instrument-teal/prototype.html), with **Instrument Teal** as the default palette and **Motion On (adaptive) / Off**.

## Start here

> Engineering preview: native Instrument Teal HOME/banks and Groove/Loop/Mix/Click now lead into EDIT for a protected sixteen-slot semantic draft. Numbered chord/timing/voicing/bass/phrase controls, explicit remove confirmation and one-step undo feed the same accompaniment clock; quarter slots, gates/ties and bar-bound groove edits preserve phase. Active and pending plans, real cursor and planned focus stay distinct. Safe stopped-only checkpoints report RAM Pending, verified Checkpoint or Recovered, with checked backup object 75. Factory originals, live Harmony/HOLD and the current manual-take/latest-three policy remain protected. Numbered Songs, chord-library/audition, arranged recording, learning/suggestions, remaining page/motion migration and physical verification are pending.

1. Open HOME. Turn SELECT to explore the complete progression catalogue; each idea supplies its starting key, mode and time signature.
2. Press PLAY to hear the selected progression. Press PLAY again to stop.
3. Turn K1 for a compatible drum groove or Drums Off, K2 to transpose, K3 for the mode and K4 for tempo. PRESETS changes the chord sound; MASTER changes volume.
4. Play the lower root keys and deliberately choose upper-key modifiers. The large chord and degree show what you hear; the phrase rail keeps the planned order visible.
5. As the later milestones arrive, use EDIT to develop a progression, REC to capture it directly, SAVE to open Songs, SEL to learn from the sound and the Next view to explore continuations.

## Read the main screen

The large chord is the sound you are hearing. Its degree/quality line might read **vi · minor**. In F major, the planned phrase can show **IV over Bb**, **V over C**, **vi over Dm**, and **I over F**. Uppercase numerals mean major; lowercase means minor. A slot counter such as 03/04 is the transport position, not the scale degree.

The notes line shows actual chord tones. The phrase cue shows planned chords; its cursor continues while you temporarily play over a loop. Pending changes are labelled until they become active. Instrument Teal is the default appearance. Display → Motion has only On and Off: On automatically adapts its response to loudness and active instruments; Off stops all visual animation while musical values, chord identity and playback/record/save status continue to update instantly. The supplied nine-screen adaptive prototype defines the visual direction; several study controls and musical engines are not implemented features.

## Delivery order

- **Play — first milestone.**
- **Edit and save — second milestone.**
- **Learn — third milestone.**
- **Suggest — fourth milestone.**

## Physical controls

| Control | On HOME | Elsewhere |
| --- | --- | --- |
| MASTER | Master volume | Always controls output volume. |
| SELECT | Browse every progression | Select an item/slot in the current workspace. |
| PRESETS | Chord sound | Keep sound browsing accessible; deep editing is under GLO → Sound. |
| ALGORITHM | Progressions / Bass / Bonfire / Nothing | Choose a visible workspace tab; secondary scenes remain optional. |
| K1 | Rhythm / groove | Edit the parameter labelled K1 on the current screen. |
| K2 | Key | Edit the parameter labelled K2 on the current screen. |
| K3 | Scale / mode | Edit the parameter labelled K3 on the current screen. |
| K4 | Tempo | Edit the parameter labelled K4 on the current screen. Glide lives in detail. |
| HOME | Cycle Core / Sound / Texture / Groove | From another screen return to Progressions Core; while on Progressions, cycle the four live control banks. |
| SEL | Inspect the sounding chord | Enter the highlighted item; follow explicit on-screen audition labels. |
| SEQ | Accompaniment | Groove, loop, arrangement and click details. |
| EDIT | Edit the selected progression slot | Keep the numbered edit focus separate from the sounding chord. |
| SAVE | Songs: save current / browse saves | Naming SAVE commits New or reviews Replace; a labelled SAVE confirms the selected explicit action. |
| PLAY | Start/stop accompaniment | One transport across workspaces; an explicitly labelled audition is temporary. |
| REC | Record directly from Progressions | Count in or arm at the next bar; finish into a new Song, with Pending/Saving/Saved truthfully shown. |
| ARP | Chord motion | Arpeggiator and performance-motion details. |
| LFO | Bass | Retains a direct route to bass rather than hiding it behind an advanced editor. |
| ENV | Envelope | Shape the current sound; advanced synthesis remains available in Sound detail. |
| FX | Effects | Presets and individual effect settings. |
| GLO | Grouped settings | Harmony, Sound, Controls, Display, MIDI/USB and System; back within settings. |
| OCT− / OCT+ | Performance octave | Keep existing panic/recovery gestures distinct and documented in System. |
| Lower note keys | Play roots | Live input takes priority according to the explicit note-ownership contract. |
| Upper chord keys | Deliberate modifiers and HOLD | Major/minor/extension choices remain explicit; no automatic Rich/Dreamy layers. |

## Step-by-step workflows

### Find a progression and press PLAY

**Availability:** Play — first milestone. Hear a musical song immediately.

1. **Turn SELECT through the complete wrapping list.** — Seed key, mode and metre update without sound or an Apply step. (Progressions · CORE).
2. **Turn K1 to a compatible groove or Off.** — Tempo remains yours. (Progressions · CORE).
3. **Press PLAY.** — Slot 1 and groove downbeat start together; large chord, degree and ordered rail agree. (Progressions · CORE).
4. **Press PLAY again.** — Accompaniment stops; independent live HOLD remains. (Progressions · CORE).

### Change the loop while hearing it

**Availability:** Play — first milestone. Change rhythm, key, mode and tempo from HOME.

1. **Turn K1 during playback.** — New groove appears as pending and begins next bar. (Progressions · CORE).
2. **Turn K2 or K3.** — Latest complete key/mode request applies at next chord onset; active labels stay truthful. (Progressions · CORE).
3. **Turn K4.** — Internal tempo changes promptly without resetting slot, bar or drum phase. (Progressions · CORE).
4. **Turn SELECT to another song.** — Next shows its seed setup; the whole new song begins at the next bar. Browse back to active to cancel. (Progressions · Next Song Queued).

### Play your own chord over the groove

**Availability:** Play — first milestone. Use live roots/HOLD without fighting scheduled chord ownership.

1. **While the loop runs, press a root; use upper modifiers deliberately.** — Live chord and linked bass temporarily take harmony ownership; drums and actual scheduled cursor continue. (Progressions · CORE).
2. **Use HOLD, or release the roots.** — HOLD sustains live takeover. After its final live root/latch releases, the currently scheduled chord resumes for the remaining slot time. (Progressions · CORE).
3. **Press PLAY to stop.** — Only accompaniment ownership stops; a live held chord survives. (Progressions · CORE).

### Use premade drums for manual playing

**Availability:** Play — first milestone. Start a groove without automatic chords.

1. **Press SEQ and turn ALGORITHM to Loop; choose Drums only with K1.** — Mode changes on the documented bar boundary. (Playback and Loop).
2. **Choose Groove tab and select the pattern.** — Kit/swing defaults apply without replacing user tempo. (Groove).
3. **Press HOME, then PLAY and play roots.** — One groove clock supports immediate manual chord performance. (Progressions · CORE).

### Change one chord and its timing

**Availability:** Edit and save — second milestone. Edit a semantic slot without changing a saved original.

1. **Press EDIT; read the explicit slot and select another with SELECT if needed.** — Factory seed becomes a protected draft; dirty-draft conflicts use an explicit choice. (Edit Chord).
2. **Turn Degree, Quality, Color and Length.** — Only this planned chord changes; global Harmony/modifiers stay intact. (Edit Chord).
3. **Choose Timing with ALGORITHM; set Bars/Quarter notes, Length, Gate or Tie.** — Units and articulation are explicit. (Chord Timing).
4. **Choose Voicing; set inversion/register. Use Bass tab for the bass tone.** — The resolved notes show the actual planned voicing. (Chord Voicing).
5. **Press SEL to audition, or PLAY to hear the phrase.** — Intentional audition/plan playback respects live ownership and safe plan boundaries. (Edit Chord).

### Extend a phrase and arrange A/B

**Availability:** Edit and save — second milestone. Represent longer forms with bounded slots and repeats.

1. **Choose Insert/Duplicate and position, then SEL.** — Slot is inserted safely up to the 16-slot design limit. (Phrase and Metre).
2. **Choose metre explicitly, or choose B to make an explicit A copy.** — Bar-based durations follow metre; quarter-note lengths remain fixed. (Phrase and Metre).
3. **Press SEQ, choose Arrange, select A/B and repeats, then Add after and SEL.** — A short A/B chain is built with visible order. (Arrange A/B).
4. **Press PLAY.** — The explicitly selected arrangement plays on the shared clock. (Arrange A/B).

### Understand the chord you hear

**Availability:** Learn — third milestone. See degree, notes, intervals and outside-scale tones.

1. **Press SEL on HOME.** — Now/Last target is explicit; actual chord quality drives the numeral and note spelling. (Notes and Degree).
2. **Choose Intervals with ALGORITHM; use K2 to focus notes.** — Chord intervals and membership are shown accurately. (Chord Intervals).
3. **Turn SELECT to a numbered planned slot.** — Inspector and EDIT focus that slot, separately from current sound. (Notes and Degree).

### Hear a theory comparison

**Availability:** Learn — third milestone. Compare one musical difference without rewriting the song.

1. **Choose Compare; select Triad/Color or Root/Smooth and side A/B.** — Browsing remains silent. (Compare Chord Sounds).
2. **Press SEL.** — One-bar deliberate audition uses temporary harmony ownership; groove and clock continue. (Comparison Audition).
3. **Wait for completion, press SEL to stop, or play a live root.** — Return to current scheduled chord for its remaining time; live input takes priority and cancels audition. (Comparison Audition).

### Build a path with offline next chords

**Availability:** Suggest — fourth milestone. Explore suggestions and commit only the chord you choose.

1. **Open Lab → Next; inspect three candidates and the visible draft slot.** — Offline, context-aware suggestions do not change sound or content. (What Could Come Next?).
2. **Press SEL to hear a candidate.** — Temporary one-bar audition does not alter committed history. (Audition Next Chord).
3. **Press EDIT to insert into the visible slot.** — Candidate replaces that slot with one-step undo and explicit local quality. (Candidate Inserted).
4. **Continue editing or use SAVE → Undo.** — No global Harmony or modifier change is hidden in insertion. (Edit Chord).

### Turn a held chord into an arp or strum

**Availability:** Play — first milestone. Change delivery while retaining harmony and groove.

1. **Press ARP; enable and choose one of the nine existing modes.** — Chord notes use arp/strum delivery on the shared tempo. (Arpeggio and Strum).
2. **Choose Feel; set Gate/Swing and chord-sound Glide.** — Expressive controls stay available after Tempo moves to HOME. (Motion Feel and Glide).
3. **Return HOME and play or start the progression.** — Large harmonic identity remains stable while note activity can move. (Progressions · CORE).

### Shape bass and envelope

**Availability:** Play — first milestone. Keep useful sound controls direct.

1. **Press LFO; browse Bass with SELECT and set level/register.** — Bass has an explicit target; PRESETS stays chord sound. (Bass Sound and Level).
2. **Press ENV and select Chord/Bass/Linked.** — Attack, Decay, Sustain and Release apply to the named target. (Envelope).
3. **Return HOME.** — Musical changes remain current and are included in future complete-song saving. (Progressions · CORE).

### Find, favorite and save a sound

**Availability:** Edit and save — second milestone. Keep patch management distinct from the primary song workflow.

1. **Open GLO → Sound → Presets; choose target and bank, then SELECT.** — Sound loads with explicit target and safe musical-boundary policy. (Sound Presets).
2. **Use K3 to favorite the selected sound.** — Favorite reference uses its stable sound ID. (Sound Presets).
3. **Press SEL on Sound Presets, choose Save As New, then SEL.** — The explicit sound-only naming route creates a user patch; replacing requires its named confirmation. (Sound Actions).
4. **Return HOME, press SAVE to open Songs, then SAVE again for current complete-Song saving.** — Patch-only persistence is not confused with saving the complete setup. (Songs).

### Shape and retain the effect chain

**Availability:** Play — first milestone. Use only the supported six effects with clear controls.

1. **Press FX and SELECT Reverb.** — K1 is On, K2 Type, K3 Decay, K4 Mix. (Reverb).
2. **ALGORITHM to Order; SELECT an effect and K1 to move it.** — Routing changes explicitly; ALGORITHM remains tab navigation. (Effect Order).
3. **Return HOME.** — Current chain continues; phase-2 complete-song save retains it. (Progressions · CORE).

### Recall or save an effect preset

**Availability:** Edit and save — second milestone. Swap effects without changing the harmony or groove.

1. **Choose Presets tab; browse and press SEL to recall.** — Only supported effect chain parameters/order/bypass change; Undo is available. (Effect Presets).
2. **Choose Save As New, enter a name and SAVE.** — Named effects-only record is saved; the whole song still uses its normal save route. (Name Effect Preset).

### Use an external clock deliberately

**Availability:** Play — first milestone. Keep Receive waiting, loss and resume understandable.

1. **Choose Receive in System, then return HOME.** — Without pulses, Waiting/armed and EXT are shown; local tempo editing is unavailable. (Progressions · CORE).
2. **Send external Start.** — Slot 1 starts. External Continue instead resumes retained position. (Progressions · CORE).
3. **If the clock disappears, read Clock lost.** — Only accompaniment stops/releases; cursor and independent live HOLD remain. Pulses alone never restart. (Progressions · CORE).
4. **Send an explicit Start/Continue, or choose Internal in System and press local PLAY.** — Playback resumes only deliberately. (Progressions · CORE).

### Record what is playing directly from Progressions

**Availability:** Edit and save — second milestone. Make and automatically save a new recorded Song without leaving HOME or reviewing a take.

1. **Choose a progression/groove and press REC while stopped.** — HOME shows one-bar count-in; REC cancels it without a Song row. (Progressions · CORE).
2. **Wait one full bar.** — Selected automatic progression and groove begin with capture; no chord suppression. (Progressions · Recording Count-in).
3. **Change key/tempo/groove or play manual overrides as desired.** — The fixed performed notes and timed musical control changes are captured. (Progressions · Recording).
4. **Press REC again.** — Finishing appears; at next bar accompaniment stops, independent live HOLD remains. (Progressions · Finishing Recording).
5. **Wait for safe validated save.** — A new source-based numbered Recording Song appears as Saved; source original is untouched. (New Song Saved).
6. **Press SAVE.** — Songs opens with the new recording selected; no hidden recordings list or required Keep. (Songs).

### Capture a running performance

**Availability:** Edit and save — second milestone. Arm at the next bar without restarting the existing phrase.

1. **During playback, press REC.** — HOME shows Armed; the current progression/groove keep playing. (Progressions · CORE).
2. **Reach the next downbeat.** — Capture begins from the actual phrase position and currently audible state. (Progressions · Record Armed).
3. **Press PLAY for a short end.** — Close at actual elapsed time, stop accompaniment promptly, and auto-create a new Song copy without padded silence. (Progressions · Recording).
4. **Press SAVE after completion.** — See duration, Recording type and true Pending/Saving/Saved status in Songs. (Songs).

### Record your own chords over drums

**Availability:** Edit and save — second milestone. Use the same capture method with an intentional accompaniment choice.

1. **Press SEQ, choose Loop and set Drums only.** — Automatic chords are off by your explicit choice, not by REC. (Playback and Loop).
2. **Press HOME and REC.** — Normal stopped count-in or running next-bar arm begins. (Progressions · CORE).
3. **Play roots/modifiers over the groove; finish with REC or PLAY.** — A new Recording Song captures the heard manual performance and accompaniment. (Progressions · Recording).

### Finish saving a Pending capture

**Availability:** Edit and save — second milestone. Keep captured content visible if sound/storage/capacity prevents persistence.

1. **Read Pending and its reason after the recording ends.** — No false Saved label; the protected capture is in Songs. (Song Pending · Kept in Songs).
2. **Press SAVE to open the selected Pending row.** — Type and duration remain visible alongside saved Songs. (Songs).
3. **Press EDIT to Manage; remove chosen Saved content if storage is full.** — Pending and protected current/draft content are excluded from bulk clear. (Manage Songs).
4. **Stop accompaniment and release live-held notes when the prompt asks.** — Existing quiet/storage guards permit the automatic safe write; audible tails are not cut. (Song Pending · Kept in Songs).
5. **Wait for validation.** — Pending becomes Saved; another REC may create the next new copy. (New Song Saved).

### Save an editable progression Song

**Availability:** Edit and save — second milestone. Keep current-save distinct from highlighted-row management.

1. **Press SAVE from HOME to open Songs, then SAVE to save current.** — A new progression opens a prefilled name; the highlighted row is not an overwrite target. (Songs).
2. **Keep the suggested name or edit it; press SAVE.** — A new Progression Song saves directly when safe, with no extra free-slot review. (Save New Song).
3. **For an edited saved current, choose Save As New or explicit Update.** — Original remains unchanged unless named Update is confirmed. (Save Current Song).
4. **If Update was chosen, check the target/count and press SAVE on Update.** — Only the named existing Song is updated. (Update Saved Song?).

### Find and load either Song type

**Availability:** Edit and save — second milestone. Make row browsing safe and playback source explicit.

1. **Press SAVE; SELECT highlights a row.** — Rows show Progression/Recording type, duration and status; highlighting alone changes no music. (Songs).
2. **Press SEL on the intended row.** — Load applies stopped or queues the complete Saved setup at next bar. (Songs).
3. **Press HOME and PLAY.** — Progression plays semantic chords; Recording plays its fixed note/control/tempo timeline. (Progressions · CORE).
4. **For a Recording, Manage → Details → EDIT develops its start.** — A new semantic Progression draft is editable without rewriting the recorded performance. (Song Details).

### Rename, duplicate or delete one Song

**Availability:** Edit and save — second milestone. Manage the highlighted row with direct physical controls.

1. **SELECT the row and press EDIT.** — Manage keeps the selected Song name/type visible. (Songs).
2. **Choose Rename, Duplicate or Delete, then SAVE/SEL.** — Rename uses text entry; Duplicate creates a new numbered ID; Delete opens one named counted confirmation. (Manage Songs).
3. **Choose Delete and press SAVE; HOME cancels.** — Only one Saved row is removed once safe; loaded audio, protected draft and Pending remain. (Delete Selected Song?).
4. **Return to Songs.** — The exact result is visible; there is no deep take browser. (Songs).

### Record → Songs → delete or clear recordings

**Availability:** Edit and save — second milestone. Make capture and cleanup one obvious route.

1. **REC finishes at next bar, or PLAY ends now.** — Accompaniment stops; a new Recording Song auto-saves when safe. (Progressions · Recording).
2. **Press SAVE to see it; SELECT the desired row, then EDIT.** — Manage acts on the visible highlighted Song. (Songs).
3. **For one item choose Delete, then confirm count 1 with SAVE.** — Only that Saved Song is deleted. (Delete Selected Song?).
4. **For a batch choose Clear Recordings in Manage; read the saved count and confirm with SAVE.** — One explicit confirmation clears Saved Recording rows only; Pending/draft/current audible copy remain. (Clear Saved Recordings?).
5. **Clear All Songs is the separate all-types option with its own visible count.** — HOME cancels either counted confirmation; no silent library-wide deletion. (Clear All Saved Songs?).

### Return to a draft, undo or recover

**Availability:** Edit and save — second milestone. Keep protected musical edits outside library clear scope.

1. **Open Songs and press EDIT for Manage.** — Return to Draft and Undo remain visible even if the Saved list is empty. (Songs).
2. **Choose Return to Draft and execute Return.** — Draft becomes current/queued without an Apply step on normal factory browsing. (Return to Working Draft).
3. **Manage → Undo restores one musical gesture.** — Saved-row deletion is not disguised as musical undo. (Undo / Redo).
4. **After restart Recover the last valid stopped checkpoint.** — Opens stopped; a never-written Pending recording is not falsely claimed recoverable. (Recover Draft Checkpoint?).

### Use the four live control banks

**Availability:** Play — first milestone. Reach existing controls without sacrificing CORE, full catalogue browsing or musical keys.

1. **Press HOME from another workspace** — Progressions opens CORE: Rhythm / Key / Scale / Tempo (Progressions · CORE).
2. **Tap HOME while already on Progressions** — SOUND: Chord sound / Bass sound / Chord level / Bass level; bank selection changes no values (Progressions · SOUND).
3. **Tap HOME again** — TEXTURE: Reverb mix / Delay mix / Chorus mix / Glide (Progressions · TEXTURE).
4. **Tap HOME again** — GROOVE: Kit / Drum level / Swing / Click (Progressions · GROOVE).
5. **Tap HOME again** — CORE returns. SELECT still browses all songs; PRESETS stays chord sound; OCT keys remain musical. (Progressions · CORE).

### Browse, audition and replace a chord

**Availability:** Edit and save — second milestone. Choose a named chord from the reference without losing the numbered edit destination or altering global Harmony.

1. **Open EDIT and SELECT the destination slot** — A numbered draft slot is focused; the source Song remains untouched. (Edit Chord).
2. **Turn ALGORITHM to Library and SELECT a candidate** — The reference list browses silently; target slot, degree, notes and variant are visible. (Chord Library).
3. **Press SEL** — A deliberate bounded audition uses the shared note-owner rules; the slot is unchanged. (Chord Library).
4. **Press EDIT Replace** — The named slot receives the candidate, one-step undo is available and any running plan change follows its musical boundary. (Edit Chord).

### Enable simple Harmony

**Availability:** Play — first milestone. Play full major/minor chords with deliberate modifiers.

1. **Choose Harmony; press SEL** — One On/Off control (Settings).
2. **Turn K1 On** — Auto active; show a direct support message if quantize/scale is unsupported (Harmony).
3. **Choose a slot; assign modifier/inversion/bass** — Assignments change deliberately; HOLD remains separate (Modifier assignments).

### Set accent and motion

**Availability:** Play — first milestone. Use Instrument Teal or optional Studio/Editorial palettes, with adaptive Motion On or totally Off.

1. **Choose Instrument Teal, Studio or Editorial** — The same type-led layout uses the selected flat palette (Appearance · Theme).
2. **Preview a solid accent or restore its palette default** — Chord/degree and focus/status stay readable (Solid Accent).
3. **Choose On or Off** — On adapts to smoothed output/volume and active parts; Off stops all motion/transitions while values update instantly (Motion).

### Use external MIDI and clock

**Availability:** Play — first milestone. Route incoming notes and synchronize without local tempo ambiguity.

1. **Choose Roots and input channel; Clock Receive** — Owned incoming notes are safely released when interpretation changes (MIDI).
2. **Start clock on the master device** — EXT lock and externally controlled tempo (Waiting for clock).
3. **Resolve lost source or explicitly choose Internal** — Accompaniment stops and retains position; live HOLD survives. Only explicit Start/Continue or choosing Internal allows a deliberate resume. (Clock lost).

### Record USB audio on a computer

**Availability:** Play — first milestone. Capture the whole mix or deliberate dry stems.

1. **Connect data cable; select input/rate in the recording app** — Host-selected capture state/rate displayed (USB capture).
2. **Inspect errors only if recording has problems** — Session deltas and measurement limits remain honest (Diagnostics · USB).

### Shape chord and bass envelopes

**Availability:** Play — first milestone. Edit musical Attack/Decay/Sustain/Release with an explicit target.

1. **Press ENV; SELECT chooses Linked/Chord/Bass** — One canonical envelope target and four visible ADSR controls (Envelope).
2. **Edit the named target** — Linked affects both; legacy Layer requires its own explicit route (Envelope).

### Edit a retained FM6 or VA sound

**Availability:** Edit and save — second milestone. Reach every retained deep page without making it the main workflow.

1. **Choose Advanced sound for named target** — Target explicit; PRESETS stays chord sound (Sound).
2. **Choose section and parameter page** — Four bounded labelled parameters (Advanced sound).
3. **Edit named parameter** — Only named target/parameter changes (FM6 OP 1 · Envelope Rate).
4. **Choose Save As New or Replace** — Canonical sound-only naming/review route (Sound Actions).
5. **SAVE commits New or reviews Replace** — Sound-scoped writing/capacity/error outcome (Name and Save Sound).

### Recover from full sound storage

**Availability:** Edit and save — second milestone. Keep edits while choosing a valid destination.

1. **Choose Sound actions or cancel** — Edits retained; no idea/Song deleted for sound capacity (Sound Storage Full).
2. **Choose named user replacement/deletion** — Exact Sound target reviewable (Sound Actions).
3. **SAVE executes visible New/Replace/Cancel** — Named Replace reviewed before a write (Name and Save Sound).

### Back up and restore safely

**Availability:** Play — first milestone. Preserve content across the successor firmware.

1. **Create a backup with the computer tool** — Computer retains the file (Backup / restore).
2. **Review content and confirm on computer while stopped** — Validated transfer begins (Restore incoming backup).
3. **Wait for actual transfer/check/write result** — Failure remains recoverable; no fabricated progress (Restoring).
4. **Restart after the final reply** — Restored data becomes active on startup (Restore complete · restart).

### Reset user content deliberately

**Availability:** Play — first milestone. Erase only after the preserved multi-step guard.

1. **Back up; turn K4 to Reset** — Armed, nothing erased (Factory reset).
2. **Hold SAVE for 3 seconds** — Separate final question (Hold SAVE to reset).
3. **Press SAVE again within 6 seconds** — Erase and restart; other input/timeout cancels (Reset everything?).

### Update or recover firmware

**Availability:** Play — first milestone. Use the actual compatible installer/recovery paths.

1. **Back up and choose the intended custom package** — Exact source build is known (Update / recovery).
2. **Hold the documented running-app gesture or release to cancel** — Explicit entry to update mode (Entering update mode).
3. **Complete the computer installer** — Restart to startup; no in-app HOME promise in the loader (Update mode).
4. **Verify the running build after restart** — Normal HOME or an explicit Safe/Recovery state (Startup).

### Find retained older recordings and sounds

**Availability:** Edit and save — second milestone. Keep compatibility explicit while the new instrument replaces workstation UI.

1. **Choose imported content** — Named older takes/patterns/loops/routes (Legacy content).
2. **Play or explicitly copy a take** — Fixed-note recording stays fixed and original content is preserved (Imported patterns / takes).
3. **Check optional part availability** — Any unsupported Layer content is disclosed rather than silently dropped (Layer compatibility).

## Screen reference

Every entry below is a planned screen or a named state/tab of a shared screen. Advanced parameter pages reuse the same visual and control template; retained functions can remain behind Advanced Sound while the new appearance is migrated. Every advanced page does not need to be rebuilt before the first playback milestone.

### Home

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Progressions · CORE | 1 | Boot, HOME from any musical workspace, or return from a completed action. | Browse and hear the complete catalogue with a large chord hero, paired degree/name cue and four text control columns. |
| Progressions · Next Song Queued | 1 | Turn SELECT to another song during accompaniment. | Keep the current sound truthful while a browsed song waits for the next bar. |
| Progressions · SOUND | 1 | On Progressions tap HOME until SOUND 2/4 appears, or open this bank from Live Control Banks. | Keep the chord visible while the four physical knobs expose audited SOUND controls. |
| Progressions · TEXTURE | 1 | On Progressions tap HOME until TEXTURE 3/4 appears, or open this bank from Live Control Banks. | Keep the chord visible while the four physical knobs expose audited TEXTURE controls. |
| Progressions · GROOVE | 1 | On Progressions tap HOME until GROOVE 4/4 appears, or open this bank from Live Control Banks. | Keep the chord visible while the four physical knobs expose audited GROOVE controls. |

### Accompaniment

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Groove | 1 | SEQ from another workspace. | Select premade synthesized drum patterns and the small controls that make them fit the chords. |
| No Matching Groove | 1 | The selected song/metre has no compatible premade groove. | Keep unusual metres usable without silently replacing the song's time signature. |
| Playback and Loop | 1 | SEQ, then ALGORITHM to Loop. | Choose chord-plus-drums or drums-only without introducing another transport. |
| Mix | 1 | SEQ, then ALGORITHM to Mix. | Balance chord, linked bass and drums without exposing an independent layer track. |
| Click | 1 | SEQ, ALGORITHM to Click; or SEQ from No Matching Groove. | Configure a metronome that follows the same clock and metre as the song. |
| Arrange A/B | 2 | SEQ, ALGORITHM to Arrange after an editable song exists. | Plan a short sequence of explicitly stored A/B sections, without returning to the old general song workstation. |
| Remove Arrangement Step? | 2 | Arrange A/B Action Remove, then SEL. | Remove one chain entry without deleting its A/B phrase. |

### Progression Editor

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Edit Chord | 2 | EDIT; last explicit slot focus, otherwise a unique live match, otherwise slot 1. Clone a factory seed into the protected draft first. | Change a numbered planned chord while preserving actual Now and global Harmony rules. |
| Chord Library | 2 | EDIT → ALGORITHM Library; keep the numbered destination slot and its draft. This is a chord picker, not HOME progression-catalogue filtering. | Browse named chord/degree candidates, audition deliberately and replace the explicitly focused slot without changing global Harmony. |
| Chord Timing | 2 | EDIT, ALGORITHM to Timing. | Set duration and articulation with explicit time units. |
| Chord Voicing | 2 | EDIT, ALGORITHM to Voicing. | Choose inversion/register deliberately; do not invent unavailable synthesis controls. |
| Slot Bass | 2 | EDIT, ALGORITHM to Bass. | Set a bass choice for one planned chord, separate from global bass sound and level. |
| Phrase and Metre | 2 | EDIT, ALGORITHM to Phrase. | Manage slot order and metre in a bounded phrase without losing longer musical forms. |
| Remove Chord Slot? | 2 | Phrase and Metre Remove, then SEL. | Make slot deletion deliberate and reversible. |

### Songs

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Manage Songs | 2 | EDIT on Songs. | Make selected-Song and library-wide actions direct, explicit and visibly scoped. |
| Songs | 2 | SAVE from Progressions or any musical workspace; auto-finished captures are selected here on the next open. | See, load, save and manage both saved progressions and recorded performances in one place. |
| Song Details | 2 | Manage → Details. | Explain saved Song type, provenance, timing and editable start without hiding recordings in another tree. |
| Songs · Empty View | 2 | No Saved/Pending rows match the chosen Songs view. | Offer a direct first recording/save route without clearing the current music. |
| Save New Song | 2 | SAVE on Songs when current progression is new, or Save As New. | Save a new semantic Song quickly, with optional naming and no extra free-slot review. |
| Save Current Song | 2 | SAVE on Songs for a current setup that edits an existing saved Song. | Separate new-copy saving from an intentional update of an existing saved Song. |
| Update Saved Song? | 2 | Save Current Song → Update original. | Make overwriting a saved Song an explicit final action. |
| Rename Song | 2 | Manage → Rename. | Rename the highlighted saved or Pending Song without changing its music. |
| Delete Selected Song? | 2 | Manage → Delete on a Saved row. | Delete exactly one named Saved row while preserving draft and current audible data. |
| Return to Working Draft | 2 | Songs → EDIT Manage → Return to Draft → SEL/SAVE. | Recover same-session edits after browsing another factory or saved song. |
| Keep the Existing Draft? | 2 | EDIT on another song while a dirty protected draft exists. | Resolve editing a second song without silently replacing an unsaved working draft. |
| Undo / Redo | 2 | Songs → EDIT Manage → Undo → SEL/SAVE. | Provide visible one-step recovery with the first editor. |
| Recover Draft Checkpoint? | 2 | Boot detects a valid recovery record distinct from the last committed user song. | Restore a validated stopped-state draft checkpoint after restart. |
| Songs Full · New Song Pending | 2 | No free space exists for a new Song copy. | Keep a completed capture/new save visible instead of dropping or mislabelling it when storage is full. |
| Song Not Saved Yet | 2 | Automatic or explicit Song write fails validation. | Preserve the new capture and prior valid library when a write fails. |
| Clear Saved Recordings? | 2 | Manage → Clear Recordings. | Confirm one counted bulk library action without clearing current/draft/Pending content. |
| Clear All Saved Songs? | 2 | Manage → Clear All Songs. | Confirm one counted bulk library action without clearing current/draft/Pending content. |
| Discard Pending Song? | 2 | Manage → Delete on a Pending row. | Make discarding unsaved captured content a separate named action rather than hiding it inside bulk clear. |
| Library Change Pending | 2 | A confirmed library operation cannot yet reach the safe storage boundary. | Keep a requested save/update/delete/clear truthful while flash is busy or sound must remain protected. |
| Song Pending · Kept in Songs | 2 | Non-empty REC ends; auto-copy cannot save immediately because of live sound, storage load or no free capacity. | Keep HOME visible and show a new automatic capture row honestly when it cannot yet be persisted. |
| Saving New Song | 2 | Non-empty captured performance finishes and reaches safe storage. | Acknowledge automatic new-copy persistence on HOME without a mandatory review route. |
| New Song Saved | 2 | Non-empty captured performance finishes and reaches successful read-back validation. | Acknowledge automatic new-copy persistence on HOME without a mandatory review route. |

### Recording

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Progressions · Record Armed | 2 | REC while accompaniment is running, with no unresolved Pending capture. | Record the performance heard on Progressions while HOME stays visible. |
| Progressions · Recording Count-in | 2 | REC while stopped, with no unresolved Pending capture. | Record the performance heard on Progressions while HOME stays visible. |
| Progressions · Recording | 2 | Count-in/arm reaches its capture start. | Record the performance heard on Progressions while HOME stays visible. |
| Progressions · Recording Limit Reached | 2 | Capture reaches 128 bars or 1024 musical events. | Record the performance heard on Progressions while HOME stays visible. |
| Progressions · Nothing to Save | 2 | A finished/cancelled capture has no musical note/groove content. | Record the performance heard on Progressions while HOME stays visible. |
| Progressions · Finishing Recording | 2 | REC during active capture. | Record the performance heard on Progressions while HOME stays visible. |

### Chord Lab

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Notes and Degree | 3 | SEL from HOME defaults to Now; during silence use Last. SELECT can then choose numbered slots. | Explain Now or a named planned slot with truthful note spelling, degree and scale membership. |
| Chord Intervals | 3 | SEL, ALGORITHM to Intervals. | Connect each actual note to its interval and scale membership. |
| Compare Chord Sounds | 3 | SEL, ALGORITHM to Compare. | Choose one explicit comparison without replacing the song or global modifiers. |
| Comparison Audition | 3 | SEL on an enabled Compare Chord Sounds side. | Hear an A/B choice temporarily, then resume the current live/loop context. |

### Next Chord

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| What Could Come Next? | 4 | Chord Lab, ALGORITHM to Next. | Offer three offline continuations with explicit audition and insertion. |
| Audition Next Chord | 4 | SEL on What Could Come Next?. | Hear a candidate without committing it to the draft or history. |
| Candidate Inserted | 4 | EDIT in What Could Come Next? or Audition Next Chord. | Acknowledge the exact undoable slot replacement without adding a confirmation before every creative edit. |
| Next Needs a Seven-Note Mode | 4 | Chord Lab Next when the active advanced scale is not supported by the seven-note suggestion rules. | Make unsupported advanced-scale context explicit instead of fabricating recommendations. |

### Motion

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Arpeggio and Strum | 1 | ARP from another workspace. | Retain the existing useful chord-delivery modes under one shared tempo. |
| Motion Feel and Glide | 1 | ARP, ALGORITHM to Feel. | Keep expressive controls available after Tempo replaces Glide on HOME. |

### Bass

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Bass Sound and Level | 1 | LFO from any musical workspace. | Retain the physical LFO button's musically useful Bass destination. |
| Linked Bass Behavior | 1 | LFO, ALGORITHM to Behavior. | Expose retained bass behavior separately from deep sound modulation. |

### Sound

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Envelope | 1 | ENV; retains the last explicit Chord/Bass/Linked target, default Linked. | Keep the direct envelope controls useful without exposing operator pages by default. |
| Sound Presets | 2 | GLO → Sound → Presets; SEL from Bass or Envelope provides an explicit target. | Browse useful chord/bass banks and favorites while preserving stable sound identity. |
| Sound Actions | 2 | SEL in Sound Presets. | Keep sound-only management explicit and separate from whole-song saving. |
| Name and Save Sound | 2 | GLO → Sound → Presets → SEL Actions → Save As New / Replace; or a scoped Sound save command. | Save a sound alone when deliberately requested. |
| Replace User Sound? | 2 | Name and Save Sound Action Replace user, then SEL. | Confirm sound-only overwrite separately from song overwrite. |
| Delete User Sound? | 2 | Sound Actions Delete user sound, then SEL. | Delete only a named saved user sound through a visible confirmation. |
| No Sounds in This Bank | 2 | Sound Presets selects an empty User/Favorites bank. | Keep an empty User/Favorites sound view recoverable without clearing the loaded patch. |
| Sound Storage Full | 2 | Sound save encounters no free named-record slot. | Keep the edited sound and prior valid record intact without displaying Song-capture status. |
| Sound Could Not Save | 2 | Sound save encounters write validation failed. | Keep the edited sound and prior valid record intact without displaying Song-capture status. |
| Sound | 1 | Settings → Sound | Keep sound selection and advanced editing available behind the writing workflow. |
| Advanced sound | 2 | Sound → Advanced | Group the retained engine's deep pages without taking note keys away for navigation. |
| Sound saved | 2 | Successful sound write | Distinguish successful persistence from merely edited or recorded content. |
| Legacy Layer voice | 2 | Layer compatibility → voice detail | Retain the current Layer-only Voice/Glide/Priority detail only where that part survives. |
| Saving sound | 2 | Core Name and Save Sound or Replace User Sound? executes a valid Sound-target write | Show the actual persistence operation without premature success or unsafe cancellation. |
| User sound deleted | 2 | Core Delete User Sound? executes Delete and the write succeeds | Report a successful deletion and its resolved references. |
| Advanced sound · VA | 2 | Sound → Advanced with a VA target | Group the retained engine's deep pages without taking note keys away for navigation. |
| Envelope · Legacy Layer | 1 | Layer compatibility → Layer envelope | Keep the independent Layer envelope only for imported content in a build that retains it. |

### Effects

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Filter | 1 | FX opens the last effect; SELECT chooses this effect along the chain. | Use the existing supported filter with its actual three musical controls. |
| Drive | 1 | FX opens the last effect; SELECT chooses this effect along the chain. | Use the existing supported drive with its actual three musical controls. |
| Chorus | 1 | FX opens the last effect; SELECT chooses this effect along the chain. | Use the existing supported chorus with its actual three musical controls. |
| Phaser | 1 | FX opens the last effect; SELECT chooses this effect along the chain. | Use the existing supported phaser with its actual three musical controls. |
| Delay | 1 | FX opens the last effect; SELECT chooses this effect along the chain. | Use the existing supported delay with its actual three musical controls. |
| Reverb | 1 | FX opens the last effect; SELECT chooses this effect along the chain. | Use the existing supported reverb with its actual three musical controls. |
| Effect Order | 1 | FX, ALGORITHM to Order. | Retain reorderable six-effect routing under a consistent tab-navigation rule. |
| Effect Presets | 2 | FX, ALGORITHM to Presets. | Recall a complete configuration of the existing six effects without treating it as a whole song. |
| Name Effect Preset | 2 | Effect Presets Action Save As New, then SEL. | Save the supported six-effect configuration as a named preset without implying a sound or song save. |
| FX preset Storage Full | 2 | FX preset save encounters no free named-record slot. | Keep the edited fx preset and prior valid record intact without displaying Song-capture status. |
| FX preset Could Not Save | 2 | FX preset save encounters write validation failed. | Keep the edited fx preset and prior valid record intact without displaying Song-capture status. |

### Settings

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Settings | 1 | GLO from a musical workspace | Reach controls and service functions without crowding the musical HOME. |
| Harmony | 1 | Settings → Harmony | Control live root-key chord assistance only; explicit Progression PLAY and fixed-note recordings retain their own content. |

### Controls

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Controls | 1 | Settings → Controls | Group musical performance rules and deliberate button assignments. |
| Play behavior | 1 | Controls → Play behavior | Control touch and chord movement without restoring global harmony styles. |
| Quantize and glide | 1 | Play behavior → SELECT next page | Move Glide off HOME while keeping key/scale setup understandable. |
| Modifier assignments | 1 | Controls → Modifiers | Assign an upper modifier slot without competing with Chord Lab's SEL destination. |
| Manual chord forms | 2 | Controls → Manual forms | Preserve user chord forms for Harmony Off and imported content under an advanced route. |
| Live Control Banks | 1 | Settings → Controls → Live control banks | Explain/select four real control banks without sacrificing CORE, catalogue browsing or musical keys. |

### Display

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Appearance · Theme | 1 | Settings → Display | Use Instrument Teal by default, with optional Studio/Editorial palettes and a custom solid accent in the same layout. |
| Motion | 1 | Appearance · Theme → SELECT Motion | Choose adaptive On or totally Off, with no additional motion-style, intensity or speed selector. |
| Secondary scenes | 1 | Appearance · Theme → SELECT Legacy scenes | Retain the existing scenes out of the primary writing workflow until deliberate retirement. |
| Solid Accent | 1 | Appearance · Theme → SELECT Accent → SEL | Preview one readable custom solid accent in the selected palette. |

### MIDI/USB

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| MIDI | 1 | Settings → MIDI/USB → MIDI | Preserve MIDI note routing and external synchronization without hiding it in a technical dashboard. |
| MIDI channels | 1 | MIDI → SELECT Channels | Show part routing explicitly, including compatibility-only Layer where retained. |
| Waiting for clock | 1 | MIDI Clock Receive; no pulses received | Explain armed external synchronization instead of showing a misleading editable local tempo. |
| Clock lost | 1 | Clock Receive loses incoming pulses | Stop accompaniment safely on lost external clock, keep position, and preserve independent live HOLD. |
| USB capture | 1 | MIDI → SELECT USB capture | Show the actual host-selected audio stream and routing; avoid an invented on-device rate switch. |

### Advanced FM6

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| FM6 OP 1 · Envelope Rate | 2 | Advanced sound → OP 1 → Envelope Rate | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 1 · Envelope Level | 2 | Advanced sound → OP 1 → Envelope Level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 1 · Tuning | 2 | Advanced sound → OP 1 → Tuning | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 1 · Sensitivity and level | 2 | Advanced sound → OP 1 → Sensitivity and level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 1 · Keyboard scaling | 2 | Advanced sound → OP 1 → Keyboard scaling | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 1 · Scaling curves | 2 | Advanced sound → OP 1 → Scaling curves | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 2 · Envelope Rate | 2 | Advanced sound → OP 2 → Envelope Rate | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 2 · Envelope Level | 2 | Advanced sound → OP 2 → Envelope Level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 2 · Tuning | 2 | Advanced sound → OP 2 → Tuning | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 2 · Sensitivity and level | 2 | Advanced sound → OP 2 → Sensitivity and level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 2 · Keyboard scaling | 2 | Advanced sound → OP 2 → Keyboard scaling | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 2 · Scaling curves | 2 | Advanced sound → OP 2 → Scaling curves | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 3 · Envelope Rate | 2 | Advanced sound → OP 3 → Envelope Rate | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 3 · Envelope Level | 2 | Advanced sound → OP 3 → Envelope Level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 3 · Tuning | 2 | Advanced sound → OP 3 → Tuning | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 3 · Sensitivity and level | 2 | Advanced sound → OP 3 → Sensitivity and level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 3 · Keyboard scaling | 2 | Advanced sound → OP 3 → Keyboard scaling | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 3 · Scaling curves | 2 | Advanced sound → OP 3 → Scaling curves | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 4 · Envelope Rate | 2 | Advanced sound → OP 4 → Envelope Rate | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 4 · Envelope Level | 2 | Advanced sound → OP 4 → Envelope Level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 4 · Tuning | 2 | Advanced sound → OP 4 → Tuning | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 4 · Sensitivity and level | 2 | Advanced sound → OP 4 → Sensitivity and level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 4 · Keyboard scaling | 2 | Advanced sound → OP 4 → Keyboard scaling | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 4 · Scaling curves | 2 | Advanced sound → OP 4 → Scaling curves | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 5 · Envelope Rate | 2 | Advanced sound → OP 5 → Envelope Rate | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 5 · Envelope Level | 2 | Advanced sound → OP 5 → Envelope Level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 5 · Tuning | 2 | Advanced sound → OP 5 → Tuning | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 5 · Sensitivity and level | 2 | Advanced sound → OP 5 → Sensitivity and level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 5 · Keyboard scaling | 2 | Advanced sound → OP 5 → Keyboard scaling | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 5 · Scaling curves | 2 | Advanced sound → OP 5 → Scaling curves | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 6 · Envelope Rate | 2 | Advanced sound → OP 6 → Envelope Rate | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 6 · Envelope Level | 2 | Advanced sound → OP 6 → Envelope Level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 6 · Tuning | 2 | Advanced sound → OP 6 → Tuning | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 6 · Sensitivity and level | 2 | Advanced sound → OP 6 → Sensitivity and level | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 6 · Keyboard scaling | 2 | Advanced sound → OP 6 → Keyboard scaling | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 OP 6 · Scaling curves | 2 | Advanced sound → OP 6 → Scaling curves | Retain the current FM6 operator editor on a shared four-parameter template. |
| FM6 Pitch Rate | 2 | Advanced sound → Pitch → FM6 Pitch Rate | Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony. |
| FM6 Pitch Level | 2 | Advanced sound → Pitch → FM6 Pitch Level | Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony. |
| FM6 Voice | 2 | Advanced sound → Global → FM6 Voice | Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony. |
| FM6 LFO | 2 | Advanced sound → Global → FM6 LFO | Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony. |
| FM6 LFO 2 | 2 | Advanced sound → Global → FM6 LFO 2 | Retain the current CF FM6 pitch/global page; do not confuse engine parameters with chord harmony. |

### Advanced VA

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| VA · Oscillator 1 | 2 | Advanced sound → OSC 1 → Oscillator 1 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Oscillator 1 detail | 2 | Advanced sound → OSC 1 → Oscillator 1 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Oscillator 2 | 2 | Advanced sound → OSC 2 → Oscillator 2 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Oscillator 2 detail | 2 | Advanced sound → OSC 2 → Oscillator 2 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Oscillator 3 | 2 | Advanced sound → OSC 3 → Oscillator 3 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Oscillator 3 detail | 2 | Advanced sound → OSC 3 → Oscillator 3 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Oscillator 4 | 2 | Advanced sound → OSC 4 → Oscillator 4 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Oscillator 4 detail | 2 | Advanced sound → OSC 4 → Oscillator 4 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Filter | 2 | Advanced sound → Filter → Filter | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Filter detail | 2 | Advanced sound → Filter → Filter detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 1 | 2 | Advanced sound → Envelope → Envelope 1 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 1 detail | 2 | Advanced sound → Envelope → Envelope 1 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 2 | 2 | Advanced sound → Envelope → Envelope 2 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 2 detail | 2 | Advanced sound → Envelope → Envelope 2 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 3 | 2 | Advanced sound → Envelope → Envelope 3 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 3 detail | 2 | Advanced sound → Envelope → Envelope 3 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 4 | 2 | Advanced sound → Envelope → Envelope 4 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Envelope 4 detail | 2 | Advanced sound → Envelope → Envelope 4 detail | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · LFO 1 | 2 | Advanced sound → LFO → LFO 1 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · LFO 2 | 2 | Advanced sound → LFO → LFO 2 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · LFO 3 | 2 | Advanced sound → LFO → LFO 3 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · LFO 4 | 2 | Advanced sound → LFO → LFO 4 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · LFO sync | 2 | Advanced sound → LFO → LFO sync | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Voice spread | 2 | Advanced sound → LFO → Voice spread | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 1 | 2 | Advanced sound → Mod → Modulation 1 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 2 | 2 | Advanced sound → Mod → Modulation 2 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 3 | 2 | Advanced sound → Mod → Modulation 3 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 4 | 2 | Advanced sound → Mod → Modulation 4 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 5 | 2 | Advanced sound → Mod → Modulation 5 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 6 | 2 | Advanced sound → Mod → Modulation 6 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 7 | 2 | Advanced sound → Mod → Modulation 7 | Retain the VA engine's current parameter page on the shared advanced template. |
| VA · Modulation 8 | 2 | Advanced sound → Mod → Modulation 8 | Retain the VA engine's current parameter page on the shared advanced template. |

### System

| Screen | Milestone | Open it | What it does |
| --- | --- | --- | --- |
| Tools / System | 1 | Settings → Tools/System | Keep support and destructive actions outside musical HOME. |
| Information | 1 | Tools / System → Information | Provide identity and support facts without cluttering the playing screen. |
| Diagnostics · overview | 1 | Tools / System → Diagnostics | Retain useful measurements in Tools with their limitations clearly labelled. |
| Diagnostics · timing | 1 | Tools / System → Diagnostics | Retain useful measurements in Tools with their limitations clearly labelled. |
| Diagnostics · memory | 1 | Tools / System → Diagnostics | Retain useful measurements in Tools with their limitations clearly labelled. |
| Diagnostics · USB | 1 | Tools / System → Diagnostics | Retain useful measurements in Tools with their limitations clearly labelled. |
| Backup / restore | 1 | Tools / System → Backup/Restore | Explain the connected computer route and preserve reliable backup/restore. |
| Restore incoming backup | 1 | Computer backup tool has a validated compatible restore plan | Show named incoming content and consequences before the computer tool commits changes. |
| Restoring | 1 | Validated restore transfer begins | Report an actual transfer and recoverable interruption without misleading progress. |
| Restore complete · restart | 1 | All requested compatible objects restored | Make required restart explicit; do not imply restored settings are already active. |
| Legacy content | 2 | Tools / System → Legacy content | Retain an explicit route for imported sounds/takes/patterns without dominating new writing. |
| Imported patterns / takes | 2 | Legacy content → Imported patterns/takes | Expose existing saved performances as fixed-note content and allow explicit conversion/copy. |
| Old loop conversion | 2 | Legacy content → Old loop conversion | Explain the current old-loop-to-pattern conversion without fabricating a second live looper. |
| DX7 voice / bank receive | 2 | Legacy content → DX7 receive | Retain the existing external FM6 SysEx route under Advanced/Legacy support. |
| Layer compatibility | 2 | Legacy content → Layer compatibility | Make retained or retired independent Layer behavior explicit for imported content. |
| Factory reset | 1 | Tools / System → Reset | Preserve the current multi-step destructive reset guard. |
| Hold SAVE to reset | 1 | Factory reset K4 Reset | Make the second reset step visible and cancelable. |
| Reset everything? | 1 | Three-second reset hold completed | Retain the separate final confirmation after the hold. |
| Resetting | 1 | Final reset confirmation | Show the actual erase/restart operation after explicit confirmation. |
| Update / recovery | 1 | Tools / System → Update | Provide a deliberate route to the existing installer and boot recovery. |
| Entering update mode | 1 | OCT− + OCT+ + HOME held | Expose the current hold-to-enter countdown and early cancellation. |
| Update mode | 1 | Completed update hold / installer request | Distinguish boot/update mode from a running musical application. |
| Startup | 1 | Power-on / successful update / restore/reset restart | Boot quickly into the progression instrument and make exceptional load states visible. |
| Safe Mode | 1 | Boot guard / documented recovery entry | Retain a recoverable boot without loading/saving unsafe user state. |
| Recovery help | 1 | Safe Mode / persistent flash error / interrupted update | Document real cable/boot/flash failure recovery without inventing a normal-screen fix. |
| Panel calibration | 1 | Documented OCT− + OCT+ power-on calibration route | Retain hardware control mapping as a service flow, separate from musical setup. |
| Backing up to computer | 1 | Computer tool starts LIST/GET backup | Report a real host-driven backup without implying an on-device backup file. |
| Restore not completed | 1 | Busy / unsupported object / checksum failure / cable loss / flash failure | Distinguish partial object restoration from a completed whole backup. |

## What stays predictable

- The large chord and its scale degree describe actual sounding harmony. The phrase rail describes planned slots and the real transport cursor; repeated pitch matches do not infer sequence position.
- Each planned chord pairs a scale-degree numeral above its chord/root name. Major uses uppercase, minor lowercase. A sounding-degree/quality label remains visible beside the large chord. Key transposition preserves degree; mode/quality changes update it.
- HOME K1–K4 are Rhythm, Key, Scale, Tempo. SELECT is one complete catalogue; metadata never hides ideas. Browsing supplies starting key/mode/metre without an Apply step.
- One accompaniment clock owns progression timing and premade grooves. Running groove/setup changes land at defined musical boundaries; tempo preserves phase. Active and pending values must be labelled separately.
- Live roots/HOLD have priority over temporary intentional audition, which has priority over scheduled harmony. The groove and transport cursor continue. Return to the current scheduled chord rather than restarting the phrase. The inventory specifies recording exceptions and cancellation.
- Harmony is one On/Off choice. On builds the agreed complete major/minor chords; deliberate 7 follows quality, including Emaj7 → Bmaj7 → D♯m7 in E major. Learning labels actual notes and outside-scale tones accurately.
- Harmony On/Off controls assistance for live root-key playing. Explicit Progression PLAY still plays its arranged chords when that assistance is Off, without switching the setting on. A recorded Take retains its fixed notes.
- Songs is the one user-facing collection for saved progressions and recordings. Factory progression browsing remains on HOME. Editable Songs use semantic chord/degree/duration data; recorded Songs retain captured performance events and timing. Existing recorded Auto takes keep their fixed-note meaning and format.
- REC is direct from HOME. It captures the scheduled progression plus deliberate live/groove/key/tempo changes rather than unexpectedly muting the arranged chords. Ending a capture creates a new numbered Song copy; no mandatory Review/Keep step. Pending, Saving and Saved are different states, and flash writes wait for an actually safe quiet boundary without cutting independent HOLD or audible tails.
- Songs supports selected-row Load, Rename, Duplicate and Delete, plus counted Clear Recordings/Clear All actions. Bulk clear excludes factory templates, the protected working draft and Pending captures. A Pending capture can be discarded only through its own explicit selected action.
- The selected visual reference is fm1_adaptive_motion_prototype.html. Instrument Teal is the factory default: background #061b1e, foreground #f2f8f4, secondary #89a7a5, separator #355054, subtle surface #113034, accent #77dacb, and on-accent text #092d2c. Studio and Editorial are optional palettes of the same typography-led interface. Preserve the large chord, paired degrees, thin separators, four text controls and the supplied prototype's restrained halo/Bloom visual vocabulary; earlier Coral Bloom, Aurora and Studio Rack are historical alternatives.
- Motion has exactly two user choices: On (adaptive) and Off. On responds automatically to output level/volume and semantic active-instrument activity using a smoothed bounded envelope, with musical motion tied to the shared clock. There is no manual Quiet/Expressive/intensity/speed selector. Off cancels every pulse, orbit, morph travel and tween; truthful chord, value, playback and record/save data still update instantly. The supplied loudness/layer sliders are simulation inputs, not new firmware settings or evidence of eight instrument parts. Measure display, input, memory and audio/USB timing on the FM-1.
- HOME from another screen returns to Progressions Core. Further HOME taps cycle Core/Sound/Texture/Groove without changing any parameter; SELECT still browses the complete catalogue, PRESETS still selects chord sound, ALGORITHM keeps its scene/workspace role and OCT± stays musical. Four text columns and a subtle bank indicator make each current mapping visible.

## Design and release reference

The [screen/workflow inventory](design/chord-first/design-board.html), [engineering handoff](design/chord-first/ENGINEERING-HANDOFF.md) and [GitHub roadmap](https://github.com/chasedeanda/chaseflower-fm1/issues/1) describe the same planned controls. The inventory's earlier screen sketches document coverage; the selected adaptive prototype defines appearance. Use the release-specific README/manual for behavior already shipped on the device.
