/* SPDX-License-Identifier: GPL-3.0-only
 * Additional choices for each Studio role. Names refer to existing factory
 * presets across both engines. Profile tones lead each row.
 */
static const char *const *const CGS_PALETTE_EXTRA[CGS_NGENRE][CGS_DRUMS]={
    [CGS_DREAM]={
        (const char *const[]){"SOFT HORIZON","LONG BLOOM","DUSK STRINGS","QUIET STRING","SPARKLE PAD",
            "EMBER KEYS","NOIR TINES","ICE PAD","TAPE STRINGS","GENTLE SWELL","SOFT AAH","WARM FOG","LUSH PAD",0},
        (const char *const[]){"DEEP SINE","WARM DRIFT","ROUND FINGER","SOFT FLOOR","CLEAR SUB","MELLOW REESE","ROUND THUMB",0},
        (const char *const[]){"GLASS RAIN","MUTED BELL","CEDAR MALLET","SOFT KALIMBA","TAPE BLOOM",
            "SOFT PRISM","AQ34 FULLFLU","GLOW PLUCK","WATER CHIME","DREAM BELLS","HIGH BLOOM","SLOW CHIMES","SOFT LEAD",0},
    },
    [CGS_INDIE]={
        (const char *const[]){"TINE EP","CLAVINET","DRAWBARS","CEDAR MALLET","LOUNGE ORGAN","DX TINE",
            "PIANO","EMBER KEYS","WORN TINES","ORGANISH","VELVET REED","POLY KEYS","WOODEN KEYS",0},
        (const char *const[]){"SOLID BASS","ROUND FINGER","ROUND THUMB","WOOD PLUCK","SOFT PUNCH","PLUCKED WOOD","WOOD BASS",0},
        (const char *const[]){"PLUCK","KALIMBA","DX MARIMBA","STEEL DRUM","MUTED MARIMB","HARP",
            "AQ33 DX SAX","NOIR TINES","DULL PLUCK","AIR FLUTE","WIRE LEAD","GLOW PLUCK","LATE BELL",0},
    },
    [CGS_WAVE]={
        (const char *const[]){"POLY KEYS","ROUND POLY","BRASS SECT","DARK STAB","OFFBEAT DUB","GLASS RAIN",
            "WAVE PAD","RAZOR PULSE","SYNTH BRASS","PWM PAD","SOFT POLY","NEON HAZE","WIDE STRINGS",0},
        (const char *const[]){"SOLID BASS","SAW BASS","SYNC BASS","CLEAN MONO","PUNCH BASS","RUBBER BASS","GLASS PULSE","FAT BASS",0},
        (const char *const[]){"GENTLE SYNC","CLAV","BELL","DX TINE","PLUCK","SYNC LEAD",
            "SPARKLE PAD","BRASS","GLOW PLUCK","SOFT LEAD","HOLLOW","SHIMMER","DIM CHIME",0},
    },
    [CGS_PUNK]={
        (const char *const[]){"DARK STAB","STRINGS","ROUND POLY","BRASS SECT","CLAVINET",
            "WAVE PAD","JANGLE ORGAN","PWM KEYS","SYNTH BRASS","RAZOR PULSE","NEON POLY","CABINET PAD","ORGANISH",0},
        (const char *const[]){"SOLID BASS","SAW BASS","PICKUP BASS","FELT PICK","PUNCH BASS","SYNC BASS","FAT BASS","MELLOW REESE",0},
        (const char *const[]){"PLUCK","SYNC LEAD","DX TINE","STAR KEYS","TINE EP","BRASS",
            "GLASS THREAD","GLOW PLUCK","NEON HAZE","SOFT LEAD","CRYSTAL STEP","HOLLOW","PWM KEYS",0},
    },
    [CGS_LOFI]={
        (const char *const[]){"DUSTY EP","FELT KEYS","TINE EP","DX TINE","NOIR TINES","WORN TINES","LOUNGE ORGAN",
            "MUTED SKANK","EMBER KEYS","NIGHT WINDOW","VELVET REED","SOFT POLY","WARM FOG",0},
        (const char *const[]){"ROUND FINGER","TAPE SUB","ROUNDED SUB","MUTED FINGER","WARM MONO","LOW MARIMBA","ROUND THUMB","DUB SUB",0},
        (const char *const[]){"MUTED BELL","AQ10 DX FLUT","SOFT KALIMBA","MARIMBA","CEDAR MALLET","AQ04 DX CRAC",
            "TAPE BLOOM","KALIMBA","GENTLE BOWL","DULL PLUCK","LATE BELL","GENTLE MALLE","WOODEN KEYS",0},
    },
    [CGS_AMBIENT]={
        (const char *const[]){"TAPE BLOOM","LONG BLOOM","DUSK STRINGS","SOFT HORIZON","ICE PAD","WAVE PAD",
            "VOX PAD","AQ15 DX LONG","CLOUD PAD","SLEEP DRONE","HIGH BLOOM","GENTLE SWELL","RESONANT PAD",0},
        (const char *const[]){"DEEP SINE","SOFT FLOOR","LOW MARIMBA","BELL BASS","DUB SUB","WARM DRIFT","CLEAR SUB","DEEP SUB",0},
        (const char *const[]){"UNEVEN BELLS","GLASS RAIN","SOFT KALIMBA","SILVER ORGAN","AQ34 FULLFLU","HARP",
            "TUBULAR","CEDAR MALLET","AIR FLUTE","SLOW CHIMES","GENTLE BOWL","BREATH PAD","SHIMMER",0},
    },
};

/* Chords, Bass, Layer. The expanded palette leads with CORD's own patches. */
static const uint8_t CGS_ORIGINAL_VOICE[CGS_NGENRE][CGS_DRUMS]={
    [CGS_DREAM]={STUDIO_VA_GLIDE_HAZE,STUDIO_VA_VELVET_PICK,STUDIO_VA_SLOW_SPARK},
    [CGS_INDIE]={STUDIO_VA_TWIN_CHOPS,STUDIO_VA_PALM_WIRE,STUDIO_VA_BRIDGE_CHIME},
    [CGS_WAVE]={STUDIO_VA_AFTERIMAGE,STUDIO_VA_MONO_SHIFT,STUDIO_VA_CHROME_ARP},
    [CGS_PUNK]={STUDIO_VA_STACK_BITE,STUDIO_VA_BREAK_PICK,STUDIO_VA_OCTAVE_CRY},
    [CGS_LOFI]={STUDIO_VA_WORN_REED,STUDIO_VA_DUST_ROUND,STUDIO_VA_GRAIN_KEYS},
    [CGS_AMBIENT]={STUDIO_VA_TIDAL_DRONE,STUDIO_VA_DEEP_ANCHOR,STUDIO_VA_AIR_LATTICE},
};

#define CGS_PALETTE_MAX 20u
static struct {
    uint32_t gen;
    uint8_t genre1, count[CGS_DRUMS];
    cf_ent_t entry[CGS_DRUMS][CGS_PALETTE_MAX];
} cgs_palette;

static void cgs_palette_add(unsigned part,unsigned engine,unsigned voice)
{
    unsigned n=cgs_palette.count[part];
    unsigned pos=pool_pos_factory(engine,voice);
    pool_ent_t entry=pool_entry(engine,pos);
    if(n>=CGS_PALETTE_MAX || cf_ent_part(engine,pos)!=(int)(part&1u) ||
       cf_primary_duplicate(engine,entry))return;
    for(unsigned i=0;i<n;i++)
        if(cgs_palette.entry[part][i].e==engine && cgs_palette.entry[part][i].pos==pos)return;
    cgs_palette.entry[part][n]=(cf_ent_t){(uint8_t)engine,(uint8_t)pos};
    cgs_palette.count[part]++;
}

static void cgs_palette_build(void)
{
    if(cgs_palette.genre1==cgs.genre+1u && cgs_palette.gen==up_gen)return;
    cgs_palette.genre1=cgs.genre+1u;
    cgs_palette.gen=up_gen;
    for(unsigned part=0;part<CGS_DRUMS;part++){
        cgs_palette.count[part]=0;
        for(unsigned i=0;i<cgs_profile_tone_count(part);i++){
            unsigned voice=cgs_profile_voice(part,i);
            if(voice!=PF_NONE)cgs_palette_add(part,ENGI_VA,voice);
        }
        cgs_palette_add(part,ENGI_VA,CGS_ORIGINAL_VOICE[cgs.genre][part]);
        const char *const *name=CGS_PALETTE_EXTRA[cgs.genre][part];
        for(;*name;name++)
            for(unsigned e=0;e<CF_NENG;e++){
                unsigned engine=CF_ENGINES[e];
                const engine_t *eng=ENGINES[engine];
                for(unsigned voice=0;voice<eng->npresets;voice++)
                    if(cb_streq(eng->presets[voice].name,*name))cgs_palette_add(part,engine,voice);
            }
    }
}
