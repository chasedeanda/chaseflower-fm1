/* SPDX-License-Identifier: GPL-3.0-only
 * Build 01: bounded shared-clock controller. All plans, sound blobs, effect
 * choices and embellishment flags are prepared in the foreground. */
#include "cf_engine_data.h"
enum { CGE_HARMONY=1, CGE_SOUND=2, CGE_GROOVE=4, CGE_FX=8 };
static struct {
    uint8_t loaded, energy, movement, variation, locks;
    uint8_t index[4], request[4], commit[4], previous[4], pending, undo;
    uint8_t bar, arp, bass_gate, drum_gate, sound_taken, fx_taken;
    uint8_t movement_applied, variation_applied, energy_applied;
    uint16_t profile[5][3], gain[3], target[3], last_target[3], ramp_left[3];
    uint32_t cycle, generation;
    cf_fx_t fx;
} cge;
static int cge_active(void){return cpa.on && cpa.plan.idea==CF_P_ENGINE;}
static int cge_arp_wanted(void){return cge_active()?cge.arp:cfp_cfg.arp_on;}
static int cge_bass_enabled(void){return !cge_active() || cge.bass_gate;}
static int cge_drums_enabled(void){return !cge_active() || cge.drum_gate;}
static unsigned cge_phase(void)
{unsigned p=0;while(p<4 && cge.bar+1u>CGE_PROFILE[p][0])p++;return p;}
static void cge_targets(void)
{
    unsigned p=cge_phase();
    for(unsigned k=0;k<3;k++)cge.target[k]=cge.profile[p][k];
}
static void cge_gates(void)
{
    unsigned p=cge_phase();cge.energy_applied=cge.energy;
    cge.bass_gate=cge.energy_applied && CGE_PROFILE[p][2];
    cge.drum_gate=cge.energy_applied && CGE_PROFILE[p][3];
}
static void cge_start(void)
{
    if(!cge_active())return;
    cge.bar=0;cge.cycle=0;cge.arp=0;cge.movement_applied=cge.movement;cge.variation_applied=cge.variation;
    cge_targets();cge_gates();
    for(unsigned k=0;k<3;k++)cge.gain[k]=cge.target[k];
}
static void cge_stop(void)
{cge.arp=0;for(unsigned k=0;k<3;k++)cge.target[k]=32768;}
/* Uses the existing staged sound loader; defined after cf_sound.c. */
static int cge_sound_boundary(void);
static int cge_bar(int onset)
{
    if(!cge_active())return 0;
    cge.bar=(cge.bar+1u)%16u;if(!cge.bar)cge.cycle++;
    cge.variation_applied=cge.variation;
    unsigned bass=cge.bass_gate;cge_targets();cge_gates();
    if(!onset && bass!=cge.bass_gate){
        const cpa_slot_t *s=&cpa.plan.slot[cpa.slot];
        if(!cge.bass_gate)sqp_bass_off();
        else if(!cpa_live_chord() && cpa.plan.mode!=CPA_DRUMS)
            sqp_bass_on(s->bass,cfp_cfg.bass_on?s->nb:0,s->ticks-cpa.elapsed,0);
    }
    if(!(cge.bar&1u)) {
        cge.movement_applied=cge.movement;
        if(cge.pending&CGE_HARMONY){cge.index[0]=cge.commit[0];cge.pending&=~CGE_HARMONY;}
    }
    uint8_t old=cge.arp;
    cge.arp=cge.movement_applied>=65 && cge.bar>=8 && cge.bar<12;
    int sound=0;
    if(cge.pending&CGE_SOUND){sound=cge_sound_boundary();if(sound){cge.index[1]=cge.commit[1];cge.pending&=~CGE_SOUND;}}
    if(cge.pending&CGE_GROOVE){cge.index[2]=cge.commit[2];cge.pending&=~CGE_GROOVE;}
    if(cge.pending&CGE_FX){cfx_target=cge.fx;cfx_pending=cfx_ready=1;cge.index[3]=cge.commit[3];cge.pending&=~CGE_FX;cge.fx_taken=1;}
    /* Never replace a manual action, extend an existing fill, or undo Drop. */
    if(cge.cycle && cge.drum_gate && !cf_moment.pending && !cf_moment.fill && !cf_moment.dropped &&
       ((cge.bar==11 && cge.variation_applied>=50 && cge.energy>=50) ||
        (cge.bar==7 && cge.variation_applied>=75 && cge.energy>=70)))
        cf_perf_queue(CFPERF_FILL);
    return old!=cge.arp || sound;
}
static void cge_block(uint32_t n)
{
    if(cge_active())cge_targets();else for(unsigned k=0;k<3;k++)cge.target[k]=32768;
    /* Every changed target gets its own linear 50ms control-rate ramp. */
    for(unsigned k=0;k<3;k++){
        if(cge.last_target[k]!=cge.target[k]){cge.last_target[k]=cge.target[k];cge.ramp_left[k]=FS/20u;}
        unsigned left=cge.ramp_left[k],take=n<left?n:left;
        if(left){int delta=(int)cge.target[k]-cge.gain[k];cge.gain[k]=(uint16_t)((int)cge.gain[k]+delta*(int)take/(int)left);cge.ramp_left[k]-=take;}
        else cge.gain[k]=cge.target[k];
    }
}
static uint32_t cge_mix_gain(unsigned part)
{
    if(part==0 || part==1)return cge.gain[part];
    if(part==CF_PART_DRUM)return cge.gain[2];
    return 32768;
}
/* Arp override is scoped to the scheduled chord owner; live ARP stays independent. */
static cf_arp_t cge_arp_save;
static uint8_t cge_arp_save_on;
static int cge_arp_push(void)
{
    if(!cge_active())return 0;
    cge_arp_save=cfp_cfg.arp;cge_arp_save_on=cfp_cfg.arp_on;
    cfp_cfg.arp_on=cge.arp;cfp_cfg.arp.rate=4;cfp_cfg.arp.oct=1;cfp_cfg.arp.gate=70;cfp_cfg.arp.latch=0;
    return 1;
}
static void cge_arp_pop(int pushed)
{if(pushed){cfp_cfg.arp=cge_arp_save;cfp_cfg.arp_on=cge_arp_save_on;}}
