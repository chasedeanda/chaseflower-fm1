/* SPDX-License-Identifier: GPL-3.0-only
 * Additional choices for each Studio role. Names refer to existing factory
 * presets across both engines. Profile tones lead each row.
 */
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
    const cgs_profile_t *profile=&CGS_PROFILES[cgs.genre];
    for(unsigned part=0;part<CGS_DRUMS;part++){
        cgs_palette.count[part]=0;
        for(unsigned i=0;i<cgs_profile_tone_count(part);i++){
            unsigned voice=cgs_profile_voice(part,i);
            if(voice!=PF_NONE)cgs_palette_add(part,ENGI_VA,voice);
        }
        if(profile->original_voice[part])cgs_palette_add(part,ENGI_VA,profile->original_voice[part]);
        const char *const *name=profile->extra_tones?profile->extra_tones[part]:0;
        for(;name && *name;name++)
            for(unsigned e=0;e<CF_NENG;e++){
                unsigned engine=CF_ENGINES[e];
                const engine_t *eng=ENGINES[engine];
                for(unsigned voice=0;voice<eng->npresets;voice++)
                    if(cb_streq(eng->presets[voice].name,*name))cgs_palette_add(part,engine,voice);
            }
    }
}
