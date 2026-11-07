/* SPDX-License-Identifier: GPL-3.0-only
 * Approved, auditioned factory recipes. Generated from previews/fx-candidates/candidates.json. */
static const fxp_preset_t FXP_FACTORY[] = {
    {"Clean", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 0, {{0, 99, 0}, {0, 0, 0}, {40, 0, 0}, {0, 50, 70}, {20, 0, 0}, {20, 0, 0}}}},
    {"Velvet", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 9, {{0, 59, 8}, {0, 12, 0}, {40, 0, 0}, {12, 24, 65}, {20, 0, 0}, {20, 0, 0}}}},
    {"Bite", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 8, {{0, 99, 0}, {0, 0, 0}, {40, 0, 0}, {57, 78, 35}, {20, 0, 0}, {20, 0, 0}}}},
    {"Slap", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 4, {{0, 99, 0}, {0, 0, 0}, {43, 8, 58}, {0, 50, 70}, {20, 0, 0}, {20, 0, 0}}}},
    {"Ping Pong", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 4, {{0, 99, 0}, {0, 0, 0}, {82, 61, 49}, {0, 50, 70}, {20, 0, 0}, {20, 0, 0}}}},
    {"Wide Drift", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 16, {{0, 99, 0}, {0, 0, 0}, {40, 0, 0}, {0, 50, 70}, {14, 88, 100}, {20, 0, 0}}}},
    {"Phase Sweep", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 32, {{0, 99, 0}, {0, 0, 0}, {40, 0, 0}, {0, 50, 70}, {20, 0, 0}, {53, 98, 100}}}},
    {"Radio", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 9, {{1, 48, 18}, {0, 0, 0}, {40, 0, 0}, {26, 68, 62}, {20, 0, 0}, {20, 0, 0}}}},
    {"Silver Plate", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 2, {{0, 99, 0}, {2, 47, 68}, {40, 0, 0}, {0, 50, 70}, {20, 0, 0}, {20, 0, 0}}}},
    {"Deep Space", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 22, {{0, 99, 0}, {1, 95, 78}, {88, 45, 23}, {0, 50, 70}, {9, 47, 45}, {20, 0, 0}}}},
    {"Orbit", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 50, {{0, 99, 0}, {2, 85, 88}, {40, 0, 0}, {0, 50, 70}, {6, 80, 70}, {40, 98, 100}}}},
    {"Phone LP", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 9, {{0, 26, 50}, {0, 0, 0}, {40, 0, 0}, {40, 0, 45}, {20, 0, 0}, {20, 0, 0}}}},
    {"Phone HP", {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, 9, {{2, 52, 45}, {0, 0, 0}, {40, 0, 0}, {100, 65, 20}, {20, 0, 0}, {20, 0, 0}}}},
};
#define FXP_FACTORY_N ((unsigned)(sizeof FXP_FACTORY / sizeof FXP_FACTORY[0]))
#define FXP_GROUP_SIZE 13u
static const char *const FXP_GROUP[] = {"Factory"};
_Static_assert(FXP_FACTORY_N == 13u, "approved factory bank");
_Static_assert(FXP_FACTORY_N + FXP_SLOTS <= 256u, "FX browser index fits one byte");
