/* SPDX-License-Identifier: GPL-3.0-only
 * Initial factory concepts, built exclusively from the six existing effects.
 * Broken Tape is filtered modulation/delay; Glitch Garden is a resonant sweep,
 * not a tape emulator, bitcrusher, gate, stutter or reverse effect. */
#define FXP_FACTORY_N 8u
#define FXP_ORDER {CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}
static const fxp_preset_t FXP_FACTORY[FXP_FACTORY_N] = {
    {"Glass Cathedral", {FXP_ORDER, 0x16, {{0,90,8},{1,72,24},{65,42,18},{25,65,65},{16,30,30},{12,35,25}}}},
    {"Beautiful Noise", {FXP_ORDER, 0x1b, {{0,83,10},{2,65,22},{52,35,12},{55,42,34},{22,40,28},{20,40,20}}}},
    {"Broken Tape",     {FXP_ORDER, 0x1d, {{0,77,8},{0,25,8},{47,30,15},{18,25,55},{26,55,36},{15,30,22}}}},
    {"Slow Motion",     {FXP_ORDER, 0x26, {{0,90,8},{1,55,17},{72,44,20},{25,50,60},{12,30,25},{9,52,34}}}},
    {"Dream Pop",       {FXP_ORDER, 0x16, {{0,88,8},{2,44,14},{50,30,10},{25,50,60},{20,25,26},{20,30,20}}}},
    {"Glitch Garden",   {FXP_ORDER, 0x25, {{1,63,22},{0,24,8},{36,40,19},{25,50,60},{20,30,25},{47,48,28}}}},
    {"Tight Electronic",{FXP_ORDER, 0x0e, {{0,94,8},{0,12,5},{28,18,7},{20,60,60},{30,25,20},{25,30,20}}}},
    {"Deep Space",      {FXP_ORDER, 0x36, {{0,90,8},{1,88,24},{85,54,19},{25,50,60},{8,35,24},{6,35,24}}}},
};
#undef FXP_ORDER
