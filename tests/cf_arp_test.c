/* SPDX-License-Identifier: GPL-3.0-only */
#define CF_SCENE_FIXTURE_ONLY 1
#include "cf_scene_free_test.c"
static void ms(unsigned n){blocks((n*FS+999)/1000/CTL+1);}
static void setup(unsigned mode)
{
    cpa_stop();sqp_stop();dmp_stop();cfp_panic();cf_intro_on=cf_boot_go=cfb.on=0;
    cu.scr=SCR_ARP;cu.page[SCR_ARP]=0;cu.pop=0;cf_motion=0;
    cs.chord.algo=CF_ALG_TOP_LOCK;cs.chord.quantize=CF_Q_BASIC;cs.chord.scale=CF_SC_MAJOR;cs.chord.tonic=0;
    cf_harmony_defaults(&ch);ch.mode=CF_H_AUTO;cs.clock_in=0;cs.bpm=120;cs.beats=4;cs.bass_on=1;
    cu.arp_on=1;cs.arp=(cf_arp_t)CF_ARP_DEFAULT;cs.arp.mode=mode;cs.arp.rate=6;
    cf_send_config();cf_send_metro();blocks(2);cu.now=cf_now_ms;
}
static void key(unsigned k,int down){cf_post(CFE_KEY,k,0,down);blocks(2);}
static void capture(const char *dir,const char *name)
{
    if(!dir)return;char path[512];snprintf(path,sizeof path,"%s/%s.ppm",dir,name);FILE *f=fopen(path,"wb");CHECK(f);if(!f)return;
    cu.now=cf_now_ms;cf_ui_draw();fprintf(f,"P6\n240 240\n255\n");
    for(unsigned p=0;p<FB_W*FB_H;p++){uint32_t rgb=cf_pal[fb[p]];uint8_t pixel[]={rgb>>16,rgb>>8,rgb};fwrite(pixel,1,3,f);}fclose(f);
}
static void walks(void)
{
    CHECK(CF_ARP_NMODES==13 && CF_ARP_STRUM_UP==7 && CF_ARP_STRUM_DOWN==8 && sizeof(cs)==188);
    static const struct {unsigned mode,n;uint8_t k[8];} fixture[]={
        {CF_ARP_CONVERGE,4,{0,3,1,2}}, {CF_ARP_DIVERGE,4,{1,2,0,3}},
        {CF_ARP_LOW_PEDAL,6,{0,1,0,2,0,3}}, {CF_ARP_HIGH_PEDAL,6,{3,0,3,1,3,2}},
        {CF_ARP_INC,8,{0,1,2,3,3,2,1,0}}, {CF_ARP_EXC,6,{0,1,2,3,2,1}}};
    for(unsigned f=0;f<NELEM(fixture);f++)for(unsigned i=0;i<fixture[f].n*3;i++)
        CHECK(cf_arp_walk(fixture[f].mode,4,i)==fixture[f].k[i%fixture[f].n]);
    for(unsigned mode=0;mode<CF_ARP_NMODES;mode++) {
        CHECK(CF_ARP_STRUMMING(mode)==(mode==7 || mode==8));
        cf_set_t saved=cs;saved.arp.mode=mode;cfs_sanitize(&saved);CHECK(saved.arp.mode==mode);
        for(unsigned len=1;len<=48;len++)for(unsigned i=0;i<200;i++)CHECK(cf_arp_walk(mode,len,i)<len);
    }
}
static void live(const char *dir)
{
    for(unsigned mode=0;mode<CF_ARP_NMODES;mode++) {
        setup(mode);key(9,1);CHECK(cfa_k.n==3);ms(600);
        CHECK(cf_sn.arp_view.n==3 && cf_sn.arp_view.count && cf_sn.arp_view.mode==mode);
        CHECK(cf_sn.arp_view.trace[(cf_sn.arp_view.count-1)%16]==cfa_k.last_note);
        cfarp_model_t m;cfarp_model(&m);CHECK(m.run && m.hi>=m.lo && m.n<=48 && m.chord[0]);
        if(mode==CF_ARP_RAND) {CHECK(m.known==cfa_k.seen);for(unsigned i=0;i<16;i++)if((m.known>>i)&1)CHECK(m.seq[i]==cfa_k.trace[i]);}
        unsigned phase=cfa_k.idx,writes=emu_flash_writes;cf_arp_t arp=cs.arp;cf_chord_out_t chord=cfc.out;
        ui_algo(1);CHECK(cu.page[SCR_ARP]==1);cfarp_model(&m);CHECK(m.page && m.run);
        CHECK(!memcmp(&arp,&cs.arp,sizeof arp) && cfa_k.idx==phase && !memcmp(&chord,&cfc.out,sizeof chord));
        if(mode>=CF_ARP_CONVERGE) {char name[32];snprintf(name,sizeof name,"pattern-%u",mode);capture(dir,name);}
        if(CF_ARP_STRUMMING(mode)) {cf_knob_t k={0};ui_knob_info(0,&k);CHECK(k.dis);ui_knob_turn(0,1);CHECK(cs.arp.gate==arp.gate);ui_knob_turn(1,1);CHECK(cs.arp.swing==arp.swing);}
        CHECK(writes==emu_flash_writes);key(9,0);CHECK(!cfa_k.n && !cfa_k.cur && !cfa_k.sn);
    }
    setup(CF_ARP_UP);cu.page[SCR_ARP]=1;ui_knob_turn(2,1);blocks(2);key(9,1);ms(170);key(9,0);
    CHECK(cfa_k.latched && cfa_k.n);cfarp_model_t m;cfarp_model(&m);CHECK(!strcmp(m.owner,"Latch holding"));
    capture(dir,"latch");ui_knob_turn(2,-1);blocks(2);CHECK(!cfa_k.n && !cfa_k.cur);
    setup(CF_ARP_UP);cs.arp.oct=4;cf_send_config();blocks(2);key(9,1);ms(900);cfarp_model(&m);CHECK(m.hi-m.lo>=36);capture(dir,"four-octaves");
    key(9,0);setup(CF_ARP_UP);cfarp_valid=0;cf_ui_draw();unsigned draws=cfarp_draws,tiles=fb_tiles_sent;
    cu.now+=5000;cf_ui_draw();CHECK(cfarp_draws==draws && tiles==fb_tiles_sent);capture(dir,"waiting");
    key(9,1);ms(380);capture(dir,"pattern");
    cf_arp_view_t before=cf_sn.arp_view;unsigned rng=cfa_k.rng;
    for(unsigned i=0;i<40;i++){cu.now+=13;cf_ui_draw();}
    CHECK(!memcmp(&before,&cf_sn.arp_view,sizeof before) && cfa_k.rng==rng);
    cf_motion=0;cfarp_model(&m);CHECK(!m.travel);cf_ui_draw();unsigned hash=cfs_fnv(fb,FB_W*FB_H);cu.now+=777;cf_ui_draw();CHECK(hash==cfs_fnv(fb,FB_W*FB_H));
    ui_algo(1);capture(dir,"timing");ui_algo(1);cf_motion=1;
    if(dir)for(unsigned frame=0;frame<60;frame++){ms(50);char name[32];snprintf(name,sizeof name,"motion-%03u",frame);capture(dir,name);}
    key(9,0);
    setup(CF_ARP_STRUM_DOWN);cf_chord_out_t high={0};high.sounding=1;high.root_pc=0;high.root_note=120;high.mask=0x91;high.n=3;high.note[0]=120;high.note[1]=124;high.note[2]=127;
    cfp_cfg.arp.oct=4;cs.arp.oct=4;cfa_chord(&high,3);ms(300);cfarp_model(&m);CHECK(m.n==3 && m.seq[0]==127 && m.seq[2]==120);
    setup(CF_ARP_STRUM_UP);key(9,1);ms(160);cfp_cfg.arp.oct=4;cs.arp.oct=4;cf_snapshot();cfarp_model(&m);CHECK(m.n==cfa_k.sn && m.n==3);key(9,0);
    setup(CF_ARP_UP);cs.bpm=108;cs.arp.rate=4;cf_send_config();blocks(2);key(3,1);key(3,0);key(9,1);ms(600);capture(dir,"hero");
    if(dir)for(unsigned frame=0;frame<60;frame++){ms(50);char name[32];snprintf(name,sizeof name,"motion-%03u",frame);capture(dir,name);}
    key(9,0);
}
static void progression(void)
{
    setup(CF_ARP_CONVERGE);cu.scr=SCR_HOME;cpg.idea=0;cpg.rhythm=CPA_OFF;cpg.mode=CPA_BOTH;
    const cf_progression_t *p=cf_progression_get(0);cs.chord.tonic=p->tonic;cs.chord.scale=p->scale;cs.beats=p->beats;
    cf_send_config();blocks(2);ui_button(B_PLAY);blocks(2);ms(150);
    CHECK(cpa.on && cfa_sq[0].n && !sqp.n && sqp.nb && cf_sn.arp_view.source==1);
    unsigned bass=sqp.bass[0];ms(300);CHECK(sqp.bass[0]==bass && cf_sn.arp_view.count>1);
    key(9,1);CHECK(cfa_k.n && !cfa_sq[0].n && cf_sn.arp_view.source==0);
    key(9,0);CHECK(!cfa_k.n && cfa_sq[0].n && cf_sn.arp_view.source==1);
    key(9,1);cu.scr=SCR_ARP;cu.page[SCR_ARP]=0;ui_knob_turn(0,-1);blocks(2);key(9,0);CHECK(!cfa_sq[0].n && sqp.n);
    ui_knob_turn(0,1);blocks(2);CHECK(cfa_sq[0].n && !sqp.n);
    unsigned slot=cpa.slot;ms(2200);CHECK(cpa.on && cpa.slot!=slot && cfa_sq[0].mask==cpa.plan.slot[cpa.slot].mask);
    cu.scr=SCR_ARP;cu.page[SCR_ARP]=0;ui_knob_turn(0,-1);blocks(2);CHECK(!cfa_sq[0].on && sqp.n);
    ui_knob_turn(0,1);blocks(2);CHECK(cfa_sq[0].n && !sqp.n);
    cpa_realtime(0xFC);CHECK(cpa.paused && !cfa_sq[0].n);cpa_realtime(0xFB);blocks(2);CHECK(!cpa.paused && cfa_sq[0].n);
    cpa_stop();cs.clock_in=1;cf_send_config();blocks(2);cu.scr=SCR_HOME;ui_button(B_PLAY);blocks(2);
    CHECK(cpa.clock_wait && !cfa_sq[0].n);cpa_tick(0,1);blocks(2);CHECK(cfa_sq[0].n);
    ms(350);CHECK(cpa.clock_lost && !cfa_sq[0].n);cpa_realtime(0xFB);cpa_tick(0,1);blocks(2);CHECK(cfa_sq[0].n);
    cf_post(CFE_PANIC,0,0,0);blocks(2);CHECK(!cpa.on && !cfa_k.n && !cfa_sq[0].n && !sqp.nb);
}
static void audio(unsigned rate,const char *path,unsigned drawing)
{
    setup(CF_ARP_UP);cf_motion=drawing;key(9,1);ua_reset();ua.cap_alt=1;ua.cap_hold=0;ua_rate_set(rate);
    FILE *file=fopen(path,"wb");CHECK(file);if(!file)return;int32_t out[CTL*2];unsigned writes=emu_flash_writes,energy=0;
    for(unsigned mode=0;mode<CF_ARP_NMODES;mode++) {
        cs.arp.mode=mode;cf_send_config();
        for(unsigned b=0;b<FS/CTL/3;b++) {
            uint32_t w=ua.cw;audio_block(out,CTL);fwrite(out,sizeof out,1,file);
            for(unsigned i=0;i<CTL*2;i++)energy|=out[i];
            for(unsigned i=w;i<ua.cw;i++)fwrite(ua.cap+(i&(UA_CAP_RING-1))*6,6*sizeof(int16_t),1,file);ua.cr=ua.cw;
            while(mo_r!=mo_w){uint32_t event=midi_out_q[mo_r++%MQ];fwrite(&event,sizeof event,1,file);}
            cf_snapshot();cu.now=cf_now_ms;if(drawing && b%17==0)cf_ui_draw();
        }
    }
    key(9,0);cu.scr=SCR_HOME;cpg.idea=0;cpg.rhythm=CPA_OFF;cpg.mode=CPA_BOTH;
    const cf_progression_t *idea=cf_progression_get(0);cs.chord.tonic=idea->tonic;cs.chord.scale=idea->scale;cs.beats=idea->beats;
    cf_send_config();blocks(2);ui_button(B_PLAY);blocks(2);cu.scr=SCR_ARP;
    for(unsigned b=0;b<FS*3/CTL;b++) {
        if(b==FS/CTL) {cs.arp.mode=CF_ARP_RAND;cf_send_config();}
        uint32_t w=ua.cw;audio_block(out,CTL);fwrite(out,sizeof out,1,file);
        for(unsigned i=w;i<ua.cw;i++)fwrite(ua.cap+(i&(UA_CAP_RING-1))*6,6*sizeof(int16_t),1,file);ua.cr=ua.cw;
        while(mo_r!=mo_w){uint32_t event=midi_out_q[mo_r++%MQ];fwrite(&event,sizeof event,1,file);}
        cf_snapshot();cu.now=cf_now_ms;if(drawing && b%17==0)cf_ui_draw();
    }
    cpa_stop();CHECK(energy && writes==emu_flash_writes);fclose(file);
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    if(argc==5)audio(atoi(argv[2]),argv[3],atoi(argv[4]));else {walks();live(argc==3?argv[2]:0);progression();}
    printf("ARP roll: %u checks, %u failures\n",checks,failures);return failures!=0;
}
