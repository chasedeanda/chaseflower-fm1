# Chord and drum voice allocation development fix

A host reproduction found a deterministic fixed-budget cut, independently of any measured CPU
load. A six-note VA chord used 12 of 16 budget units, leaving room for only two drum voices
at two units each. Additional hits caused the allocator to fade existing drum voices. A six-note
FM chord allowed five drum voices before the same limit. Drum choke/ownership rules are separate.

The development policy splits the fixed percussion and melodic allocation pools. It retains the
existing per-engine polyphony caps and memory allocations. The voice-victim search cannot choose
a drum for a new chord voice or a melodic voice for a drum hit. Drum lanes still reuse/choke
according to their existing rules. The policy is enabled only in the CoralFlower device/emulator;
other inherited profiles retain their original budget. CF_PERCUSSION_BUDGET=0 restores the old
policy for comparisons.

The VA load-dependent cap still counts actual drum rendering time, and the real audio overload
guard still fades voices when the device exceeds its deadline. This is not additional CPU capacity
or a guarantee of full polyphony under every patch/FX/USB workload. Chord thinning under actual
CPU pressure may remain and needs the specific sound/groove/kit and hardware diagnostics.

The reproduction with the new policy retains six VA chord voices alongside up to eight drum
voices with no fixed-budget kills. The regression covers both FM and VA, cross-pool victim
selection, and that the CPU shed function can still reduce work. Host timing does not establish
hardware audio headroom. Run sh tools/test_chord_drum_budget.sh after generated headers exist.
