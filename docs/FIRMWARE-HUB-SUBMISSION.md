# CORD Firmware Hub submission

Prepared 8 November 2026 against [submission requirements](https://thefirmwarehub.com/submit/) and the [detailed device checklist](https://thefirmwarehub.com/kb/#listed). This is preparation, not a claim that the Hub has accepted CORD.

## Listing details

- Firmware: **CORD**, a fork of CoralFlower.
- Author display: **Chase DeAnda**.
- Repository: https://github.com/chasedeanda/cord-fm1
- Licence: **GPL-3.0-only**, with inherited component licences documented in LICENSING.md.
- Identity: **FM-1_936**, constant across releases; **ota-FM-1_936** in update mode. Absent from the live Hub identity table on 8 November 2026; this is not a reservation. The 9xx form preserves the Hub’s documented Felucca-loader recovery convention.
- First release with independent identity and both flash chips: **1.0.2**. Earlier CORD packages used CoralFlower’s FM-1_930.
- Ports: **CORD FM-1** (MIDI), **CORD In** (audio); the inherited loader port remains **Felucca Update**.
- Card description, image, browser player and quick-start link: repository-root `fmhub.json`. The image is an actual 240 × 240 firmware-renderer capture, not a mockup.
- Installer: https://chasedeanda.github.io/cord-fm1/install/
- Releases: https://github.com/chasedeanda/cord-fm1/releases

## Checks available without the device

`python3 tools/test_hub_submission.py` validates the metadata/image and compiles the actual loader erase/program hooks with mock hardware to verify both known IDs and refusal of unknown/wrong-capacity chips. The app and loader share `fm1_flash_ids.h`; IRQ-disabled/RAM execution requirements and write ranges are preserved.

`node web/test_installer.mjs` includes CORD FM-1_936 package installation from V15, update from the older FM-1_930 identity and resume from ota-FM-1_936. Existing old-package cases remain. Backups retain the compatible INFO prefix and format; no saved-data migration is introduced.

The GitHub release build checks the target package/loader. The optional Hub-specific build attestation badge is not configured; ordinary GitHub-built releases remain eligible for listing.

## Physical acceptance — still required

Use the exact downloaded release package, with a verified backup saved outside the device. Record the release, package SHA-256, device flash ID and result for every step; do not substitute an emulator result.

- [ ] Install from official V15; CORD boots and plays.
- [ ] Install from another custom firmware using a supported path; CORD boots and plays.
- [ ] With CORD running, install another firmware normally; CORD starts that package’s loader successfully.
- [ ] USB MIDI enumerates with the documented port and responds as FM-1_936.
- [ ] Perform the Hub’s interrupted-write/reconnect/resume check on hardware with an available recovery path and external backup; record the result. This has not been attempted by the agent.
- [ ] Save, power-cycle, reload and restore projects/recordings. Test both supported chip variants when available; explicitly state which chip was tested.

CORD does not read every other firmware’s projects or samples. Switching or using other firmware can overwrite saved data. The README and installer tell players to back up first.

## Submission email draft — do not send until acceptance is recorded

To: zven@thefirmwarehub.com

Subject: CORD — FM-1 firmware submission

Hello,

I’d like to submit CORD, my CoralFlower fork, under the creator name Chase DeAnda.

Repository: https://github.com/chasedeanda/cord-fm1
Licence: GPL-3.0-only; inherited component licences are documented in LICENSING.md.
Identity: FM-1_936 (ota-FM-1_936 in update mode).
USB MIDI port: CORD FM-1; loader port: Felucca Update.

The card description, screenshot, browser player and quick-start link are in fmhub.json.

[Replace this paragraph with the exact tested release/SHA-256 and real-device results for V15 install, custom-firmware install, switch-away, USB identity and interrupted-update recovery. Hardware acceptance is currently pending. Do not claim these passed until tested.]

Thank you,
Chase
