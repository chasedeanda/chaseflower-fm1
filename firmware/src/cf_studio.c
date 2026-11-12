/* SPDX-License-Identifier: GPL-3.0-only
 * Studio uses the accompaniment clock and sequencer note owners. Each clip is
 * two bars of prepared rhythmic events; pitch roles follow the committed plan.
 */
enum { CGS_CHORDS, CGS_BASS, CGS_LAYER, CGS_DRUMS, CGS_NPART };
enum { CGS_ANCHOR=1, CGS_THIRD, CGS_FIFTH, CGS_OCTAVE, CGS_PICKUP, CGS_UPPER, CGS_STEP_IN };
enum { CGS_DREAM, CGS_INDIE, CGS_WAVE, CGS_PUNK, CGS_LOFI, CGS_AMBIENT, CGS_NGENRE };
enum { CGS_PATCH_ROLL=251, CGS_PATCH_STAGE, CGS_STEPS=16, CGS_TICKS=SQ_PPQN/4, CGS_BAR=4*SQ_PPQN };
enum { CGS_KICK=1, CGS_SNARE=2, CGS_HAT=4, CGS_OPEN=8, CGS_RIM=16, CGS_LOW_TOM=32, CGS_MID_TOM=64 };
typedef struct {
    uint8_t event[2][CGS_STEPS]; /* pitch role and gate, or drum lane bits */
    uint8_t voice;
    uint8_t feel[2][CGS_STEPS];  /* velocity trim in high bits, onset delay in low bits */
    uint32_t seed;             /* repeat the musical choices when shaping the clip */
} cgs_clip_t;
typedef struct { uint8_t note[CF_P_MAX][7]; } cgs_pitches_t;
static struct {
    cgs_pitches_t pitch, setup_pitch, edit_pitch;
    cgs_clip_t active[CGS_NPART], next[CGS_NPART], take[CGS_NPART];
    uint32_t seed[CGS_NPART], harmony_seed, last_tick, developed, bar, origin_bar;
    uint8_t style[CGS_NPART], activity[CGS_NPART], level[CGS_NPART], touch[CGS_NPART];
    uint8_t gate[CGS_NPART], muted, mute_applied, focus, zoom, space, ready, mix_idle;
    uint8_t defaults_offered, boot_sound_gen[3];
    uint8_t genre, queued_genre, playing_genre, energy, evolution, solo, kit_pinned, feel, queued_feel, playing_feel, meter;
    uint8_t custom, bank_meter;
    volatile uint8_t pending;
    uint8_t generation[CGS_NPART], harmony_generation, arp, held_solo;
    volatile uint8_t drum_mutes, fill_held;
    uint16_t gain[CGS_NPART];
} cgs;
static void cgs_sound_boundary(unsigned tag);
static void cgs_cancel_rolled_sounds(void);
static int cgs_active(void)
{ return CF_STUDIO && cpa.on && cpa.plan.studio; }
static unsigned cgs_rand(uint32_t *seed, unsigned limit)
{
    uint32_t x=*seed;
    x^=x<<13; x^=x>>17; x^=x<<5;
    *seed=x;
    return x%limit;
}
static void cgs_release(unsigned part)
{
    if(part==CGS_CHORDS){sqp_arp_off(part);sqp_chord_off();}
    else if(part==CGS_BASS)sqp_bass_off();
    else if(part==CGS_LAYER){sqp_arp_off(part);sqp_layer_off();}
    else sqp_drum_offs();
    cgs.gate[part]=0;
}
static void cgs_midi_takeover(unsigned part)
{
    if(cgs_active())cgs_release(part);
}
static int cgs_manual(unsigned part)
{
    return (part<2 && cr_mheld[part]) ||
        (part==cfp_cp && (cfp.n || cfa_k.n)) ||
        (part==CGS_BASS && cfp.nb);
}
static unsigned cgs_mute_mask(void)
{
    unsigned solo=cgs.solo|cgs.held_solo;
    return cgs.muted|(solo?(15u&~solo):0u);
}
static void cgs_start(void)
{
    cgs.last_tick=UINT32_MAX;
    memset(cgs.gate,0,sizeof cgs.gate);
    cgs.developed=cgs.bar=cgs.origin_bar=0;
    cgs.mute_applied=(uint8_t)cgs_mute_mask();
    for(unsigned p=0;p<CGS_NPART;p++)cgs.gain[p]=cgs.level[p]*32768u/100u;
}
static void cgs_common_chord(const cpa_slot_t *s, unsigned velocity)
{
    uint8_t previous[6], n=sqp.n;
    memcpy(previous,sqp.note,n);
    memcpy(sqp.note,s->note,s->n);
    sqp.n=s->n;
    unsigned released=0;
    cfp_fade_released(CGS_CHORDS);
    for(unsigned i=0;i<n;i++){
        if(has(s->note,s->n,previous[i]))continue;
        sqp_off(CGS_CHORDS,previous[i]);
        if(released++)cfp_fade_note(CGS_CHORDS,previous[i]);
    }
    for(unsigned i=0;i<s->n;i++)
        if(!has(previous,n,s->note[i]))cfp_on(CGS_CHORDS,s->note[i],velocity);
}
static void cgs_line(unsigned part, unsigned note, unsigned velocity)
{
    uint8_t *held=part==CGS_BASS?&sqp.nb:&sqp.ln;
    uint8_t *pitch=part==CGS_BASS?sqp.bass:sqp.lnote;
    if(*held && pitch[0]!=note)cgs_release(part);
    if(!*held){
        if(part==CGS_BASS)cfp_bass_mono();
        else cfp_fade_released(part);
        pitch[0]=(uint8_t)note;
        *held=1;
        cfp_on(part,note,velocity);
    }
}
static unsigned cgs_velocity(unsigned part,unsigned velocity)
{
    if(part!=CGS_CHORDS)velocity=velocity*(50u+cgs.touch[part])/100u;
    return (unsigned)clamp(velocity,1,127);
}

static void cgs_drums(unsigned hit, unsigned step,unsigned steps,unsigned accent)
{
    static const uint8_t lanes[]={0,1,3,4,8,5,6,7};
    static const uint8_t groups[]={0,1,2,2,3,3,3,3};
    static const uint8_t level[]={105,86,53,47,39,69,63,59};
    if(cf_moment.dropped)return;
    unsigned punch=cgs.fill_held, half=steps/2u;
    unsigned style=punch?cf_perf_kit_style(cpa.plan.kit):cf_moment.style;
    for(unsigned i=0;i<sizeof lanes;i++){
        unsigned l=lanes[i];
        if((cgs.drum_mutes>>groups[i])&1u)continue;
        if(punch ? !cf_perf_fill_hit(style,l,step%half,half) : !cf_perf_hit(l,step,steps,(hit>>i)&1u))continue;
        unsigned original=level[i]+accent+(!(step&3u)?7u:0u);
        unsigned v=punch?cf_perf_fill_level(style,l,step%half+half,steps,original):cf_perf_level(l,step,steps,original);
        sqp_lane_kit(l,cpa.plan.kit,cgs_velocity(CGS_DRUMS,v));
        cfp_on(CGS_DRUMS,SQ_LANE_NOTE[l],95);
        sqp.doff|=(uint16_t)(1u<<l);
        sqp.lane_t[l]=sqp.lit_t[SQ_DRUMS]=cf_now_ms?cf_now_ms:1;
    }
}
static void cgs_arp(const cpa_slot_t *s,unsigned part)
{
    sq_ev_t chord={.n=s->n,.root=s->root,.mask=(uint16_t)s->mask,.btns=cf_harmony_event_buttons(s->mask)};
    memcpy(chord.note,s->note,s->n);
    sqp_arp_on(part,&chord);
}

static void cgs_tick(void)
{
    if(cgs.last_tick==cpa.q2 || cpa.clock_wait || cpa.paused)return;
    cgs.last_tick=cpa.q2;
    for(unsigned p=0;p<CGS_DRUMS;p++)
        if(cgs.gate[p] && !--cgs.gate[p])cgs_release(p);
    unsigned ticks=cpa.plan.beats==6u?CGS_TICKS*2u:CGS_TICKS;
    unsigned steps=cpa.plan.beats==4u?CGS_STEPS:12u;
    unsigned offset=cpa.t%ticks;
    if(offset>5)return;
    if(!offset && !cpa_setup.flags)cgs_sound_boundary(CGS_PATCH_ROLL);
    unsigned pending=offset?0:cgs.pending;
    unsigned step=cpa.t/ticks, phrase=cgs.bar-cgs.origin_bar, bar=phrase&1u;
    const cpa_slot_t *s=&cpa.plan.slot[cpa.slot];
    int custom=cpa.plan.idea==CF_P_DRAFT;
    int slot_closed=custom && !s->tie && s->gate<100 && cpa.elapsed>=(uint32_t)s->ticks*s->gate/100u;
    for(unsigned p=0;p<CGS_NPART;p++){
        unsigned bit=1u<<p, changed=pending&bit, rerolled=0;
        if(changed){
            rerolled=cgs.active[p].seed!=cgs.next[p].seed;
            cgs.active[p]=cgs.next[p];
            cgs.generation[p]++;
            if(p!=CGS_CHORDS && rerolled)cgs_release(p);
            if(p==CGS_CHORDS && cgs.active[p].event[bar][0]<=1)cgs.gate[p]=0;
        }
        if(cgs.mute_applied&bit)continue;
        if(p<CGS_DRUMS && (cpa.plan.mode==CPA_DRUMS || cgs_manual(p)))continue;
        if((slot_closed && p<CGS_DRUMS) || (custom && p==CGS_BASS && !s->nb)){
            cgs_release(p);
            continue;
        }
        if((p==CGS_CHORDS || p==CGS_LAYER) && !offset){
            if(cgs.arp&bit){
                if(!(sqp.aon&bit)){cgs_release(p);cgs_arp(s,p);}
                else if(!cpa.elapsed)cgs_arp(s,p);
            } else if(sqp.aon&bit)sqp_arp_off(p);
        }
        if(cgs.arp&bit)continue;
        if(p==CGS_DRUMS && cpa.plan.rhythm>=DM_N)continue;
        const cgs_clip_t *clip=&cgs.active[p];
        unsigned event=clip->event[bar][step], feel=clip->feel[bar][step];
        if(cgs.playing_genre==CGS_DREAM){
            /* The middle of the form opens room. The last bar brings the answer forward. */
            if((phrase&7u)==4u && p==CGS_LAYER)event=0;
            if((phrase&7u)==4u && p==CGS_DRUMS)event&=~(CGS_HAT|CGS_OPEN);
            if((phrase&7u)==7u && p==CGS_DRUMS && step==steps-2u)event|=CGS_RIM;
        } else if(cgs.playing_genre==CGS_PUNK){
            unsigned section=phrase&7u;
            if(section==4u){
                if(p==CGS_LAYER || (p==CGS_CHORDS && step && (step&3u)))event=0;
                if(p==CGS_DRUMS)event&=~(CGS_OPEN|CGS_HAT);
            }
            if(p==CGS_CHORDS && (section==6u || section==7u) && event>1 &&
               (step==0 || step==steps/2))event=16;
            if(p==CGS_DRUMS && section==7u && step>=steps-3u)
                event|=step==steps-2u?CGS_MID_TOM:CGS_LOW_TOM;
        }
        if(p==CGS_CHORDS){
            if(offset)continue;
            unsigned pulse=clip->event[bar][0]>1;
            if(event==1 || (pulse && event && cpa.elapsed))sqp_chord_off();
            if(rerolled || (!pulse && !sqp.n) || !cpa.elapsed || event){
                cgs_common_chord(s,feel?feel:79);
                cgs.gate[p]=(uint8_t)(pulse?(event>1?event:clip->event[bar][0])*6u:0u);
            }
        } else if(offset==(feel&7u)){
            if(p==CGS_DRUMS)cgs_drums(event,step,steps,feel>>3);
            else if(event){
                unsigned tone=(event&7u)-1u;
                unsigned note=p==CGS_BASS?cgs.pitch.note[cpa.slot][tone]:s->note[tone<s->n?tone:s->n-1];
                if(p==CGS_BASS && (tone==CGS_PICKUP-1u || tone==CGS_STEP_IN-1u) &&
                    s->ticks-cpa.elapsed>4*CGS_TICKS)note=cgs.pitch.note[cpa.slot][CGS_FIFTH-1u];
                if(p==CGS_LAYER && cgs.playing_genre==CGS_PUNK){
                    if(tone==1)note=cgs.pitch.note[cpa.slot][CGS_THIRD-1u];
                    if(tone==3)note=cgs.pitch.note[cpa.slot][CGS_UPPER-1u];
                    while(note<64)note+=12;
                } else if(p==CGS_LAYER && note<60)note+=12;
                if(p==CGS_LAYER && cgs.playing_genre==CGS_DREAM && clip->voice==3 && note<70)note+=12;
                cgs_line(p,note,cgs_velocity(p,48u+2u*(feel>>3)));
                cgs.gate[p]=(uint8_t)((event>>3)*6u);
            }
        }
    }
    if(!offset)cgs.pending=0;
}
static void cgs_apply_mute(void)
{
    unsigned muted=cgs_mute_mask(), release=muted&~cgs.mute_applied;
    cgs.mute_applied=(uint8_t)muted;
    for(unsigned p=0;p<CGS_NPART;p++)
        if(release&(1u<<p))cgs_release(p);
}

static int cgs_mix_live(void)
{
    return cgs_active() || (CF_STUDIO && cgs.mix_idle && !cpa.on && !sqp.on && !dmp.on);
}

static void cgs_audio_block(uint32_t n)
{
    for(unsigned p=0;p<CGS_NPART;p++){
        int32_t target=cgs.level[p]*32768u/100u;
        int32_t delta=target-cgs.gain[p];
        int32_t move=delta*(int32_t)n/(int32_t)(FS/40u);
        cgs.gain[p]=(uint16_t)(move?cgs.gain[p]+move:target);
    }
}
static void cgs_commit_take(void)
{
    cgs.pitch=cgs.setup_pitch;
    cgs.origin_bar=cgs.bar;
    cgs.developed=0;
    for(unsigned p=0;p<CGS_NPART;p++){
        cgs.next[p]=cgs.take[p];
    }
    cgs.pending=(1u<<CGS_NPART)-1u;
    cgs.playing_genre=cgs.queued_genre;
    cgs.playing_feel=cgs.queued_feel;
    cgs.harmony_generation++;
}

/* Original motifs. The same rhythmic question recurs over successive
 * chords. The response is voiced against the current and incoming harmony. */
enum { CGS_KICK_REPLY=10, CGS_FOLLOW, CGS_ANSWER, CGS_FLOAT };
typedef struct {
    uint8_t pocket;
    uint8_t cell[5][3]; /* sixteenth, pitch role, length in units of six ticks */
} cgs_riff_t;
static const cgs_riff_t CGS_RIFFS[]={
    {CGS_FOLLOW,{{0,CGS_ANCHOR,11},{5,CGS_UPPER,3},{7,CGS_THIRD,5},{10,CGS_FIFTH,10},{15,CGS_PICKUP,3}}},
    {CGS_ANSWER,{{0,CGS_ANCHOR,7},{4,CGS_OCTAVE,5},{8,CGS_UPPER,6},{11,CGS_FIFTH,9},{15,CGS_STEP_IN,3}}},
    {CGS_FLOAT, {{0,CGS_ANCHOR,17},{6,CGS_THIRD,7},{9,CGS_FIFTH,12},{13,CGS_UPPER,5},{15,CGS_STEP_IN,3}}},
    {CGS_FOLLOW,{{0,CGS_ANCHOR,7},{3,CGS_FIFTH,4},{7,CGS_OCTAVE,6},{10,CGS_ANCHOR,11},{15,CGS_PICKUP,3}}},
    {CGS_FLOAT, {{0,CGS_ANCHOR,23},{7,CGS_UPPER,5},{9,CGS_THIRD,10},{13,CGS_FIFTH,5},{15,CGS_STEP_IN,3}}},
    {CGS_ANSWER,{{0,CGS_ANCHOR,9},{5,CGS_THIRD,6},{8,CGS_FIFTH,5},{11,CGS_OCTAVE,6},{14,CGS_STEP_IN,5}}},
    {CGS_FLOAT, {{0,CGS_ANCHOR,12},{4,CGS_UPPER,5},{7,CGS_FIFTH,13},{12,CGS_THIRD,5},{15,CGS_PICKUP,3}}},
    {CGS_FOLLOW,{{0,CGS_ANCHOR,8},{3,CGS_OCTAVE,5},{6,CGS_FIFTH,6},{10,CGS_THIRD,9},{15,CGS_STEP_IN,3}}},
    {CGS_ANSWER,{{0,CGS_ANCHOR,16},{5,CGS_FIFTH,4},{8,CGS_OCTAVE,7},{11,CGS_UPPER,7},{15,CGS_PICKUP,3}}},
    {CGS_FLOAT, {{0,CGS_ANCHOR,13},{4,CGS_THIRD,6},{7,CGS_UPPER,14},{12,CGS_FIFTH,7},{15,CGS_STEP_IN,3}}},
    {CGS_FOLLOW,{{0,CGS_ANCHOR,7},{3,CGS_UPPER,5},{7,CGS_THIRD,8},{10,CGS_FIFTH,12},{15,CGS_PICKUP,3}}},
    {CGS_ANSWER,{{0,CGS_ANCHOR,12},{5,CGS_OCTAVE,4},{8,CGS_THIRD,8},{11,CGS_FIFTH,9},{15,CGS_STEP_IN,3}}},
};
static const uint8_t CGS_RIFF_WEIGHT[2][sizeof CGS_RIFFS/sizeof CGS_RIFFS[0]]={
    {6,5,4,6,5,4,2,2,2,1,4,4},
    {2,4,5,1,3,5,6,4,5,6,2,3}
};
static const cgs_riff_t *cgs_riff(unsigned style, uint32_t *seed)
{
    const uint8_t *weight=CGS_RIFF_WEIGHT[style];
    unsigned total=0;
    for(unsigned i=0;i<sizeof CGS_RIFF_WEIGHT[0];i++)total+=weight[i];
    unsigned draw=cgs_rand(seed,total), index=0;
    while(draw>=weight[index])draw-=weight[index++];
    return &CGS_RIFFS[index];
}
static void cgs_bass_clip(cgs_clip_t *clip, const cgs_riff_t *riff, unsigned activity, uint32_t *seed)
{
    unsigned answer=cgs_rand(seed,3);
    for(unsigned bar=0;bar<2;bar++){
        for(unsigned i=0;i<5;i++){
            const uint8_t *cell=riff->cell[i];
            unsigned step=cell[0], role=cell[1], gate=cell[2];
            if(i==1 && activity<36)continue;
            if(i==4 && (!bar || activity<39))continue;
            if(bar && i==2 && activity>60 && answer==1)role=CGS_UPPER;
            if(bar && i==3 && answer==2 && riff->pocket==CGS_ANSWER)step++;
            unsigned velocity=i==0?24:i==4?10:riff->pocket==CGS_FOLLOW && step==CGS_KICK_REPLY?22:15;
            unsigned delay=i>0 && i<4 && (step&1u) && riff->pocket!=CGS_FOLLOW?2u+answer:0;
            clip->event[bar][step]=(uint8_t)(role+(gate<<3));
            clip->feel[bar][step]=(uint8_t)((velocity<<3)+delay);
        }
    }
}
static void cgs_chord_clip(cgs_clip_t *clip,unsigned activity,unsigned shape)
{
    static const uint8_t second_attack[]={7,8,10,11};
    for(unsigned bar=0;bar<2;bar++){
        uint8_t *e=clip->event[bar], *v=clip->feel[bar];
        v[0]=(uint8_t)(80u+shape*2u);
        unsigned second=second_attack[shape];
        e[0]=(uint8_t)(12u+shape);
        if(activity>=26){
            e[second]=(uint8_t)(bar?10u:12u);
            v[second]=(uint8_t)(bar?76u:72u);
        }
        if(bar && activity>=65){e[13]=8;v[13]=68;}
    }
}

/* These pockets are phrased against the same chord roles as the original
 * Dream Pop riffs. Drum anchors and bass attacks are written together. */
static const cgs_riff_t CGS_GENRE_RIFFS[][2]={
    {0}, /* Dream Pop uses its weighted riff collection. */
    {{CGS_ANSWER,{{0,CGS_ANCHOR,10},{3,CGS_OCTAVE,4},{6,CGS_FIFTH,7},{11,CGS_THIRD,8},{15,CGS_STEP_IN,3}}},
     {CGS_FOLLOW,{{0,CGS_ANCHOR,7},{4,CGS_THIRD,4},{6,CGS_FIFTH,11},{12,CGS_UPPER,8},{15,CGS_PICKUP,3}}}},
    {{CGS_FOLLOW,{{0,CGS_ANCHOR,5},{2,CGS_OCTAVE,5},{4,CGS_FIFTH,5},{8,CGS_ANCHOR,5},{14,CGS_OCTAVE,4}}},
     {CGS_FOLLOW,{{0,CGS_ANCHOR,6},{2,CGS_FIFTH,6},{6,CGS_OCTAVE,6},{10,CGS_THIRD,6},{15,CGS_PICKUP,3}}}},
    {{CGS_FOLLOW,{{0,CGS_ANCHOR,7},{2,CGS_ANCHOR,3},{6,CGS_ANCHOR,3},{10,CGS_FIFTH,7},{15,CGS_PICKUP,3}}},
     {CGS_ANSWER,{{0,CGS_ANCHOR,8},{2,CGS_OCTAVE,4},{6,CGS_FIFTH,5},{10,CGS_THIRD,8},{14,CGS_STEP_IN,5}}}},
    {{CGS_FLOAT,{{0,CGS_ANCHOR,14},{5,CGS_OCTAVE,4},{7,CGS_THIRD,8},{11,CGS_FIFTH,10},{15,CGS_STEP_IN,3}}},
     {CGS_ANSWER,{{0,CGS_ANCHOR,17},{5,CGS_FIFTH,5},{8,CGS_UPPER,8},{13,CGS_THIRD,5},{15,CGS_PICKUP,3}}}},
    {{CGS_FLOAT,{{0,CGS_ANCHOR,31},{6,CGS_FIFTH,31},{9,CGS_FIFTH,29},{12,CGS_ANCHOR,31},{15,CGS_STEP_IN,3}}},
     {CGS_FLOAT,{{0,CGS_ANCHOR,31},{5,CGS_THIRD,30},{8,CGS_OCTAVE,31},{12,CGS_FIFTH,31},{15,CGS_UPPER,6}}}},
};
typedef struct { uint16_t chords, kick; uint8_t gate, snare, hat_gap, drift; } cgs_pocket_t;
static const cgs_pocket_t CGS_POCKET[]={
    {0,0,0,0,0,0},
    {0x0449,0x0441,8,4,4,1},
    {0x1111,0x4141,7,4,2,0},
    {0}, /* Pop Punk's accents and spaces are composed below. */
    {0x0881,0x0881,11,8,4,4},
    {0,1,0,0,8,5},
};

/* Each take keeps one kick map across independent musician rolls. */
static const uint16_t CGS_PUNK_KICK[][2]={
    {0x0445,0x2441}, /* syncopated chug */
    {0x2085,0x14a1}, /* late kick and pushed answer */
    {0x0109,0x4481}, /* half-time breakdown */
    {0x1541,0x1145}, /* straight drive */
};

static void cgs_punk_clip(cgs_clip_t *clip,unsigned part,unsigned style,unsigned activity,unsigned shape)
{
    unsigned pocket=clip->voice;
    for(unsigned bar=0;bar<2;bar++){
        uint8_t *event=clip->event[bar], *feel=clip->feel[bar];
        uint16_t kicks=CGS_PUNK_KICK[pocket][bar];
        if(part==CGS_CHORDS){
            feel[0]=107;
            if(style){
                unsigned lift=8u+((shape+bar)&1u)*4u;
                event[0]=(uint8_t)(activity>=48?24:32);
                if(activity>=35){event[lift]=20;feel[lift]=98;}
                if(bar && activity>=72){event[15]=4;feel[15]=92;}
                continue;
            }
            event[0]=(uint8_t)(pocket==2?13u:5u+(shape&1u)*2u);
            for(unsigned i=1;i<CGS_STEPS;i++){
                if(!((kicks>>i)&1u) || (activity<32 && i<6))continue;
                int accent=(i&3u)==0 || (i>=8 && ((i+shape)&3u)>=2u);
                event[i]=(uint8_t)(accent?8:3);
                feel[i]=(uint8_t)(accent?103:83);
            }
            unsigned push=bar?12u+(shape&1u):5u+(shape&1u);
            if(activity>=47 && !event[push]){event[push]=3;feel[push]=82;}
            if(bar && activity>=78){event[15]=3;feel[15]=96;}
        } else if(part==CGS_BASS){
            static const uint8_t replies[]={CGS_FIFTH,CGS_THIRD,CGS_OCTAVE,CGS_UPPER};
            event[0]=CGS_ANCHOR+((pocket==2?13u:7u)<<3);feel[0]=120;
            for(unsigned i=1;i<CGS_STEPS;i++){
                if(!((kicks>>i)&1u))continue;
                unsigned role=style || (i>=7 && activity>=40)?replies[(shape+bar+i/2)%NELEM(replies)]:CGS_ANCHOR;
                unsigned gate=pocket==2?9u:(i&3u)?3u:6u;
                event[i]=(uint8_t)(role+(gate<<3));feel[i]=(uint8_t)((i&3u)?104:120);
            }
            unsigned answer=bar?11u+(shape&1u):5u+(shape&1u);
            if(activity>=39 && !event[answer]){
                event[answer]=(uint8_t)(replies[(shape+bar)%NELEM(replies)]+(4u<<3));
                feel[answer]=101;
            }
            if(activity>=52){event[15]=(uint8_t)((bar?CGS_PICKUP:CGS_STEP_IN)+(3u<<3));feel[15]=96;}
        } else if(part==CGS_DRUMS){
            for(unsigned i=0;i<CGS_STEPS;i++){
                if((kicks>>i)&1u)event[i]|=CGS_KICK;
                if(activity>=29 && i%(activity>=55?2u:4u)==0)event[i]|=CGS_HAT;
                if(event[i])feel[i]=(uint8_t)(((i&3u)?(shape+i)%3u:4u)<<3);
            }
            event[0]|=CGS_OPEN;
            if(pocket==2 || (!style && pocket==0)){event[8]|=CGS_SNARE;feel[8]=5u<<3;}
            else {event[4]|=CGS_SNARE;event[12]|=CGS_SNARE;feel[4]=feel[12]=5u<<3;}
            if(style && activity>=55)event[bar?11:7]|=CGS_KICK;
            if(bar && activity>=43){
                event[13+(shape&1u)]|=(shape&2u)?CGS_SNARE:CGS_MID_TOM;
                event[15]|=CGS_LOW_TOM;
            } else if(activity>=48)event[bar?14:10]|=CGS_OPEN;
        } else {
            static const uint8_t hooks[][2]={{2,4},{3,2},{4,3},{2,3}};
            unsigned onset=(bar?9u:4u)+(pocket&1u)+shape%3u;
            if(((kicks>>onset)&1u) && onset<14u)onset++;
            event[onset]=(uint8_t)(hooks[shape][bar]+((style?7u:19u)<<3));
            feel[onset]=style?116:91;
            if((style && activity>=31) || activity>=55){event[onset-3u]=1u+(5u<<3);feel[onset-3u]=98;}
            if(bar && activity>=44){event[15]=(uint8_t)(hooks[shape][0]+(5u<<3));feel[15]=101;}
        }
    }
}

static void cgs_genre_clip(cgs_clip_t *clip, unsigned part, unsigned genre, unsigned style,
                           unsigned activity, unsigned shape)
{
    const cgs_pocket_t *p=&CGS_POCKET[genre];
    for(unsigned bar=0;bar<2;bar++){
        uint8_t *events=clip->event[bar], *feel=clip->feel[bar];
        if(part==CGS_CHORDS && p->chords){
            for(unsigned i=0;i<CGS_STEPS;i++){
                if(!((p->chords>>i)&1u) || (i && activity<32 && i!=8))continue;
                events[i]=(uint8_t)(p->gate+style*3);
                feel[i]=(uint8_t)(i?63u+((i+shape+bar)%4)*5u:87u);
            }
            if(bar && activity>68){events[15]=4;feel[15]=70;}
        } else if(part==CGS_BASS){
            if(genre==CGS_AMBIENT){
                for(unsigned i=1;i<CGS_STEPS;i++)
                    if((!style && (!bar || i<9)) || activity<23)events[i]=0;
                continue;
            }
            for(unsigned i=2;i<CGS_STEPS;i+=2){
                if(!events[i] && genre==CGS_WAVE && activity>=53 && (i&3u)==2u){
                    events[i]=(uint8_t)((genre==CGS_WAVE && (i&3u) ? CGS_OCTAVE : CGS_ANCHOR)+(6u<<3));
                    feel[i]=(uint8_t)((i&3u?13u:18u)<<3);
                }
            }
            if(genre==CGS_LOFI)for(unsigned i=2;i<CGS_STEPS;i+=4)
                if(events[i])feel[i]=(uint8_t)((feel[i]&~7u)|5u);
        } else if(part==CGS_LAYER){
            if(genre==CGS_AMBIENT){
                unsigned i=bar?9u+shape:3u+shape;
                events[i]=(uint8_t)((style?3u:4u)+(31u<<3));
                feel[i]=91;
                if(bar && activity>65){events[15]=2u+(21u<<3);feel[15]=65;}
            } else if(genre==CGS_LOFI){
                unsigned i=(bar?9u:5u)+shape;
                events[i]=(uint8_t)((style?4u:2u)+(12u<<3));feel[i]=101;
                if(activity>=45){events[14]=3u+(5u<<3);feel[14]=68;}
            } else {
                unsigned start=genre==CGS_WAVE?2u:3u;
                unsigned count=1u+(activity>=30)+(activity>=53)+(activity>=76);
                for(unsigned j=0;j<count;j++){
                    unsigned i=start+j*(style?3u:2u)+(shape&1u);
                    events[i]=(uint8_t)((1u+(j+shape+(bar && j==count-1u))%4u)+((style?9u:5u)<<3));
                    feel[i]=(uint8_t)((j?9u:14u)<<3);
                }
            }
        } else if(part==CGS_DRUMS){
            if(genre==CGS_AMBIENT){
                if(activity>=32 && !bar){events[0]=CGS_KICK;feel[0]=2;}
                if(activity>=47 && bar){events[10+shape]=CGS_RIM;feel[10+shape]=4;}
                if(style && activity>=65)events[14]=CGS_OPEN;
                continue;
            }
            for(unsigned i=0;i<CGS_STEPS;i++){
                if((p->kick>>i)&1u)events[i]|=CGS_KICK;
                if(activity>=26 && i%(activity>=68?2u:p->hat_gap)==0)events[i]|=CGS_HAT;
                if(events[i] && i)feel[i]=(uint8_t)((((shape+i+bar)%4u)<<3)+(i&3u?p->drift:0));
            }
            events[p->snare]|=CGS_SNARE;
            feel[p->snare]|=p->drift;
            if(p->snare==4){events[12]|=CGS_SNARE;feel[12]|=p->drift;}
            if(bar && activity>=48)events[14u+(shape&1u)]|=style?CGS_OPEN:CGS_RIM;
            if(bar && activity>=79)events[13]|=CGS_SNARE;
        }
    }
}

/* Four Dream Pop feels give each musician a different job in the same take.
 * Kick maps are anchors for the bass, and Layer answers in the gaps. */
static const uint16_t CGS_DREAM_KICK[][2]={
    {0x0401,0x4001},{0x0081,0x1041},{0x1101,0x4141},{0x0221,0x4201}
};
static void cgs_dream_clip(cgs_clip_t *clip,unsigned p,unsigned voice,unsigned style,unsigned activity,uint32_t *seed)
{
    unsigned shape=cgs_rand(seed,4);
    if(p==CGS_CHORDS && !style){
        clip->feel[0][0]=clip->feel[1][0]=(uint8_t)(73u+voice*4u+activity/12u);
        return;
    }
    if(p==CGS_CHORDS && voice<2){
        cgs_chord_clip(clip,activity,shape);
        return;
    }
    if(p==CGS_BASS && voice==3){
        cgs_bass_clip(clip,cgs_riff(style,seed),activity,seed);
        return;
    }
    for(unsigned bar=0;bar<2;bar++){
        uint8_t *e=clip->event[bar], *v=clip->feel[bar];
        uint16_t kick=CGS_DREAM_KICK[voice][bar];
        if(p==CGS_CHORDS){
            e[0]=(uint8_t)((voice==2?8:15)+style*3);
            e[voice==2?(bar?7:6):10]=(uint8_t)(voice==2?7:12);
            v[0]=83; v[voice==2?(bar?7:6):10]=72;
            if(activity>55)e[14]=5;
        } else if(p==CGS_BASS){
            unsigned count=0;
            for(unsigned step=0;step<16;step++){
                if(!(kick&(1u<<step)))continue;
                unsigned role=count==0?CGS_ANCHOR:voice==2?CGS_OCTAVE:
                    (shape&1u)?CGS_THIRD:CGS_FIFTH;
                unsigned len=voice==0?25u:voice==1?14u:7u;
                e[step]=(uint8_t)(role+(len<<3));v[step]=(uint8_t)((count++?15u:24u)<<3);
            }
            if(activity>=40){
                unsigned step=voice==0?6u:voice==1?(bar?10u:12u):4u+2u*(shape&1u);
                if(!e[step]){
                    e[step]=(uint8_t)((style?CGS_UPPER:CGS_FIFTH)+((voice==0?12u:7u)<<3));
                    v[step]=(uint8_t)(104u+(voice==1?2u:0u));
                }
            }
            if(bar && activity>=53){e[15]=CGS_STEP_IN+(3u<<3);v[15]=88;}
        } else if(p==CGS_DRUMS){
            for(unsigned step=0;step<16;step++){
                if(kick&(1u<<step))e[step]=CGS_KICK;
                if(activity>=27 && (step+shape%2u)%(voice<2?4u:2u)==0){
                    e[step]|=CGS_HAT; v[step]=(uint8_t)((((step+bar)%3u)<<3)+(step&1u));
                }
            }
            unsigned backbeat=voice==0?8u:4u;
            e[backbeat]|=voice==1?CGS_RIM:CGS_SNARE;v[backbeat]=2;
            if(voice)e[12]|=CGS_SNARE;
            if(bar && activity>39)e[voice==0?15:14]|=style?CGS_OPEN:CGS_RIM;
        } else {
            unsigned first=voice==0?5u:voice==1?3u:voice==2?2u:7u;
            unsigned second=voice==0?12u:voice==1?10u:voice==2?8u:13u;
            unsigned gate=voice==0?22u:voice==1?12u:6u;
            unsigned tone=1u+shape%3u;
            e[first]=(uint8_t)(tone+(gate<<3));v[first]=105;
            if(activity>=25){
                e[second]=(uint8_t)((bar?2u:1u+(tone%4u))+(gate<<3));v[second]=81;
            }
            if(voice==2 && activity>40){e[5]=3u+(5u<<3);v[5]=86;}
            if(style && bar && activity>=55){e[15]=1u+(4u<<3);v[15]=65;}
        }
    }
}

static void cgs_meter_clip(cgs_clip_t *,unsigned,unsigned);
static void cgs_compose(cgs_clip_t *clip, unsigned p, uint32_t *seed)
{
    unsigned style=cgs.style[p], genre=cgs.genre;
    unsigned activity=(unsigned)clamp(cgs.activity[p]+(int)cgs.energy-50,0,100);
    memset(clip,0,sizeof *clip);
    clip->seed=*seed;
    if(genre==CGS_DREAM){
        clip->voice=cgs.feel;
        cgs_dream_clip(clip,p,clip->voice,style,activity,seed);
        cgs_meter_clip(clip,p,cgs.meter);
        return;
    }
    unsigned shape;
    if(p==CGS_BASS && genre!=CGS_PUNK){
        cgs_bass_clip(clip,&CGS_GENRE_RIFFS[genre][style],activity,seed);
    }
    shape=cgs_rand(seed,4);
    if(genre==CGS_PUNK){
        clip->voice=cgs.feel;
        cgs_punk_clip(clip,p,style,activity,shape);
    }
    else if(p==CGS_CHORDS && (genre==CGS_AMBIENT || style))
        clip->feel[0][0]=clip->feel[1][0]=(uint8_t)(74u+shape*2u+activity/12u+style*7u);
    else cgs_genre_clip(clip,p,genre,style,activity,shape);
    cgs_meter_clip(clip,p,cgs.meter);
}

/* Fit existing rhythmic material to 3/4 or compound 6/8. The second
 * accent in 6/8 falls on the fourth eighth; 3/4 gets two light afterbeats. */
static void cgs_meter_clip(cgs_clip_t *clip,unsigned part,unsigned meter)
{
    if(meter==4u)return;
    for(unsigned bar=0;bar<2;bar++){
        uint8_t *e=clip->event[bar],*v=clip->feel[bar];
        for(unsigned step=1;step<12;step++){
            unsigned src=step*4u/3u;
            e[step]=e[src];v[step]=v[src];
        }
        memset(e+12,0,4);
        memset(v+12,0,4);
        if(part!=CGS_DRUMS)continue;
        for(unsigned step=0;step<12;step++)e[step]&=(uint8_t)~CGS_SNARE;
        if(meter==6u)e[6]|=CGS_SNARE;
        else {e[4]|=CGS_HAT;e[8]|=CGS_SNARE;}
    }
}

static void cgs_vary_bass(cgs_clip_t *clip, uint32_t *seed)
{
    uint8_t *answer=clip->event[1], *feel=clip->feel[1];
    unsigned start=3+cgs_rand(seed,11), move=cgs_rand(seed,3);
    for(unsigned i=0;i<11;i++){
        unsigned step=3+(start-3+i)%11;
        if(!answer[step] || step==CGS_KICK_REPLY)continue;
        if(move==0){
            unsigned role=(answer[step]&7u)==CGS_THIRD?CGS_FIFTH:CGS_THIRD;
            answer[step]=(uint8_t)((answer[step]&~7u)|role);
        } else if(move==1 && !answer[step+1] && step+1!=CGS_KICK_REPLY){
            answer[step+1]=answer[step]; feel[step+1]=feel[step];
            answer[step]=0;
        } else feel[step]=(uint8_t)(feel[step]>96?64:120);
        return;
    }
}
