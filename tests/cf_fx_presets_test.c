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
    cf_set_t before=cs;cf_harmony_t harmony=ch;
    CHECK(sizeof(cs)==188 && sizeof(sq_ev_t)==22);
    cu.scr=SCR_HOME;ui_button(B_FX);CHECK(cu.scr==SCR_FX && fxpu.mode==FXP_BROWSE);
    ui_algo(1);CHECK(fxpu.mode==FXP_EDIT);ui_algo(-1);CHECK(fxpu.mode==FXP_BROWSE);
    uint8_t sounds[sizeof cf_snd];memcpy(sounds,cf_snd,sizeof sounds);
    ui_select(1);CHECK(fxpu.pick==1 && !memcmp(&cs,&before,sizeof cs));
    ui_button(B_SCL);CHECK(!memcmp(&cs.fx,&FXP_FACTORY[1].fx,sizeof cs.fx));
    CHECK(!memcmp(sounds,cf_snd,sizeof sounds));
    before.fx=cs.fx;CHECK(!memcmp(&cs,&before,sizeof cs) && !memcmp(&ch,&harmony,sizeof ch));
    CHECK(cfx_pending);render(40);CHECK(!cfx_pending && cfx_blend==32768);
    CHECK(!memcmp(&cfx_set,&cs.fx,sizeof cs.fx));
    cf_fx_bypass(1);render(30);CHECK(cfx_bypass && !cfx_blend);
    cf_fx_bypass(0);render(30);CHECK(!cfx_bypass && cfx_blend==32768);
    cf_fx_bypass(1);render(30);ui_select(1);
    CHECK(fxpu.pick==2 && cfx_bypass && !cfx_pending);
    ui_button(B_SCL);render(40);CHECK(!cfx_bypass && !memcmp(&cfx_set,&FXP_FACTORY[2].fx,sizeof cs.fx));
    fxp_previous();render(40);CHECK(cfx_bypass && !memcmp(&cs.fx,&FXP_FACTORY[1].fx,sizeof cs.fx));
    for(unsigned i=0;i<FXP_FACTORY_N;i++){fxpu.pick=i;ui_button(B_SCL);render(1);}
    render(40);CHECK(!cfx_pending && !memcmp(&cfx_set,&FXP_FACTORY[FXP_FACTORY_N-1].fx,sizeof cs.fx));
    cf_fx_t last=cs.fx;cf_fx_bypass(1);ui_select(1);
    CHECK(fxpu.pick==FXP_FACTORY_N && !fxp_pick());
    ui_button(B_SCL);CHECK(!memcmp(&last,&cs.fx,sizeof last) && cfx_bypass);cu.pop=0;
    ui_select(1000);CHECK(fxpu.pick==FXP_FACTORY_N+FXP_SLOTS-1);
    ui_select(-1000);CHECK(fxpu.pick==0 && fxp_pick()==&FXP_FACTORY[0]);
    cf_fx_t previous=fxpu.prev;ui_select(-1);CHECK(fxpu.pick==0 && !memcmp(&previous,&fxpu.prev,sizeof previous));
    ui_button(B_SAVE);CHECK(fxpu.mode==FXP_MANAGE && !cu.save_hold);
    fxpu.slot=3;fxpu.action=0;memset(fxpu.name,0,sizeof fxpu.name);strcpy(fxpu.name,"MY SPACE");
    cu.now=100;ui_button(B_SAVE);CHECK(cu.save_hold==100);
    fm1_in.buttons|=1u<<panel.btn[B_SAVE];cu.now=1100;cf_save_poll();fm1_in.buttons&=~(1u<<panel.btn[B_SAVE]);
    CHECK(!cu.save_hold && fxpu.mode==FXP_BROWSE && fxp_bank.used==(1u<<3));
    CHECK(fxp_pending() && !strcmp(fxp_bank.slot[3].name,"MY SPACE"));
    CHECK(fxp_write(ST_NOERASE)==0 && !fxp_pending());
    fxp_bank_t saved=fxp_bank;fxp_bank_init(&fxp_bank);fxp_load();CHECK(!memcmp(&saved,&fxp_bank,sizeof saved));
    fxpu.pick=FXP_FACTORY_N+3;cs.fx=CFX_DEFAULT;fxp_recall_selected();render(40);
    CHECK(!memcmp(&cs.fx,&saved.slot[3].fx,sizeof cs.fx));
    fxp_manage_open();fxpu.slot=3;fxpu.action=0;fxp_manage_do();CHECK(!memcmp(&saved,&fxp_bank,sizeof saved));
    fxpu.action=1;cs.fx=FXP_FACTORY[1].fx;fxp_manage_do();CHECK(memcmp(&saved,&fxp_bank,sizeof saved)!=0);
    fxp_manage_open();fxpu.slot=3;fxpu.action=2;fxp_manage_do();CHECK(!fxp_bank.used);
    fxpu.mode=FXP_BROWSE;ui_button(B_FX);CHECK(cu.scr==SCR_HOME);
    cu.scr=SCR_FX;fxpu.mode=FXP_MANAGE;cu.save_hold=10;ui_select(1);CHECK(!cu.save_hold);
    cr_safe=1;CHECK(!fxp_save_slot(0,"SAFE"));cr_safe=0;
    cfs_restore_lock=1;CHECK(!fxp_save_slot(0,"RESTORE"));cfs_restore_lock=0;
}
static void clear_controls(void)
{
    cu.scr=SCR_FX;fxpu.mode=FXP_BROWSE;fxpu.pick=9;cfx_recall();render(40);
    cf_boot_go=0;cfb.on=0;int32_t l[CTL],r[CTL];unsigned tail=0;
    for(unsigned b=0;b<400;b++){
        for(unsigned i=0;i<CTL;i++)l[i]=r[i]=b<64?12000:0;
        cf_fx_chain(l,r,CTL);if(b>=336)for(unsigned i=0;i<CTL;i++)tail|=l[i]!=0 || r[i]!=0;
    }
    CHECK(tail);cf_set_t before=cs;
    uint8_t sounds[sizeof cf_snd];memcpy(sounds,cf_snd,sizeof sounds);unsigned queue=cf_evq_w;
    fxp_manage_open();fxpu.action=3;fxp_manage_do();
    CHECK(!cs.fx.on && cfx_bypass && cfx_pending);before.fx.on=0;
    CHECK(!memcmp(&before,&cs,sizeof cs) && !memcmp(sounds,cf_snd,sizeof sounds) && queue==cf_evq_w);
    render(40);CHECK(!cfx_set.on && !cfx_pending && !cfx_blend);
    for(unsigned i=0;i<CTL;i++){l[i]=(int32_t)i*301-4000;r[i]=3000-(int32_t)i*173;}
    cf_fx_chain(l,r,CTL);for(unsigned i=0;i<CTL;i++)CHECK(l[i]==(int32_t)i*301-4000 && r[i]==3000-(int32_t)i*173);
    fxp_clear();fxp_manage_open();fxpu.action=4;fxp_manage_do();render(40);
    CHECK(!cfx_bypass && cfx_blend==32768 && !memcmp(&cs.fx,&FXP_FACTORY[9].fx,sizeof cs.fx));
    fxp_previous();render(40);CHECK(!cs.fx.on && cfx_bypass && !cfx_blend);
    CHECK(cf_settings_write(0)==1);cs.fx=FXP_FACTORY[9].fx;CHECK(cf_settings_load() && !cs.fx.on);
    fxpu.pick=9;ui_button(B_SCL);render(40);CHECK(cs.fx.on && !cfx_bypass && cfx_blend==32768);
    fxp_clear();render(40);fxpu.mode=FXP_EDIT;ui_knob_turn(0,1);render(40);
    CHECK(!cfx_bypass && cfx_blend==32768 && cs.fx.on==(1u<<cfx_sel()));
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
    cu.scr=SCR_FX; fxpu.mode=FXP_BROWSE; fxpu.pick=FXP_FACTORY_N+FXP_SLOTS-1;
    for(unsigned b=0;b<FXP_FACTORY_N*500u;b++) {
        if(b%500==0) {
            unsigned queue=cf_evq_w; cf_chord_out_t held=cfc.out;
            ui_select(b ? 1 : -1000); /* Same SELECT journey as the physical FX browser. */
            CHECK(fxpu.pick==b/500);ui_button(B_SCL);CHECK(!cfx_bypass);
            CHECK(queue==cf_evq_w && !memcmp(&held,&cfc.out,sizeof held));
        }
        if(b%500==150) cf_fx_bypass(1);
        if(b%500==250) cf_fx_bypass(0);
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
    for(unsigned page=0;page<9;page++) {
        fxpu.mode=page<3?page:FXP_BROWSE;
        fxpu.pick=page<3?0:(page==7?FXP_FACTORY_N-1:(page-2)*FXP_GROUP_SIZE);
        if(fxpu.mode==FXP_MANAGE) fxp_manage_open();
        if(page==8) { fxpu.pick=0; fxp_clear(); cu.pop=POP_NONE; }
        cf_ui_draw();
        char path[1024]; snprintf(path,sizeof path,"%s/fx-%u.ppm",dir,page);
        FILE *f=fopen(path,"wb"); CHECK(f); if(!f)continue;
        fprintf(f,"P6\n240 240\n255\n");
        for(unsigned i=0;i<FB_W*FB_H;i++) {
            uint32_t rgb=cf_pal[fb[i]&63]; uint8_t p[3]={(uint8_t)(rgb>>16),(uint8_t)(rgb>>8),(uint8_t)rgb};
            fwrite(p,3,1,f);
        }
        fclose(f);
    }
}
static void catalog(void)
{
    CHECK(FXP_FACTORY_N==13);
    for(unsigned i=0;i<FXP_FACTORY_N;i++) {
        CHECK(fxp_preset_valid(&FXP_FACTORY[i]));
        for(unsigned j=0;j<i;j++) {
            const cf_fx_t *a=&FXP_FACTORY[i].fx,*b=&FXP_FACTORY[j].fx;
            int distinct=a->on!=b->on;
            CHECK(strcmp(FXP_FACTORY[i].name,FXP_FACTORY[j].name)!=0);
            /* Changing disabled parameters alone does not make a new sound. */
            unsigned ao[CFX_N],bo[CFX_N],an=0,bn=0;
            for(unsigned e=0;e<CFX_N;e++) {
                if((a->on>>e)&1) distinct |= memcmp(a->v[e],b->v[e],3)!=0;
                if((a->on>>a->order[e])&1) ao[an++]=a->order[e];
                if((b->on>>b->order[e])&1) bo[bn++]=b->order[e];
            }
            distinct |= an!=bn || memcmp(ao,bo,an*sizeof *ao)!=0;
            CHECK(distinct);
        }
    }
}
static void wav_u32(FILE *f,uint32_t n)
{
    uint8_t b[4]={n,n>>8,n>>16,n>>24}; fwrite(b,1,4,f);
}
/* A separate fresh process per factory/voice makes level comparisons reproducible.
 * Full-volume six-note Cmaj7 + a bass, held for 2 s then released for 1 s.
 * These host fixtures measure output behavior, not FM1 listening quality. */
static void audition(unsigned preset,unsigned fm,const char *path)
{
    CHECK(preset<FXP_FACTORY_N); if(preset>=FXP_FACTORY_N)return;
    unsigned found=0;
    for(unsigned i=0;i<cf_snd_count(CF_PART_CHORD);i++) {
        uint32_t e,pos; cf_snd_t meta; cf_snd_at(CF_PART_CHORD,i,&e,&pos); cf_snd_meta(e,pos,&meta);
        if(e==(fm?ENGI_FM6:ENGI_VA) && !strcmp(meta.name,fm?"PAD":"POLY KEYS")) {
            cf_snd_load_index(CF_PART_CHORD,i); found=1; break;
        }
    }
    CHECK(found); cf_boot_go=0; cfb.on=0; cf_intro_on=0;
    song.master_q12=4096; fx_output_set(1,1);
    cs.fx=FXP_FACTORY[preset].fx; cf_fx_apply(&cs.fx);
    int32_t out[CTL*2];
    for(unsigned b=0;b<64;b++)audio_block(out,CTL);
    static const uint8_t notes[]={48,55,60,64,67,71};
    for(unsigned i=0;i<sizeof notes;i++)trk_note_on(&trk[0],notes[i],100);
    trk_note_on(&trk[1],36,100);
    FILE *f=fopen(path,"wb"); CHECK(f); if(!f)return;
    unsigned blocks=FS*3/CTL,frames=blocks*CTL,peak=0,clips=0,count=0; double squares=0;
    fwrite("RIFF",1,4,f); wav_u32(f,36+frames*4); fwrite("WAVEfmt ",1,8,f); wav_u32(f,16);
    wav_u32(f,0x00020001u); wav_u32(f,FS); wav_u32(f,FS*4); wav_u32(f,0x00100004u);
    fwrite("data",1,4,f); wav_u32(f,frames*4);
    for(unsigned b=0;b<blocks;b++) {
        if(b==FS*2/CTL) {
            for(unsigned i=0;i<sizeof notes;i++)trk_note_off(&trk[0],notes[i]);
            trk_note_off(&trk[1],36);
        }
        audio_block(out,CTL);
        for(unsigned i=0;i<CTL*2;i++) {
            int16_t s=ua_sat(out[i]>>8); uint8_t le[2]={(uint8_t)s,(uint8_t)((uint16_t)s>>8)};
            fwrite(le,1,2,f);
            unsigned a=(unsigned)abs((int)s); if(a>peak)peak=a;
            clips+=a>=32767;
            if(b>=FS/4/CTL && b<FS*2/CTL) { squares+=(double)s*s; count++; }
        }
    }
    double rms=10*log10(squares/count/(32768.0*32768.0));
    fclose(f); CHECK(peak>100 && !clips && rms>-27 && rms<-10);
    printf("FX_LEVEL,%u,%s,%s,%.2f,%.2f,%u\n",preset+1,fm?"FM PAD":"VA POLY KEYS",FXP_FACTORY[preset].name,
           20*log10((double)peak/32768),rms,clips);
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0);
    cu.booted=1; render(10);
    catalog();
    if(argc==2 && !strcmp(argv[1],"--count")) printf("FX_COUNT,%u\n",FXP_FACTORY_N);
    else if(argc==5 && !strcmp(argv[1],"--render")) render_presets(atoi(argv[2]),atoi(argv[3]),argv[4]);
    else if(argc==3 && !strcmp(argv[1],"--shots")) shots(argv[2]);
    else if(argc==5 && !strcmp(argv[1],"--audition")) audition(atoi(argv[2]),atoi(argv[3]),argv[4]);
    else {
        controls(); storage();
        unsigned hard=transition_jump(0), smooth=transition_jump(1);
        printf("FX transition max sample step: direct %u, controlled %u (constant input fixture)\n",hard,smooth);
        CHECK(hard>0 && smooth<hard/4);
        clear_controls(); /* Runs last: this fixture deliberately leaves real delay/reverb tails alive. */
    }
    printf("FX workflow/storage: %u checks, %u failures\n",checks,failures); return failures!=0;
}
