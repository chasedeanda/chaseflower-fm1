/* SPDX-License-Identifier: GPL-3.0-only; generated G1 palettes, not authored songs. */
typedef struct {
    const char *name,*chord[3],*bass[3],*fx[3];
    uint8_t bpm,groove[4],weight[8],gain[4][3],defaults[4];
} cge_vibe_t;
static const cge_vibe_t CGE_VIBES[2]={
    {"Haze",{"GHOST CHOIR","BREATH PAD","TAPE STRINGS"},{"TAPE SUB","WARM TRIANGL","DEEP SINE"},{"Wide Drift","Deep Space","Orbit"},104,{56,57,55,58},{2,3,5,0,0,0,0,1},{{82,20,25},{90,60,62},{95,78,78},{84,43,45}},{52,35,55,35}},
    {"Drive",{"TAPE STRINGS","GENTLE SYNC","RAZOR PULSE"},{"CLEAN MONO","DEEP SINE","TAPE SUB"},{"Orbit","Bite","Phase Sweep"},122,{60,59,4,1},{3,0,0,0,4,1,5,0},{{78,60,64},{88,78,82},{94,90,95},{82,64,68}},{66,60,25,45}},
};
/* Stable catalog ID plus independent role indices; a bounded audio mailbox. */
typedef struct {
    uint16_t id;
    uint8_t vibe,groove,chord,bass,fx,feel;
} cge_take_t;
