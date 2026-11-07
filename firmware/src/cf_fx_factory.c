/* SPDX-License-Identifier: GPL-3.0-only
 * Forty factory chains, built exclusively from the six existing effects.
 * Broken Tape is filtered modulation/delay; Glitch Garden is a resonant sweep,
 * not a tape emulator, bitcrusher, gate, stutter or reverse effect. */
#define FXP_ORDER {CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}
#define FXP_WASH {CFX_FILTER, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB, CFX_DIST}
#define FXP_ECHO {CFX_FILTER, CFX_DIST, CFX_DELAY, CFX_CHORUS, CFX_PHASER, CFX_REVERB}
#define FXP_SWIRL {CFX_FILTER, CFX_DIST, CFX_REVERB, CFX_PHASER, CFX_CHORUS, CFX_DELAY}
static const fxp_preset_t FXP_FACTORY[] = {
    /* Signature: the original eight retain their positions. */
    {"Glass Cathedral", {FXP_ORDER, 0x16, {{0,90,8},{1,72,24},{65,42,18},{25,65,65},{16,30,30},{12,35,25}}}},
    {"Beautiful Noise", {FXP_ORDER, 0x1b, {{0,83,10},{2,65,22},{52,35,12},{55,42,34},{22,40,28},{20,40,20}}}},
    {"Broken Tape",     {FXP_ORDER, 0x1d, {{0,77,8},{0,25,8},{47,30,15},{18,25,55},{26,55,36},{15,30,22}}}},
    {"Slow Motion",     {FXP_ORDER, 0x26, {{0,90,8},{1,55,17},{72,44,20},{25,50,60},{12,30,25},{9,52,34}}}},
    {"Dream Pop",       {FXP_ORDER, 0x16, {{0,88,8},{2,44,14},{50,30,10},{25,50,60},{20,25,26},{20,30,20}}}},
    {"Glitch Garden",   {FXP_ORDER, 0x25, {{1,24,5},{0,24,8},{36,40,19},{25,50,60},{20,30,25},{47,48,28}}}},
    {"Tight Electronic",{FXP_ORDER, 0x0e, {{0,94,8},{0,12,5},{28,18,7},{20,60,46},{30,25,20},{25,30,20}}}},
    {"Deep Space",      {FXP_ORDER, 0x36, {{0,90,8},{1,88,24},{85,54,19},{25,50,60},{8,35,24},{6,35,24}}}},
    /* Space: small rooms through long, moving washes. */
    {"Moonlit Room",    {FXP_ORDER, 0x02, {{0,99,8},{0,34,18},{40,25,8},{20,50,60},{20,30,25},{12,35,25}}}},
    {"Velvet Hall",     {FXP_ORDER, 0x03, {{0,88,6},{1,57,25},{50,25,10},{20,50,60},{20,30,25},{12,35,25}}}},
    {"Silver Plate",    {FXP_ORDER, 0x12, {{0,99,8},{2,54,25},{50,25,10},{20,50,60},{28,20,20},{12,35,25}}}},
    {"Cloud Atlas",     {FXP_SWIRL, 0x32, {{0,99,8},{1,78,32},{60,30,12},{20,50,60},{9,42,35},{7,28,26}}}},
    {"Distant Signals", {FXP_ORDER, 0x07, {{0,85,8},{1,68,20},{96,61,28},{20,50,60},{20,30,25},{12,35,25}}}},
    {"Frozen Lake",     {FXP_ECHO, 0x16, {{0,99,8},{2,82,28},{78,52,26},{20,50,60},{3,48,42},{12,35,25}}}},
    {"Afterglow",       {FXP_WASH, 0x1a, {{0,99,8},{1,74,28},{50,25,10},{22,36,50},{12,28,30},{12,35,25}}}},
    {"Nebula Drift",    {FXP_SWIRL, 0x36, {{0,99,8},{1,94,32},{90,64,24},{20,50,60},{4,55,34},{3,58,38}}}},
    /* Lo-fi: colour and worn echoes, without unsupported tape/crusher DSP. */
    {"Dusty Cassette",  {FXP_ORDER, 0x1d, {{0,70,7},{0,25,8},{34,21,13},{16,28,64},{34,42,31},{12,35,25}}}},
    {"VHS Dreams",      {FXP_ECHO, 0x17, {{0,79,8},{1,48,16},{58,37,18},{20,50,60},{38,66,43},{12,35,25}}}},
    {"Worn Postcard",   {FXP_ORDER, 0x0b, {{0,73,5},{0,42,22},{40,25,8},{12,20,61},{20,30,25},{12,35,25}}}},
    {"AM After Dark",   {FXP_ORDER, 0x0d, {{1,34,0},{0,25,8},{48,34,20},{24,35,62},{20,30,25},{12,35,25}}}},
    {"Velvet Saturation",{FXP_ORDER,0x09,{{0,86,5},{0,25,8},{40,25,8},{32,43,38},{20,30,25},{12,35,25}}}},
    {"Hazy Polaroid",   {FXP_ORDER, 0x13, {{0,76,6},{2,58,24},{40,25,8},{20,50,60},{21,52,35},{12,35,25}}}},
    {"Melted Chrome",   {FXP_WASH, 0x3a, {{0,99,8},{2,63,24},{40,25,8},{43,35,37},{33,56,38},{17,44,32}}}},
    {"Old Answerphone", {FXP_ECHO, 0x0d, {{1,37,0},{0,25,8},{24,58,31},{36,54,57},{20,30,25},{12,35,25}}}},
    /* Motion: chorus and phaser with different placements and rates. */
    {"Liquid Glass",    {FXP_ORDER, 0x10, {{0,99,8},{0,25,8},{40,25,8},{20,50,60},{19,39,46},{12,35,25}}}},
    {"Neon Chorus",     {FXP_ORDER, 0x12, {{0,99,8},{2,30,12},{40,25,8},{20,50,60},{46,62,56},{12,35,25}}}},
    {"Slow Orbit",      {FXP_ORDER, 0x22, {{0,99,8},{1,56,18},{40,25,8},{20,50,60},{20,30,25},{2,73,48}}}},
    {"Velvet Swirl",    {FXP_ORDER, 0x31, {{0,86,8},{0,25,8},{40,25,8},{20,50,60},{14,36,25},{21,57,44}}}},
    {"Prism Echoes",    {FXP_ECHO, 0x34, {{0,99,8},{0,25,8},{66,48,27},{20,50,60},{24,38,36},{34,42,27}}}},
    {"Underwater Arcade",{FXP_ORDER,0x25,{{0,64,27},{0,25,8},{42,38,20},{20,50,60},{20,30,25},{63,64,45}}}},
    {"Carousel Lights", {FXP_SWIRL, 0x32, {{0,99,8},{0,38,15},{40,25,8},{20,50,60},{57,45,38},{48,45,34}}}},
    {"Solar Wind",      {FXP_SWIRL, 0x37, {{2,2,5},{1,64,22},{81,46,22},{20,50,60},{9,57,37},{11,68,43}}}},
    /* Punch: focused rhythm, tight echoes, drive and shoegaze contrast. */
    {"Dry Heat",        {FXP_ORDER, 0x08, {{0,99,8},{0,25,8},{40,25,8},{22,70,43},{20,30,25},{12,35,25}}}},
    {"Concrete Room",   {FXP_ORDER, 0x0a, {{0,99,8},{0,18,15},{40,25,8},{28,62,45},{20,30,25},{12,35,25}}}},
    {"Garage Slap",     {FXP_ORDER, 0x0c, {{0,99,8},{0,25,8},{39,9,23},{18,57,73},{20,30,25},{12,35,25}}}},
    {"Dub Lantern",     {FXP_ORDER, 0x07, {{0,78,9},{0,33,13},{72,72,32},{20,50,60},{20,30,25},{12,35,25}}}},
    {"Acid Corridor",   {FXP_ORDER, 0x2d, {{0,72,35},{0,25,8},{49,42,20},{39,65,39},{20,30,25},{32,52,34}}}},
    {"Shoegaze Bloom",  {FXP_WASH, 0x1e, {{0,99,8},{1,82,31},{69,46,19},{48,38,33},{17,47,34},{12,35,25}}}},
    {"Bittersweet Drive",{FXP_ORDER,0x0b,{{0,90,8},{2,38,15},{40,25,8},{46,52,35},{20,30,25},{12,35,25}}}},
    {"Warehouse Exit",  {FXP_SWIRL, 0x2e, {{0,99,8},{2,39,19},{60,53,24},{32,61,48},{20,30,25},{27,54,36}}}},
};
#define FXP_FACTORY_N ((unsigned)(sizeof FXP_FACTORY / sizeof FXP_FACTORY[0]))
#define FXP_GROUP_SIZE 8u
static const char *const FXP_GROUP[] = {"Signature", "Space", "Lo-fi", "Motion", "Punch"};
_Static_assert(FXP_FACTORY_N == FXP_GROUP_SIZE * sizeof FXP_GROUP / sizeof FXP_GROUP[0], "FX groups cover the catalog");
_Static_assert(FXP_FACTORY_N + FXP_SLOTS <= 256u, "FX browser index fits one byte");
#undef FXP_ORDER
#undef FXP_WASH
#undef FXP_ECHO
#undef FXP_SWIRL
