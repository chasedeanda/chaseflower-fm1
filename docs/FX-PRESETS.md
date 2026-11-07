# Phase 2 — FX presets

Implementation branch: `codex/phase2-fx-presets`, branched from main `89d2599` and rebased onto main `38bdd14`, including chase5 release preparation and the progression browser.
Scope: roadmap #1, storage #13, workflow #14 and initial factory concepts #15.
Status: implemented for host verification; FM1 listening, real-time timing and physical USB validation remain release gates.

## Playing and saving

The controls were reviewed with Chase on 2026-11-06: complete-chain replacement, 16 independent user slots,
FX first opens the existing editor and FX again opens the browser. Individual effects keep their original controls.
Chase subsequently prioritized ready-made sounds: the factory catalog now contains 40 presets in five groups of eight.
The browser shows the group and position (for example, `Space  9 / 40`); factory presets come before optional user slots.

| Page | Control | Result |
| --- | --- | --- |
| FX editor | SELECT / ALGORITHM | Select an effect / move it in the chain, as before |
| FX editor | K1 / K2–K4 | Enable an effect / edit its three parameters, as before |
| FX editor | FX again | Open the FX preset browser |
| Browser | SELECT | Browse 40 factory presets, followed by 16 user slots; browsing does not change the sound |
| Browser | K1 clockwise | Recall the selected complete chain; an empty user slot does nothing |
| Browser | K2 right / left | Bypass / enable the complete chain |
| Browser | K3 clockwise | Swap with the previous chain and its bypass state; available after a recall |
| Browser | K4 clockwise | Return to the individual-effect editor |
| Browser | FX again | Return HOME |
| Any FX page | SAVE | Open user-slot management |
| Management | SELECT or K1 | Select a user slot (1–16) |
| Management | K2 / K3 | Choose a name character / change it (20 characters maximum) |
| Management | K4 | Choose New, Replace or Delete |
| Management | Hold SAVE for one second | Queue the chosen operation |
| Management | Tap SAVE or FX | Cancel and return to the browser |

New requires an empty slot. Replace and Delete require an occupied user slot. Changing the target, name or action
cancels a hold already in progress. The original synth SAVE screen remains available from other pages.
Factory presets are immutable: recall one again to restore its factory configuration, or save a modified version to a user slot.
Deleting a user preset leaves the current chain playing. Previous-chain recovery is a single runtime A/B pair;
it survives individual FX edits but resets at startup. Bypass is temporary and resets at startup. Recall retains the current bypass state.

A preset contains effect order, enabled flags and all 18 parameters, including the effects' supported mix/level controls.
It does not contain synth patches, Harmony, progression, drums, ARP, tempo, recording state or MASTER volume.
The existing settings continue to remember the active chain across startup, independently of the named user bank.
The UI shows `Save pending: keep power on` while a bank edit waits for flash; queued edits are not yet durable.
The browser does not claim that a browsed name identifies the currently sounding chain.

## Factory catalog

| Group | Name | Character and processing |
| --- | --- | --- |
| Signature | Glass Cathedral | Spacious chorus, long hall and delay |
| Signature | Beautiful Noise | Warm low-pass, drive, chorus and plate |
| Signature | Broken Tape | Filtered drive, chorus and worn echoes |
| Signature | Slow Motion | Slow phaser, delay and hall |
| Signature | Dream Pop | Clear chorus, light delay and plate |
| Signature | Glitch Garden | Mid-focused band-pass, phaser and short delay |
| Signature | Tight Electronic | Mild drive, short delay and short room |
| Signature | Deep Space | Slow chorus/phaser, long delay and hall |
| Space | Moonlit Room | Intimate room ambience |
| Space | Velvet Hall | Softened highs and a warm hall |
| Space | Silver Plate | Bright plate with light chorus |
| Space | Cloud Atlas | Long hall moving through phaser and chorus |
| Space | Distant Signals | Long, repeating echoes into hall |
| Space | Frozen Lake | Slow chorus after delay, followed by a long plate |
| Space | Afterglow | Chorus and hall fed into gentle saturation |
| Space | Nebula Drift | Very slow modulation, long hall and dense echoes |
| Lo-fi | Dusty Cassette | Darkened chorus, mild drive and short echoes |
| Lo-fi | VHS Dreams | Filtered, modulated repeats with hall |
| Lo-fi | Worn Postcard | Dark drive and small-room reflections |
| Lo-fi | AM After Dark | Broad midrange filter, drive and delay |
| Lo-fi | Velvet Saturation | Warm low-pass saturation without a reverb tail |
| Lo-fi | Hazy Polaroid | Soft chorus, rolled-off highs and plate |
| Lo-fi | Melted Chrome | Chorus/phaser and plate into stronger saturation |
| Lo-fi | Old Answerphone | Telephone-like mids and short driven echoes |
| Motion | Liquid Glass | Pure chorus, with no delay or reverb |
| Motion | Neon Chorus | Faster, deeper chorus with a short plate |
| Motion | Slow Orbit | Deep, slow phaser and hall |
| Motion | Velvet Swirl | Warm chorus and phaser without echoes |
| Motion | Prism Echoes | Delay repeats passing through two modulation stages |
| Motion | Underwater Arcade | Resonant low-pass, fast phaser and delay |
| Motion | Carousel Lights | Room reflections passing through faster modulation |
| Motion | Solar Wind | Low-cut chorus/phaser, hall and long echoes |
| Punch | Dry Heat | Focused drive without ambience |
| Punch | Concrete Room | Punchy drive and short room reflections |
| Punch | Garage Slap | Mild drive and a single short echo |
| Punch | Dub Lantern | Warm, high-feedback echoes with a little room |
| Punch | Acid Corridor | Resonant filter, drive, phaser and delay |
| Punch | Shoegaze Bloom | Chorus, echoes and long hall into distortion |
| Punch | Bittersweet Drive | Stronger drive softened by low-pass and plate |
| Punch | Warehouse Exit | Drive and plate moving through phaser and delay |

All 40 use the existing six effects, with varied parameters, enabled combinations and chain placements.
Names suggest musical character: tape transport, freeze, stutter, rhythmic gating, bitcrushing, reverse and granular
processing are not implemented. Delay times and modulation rates are free-running rather than tempo synchronized.
The first eight positions are preserved; Glitch Garden's filtering and several output levels were retuned after host auditioning.
Real FM1 listening and level matching remain release gates.

## Persistence and compatibility

The active 188-byte `cf_set_t`, manual forms, Harmony record, sound stores and sequencer layout retain their formats.
The new storage object is appended after `OBJ_CFHARM`, preserving every existing object's numeric type.
The FX bank uses the CoralFlower build's unused QUAD A/B pair at **0xB2000 / 0xB3000**. Harmony remains at
0xB4000 / 0xB5000; no existing CoralFlower object shares either FX sector. This allocation applies to this fork,
which disables CZ and QUAD; it is not an allocation for a QUAD-enabled firmware.

The v1 little-endian `FXP1` bank is 752 bytes: a 12-byte header, sixteen 46-byte slots, and a 4-byte FNV-1a checksum.
The header stores magic, version, exact record size, a 16-bit occupied mask and zero reserved bytes. Each slot holds
a canonical zero-padded 21-byte printable name and the existing 25-byte `cf_fx_t`. Empty slots are zeroed.
Validation checks all ranges, a permutation of the six unique effects, supported enabled bits, terminated nonblank names,
empty-slot contents, version, size, reserved bytes and checksum. Validation occurs before mutation and restore.
A valid storage frame carrying an incompatible/corrupt FX payload becomes protected; it is not silently replaced.
Use a compatible backup or an explicit factory reset to recover such a bank. Old firmware with no bank starts with empty user slots.

The bank fits a 1024-byte logged entry (992 bytes payload); existing objects keep their original entry sizes.
It uses the existing verified A/B commit and delta log, with no flash operation in audio processing. Edits rest for
500 ms. Busy audio uses `ST_NOERASE`, with the existing 85% audio-load gate; a full log leaves the edit pending.
Erases/tidying require at least one second of silence including effect tails, and 1.5 seconds since the bank edit.
SAFE MODE and the restore lock prevent writes. Backup LIST tries to commit pending FX changes without erase and
returns a flash failure instead of exporting a stale bank if that cannot complete; let the unit become quiet and retry.

Backup object **69** carries the bank; 68 remains Harmony. Browser and Python backup tools know the new ID and name.
PUT validates the complete payload before writing; a successful restore locks automatic saves until restart.
A zero-length bank restore clears the bank. Older backups simply lack object 69. The existing reset includes both sectors.

## Audio transitions and memory

Preset recall fades the processed output to the dry input for about 10 ms, changes configuration at a block boundary
while fully dry, and returns over about 10 ms. Rapid recalls replace the pending target. One chain runs throughout;
there is no second reverb/delay allocation. Delay/reverb buffers remain alive. Reordering, time changes and reverb
size changes can still reshape tails; the fade briefly reduces their audibility. Bypass fades through the same blend
and keeps processing the chain, preserving its running state and CPU cost.
Individual parameter editing retains the existing filter/mix smoothing and behavior.

The principal added runtime storage is a 752-byte bank, a 256-byte dry scratch pair, a 25-byte pending chain and a
small UI/previous-chain state. The 40 factory records occupy 1,840 bytes of constants, plus small group labels:
1,472 bytes more preset data than the original eight, without additional DSP buffers. These are source-layout measurements;
final linked device memory and real CPU headroom must be checked before release.

## Verification

Run `sh tools/test_fx_presets.sh` after `python3 tools/build.py --gen-only` with the repository's pinned dependencies.
CI now runs the same FX suite. It covers validation/corruption, all factory configurations, UI holds and cancellation,
new/replace/delete, full-bank capacity, reload, SAFE MODE/restore lock, incompatible payload protection,
nonoverlapping sectors, deferral without erase, transitions and held-note/sequencer playback with USB capture.

Observed locally:

- Format validation: 849 checks; catalog names, effective-chain uniqueness, workflow/storage and transition checks pass.
- Extended NOR log test: 1,500 steps, 2,189 power cuts (215 interrupted again during reboot), 18,432 checks, no failures.
- The 752-byte bank allowed 177 small fixture changes without erase; actual capacity varies with edit size.
- Constant-input transition fixture: maximum sample step 8,862 for an immediate distortion-level change, 23 with the recall fade.
  This measures the fade on a controlled signal, not perceived quality across all presets.
- Physical output is byte-identical with USB capture on/off at 44.1 and 48 kHz during preset/bypass changes.
  At 44.1 kHz the captured master also matches the physical output frame by frame.
- Each of the 40 chains is exercised with bypass transitions during held-note/sequencer playback at both USB rates.
- Eighty independent level auditions: each factory on a six-note VA POLY KEYS or FM PAD chord plus bass at full MASTER,
  held for two seconds and released for one. No PCM samples clipped. Sustained RMS was -20.72..-15.22 dBFS on VA and
  -23.80..-14.39 dBFS on FM. The colored filters intentionally change energy; this is not perceptual loudness normalization.
  The fixtures reject unexpectedly quiet/loud output outside -27..-10 dBFS and saturation at the PCM boundary.

The ARM-target syntax check, fresh header generation, backup/restore (147 checks), Auto Harmony, scenes,
sound-library, diagnostics and the existing 64 USB render comparisons also pass.

Rendered screen previews and test logs are in `build/fx-tests/`. No flashing or release package is part of this change.
The suite also writes 80 three-second stereo WAVs and `levels.csv` to `build/fx-auditions/`. To rerender only these
auditions after building the test executable, run `python3 tools/audition_fx_presets.py`.

Before release on FM1:

- Save a named configuration, change it, recall it, wait for pending storage to clear, power-cycle and recall it again.
- Save/replace/delete during held or latched chords, ARP, sequencer and recording; ensure no note/transport changes or audio holes.
- Record all 40 factories and repeated transitions over six-channel USB at both rates; listen for clicks, level jumps and tail changes.
- Capture diagnostics before/after for CPU peak/history, voice shedding, audio deadlines, flash holes, USB drops and stack/memory margins.
- Audition each factory at varied input levels and dense FM/VA chords; adjust levels before approving the factory bank.
- Test backup/restore including older files without object 69, factory reset and SAFE MODE on the device.

Host emulation cannot close the physical acceptance items in #13–#15 or the release validation gate #31.
