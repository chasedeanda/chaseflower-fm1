/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; if (failures < 20) printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)
static void blocks(unsigned n) { int32_t out[CTL*2]; while(n--) audio_block(out,CTL); cf_snapshot(); }
static void quiet(void)
{
    cf_boot_go=0; cfb.on=0; cf_intro_on=0;
    cf_post(CFE_PANIC,0,0,0); cf_post(CFE_SEQ,0,0,0); cf_post(CFE_DRUM,0,0,0);
    cfm.running=0; cu.keys=0; blocks(6000);
}
static void properties(void)
{
    cv_value_t base,a,b;
    cvr_capture(&base,0); CHECK(cvr_value_valid(&base));
    for(unsigned seed=1;seed<=2000;seed++) for(unsigned ctx=0;ctx<CV_N;ctx++) for(unsigned level=0;level<3;level++) {
        a=b=base;
        int ok=cvr_generate(&a,ctx,level,seed);
        CHECK(ok); CHECK(cvr_value_valid(&a));
        CHECK(cvr_generate(&b,ctx,level,seed)==ok && !memcmp(&a,&b,sizeof a));
        if(ctx==CV_DRUMS) { CHECK(b.bpm==base.bpm && b.beats==base.beats); }
        if(ctx==CV_ARP) { CHECK(b.arp.latch==base.arp.latch); if(level==1)CHECK(b.arp.mode!=CF_ARP_RAND && !CF_ARP_STRUMMING(b.arp.mode)); }
        if(ctx==CV_SCALE) { CHECK(!base.auto_on || cf_harmony_supported(b.scale,b.quantize)); if(level<2)CHECK(b.tonic==base.tonic); }

    }
    for(unsigned scale=0;scale<CF_NSCALES;scale++) for(unsigned quant=0;quant<=CF_Q_FULL;quant++) {
        base.scale=scale;base.quantize=quant;
        for(unsigned ctx=0;ctx<CV_N;ctx++) { a=base;if(cvr_generate(&a,ctx,2,42)) CHECK(cvr_value_valid(&a)); }
    }
    /* Chaos can reach the complete explorer catalog, including entries beyond
     * the old 64-element temporary choice array. It does not alter key/scale. */
    uint8_t seen[CF_P_NONE]={0}; unsigned reached=0;
    for(unsigned seed=1;seed<=4000;seed++) {
        a=base; CHECK(cvr_generate(&a,CV_HARMONY,2,seed));
        CHECK(a.tonic==base.tonic && a.scale==base.scale);
        if(!seen[a.idea]) { seen[a.idea]=1; reached++; }
    }
    CHECK(reached==cf_progression_count());
    a=base; CHECK(!cvr_generate(&a,99,0,1)); CHECK(!cvr_generate(&a,CV_FX,3,1));
}
static void controls(void)
{
    cf_set_t original=cs;cf_harmony_t harmony=ch;
    uint32_t patterns=cfs_fnv(&sq_st,sizeof sq_st),bank=cfs_fnv(&up_bank,sizeof up_bank),flash=emu_flash_writes;
    CHECK(sizeof(cs)==188 && sizeof(sq_ev_t)==22);
    cu.scr=SCR_ARP;ui_button(B_GLO);CHECK(cvr.context==CV_ARP);
    cu.page[SCR_GLO]=CF_GLO_VARIATION;cvr.level=1;
    cf_arp_t before=cs.arp;
    ui_knob_turn(2,1);cf_arp_t generated=cs.arp;CHECK(cvr.have_previous);
    ui_knob_turn(3,1);CHECK(!memcmp(&cs.arp,&before,sizeof before));
    ui_knob_turn(3,1);CHECK(!memcmp(&cs.arp,&generated,sizeof generated));
    CHECK(emu_flash_writes==flash && !memcmp(&ch,&harmony,sizeof harmony));
    CHECK(cfs_fnv(&sq_st,sizeof sq_st)==patterns && cfs_fnv(&up_bank,sizeof up_bank)==bank);
    /* Presets keep the existing save/confirmation UI. No random action saves a bank. */
    cvr.context=CV_FX;fxp_bank_t fxbank=fxp_bank;ui_button(B_SAVE);
    CHECK(cu.scr==SCR_FX && fxpu.mode==FXP_MANAGE && !cu.save_hold);
    CHECK(!memcmp(&fxbank,&fxp_bank,sizeof fxbank));
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=CF_GLO_VARIATION;cvr.context=CV_SOUND;cvr.part=1;
    ui_button(B_SAVE);CHECK(cu.scr==SCR_SAVE && cu.page[SCR_SAVE]==1 && cu.save_new);
    CHECK(cfs_fnv(&up_bank,sizeof up_bank)==bank && emu_flash_writes==flash);
    cu.scr=SCR_GLO;cu.page[SCR_GLO]=CF_GLO_VARIATION;
    for(unsigned ctx=0;ctx<CV_N;ctx++) for(unsigned level=0;level<3;level++) {
        cvr.context=ctx;cvr.level=level;
        for(int k=0;k<4;k++){cf_knob_t knob={0};ui_knob_info(k,&knob);}cf_ui_draw();
    }
    ui_knob_turn(0,1000);CHECK(cvr.context==CV_N-1);ui_knob_turn(1,-1000);CHECK(cvr.level==0);
    ui_select(1);CHECK(cu.page[SCR_GLO]==0 && screen_pages(SCR_GLO)==CF_GLO_NPAGES);
    cs=original;ch=harmony;cf_send_config();quiet();
}
static void sound(void)
{
    for(unsigned part=0;part<3;part++) {
        cv_value_t before,after,undo;
        cvr_capture(&before,part); cvr.context=CV_SOUND;cvr.part=part;cvr.level=2;
        cvr_new(); cvr_capture(&after,part); CHECK(memcmp(before.env,after.env,30));
        cvr_undo(); cvr_capture(&undo,part);CHECK(!memcmp(&before,&undo,sizeof before));
        cvr_undo(); cvr_capture(&undo,part);CHECK(!memcmp(&after,&undo,sizeof after));
        /* Never apply a saved envelope to an unrelated patch. */
        cvr.previous.sound.name[0]^=1;unsigned q=cf_evq_w;cvr_undo();CHECK(cf_evq_w==q);
    }
#if FELUCCA_VA
    CHECK(cf_snd_find(0,ENGI_VA,"SOFT LEAD"));
    cv_value_t before,after;cvr_capture(&before,0);CHECK(before.env_kind==2);
    cvr.part=0;cvr_new();cvr_undo();cvr_capture(&after,0);CHECK(!memcmp(&before,&after,sizeof before));
#endif
}
static void boundaries(void)
{
    quiet();cvr.context=CV_DRUMS;cvr.level=2;cvr.part=0;
    dm_rec_t old=dmr;uint8_t view=dm_view;uint16_t bpm=cs.bpm;
    cvr_new();dm_rec_t generated=dmr;CHECK(cs.bpm==bpm);cvr_undo();CHECK(!memcmp(&old,&dmr,sizeof dmr)&&dm_view==view);
    cvr_undo();CHECK(!memcmp(&generated,&dmr,sizeof dmr));
    /* The actual ISR retains the current groove until its next bar. */
    dm_play(dm_cur());blocks(1);CHECK(dmp.on);unsigned playing=dmp.r;
    cvr_new();unsigned chosen=dm_cur();blocks(1);
    CHECK(dmp.r==playing && dmp.next==chosen);
    dmp_next_bar();CHECK(dmp.r==chosen && dmp.next==0xFF && dmp.t==0);
    cvr_undo();blocks(1);CHECK(dmp.r==chosen);dmp_next_bar();CHECK(dmp.r==playing);
    /* Recording cannot be randomized; no saved sequencer data changes. */
    cf_snapshot();uint32_t pat=cfs_fnv(&sq_st,sizeof sq_st);cf_arp_t arp=cs.arp;
    cvr.context=CV_ARP;unsigned seed=cvr.seed;cvr_new();CHECK(seed==cvr.seed&&!memcmp(&arp,&cs.arp,sizeof arp));
    CHECK(cfs_fnv(&sq_st,sizeof sq_st)==pat);
    quiet();
}
/* Rejected changes while playing must produce bit-identical physical and all
 * six USB channels, including a held Auto chord, bass, arp, rhythm and FX. */
static void audio(int attempt,unsigned rate,const char *path)
{
    FILE *file=fopen(path,"wb");CHECK(file);if(!file)return;
    int32_t out[CTL*2];cf_intro_on=0;cf_boot_go=0;
    ch.mode=CF_H_AUTO;cu.arp_on=1;cf_send_config();blocks(64);
    cf_post(CFE_KEY,1,0,1);cf_post(CFE_KEY,1,0,0);cf_post(CFE_KEY,7,0,1);
    dm_play(dm_cur());blocks(2);
    ua_reset();ua.cap_alt=1;ua.cap_hold=0;ua_rate_set(rate);
    cvr.context=CV_SCALE;uint32_t flash=emu_flash_writes;
    for(unsigned b=0;b<FS*2u/CTL;b++) {
        if(attempt&&b%17==0) { cvr_new();CHECK(!cvr.have_previous); }
        uint32_t w=ua.cw;audio_block(out,CTL);fwrite(out,sizeof out,1,file);
        for(unsigned i=w;i<ua.cw;i++)fwrite(ua.cap+(i&(UA_CAP_RING-1u))*6u,6*sizeof(int16_t),1,file);
        ua.cr=ua.cw;
    }
    CHECK(flash==emu_flash_writes && cfc.out.sounding && dmp.on);fclose(file);
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    if(argc==5)audio(atoi(argv[2]),(unsigned)atoi(argv[3]),argv[4]);
    else { quiet();properties();controls();sound();boundaries(); }
    printf("variations: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
