# CORD public-release audit

Reviewed 8 November 2026, starting from the published CORD 1.0 source (`712bcd8`). The fixes below are included in 1.0.1. CORD is a fork of CoralFlower; this audit preserves upstream authorship rather than relabelling their work.

## Findings and fixes

| Finding | Result |
| --- | --- |
| Downloaded backups were named `coralflower-backup-…` | Browser and Python exports now use `cord-backup-YYYYMMDD.json` for the compatible CORD/CoralFlower family. Other actual firmware families keep their own names. |
| Backup item labels said ChaseFlower or CoralFlower | Fork-owned settings, Songs, draft, recordings, appearance, Harmony, FX and pattern labels now say CORD in both clients. |
| USB manufacturer, MIDI and audio names said ChaseFlower | Now `CORD`, `CORD FM-1` and `CORD In`. USB descriptor revisions were bumped so hosts can refresh cached names. Re-select the device in a DAW after updating. VID/PID, interfaces, endpoints and audio formats are unchanged. |
| Native emulator MIDI ports and optional older boot screen had other product names | Now CORD. The browser's visible version/status line also displays CORD. |
| Player metadata still mentioned CoralFlower and referenced a missing social-preview image | Public metadata now names CORD and uses the bundled mascot playback image. |
| Installer recovery/backup sections retained upstream product wording | CORD-facing text is corrected; actual third-party firmware names and recovery links remain accurate. |
| Licensing page looked like the upstream product's own licensing page/contact | It now identifies CORD as a CoralFlower fork and distinguishes CORD issues from upstream contact information. Original licences, contributor credits and copyright notices remain. |
| Obsolete manual Pages workflow could attempt to publish the inherited upstream site | Removed. The CORD player/installer workflow is the publishing path. Its generated pages are checked for retired product labels and inherited donation/contact UI. |
| Local emulator default version came from upstream `web/releases.json` | It now uses the current source version unless explicitly overridden. |
| Release workflow marked prereleases as latest | Latest is now requested only for stable releases. |

## Deliberately retained compatibility names

These are not unresolved public branding mistakes:

- **`CoralFlower <version>` firmware INFO prefix:** existing installers and backup tools recognize this family. The public display, USB names, release title, package filename and backup download filename use CORD. Keeping INFO avoids making older external tools treat this firmware as unknown.
- **`FM-1_930` package identity, existing SysEx commands, storage IDs and record magic:** these identify compatible update/storage formats, not a marketing name. No flash layout or stored data migration is introduced.
- **`felucca-backup` JSON envelope and version:** existing archives remain readable. Firmware metadata inside a backup continues to describe the source that actually made it; it is not forged as CORD.
- **`coralflower-fm1` browser save namespace and internal AudioWorklet identifier:** changing the IndexedDB name would hide users' existing browser saves. The internal identifiers stay stable while the visible UI says CORD.
- **`cf_*`, `coralflower.c`, intermediate build filenames and protocol family keys:** retained to avoid unnecessary source/build churn. Published packages are named `cord-<version>.fwsc`.
- **Other firmware names in compatibility checks/tests, upstream references and historical releases:** these describe real projects or old artifacts. They must not be falsely renamed to CORD.

The inherited `web/index.html` remains a licensed source template for the reused installer. Its full marketing/support page is not deployed. `tools/make_cord_installer.py` packages only the CORD installer, tools and attribution, without upstream donation/contact UI.

## Checks performed

- Complete installer simulation suite passes, including package structure/checksums, backup before install, allowed/refused firmware, interrupted updates, restore and recovery. CORD is accepted as a compatibility alias; existing CoralFlower cases still pass.
- Backup round trips cover CoralFlower → CoralFlower, CoralFlower → CORD, CORD → CoralFlower and CORD → CORD. Recordings remain byte-identical and restore ordering is preserved. Browser and Python behavior is covered.
- USB protocol, descriptors, packet/resampler/drift tests and 64 real-render audio comparisons pass at both capture rates, including Auto Harmony. Exact UTF-16 product/audio strings are checked.
- `tools/audit_public_release.py` checks public labels, retained compatibility identifiers, attribution, required licence files and the pinned SHA-256 of all three bundled JieLi SDK files. CI also audits generated player/installer pages and the hosted package checksum.
- A limited signature scan of tracked text found no private-key blocks or supported GitHub/OpenAI token patterns. This is not a guarantee that every possible secret format is absent; matching values are never printed.
- The release pipeline builds the JieLi target, checks package/app checksums, carries LICENSE/LICENSING/LICENSES, and publishes from a source tag. Pages only mirrors a published stable release whose tag is on main, verifies its size/hash, and serves it from the same origin. A separate source provenance/attestation system has not been added.
- Published installer code retains package identity, loader/layout/CRC checks, WebMIDI permission on user action, backup/restore controls and explicit write actions. Opening the page does not flash the device. The compatibility override remains an explicit user choice; it was not made automatic.

## What remains a device acceptance check

This source/host audit is not a claim of independent security certification, legal clearance, or completed physical-device testing.

Before presenting a build as hardware-verified, confirm on an FM-1:

1. Green PLAY, orange stopped/waiting and red recording/count-in across accompaniment and recording screens.
2. CORD MIDI/audio enumeration and re-selection in a DAW, with no stream or timing regressions during dense chord/drum/FX playback.
3. Backup before update, install, power-cycle and restore, including an older CoralFlower archive and current recordings/FX/Songs.
4. The firmware/package version, installer selection and GitHub latest release all refer to the same build.

The first three require device access and listening. Automated tests and browser verification cannot replace them. Existing historical resource reports and old release notes are evidence for their named builds, not proof of this build's hardware acceptance.
