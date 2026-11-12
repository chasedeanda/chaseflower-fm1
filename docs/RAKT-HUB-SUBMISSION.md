# RAKT Firmware Hub submission

## Listing details

| Field | Value |
| --- | --- |
| Firmware | RAKT |
| Creator | Chase DeAnda |
| Proposed repository | `chasedeanda/rakt-fm1` |
| Licence | GPL-3.0-only, with inherited component licences in [LICENSING.md](../LICENSING.md) |
| Running identity | `FM-1_57258001` |
| Update loader identity | `ota-FM-1_57258001` |
| USB ports | RAKT FM-1 (MIDI), RAKT In (audio), Felucca Update (loader) |
| Listing copy and image | [fmhub.json](../fmhub.json) |

The Hub's live checker returned **Free** for `FM-1_57258001` on November 12, 2026.
Recheck the number before release. CORD continues to use `FM-1_26730001` in its own
repository. RAKT keeps the inherited backup object format and on-device flash layout.
RAKT's own installer recognizes CORD backups and compatible firmware. Back up before
switching between firmwares because they share the FM-1's physical flash.

The app and loader support both FM-1 flash chips, Puya P25Q80 (`0x856014`) and
UC25HQ80 (`0xB36014`). The loader rejects flash IDs outside that list.

## Before sending the form

1. Publish the RAKT source and root `fmhub.json` on the default branch of its own
   public repository. Publish the RAKT branch as its default branch and tag RAKT
   releases there. Add the optional browser player and quick-start links to
   `fmhub.json` when their destinations are live.
2. Build a candidate with the release workflow and record its SHA-256. Use that exact
   package for the hardware checks, then attach it and its checksum to the tagged
   GitHub release.
3. Run the physical acceptance checks below, then enter the repository, RAKT, Chase
   DeAnda and the hardware results in the Hub's submission form.
   [Firmware Hub submission](https://thefirmwarehub.com/submit/)

## Physical acceptance

- [ ] Install from stock V15 on a real FM-1 and confirm RAKT boots and plays.
- [ ] Install from CORD or another supported custom firmware and confirm RAKT boots and plays.
- [ ] From running RAKT, install another firmware and confirm it boots.
- [ ] Verify the RAKT FM-1 MIDI port and both firmware and update-loader identities.
- [ ] Verify interrupted-write resume with a recovery path and an external backup.
- [ ] Save, power-cycle and reload a project. Record the flash chip and package SHA-256 with the results.

`python3 tools/test_hub_submission.py` checks the root metadata, image, identity and
loader chip gates. `node web/test_installer.mjs` exercises stock/custom installs
and resume using the simulated device. The Hub requires the physical results above.
[Firmware Hub device checklist](https://thefirmwarehub.com/kb/#listed)
