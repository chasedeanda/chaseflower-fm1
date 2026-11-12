/* SPDX-License-Identifier: GPL-3.0-only
 * Append-only Studio VA voices. Oscillator levels are set for a four-part mix. */
STUDIO_VOICE(SOFT_LENS, "SOFT LENS", 0, TRI, 58, PWM, 36, 0, 8, 65, 12, 8, 0, 77, 90, 112, 98, 12, 40)
STUDIO_VOICE(SILK_PICK, "SILK PICK", 1, TRI, 77, SIN, 26, 12, 0, 50, 13, 24, 4, 1, 80, 60, 41, 0, 0)
STUDIO_VOICE(ROOM_KEYS, "ROOM KEYS", 0, PWM, 52, TRI, 46, 0, 3, 73, 8, 30, 3, 2, 83, 72, 58, 6, 13)
STUDIO_VOICE(FELT_PICK, "FELT PICK", 1, TRI, 82, SAW, 18, 12, 0, 61, 8, 36, 7, 0, 78, 51, 39, 0, 0)
STUDIO_VOICE(NEON_POLY, "NEON POLY", 0, SAW, 44, PWM, 42, 0, 9, 77, 24, 31, 4, 1, 80, 96, 60, 8, 22)
STUDIO_VOICE(RELAY_BASS, "RELAY BASS", 1, PWM, 74, SIN, 48, 0, 0, 55, 24, 42, 12, 0, 75, 76, 30, 0, 0)
STUDIO_VOICE(AMP_EDGE, "AMP EDGE", 0, SAW, 54, TRI, 27, 12, -4, 66, 6, 25, 21, 0, 74, 95, 37, 2, 6)
STUDIO_VOICE(DOWN_PICK, "DOWN PICK", 1, SAW, 53, TRI, 58, 0, 0, 62, 8, 38, 18, 0, 72, 91, 32, 0, 0)
STUDIO_VOICE(DUSTED_KEYS, "DUSTED KEYS", 0, TRI, 56, SIN, 25, 12, -5, 58, 11, 25, 8, 3, 85, 30, 73, 8, 18)
STUDIO_VOICE(TAPE_FINGER, "TAPE FINGER", 1, SIN, 80, TRI, 25, 12, 0, 42, 6, 19, 9, 2, 84, 75, 51, 0, 0)
STUDIO_VOICE(SLOW_ORBIT, "SLOW ORBIT", 0, PWM, 49, SIN, 49, 0, 7, 55, 10, 8, 0, 99, 94, 122, 112, 18, 47)
STUDIO_VOICE(LOW_TIDE, "LOW TIDE", 1, SIN, 67, TRI, 28, 12, 0, 43, 5, 4, 0, 70, 92, 117, 78, 0, 0)
/* Voiced as a set with the Room kit. */
STUDIO_VOICE(CABINET_PAD, "CABINET PAD", 0, TRI, 72, SAW, 22, 0, 4, 57, 8, 10, 2, 64, 91, 115, 99, 10, 37)
STUDIO_VOICE(ROUNDWIRE, "ROUNDWIRE", 1, TRI, 86, PWM, 23, 12, 0, 51, 11, 25, 8, 1, 77, 79, 44, 0, 0)
STUDIO_VOICE(GLASS_THREAD, "GLASS THREAD", 0, SIN, 69, TRI, 23, 19, -4, 76, 17, 12, 0, 6, 91, 23, 91, 9, 31)
STUDIO_VOICE(MUTED_WIRE, "MUTED WIRE", 1, TRI, 79, SAW, 27, 12, 0, 59, 8, 31, 7, 0, 73, 52, 37, 0, 0)

/* Indie. A small electric organ and a pick transient paired with Glass Thread. */
STUDIO_RECIPE(JANGLE_ORGAN, "JANGLE ORGAN", 0, 75, 7, 0, 4, 5, 81, 108, 61,
    OSC(0, SIN, 63, 0, 0), OSC(1, SIN, 30, 12, 0), OSC(2, TRI, 17, 19, 0), VA_VEL, 74,
    LFO(0, 62, 0, 98, 0), M_(0, VS_LFO1, VD_PITCH, 2), M_(1, VS_LFO1, VD_AMP, 10))
STUDIO_RECIPE(PICKUP_BASS, "PICKUP BASS", 1, 62, 11, 12, 9, 0, 82, 84, 44,
    OSC(0, TRI, 88, 0, 0), OSC(1, PWM, 11, 12, 0), O_(1, SHAPE), 58, ENV2(0,56,0,34), VA_VEL, 67,
    M_(0, VS_ENV1 + 1, VD_LVL1 + 1, 42), M_(1, VS_VEL, VD_FENV, 12))

/* New Wave. Pulse-width motion, a clocklike sync attack and a glassy upper voice. */
STUDIO_RECIPE(PULSE_STACK, "PULSE STACK", 0, 73, 13, 18, 3, 4, 82, 104, 61,
    OSC(0, PWM, 48, 0, -4), O_(0, SHAPE), 62, OSC(1, PWM, 40, 0, 4), O_(1, SHAPE), 38,
    ENV2(0,76,0,51), LFO(0, 43, 0, 127, 0), VA_VEL, 76,
    M_(0, VS_LFO1, VD_SHP1, 30), M_(1, VS_LFO1, VD_SHP1 + 1, -25))
STUDIO_RECIPE(SYNC_RELAY, "SYNC RELAY", 1, 57, 19, 18, 12, 0, 78, 84, 34,
    OSC(0, TRI, 74, 0, 0), OSC(1, SAW, 44, 12, 0), VA_SYNC2, 1,
    ENV2(0,64,0,33), VA_VEL, 65, M_(0, VS_ENV1 + 1, VD_PIT1 + 1, 22))
STUDIO_RECIPE(CRYSTAL_STEP, "CRYSTAL STEP", 0, 83, 13, 4, 0, 3, 90, 28, 75,
    OSC(0, SIN, 54, 0, 0), OSC(2, SIN, 18, 12, 0), OSC(3, SIN, 12, 19, 3), VA_RING4, 1,
    ENV2(0,65,0,54), VA_VEL, 85, M_(0, VS_ENV1 + 1, VD_LVL1 + 3, 37))

/* Pop Punk. A driven midrange rhythm voice, a firm low pick and a short lead. */
STUDIO_RECIPE(AMP_STACK, "AMP STACK", 0, 66, 10, 10, 19, 1, 73, 104, 49,
    OSC(0, SAW, 40, 0, -3), OSC(1, SQR, 31, 0, 3), O_(1, SHAPE), 16, OSC(2, TRI, 21, 12, 0),
    ENV2(0,55,0,33), VA_VEL, 70, M_(0, VS_ENV1 + 1, VD_LVL1 + 2, 32))
STUDIO_RECIPE(POWER_PICK, "POWER PICK", 1, 62, 8, 12, 14, 0, 75, 98, 31,
    OSC(0, TRI, 93, 0, 0), OSC(1, SAW, 16, 12, 0), ENV2(0,50,0,31), VA_VEL, 64,
    M_(0, VS_ENV1 + 1, VD_LVL1 + 1, 35), M_(1, VS_ENV1 + 1, VD_DRIVE, 12))
STUDIO_RECIPE(WIRE_LEAD, "WIRE LEAD", 0, 80, 11, 12, 7, 3, 80, 59, 58,
    OSC(0, TRI, 61, 0, 0), OSC(1, PWM, 26, 0, -3), O_(1, SHAPE), 77, ENV2(0,65,0,39),
    LFO(0, 77, 0, 110, 73), VA_VEL, 85, M_(0, VS_LFO1, VD_PITCH, 4))

/* Lo-Fi. The electric-piano overtones decay faster than the body. The bass
 * and tines have their own slow pitch motion so the band does not wobble in unison. */
STUDIO_RECIPE(TAPE_EP, "TAPE EP", 0, 67, 8, 7, 4, 7, 99, 54, 77,
    OSC(0, SIN, 73, 0, 0), OSC(1, TRI, 18, 0, 0), OSC(2, SIN, 5, 24, 0), ENV2(0,69,0,51),
    LFO(0, 30, 0, 112, 0), LFO(1, 64, 0, 76, 0), VA_VEL, 88,
    M_(0, VS_ENV1 + 1, VD_LVL1 + 2, 45), M_(1, VS_LFO1, VD_PITCH, 4), M_(2, VS_LFO1 + 1, VD_AMP, 9))
STUDIO_RECIPE(FELT_SUB, "FELT SUB", 1, 51, 6, 5, 10, 3, 90, 86, 55,
    OSC(0, SIN, 95, 0, 0), OSC(1, TRI, 29, 0, 0), ENV2(0,75,0,47), VA_VEL, 59,
    LFO(0, 21, 0, 86, 0), M_(0, VS_LFO1, VD_PITCH, 3), M_(1, VS_ENV1 + 1, VD_LVL1 + 1, 19))
STUDIO_RECIPE(DUST_TINES, "DUST TINES", 0, 75, 9, 2, 0, 10, 96, 16, 87,
    OSC(0, SIN, 57, 0, 0), OSC(1, SIN, 9, 19, 8), OSC(2, SIN, 8, 24, -10), ENV2(0,74,0,58),
    LFO(0, 35, 0, 96, 0), VA_VEL, 83,
    M_(0, VS_ENV1 + 1, VD_LVL1 + 1, 28), M_(1, VS_LFO1, VD_PIT1 + 2, 7))

/* Ambient Soundscape. Different time scales for the chord shimmer, low
 * foundation and the upper swell. Keep the bass center stable. */
STUDIO_RECIPE(PRISM_CLOUD, "PRISM CLOUD", 0, 68, 10, 5, 0, 95, 99, 112, 106,
    OSC(0, TRI, 46, 0, -4), MODE(0, MORPH), O_(0, SHAPE), 28, OSC(1, PWM, 31, 0, 4), O_(1, SHAPE), 65,
    OSC(2, SIN, 20, 12, 0), ENV2(103,102,55,104), LFO(0, 13, 0, 127, 0), LFO(1, 29, 0, 127, 0), VA_VEL, 56,
    M_(0, VS_LFO1, VD_SHP1, 22), M_(1, VS_LFO1 + 1, VD_SHP1 + 1, 31), M_(2, VS_ENV1 + 1, VD_LVL1 + 2, 24))
STUDIO_RECIPE(FLOOR_SWELL, "FLOOR SWELL", 1, 46, 8, 0, 2, 76, 96, 114, 82,
    OSC(0, SIN, 86, 0, 0), OSC(1, TRI, 26, 12, 0), ENV2(98,102,63,88), VA_VEL, 53,
    LFO(0, 15, 0, 110, 0), M_(0, VS_ENV1 + 1, VD_LVL1 + 1, 26), M_(1, VS_LFO1, VD_CUT, 7))
STUDIO_RECIPE(HALO_BLOOM, "HALO BLOOM", 0, 82, 10, 4, 0, 69, 105, 57, 104,
    OSC(0, SIN, 50, 0, 0), OSC(1, SIN, 22, 12, -5), OSC(2, SIN, 11, 19, 4), ENV2(94,104,55,101),
    LFO(0, 19, 0, 127, 0), VA_VEL, 64, M_(0, VS_ENV1 + 1, VD_LVL1 + 2, 34), M_(1, VS_LFO1, VD_PIT1 + 1, 5))

/* Pop Punk follow-up. Fast, dark drive stages and softer upper feedback leave
 * room around the drum accents. The earlier tones keep their factory IDs. */
STUDIO_RECIPE(BARBED_AMP, "BARBED AMP", 0, 64, 9, 14, 49, 0, 72, 103, 41,
    OSC(0, SAW, 23, 0, -3), OSC(1, PWM, 18, 0, 4), O_(1, SHAPE), 43, OSC(2, TRI, 13, 12, 0),
    ENV2(0,58,0,35), VA_VEL, 69, M_(0, VS_ENV1 + 1, VD_LVL1 + 2, 16), M_(1, VS_VEL, VD_DRIVE, 13))
STUDIO_RECIPE(RUST_PICK, "RUST PICK", 1, 60, 10, 11, 30, 0, 71, 94, 28,
    OSC(0, TRI, 79, 0, 0), OSC(1, SAW, 16, 12, 0), ENV2(0,54,0,28), VA_VEL, 67,
    M_(0, VS_ENV1 + 1, VD_LVL1 + 1, 45), M_(1, VS_ENV1 + 1, VD_CUT, 8))
STUDIO_RECIPE(FRET_HOWL, "FRET HOWL", 0, 75, 22, 2, 17, 21, 88, 81, 75,
    OSC(0, TRI, 52, 0, 0), OSC(1, PWM, 18, 12, 4), O_(1, SHAPE), 72, ENV2(72,94,67,72),
    LFO(0, 89, 0, 99, 75), VA_VEL, 76, M_(0, VS_ENV1 + 1, VD_LVL1 + 1, 38), M_(1, VS_LFO1, VD_PITCH, 5))
