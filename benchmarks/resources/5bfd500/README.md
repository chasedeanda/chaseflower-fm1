# Exploration build baseline: 5bfd500

Captured with `tools/audit_resources.py --build` from source revision
`5bfd5005901b002a19cc3b955b774d1395b1460b`, using the default device build.
The working tree contained the audit tooling/build-flag extraction, recorded in
`identity.json`; firmware sources and formats were unchanged. Repeated device
builds produced the same ELF SHA256.

- `report.md`: region and selected-allocation tables.
- `report.json`: complete sized symbol inventory, dependencies, effective build
  macros, source/compiler identity, allocation groups and limitations.
- `identity.json`: build/ELF hashes, flags, compiler, Python dependencies, linker
  regions and source-input hashes.
- `dependencies.d`: real pi32 compiler's preprocessor dependency closure.
- `macros.txt`: real pi32 compiler's effective preprocessor definitions.

The binary/package/build log stay in the ignored local build directory. This is
static device-link evidence, not an FM-1 runtime measurement. No focused/scene-free
build or net-savings claim is included. See `docs/CHORD-FIRST-AUDIT.md` for the
feature matrix, migration gates and next paired experiment.
