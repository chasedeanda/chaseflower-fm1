# Chase's FM-1 backups

`coralflower-backup-20261106.json` is the original backup Chase supplied on November 6, 2026, before confirming the
first ChaseFlower build. It is stored byte for byte as supplied; the copy in Downloads was not changed.

Validation: the existing `web/fm1backup.js` `readBackup` validator accepted all 26 objects and verified their lengths
and CRCs. `SHA256SUMS` records the file's SHA-256. From the repository root, check it with:

```sh
shasum -a 256 -c backups/SHA256SUMS
```

Keep a separate local copy too. This backup is data for the installer's restore function, not a `.fwsc` firmware
package; do not select it in the firmware file picker. No restore was performed by the agent.
