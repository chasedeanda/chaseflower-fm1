/* SPDX-License-Identifier: GPL-3.0-only
 * Full host HAL: FX UI -> DSP -> USB and persistence -> backup. */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)
static void render(unsigned blocks)
{
    int16_t out[HALF_FRAMES * 2];
    for (unsigned i=0; i<blocks; i++) { emu_fw_audio(out,HALF_FRAMES); }
}
static void controls(void)
{
    cf_set_t before = cs; cf_harmony_t harmony = ch;
    CHECK(sizeof(cs)==188 && sizeof(sq_ev_t)==22);
    cu.scr=SCR_HOME; ui_button(B_FX); CHECK(cu.scr==SCR_FX && fxpu.mode==FXP_EDIT);
    ui_button(B_FX); CHECK(cu.scr==SCR_FX && fxpu.mode==FXP_BROWSE);
    ui_select(1); CHECK(fxpu.pick==1 && !memcmp(&cs,&before,sizeof cs));
    ui_knob_turn(0,1); CHECK(!memcmp(&cs.fx,&FXP_FACTORY[1].fx,sizeof cs.fx));
    before.fx=cs.fx; CHECK(!memcmp(&cs,&before,sizeof cs) && !memcmp(&ch,&harmony,sizeof ch));
    CHECK(cfx_pending); render(40); CHECK(!cfx_pending && cfx_blend==32768);
    CHECK(!memcmp(&cfx_set,&cs.fx,sizeof cs.fx));
    ui_knob_turn(1,1); render(30); CHECK(cfx_bypass && !cfx_blend);
    ui_knob_turn(1,-1); render(30); CHECK(!cfx_bypass && cfx_blend==32768);
    ui_knob_turn(2,1); CHECK(memcmp(&cs.fx,&fxpu.prev,sizeof cs.fx)!=0); render(40);
    CHECK(!memcmp(&cs.fx,&CFX_DEFAULT,sizeof cs.fx));
    /* Rapid recall keeps the final target and unrelated configuration. */
    for(unsigned i=0;i<FXP_FACTORY_N;i++) { fxpu.pick=i; ui_knob_turn(0,1); render(1); }
    render(40); CHECK(!cfx_pending && !memcmp(&cfx_set,&FXP_FACTORY[7].fx,sizeof cs.fx));
    ui_button(B_SAVE); CHECK(fxpu.mode==FXP_MANAGE && !cu.save_hold);
    fxpu.slot=3; fxpu.action=0; memset(fxpu.name,0,sizeof fxpu.name); strcpy(fxpu.name,"MY SPACE");
    /* The actual held SAVE path. */
    cu.now=100; ui_button(B_SAVE); CHECK(cu.save_hold==100);
    fm1_in.buttons |= 1u << panel.btn[B_SAVE]; cu.now=1100; cf_save_poll();
    fm1_in.buttons &= ~(1u << panel.btn[B_SAVE]);
    CHECK(!cu.save_hold && fxpu.mode==FXP_BROWSE && fxp_bank.used==(1u<<3));
    CHECK(fxp_pending() && !strcmp(fxp_bank.slot[3].name,"MY SPACE"));
    CHECK(fxp_write(ST_NOERASE)==0 && !fxp_pending());
    fxp_bank_t saved=fxp_bank; fxp_bank_init(&fxp_bank); fxp_load(); CHECK(!memcmp(&saved,&fxp_bank,sizeof saved));
    fxpu.pick=FXP_FACTORY_N+3; cs.fx=CFX_DEFAULT; fxp_recall_selected(); render(40);
    CHECK(!memcmp(&cs.fx,&saved.slot[3].fx,sizeof cs.fx));
    /* New never replaces an occupied user slot; no user operation edits a factory. */
    fxp_manage_open(); fxpu.slot=3; fxpu.action=0; fxp_manage_do(); CHECK(!memcmp(&saved,&fxp_bank,sizeof saved));
    fxpu.action=1; cs.fx=FXP_FACTORY[0].fx; fxp_manage_do(); CHECK(memcmp(&saved,&fxp_bank,sizeof saved)!=0);
    fxp_manage_open(); fxpu.slot=3; fxpu.action=2; fxp_manage_do(); CHECK(!fxp_bank.used);
    fxpu.mode=FXP_BROWSE; ui_button(B_FX); CHECK(cu.scr==SCR_HOME);
    cu.scr=SCR_FX; fxpu.mode=FXP_MANAGE; cu.save_hold=10; ui_select(1); CHECK(!cu.save_hold);
    cr_safe=1; CHECK(!fxp_save_slot(0,"SAFE")); cr_safe=0;
    cfs_restore_lock=1; CHECK(!fxp_save_slot(0,"RESTORE")); cfs_restore_lock=0;
}
static void storage(void)
{
    CHECK(st_sector(OBJ_CFXPRESET,0)==0xB2000 && st_sector(OBJ_CFXPRESET,1)==0xB3000);
    for(unsigned a=0;a<OBJ_COUNT;a++) for(unsigned b=a+1;b<OBJ_COUNT;b++)
        for(unsigned c=0;c<2;c++) for(unsigned d=0;d<2;d++) CHECK(st_sector(a,c)!=st_sector(b,d));
    CHECK(ST_LSZ(OBJ_CFXPRESET)==1024 && ST_LMAX(OBJ_CFXPRESET)>=sizeof(fxp_bank_t));
    CHECK(crb_obj(CFB_FX)==OBJ_CFXPRESET && crb_size_ok(CFB_FX,sizeof(fxp_bank_t)) && !crb_size_ok(CFB_FX,5));
    unsigned erased=st_erases; int rc=0;
    for(unsigned i=0;i<200 && !rc;i++) {
        cs.fx=FXP_FACTORY[i%FXP_FACTORY_N].fx; cs.fx.v[CFX_DELAY][0]=i%101; CHECK(fxp_save_slot(i%FXP_SLOTS,"ENDURANCE"));
        rc=fxp_write(ST_NOERASE); CHECK(rc>=0 && st_erases==erased);
    }
    CHECK(rc==1 && fxp_pending());
    felucca_dbg.last_us=0; fxp_retry=0; fxp_poll(cu.now+2000,0);
    CHECK(fxp_pending() && st_erases==erased);
    fxp_retry=0; fxp_poll(cu.now+4000,1);
    CHECK(!fxp_pending() && st_erases>erased);
    fxp_bank_t saved=fxp_bank; fxp_load(); CHECK(!memcmp(&saved,&fxp_bank,sizeof saved));
    /* A well-framed future format or corrupt payload remains untouched. */
    saved.version=99; CHECK(!st_save(OBJ_CFXPRESET,&saved,sizeof saved));
    fxp_load(); CHECK(fxp_protected && !fxp_save_slot(0,"NO"));
    erased=emu_flash_writes; fxp_poll(cu.now+10000,1); CHECK(emu_flash_writes==erased);
    CHECK(!st_save(OBJ_CFXPRESET,0,0)); fxp_load(); CHECK(!fxp_protected && !fxp_bank.used);
}
static unsigned transition_jump(int smooth)
{
    cf_fx_t f=CFX_DEFAULT; int32_t l[CTL],r[CTL],last=0; unsigned jump=0;
    memset(&cfx,0,sizeof cfx); cfx_pending=cfx_bypass=0; cfx_blend=32768;
    cf_boot_go=0; cfb.on=0; f.on=1u<<CFX_DIST; f.v[CFX_DIST][0]=0; f.v[CFX_DIST][2]=80;
    cf_fx_apply(&f);
    for(unsigned b=0;b<200;b++) {
        for(unsigned i=0;i<CTL;i++) l[i]=r[i]=10000;
        cf_fx_chain(l,r,CTL); last=l[CTL-1];
    }
    f.v[CFX_DIST][2]=0;
    if(smooth) cf_fx_recall(&f); else cf_fx_apply(&f);
    for(unsigned b=0;b<60;b++) {
        for(unsigned i=0;i<CTL;i++) l[i]=r[i]=10000;
        cf_fx_chain(l,r,CTL);
        for(unsigned i=0;i<CTL;i++) { unsigned d=(unsigned)abs(l[i]-last); if(d>jump)jump=d; last=l[i]; }
    }
    CHECK(!cfx_pending && cfx_blend==32768 && !last);
    return jump;
}
static void render_presets(unsigned capture,unsigned hz,const char *path)
{
    FILE *f=fopen(path,"wb"); CHECK(f); if(!f)return;
    int32_t out[CTL*2]; unsigned nonzero=0;
    cf_boot_go=0; cfb.on=0; cf_intro_on=0;
    for(unsigned b=0;b<64;b++) audio_block(out,CTL);
    cf_post(CFE_KEY,1,0,1); cf_post(CFE_KEY,1,0,0); cf_post(CFE_KEY,7,0,1);
    sq_fresh(&sq_st.pat[0],4); dm_fill(&sq_st.pat[0],dm_nth(4,0),0);
    sqp_start(0,0); cfm_restart(); cfm.running=1;
    ua_reset(); ua.cap_alt=capture; ua.cap_hold=0; usb.config=usb.up=1; ua_rate_set(hz);
    for(unsigned b=0;b<4800;b++) {
        if(b%500==0) {
            unsigned queue=cf_evq_w; cf_chord_out_t held=cfc.out;
            fxpu.pick=(b/500)%FXP_FACTORY_N; fxp_recall_selected();
            CHECK(queue==cf_evq_w && !memcmp(&held,&cfc.out,sizeof held));
        }
        if(b%700==0) cf_fx_bypass((b/700)&1);
        unsigned w=ua.cw; audio_block(out,CTL);
        CHECK(cfc.out.sounding && sqp.on && cfc.cfg.algo==CF_ALG_TOP_LOCK);
        fwrite(out,sizeof out,1,f);
        for(unsigned i=0;i<CTL*2;i++) nonzero+=out[i]!=0;
        if(capture) {
            unsigned expected=hz==48000 ? ((b+1)*CTL*160u+146u)/147u : (b+1)*CTL;
            CHECK(ua.cw==expected);
            if(hz==44100) for(unsigned i=w;i<ua.cw;i++) {
                const int16_t *frame=ua.cap+(i&(UA_CAP_RING-1))*6;
                CHECK(frame[0]==ua_sat(out[2*(i-w)]>>8) && frame[1]==ua_sat(out[2*(i-w)+1]>>8));
            }
            ua.cr=ua.cw;
        } else CHECK(ua.cw==w);
    }
    CHECK(nonzero>0); fclose(f);
}
static void shots(const char *dir)
{
    cf_intro_on=0; cu.scr=SCR_FX; cu.pop=POP_NONE;
    for(unsigned mode=0;mode<3;mode++) {
        fxpu.mode=mode; fxpu.pick=0;
        if(mode==FXP_MANAGE) fxp_manage_open();
        cf_ui_draw();
        char path[1024]; snprintf(path,sizeof path,"%s/fx-%u.ppm",dir,mode);
        FILE *f=fopen(path,"wb"); CHECK(f); if(!f)continue;
        fprintf(f,"P6\n240 240\n255\n");
        for(unsigned i=0;i<FB_W*FB_H;i++) {
            uint32_t rgb=cf_pal[fb[i]&63]; uint8_t p[3]={(uint8_t)(rgb>>16),(uint8_t)(rgb>>8),(uint8_t)rgb};
            fwrite(p,3,1,f);
        }
        fclose(f);
    }
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0);
    cu.booted=1; render(10);
    for(unsigned i=0;i<FXP_FACTORY_N;i++) CHECK(fxp_preset_valid(&FXP_FACTORY[i]));
    if(argc==5 && !strcmp(argv[1],"--render")) render_presets(atoi(argv[2]),atoi(argv[3]),argv[4]);
    else if(argc==3 && !strcmp(argv[1],"--shots")) shots(argv[2]);
    else {
        controls(); storage();
        unsigned hard=transition_jump(0), smooth=transition_jump(1);
        printf("FX transition max sample step: direct %u, controlled %u (constant input fixture)\n",hard,smooth);
        CHECK(hard>0 && smooth<hard/4);
    }
    printf("FX workflow/storage: %u checks, %u failures\n",checks,failures); return failures!=0;
}
