/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include <time.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#ifndef FW_UNIT
#define FW_UNIT "../tools/emu/emu_fw.c"
#endif
#include FW_UNIT
static void ua_service_paced(uint32_t t0,int isr){(void)t0;(void)isr;ua_service();}
static unsigned checks,failures;
#define CHECK(x) do {checks++;if(!(x)){failures++;fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);}}while(0)
static int32_t out[CTL*2];
static void settle(void){for(unsigned i=0;i<64;i++){audio_block(out,CTL);ua.cr=ua.cw;}}
static void inventory(const char *path){
 FILE *f=fopen(path,"wb");CHECK(f);if(!f)return;
 for(unsigned engine=0;engine<2;engine++)for(unsigned k=0;k<(engine?93u:97u);k++){
 const preset_t *p=engine?&VA_PRESETS[k]:&FM6_PRESETS[k];
 fwrite(p->name,1,strlen(p->name)+1,f);fwrite(p->e,1,8,f);fwrite(p->env,1,4,f);
 fwrite(&p->fenv,1,1,f);fwrite(&p->mono,1,1,f);fwrite(p->fx,1,4,f);fwrite(&p->pat,1,1,f);
 }
 for(unsigned k=0;k<97;k++){uint8_t pk[128];fm6_slot_get((unsigned)FM6_PRESETS[k].e[7],pk);fwrite(pk,1,128,f);}
 for(unsigned k=0;k<93;k++){int8_t p[VA_NP];va_preset_patch(k,p);fwrite(p,1,sizeof p,f);}
 for(unsigned part=0;part<2;part++){
 unsigned n=cf_snd_signature_span(part);
#ifdef SIG8_FILTER
 if(!part)n-=8;
#endif
 fwrite(&n,sizeof n,1,f);
 for(unsigned i=0;i<n;i++){uint32_t e,pos;cf_snd_at(part,i,&e,&pos);fwrite(&e,4,1,f);fwrite(&pos,4,1,f);}
 }
 fclose(f);
}
#ifdef SIG8_FILTER
static const char *names[]={"EMBER KEYS","GHOST CHOIR","NOIR TINES","APOLLO VEIL","FROSTWIRE","OFFBEAT DUB","CEDAR MALLET","RAZOR PULSE"};
static const unsigned engines[]={ENGI_FM6,ENGI_VA,ENGI_FM6,ENGI_VA,ENGI_VA,ENGI_VA,ENGI_FM6,ENGI_VA};
static const unsigned fac[]={97,93,98,94,95,96,99,97};
static const char *fxnames[]={"Velvet","Wide Drift","Silver Plate","Deep Space","Phase Sweep","Slap","Silver Plate","Bite"};
static void load(unsigned i){cf_pool_load(0,engines[i],pool_pos_factory(engines[i],fac[i]));settle();}
static void invariants(void){
 CHECK(!strcmp(cf_snd[0].name,"EMBER KEYS"));CHECK(!strcmp(cf_snd[2].name,"GHOST CHOIR"));
 CHECK(cf_bank[0]==SIG8_PRIMARY_FILTER && cf_bank[1]==0 && cf_bank[2]==SIG8_PRIMARY_FILTER);
 for(unsigned part=0;part<3;part++)for(unsigned j=0;j<cf_snd_count(part);j++)
  CHECK(cf_snd_browse_rank(part,cf_snd_browse_index(part,j))==j);
 for(unsigned j=0;j<8;j++){
  uint32_t e,p;cf_snd_at(0,cf_snd_browse_index(0,j),&e,&p);
  CHECK(e==engines[j]&&pool_entry(e,p).fk==fac[j]);
  CHECK(!strcmp(cf_snd[0].name,names[j]));CHECK(cf_snd_step(0,1));
 }
 /* Presentation rotation never changes persisted old indices. */
 CHECK(cf_snd_find(0,ENGI_FM6,"ORGAN"));uint32_t old=cf_snd_pos(0);cs.snd[0]=old;cs.snd_layer=cf_snd_pos(2);cfs_loaded=1;
 cf_ui_init();CHECK(!strcmp(cf_snd[0].name,"ORGAN")&&cf_snd_pos(0)==old);
 cfs_loaded=0;
 CHECK(FM6_NSLOT==128);CHECK(NELEM(FM6_PRESETS)==100);CHECK(STUDIO_VA_GLIDE_HAZE==131 && VA_NPRESETS==149);
 cf_bank[0]=SIG8_FILTER;unsigned found=0;
 for(unsigned k=0;k<cf_snd_count(0);k++){uint32_t e,p;cf_snd_at(0,k,&e,&p);if(cf_bank_match(0,e,p)){
 CHECK(found<8);if(found<8){CHECK(e==engines[found]);CHECK(pool_entry(e,p).fk==fac[found]);CHECK(cf_entry_bank(e,p)==found+1);}found++;}}
 CHECK(found==8);
 for(unsigned i=0;i<8;i++){
 load(i);CHECK(!strcmp(cf_snd[0].name,names[i]));uint32_t e=engines[i],pos=pool_pos_factory(e,fac[i]);
 cf_stage_t st;cf_snd_build(0,e,pos,&st);uint8_t before[ENG_BLOB_MAX],after[ENG_BLOB_MAX];
 ENGINES[e]->deep->blob_get(&trk[0],before);CHECK(!memcmp(before,st.blob,st.blen));
 if(e==ENGI_FM6){uint8_t pk[128];fm6_pack(fm6_patch[0],pk);CHECK(!memcmp(pk,SIG8_FM_PATCHES[fac[i]-SIG8_FM_BASE],128));CHECK(fm6_patch[0][FP_TRNSP]==24);}
 else{const uint8_t *v=VA_PRESET_EDITS[fac[i]];for(;*v!=255;v+=2){CHECK(v[0]<VA_NP);va_rng_t r=va_range(v[0]);CHECK((int8_t)v[1]>=r.min&&(int8_t)v[1]<=r.max);} }
 CHECK(cf_snd_save(0,1,"SIG ROUNDTRIP")==0);uint32_t slot=cf_snd[0].slot;
 cf_load_init(0,ENGI_VA);settle();cf_pool_load(0,e,pool_pos_slot(e,slot));settle();
 ENGINES[e]->deep->blob_get(&trk[0],after);CHECK(!memcmp(before,after,ENGINES[e]->deep->blob_size));
 memset(up_bank,0,sizeof up_bank);up_gen++;cf_lst.ok=0;
 }
 cu.scr=SCR_GLO;cu.page[SCR_GLO]=5;
 cf_bank[0]=0;uint8_t saved[sizeof cf_snd];memcpy(saved,cf_snd,sizeof saved);
 ui_knob_turn(0,11);CHECK(cf_bank[0]==SIG8_FILTER);CHECK(!memcmp(saved,cf_snd,sizeof saved));
 ui_knob_turn(3,1);CHECK(cf_bank[0]==0);
 /* Real UI filter selection never loads a sound; empty Bass pilot is safe. */
 uint8_t state[sizeof cf_snd];memcpy(state,cf_snd,sizeof state);uint8_t gen=cf_snd_gen[1];
 cf_bank[1]=SIG8_FILTER;CHECK(!cf_snd_step(1,1));CHECK(cf_snd_gen[1]==gen);CHECK(!memcmp(state,cf_snd,sizeof state));
 cf_bank[0]=SIG8_PRIMARY_FILTER;for(unsigned i=0;i<8;i++)CHECK(cf_bank_match(0,engines[i],pool_pos_factory(engines[i],fac[i])));
 CHECK(!cf_bank_match(0,ENGI_FM6,pool_pos_factory(ENGI_FM6,CHASE_FM_BASE+16)));
 cf_bank[0]=0;for(unsigned i=0;i<8;i++)CHECK(cf_bank_match(0,engines[i],pool_pos_factory(engines[i],fac[i])));
}
static void drum_stress(void){
 for(unsigned i=0;i<8;i++){
  load(i);unsigned shed0=shed_count;cf_ready=1;cpa.on=0;sqp_stop();cf_evq_r=cf_evq_w;
  cfa.on=0;cfm.beats=sq_beats=4;cfm.bpm=120;cfm.level=0;cfm.on=0;cfx_in.active=0;
  cf_fx_t fx=CFX_DEFAULT;fx.on=0;cf_fx_apply(&fx);settle();
  dmp_start(dm_nth(4,0),14+i,0);cfm_restart();cfm.running=1;
  static const uint8_t notes[]={60,64,67,71,74};
  for(unsigned j=0;j<NELEM(notes);j++)trk_note_on(&trk[0],notes[j],80);
  for(unsigned b=0;b<(FS*8u+CTL-1)/CTL;b++){audio_block(out,CTL);ua.cr=ua.cw;}
  if(dmp.nbar!=4)fprintf(stderr,"drum bars %u t %u\n",dmp.nbar,dmp.t);CHECK(dmp.nbar==4);CHECK(shed_count==shed0);
  for(unsigned j=0;j<NELEM(notes);j++)trk_note_off(&trk[0],notes[j]);
  dmp_stop();cfm.running=0;for(unsigned b=0;b<FS*30u/CTL;b++){audio_block(out,CTL);ua.cr=ua.cw;}
  unsigned active=0;for(unsigned j=0;j<NVOICE;j++)active+=trk[0].v[j].active!=0;CHECK(!active);
 }
}
/* Fresh-process PCM comparison with USB idle/capture and both nominal rates. */
static void usb_audio(unsigned capture,unsigned rate,const char *path){
 FILE *f=fopen(path,"wb");CHECK(f);if(!f)return;
 cf_boot_go=0;cfb.on=0;cf_intro_on=0;cf_ready=0;cf_evq_r=cf_evq_w;
 song.master_q12=4096;fx_output_set(1,1);cf_fx_t fx=CFX_DEFAULT;fx.on=0;cf_fx_apply(&fx);
 for(unsigned i=0;i<8;i++){
  load(i);ua_reset();ua.cap_alt=capture;ua.cap_hold=0;ua_rate_set(rate);
  static const uint8_t notes[]={60,64,67,71,74};
  for(unsigned j=0;j<NELEM(notes);j++)trk_note_on(&trk[0],notes[j],80);
  for(unsigned b=0;b<FS*4u/CTL;b++){
   if(b==FS*2u/CTL)for(unsigned j=0;j<NELEM(notes);j++)trk_note_off(&trk[0],notes[j]);
   audio_block(out,CTL);fwrite(out,sizeof out,1,f);
   if(capture){CHECK(ua.cw>ua.cr);if(rate==44100)for(unsigned j=0;j<CTL;j++){
    const int16_t *cap=ua.cap+((ua.cr+j)&(UA_CAP_RING-1u))*6u;
    CHECK(cap[0]==ua_sat(out[j*2]>>8)&&cap[1]==ua_sat(out[j*2+1]>>8));
   }}
   ua.cr=ua.cw;
  }
 }
 fclose(f);
}
static void wav32(FILE *f,unsigned x){unsigned char b[]={x,x>>8,x>>16,x>>24};fwrite(b,1,4,f);}
/* Fixed segments: velocity C4 singles; 2s Cmaj9; 250ms A3/C4/E4/G4;
 * C3/C5; 3s sustained C4 + 2s release. Every segment has a quiet gap;
 * the final natural decay is rendered to 60s or confirmed voice release. */
static void audition(const char *name,const char *effect,const char *path){
 unsigned found=0,e=0,factory=0;
 for(unsigned ei=0;ei<CF_NENG;ei++)for(unsigned k=0;k<ENGINES[CF_ENGINES[ei]]->npresets;k++)
 if(!strcmp(ENGINES[CF_ENGINES[ei]]->presets[k].name,name)){e=CF_ENGINES[ei];factory=k;found++;}
 CHECK(found==1);if(found!=1)return;
 cf_pool_load(0,e,pool_pos_factory(e,factory));settle();
 cf_boot_go=0;cfb.on=0;cf_intro_on=0;cf_ready=0;cf_evq_r=cf_evq_w;
 song.master_q12=4096;fx_output_set(1,1);cs.fx=CFX_DEFAULT;cs.fx.on=0;
 if(strcmp(effect,"dry")){unsigned n=0;for(unsigned j=0;j<FXP_FACTORY_N;j++)if(!strcmp(effect,FXP_FACTORY[j].name)){cs.fx=FXP_FACTORY[j].fx;n++;}CHECK(n==1);}
 cf_fx_apply(&cs.fx);settle();trk[0].p[P_LEVEL]=92;
 FILE *file=fopen(path,"wb+");CHECK(file);if(!file)return;unsigned char blank[44]={0};fwrite(blank,1,44,file);
 unsigned peak=0,clips=0,frames=0,maxvoices=0,active=0;double energy=0,sum=0,tailenergy=0;unsigned tailn=0;
 clock_t start=clock();
 for(unsigned block=0;block<FS*90u/CTL;block++){
  double sec=(double)(block*CTL)/FS,prev=(double)((int)block-1)*CTL/FS;
  static const unsigned nn[]={60,60,60,60,64,67,71,74,57,60,64,67,48,72,60};
  static const unsigned vel[]={45,80,115,80,80,80,80,80,80,80,80,80,80,80,80};
  static const double on[]={0,2,4,6,6,6,6,6,10,10.25,10.5,10.75,13,16,19};
  static const double off[]={.75,2.75,4.75,8,8,8,8,8,10.24,10.49,10.74,10.99,14,17,22};
  for(unsigned j=0;j<NELEM(nn);j++){
   if(sec>=on[j] && (block==0||prev<on[j]))trk_note_on(&trk[0],nn[j],vel[j]);
   if(sec>=off[j] && prev<off[j])trk_note_off(&trk[0],nn[j]);
  }
  audio_block(out,CTL);ua.cr=ua.cw;
  active=0;for(unsigned v=0;v<NVOICE;v++)active+=trk[0].v[v].active!=0;
  if(active>maxvoices)maxvoices=active;
  for(unsigned j=0;j<CTL*2;j++){
   int32_t s=out[j]>>8;unsigned a=s<0?-s:s;if(a>peak)peak=a;clips+=a>=32767;energy+=(double)s*s;sum+=s;
   if(sec>=24){tailenergy+=(double)s*s;tailn++;}
   int16_t p=ua_sat(s);unsigned char bytes[]={p,(uint16_t)p>>8};fwrite(bytes,1,2,file);
  }
  frames+=CTL;
  if(sec>=24 && !active && fabs((double)(out[0]>>8))<=1 && fabs((double)(out[1]>>8))<=1)break;
 }
 double cpu=(double)(clock()-start)/CLOCKS_PER_SEC,seconds=(double)frames/FS;
 rewind(file);fwrite("RIFF",1,4,file);wav32(file,36+frames*4);fwrite("WAVEfmt ",1,8,file);wav32(file,16);wav32(file,0x00020001);wav32(file,FS);wav32(file,FS*4);wav32(file,0x00100004);fwrite("data",1,4,file);wav32(file,frames*4);fclose(file);
 CHECK(peak>10);CHECK(!clips);CHECK(active==0);CHECK(fabs(sum/(frames*2.0))<328);
 printf("SIG8_RENDER,%s,%s,%.6f,%.3f,%.3f,%u,%u,%u,%.8f,%.6f,%.3f\n",name,effect,seconds,20*log10(peak/32768.0+1e-12),10*log10(energy/(frames*2.0*32768*32768)+1e-12),clips,maxvoices,active,cpu/seconds,sum/(frames*2.0),10*log10(tailenergy/(tailn*32768.0*32768)+1e-12));
}
#endif
int main(int argc,char **argv){emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);
 if(argc==3&&!strcmp(argv[1],"--inventory"))inventory(argv[2]);
#ifdef SIG8_FILTER
 else if(argc==5&&!strcmp(argv[1],"--usb"))usb_audio(atoi(argv[2]),atoi(argv[3]),argv[4]);
 else if(argc==5&&!strcmp(argv[1],"--render"))audition(argv[2],argv[3],argv[4]);
 else {invariants();drum_stress();}
#endif
 printf("signature8: %u checks, %u failures\n",checks,failures);return failures!=0;}
