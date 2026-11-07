/* SPDX-License-Identifier: GPL-3.0-only
 * One temporary chord owner. Foreground publishes only resolved notes. The
 * accompaniment keeps its clock and drums while its chord/bass owner is silent.
 */
static struct { cf_lab_chord_t chord; volatile uint8_t request; } cla_mail;
static struct { cf_lab_chord_t chord; uint8_t on,blocked; uint32_t ticks,acc; } cla;
static int cla_active(void){return cla.on;}
static void cla_cancel(int restore)
{
    cla_mail.request=0;
    if(!cla.on)return;
    cla.on=0;
    for(unsigned i=0;i<cla.chord.n;i++)sqp_off(0,cla.chord.note[i]);
    if(restore && cpa.on && !cpa.paused && !cpa.clock_wait) {
        const cpa_slot_t *s=&cpa.plan.slot[cpa.slot];
        if(s->tie || cpa.elapsed<(uint32_t)s->ticks*s->gate/100u){cpa_tie_from=0;cpa_emit();}
    }
}
static void cla_tick1(void)
{ if(cla.on && cla.ticks && !--cla.ticks)cla_cancel(1); }
static void cla_process(uint32_t n)
{
    unsigned request=cla_mail.request;
    if(request) {
        cf_lab_chord_t chord=cla_mail.chord;cla_mail.request=0;
        if(request==2)cla_cancel(1);
        else {
            int valid=chord.n>0 && chord.n<=6 && chord.mask && chord.root<=127;
            for(unsigned i=0;i<chord.n && i<CF_MAX_NOTES;i++)if(chord.note[i]>127)valid=0;
            if(!valid || cfc.out.sounding || cfp.n || cfp.nb || sqp.on || sqp.rec!=255 ||
                (cpa.on && (cpa.paused || cpa.clock_wait))){cla.blocked=1;cla_cancel(1);return;}
            cla_cancel(0);sqp_chord_off();sqp_bass_off();sqp.dn=0;
            cla.chord=chord;cla.on=1;cla.blocked=0;cla.acc=0;
            cla.ticks=cfm.beats*SQ_PPQN;
            for(unsigned i=0;i<chord.n;i++)cfp_on(0,chord.note[i],cfp_cfg.vel);
        }
    }
    if(!cla.on)return;
    if(cfc.out.sounding || cfp.n || cfp.nb || sqp.on || sqp.rec!=255){cla_cancel(1);return;}
    if(!cpa.on) {
        cla.acc+=n*cfm.bpm*SQ_PPQN;
        unsigned limit=FS*60u*(cfm.beats==6?1u:2u)/2u;
        while(cla.on && cla.acc>=limit){cla.acc-=limit;cla_tick1();}
    }
}
/* Latest complete request, copied in a bounded critical section. Repeated
 * intentional auditions coalesce; navigation/stop cannot leave a queued start. */
static void cla_request(const cf_lab_chord_t *chord)
{
    fm1_irq_off();
    if(chord)cla_mail.chord=*chord;
    cla_mail.request=chord?1:2;
    fm1_irq_on();
}
