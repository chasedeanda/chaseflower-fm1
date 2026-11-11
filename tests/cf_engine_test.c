/* SPDX-License-Identifier: GPL-3.0-only; real firmware clocks, ownership and DSP. */
#include <stdio.h>
#include <time.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0,int isr){(void)t0;(void)isr;ua_service();}
static unsigned checks,failures;
static FILE *engine_raw;
#define CHECK(x) do{checks++;if(!(x)){failures++;if(failures<35)fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void blocks(unsigned n){int32_t out[CTL*2];while(n--){audio_block(out,CTL);ua.cr=ua.cw;}cf_snapshot();cpa_ui_poll();cge_poll();}
static void ticks(unsigned n){while(n--){cpa_tick1();mo_r=mo_w;}cf_snapshot();cpa_ui_poll();cge_poll();}
static void start(void){cpa_stop();sqp_stop();dmp_stop();cfp_panic();cu.keys=0;cf_boot_go=0;cfb.on=0;cf_intro_on=0;blocks(4);cu.scr=SCR_HOME;home_boot_main=1;cf_scene_set(CF_SCENE_PROGRESSIONS);cf_home_bank=CF_HB_ENGINE;cs.chord.tonic=4;cs.chord.scale=CF_SC_MAJOR;cs.chord.octave=0;cs.bpm=104;cs.bass_on=1;cge_load();blocks(64);cpa_ui_play();blocks(1);CHECK(cge_active());}
static void plan(void){
 CHECK(CF_P_ENGINE==253 && CF_P_DRAFT==254 && CF_P_NONE==255);start();
 CHECK(cpa.plan.n==8 && cpa.plan.beats==4 && cs.bpm==104);
 CHECK(cge.energy==55 && cge.feel==42 && cge.space==100 && !cge.locks);
 CHECK(!strcmp(cf_snd[0].name,"GHOST CHOIR")&&!strcmp(cf_snd[1].name,"TAPE SUB"));
 CHECK(DM_RHY[cpa.plan.rhythm].id==56 && cpa.plan.kit==DMK_AIR);
 for(unsigned i=0;i<8;i++){const cpa_slot_t *s=&cpa.plan.slot[i];CHECK(s->ticks==768 && s->gate==100);CHECK(s->n==CGE_NOTES[0][i%4].n);CHECK(!memcmp(s->note,CGE_NOTES[0][i%4].note,s->n));}
 uint32_t d=sqp.dser;ticks(384);CHECK(sqp.dser==d && cpa.slot==0 && cge.bar==1);ticks(384);CHECK(cpa.slot==1 && cge.bar==2 && sqp.dser==d+1);
 cf_song_t snapshot;CHECK(cge_snapshot(&snapshot));CHECK(snapshot.n==8 && snapshot.slot[0].bars==2);
 uint8_t wire[CF_SONG_WIRE];cf_song_t restored;CHECK(cf_song_encode(&snapshot,wire));CHECK(cf_song_decode(&restored,wire,sizeof wire));CHECK(restored.rhythm==56);
 cf_chord_cfg_t cfg=cs.chord;for(unsigned i=0;i<8;i++){cf_song_chord_t o;CHECK(cf_song_resolve(&restored,i,&cfg,0,0,&o));CHECK(o.n==cpa.plan.slot[i].n && !memcmp(o.note,cpa.plan.slot[i].note,o.n));}
}
static void cycles(FILE *trace,int interactive){
 start();uint32_t first=sqp.dser;unsigned expected_serial=0;unsigned mix[3]={cs.level_chord,cs.bass_level,cs.level_drums};
 if(trace)fprintf(trace,"cycle,bar,slot,stage,chord_notes,chord_gain_q15,bass_gain_q15,drum_gain_q15,arp,locks,fill,dropped,pending\n");
 for(unsigned b=0;b<64;b++){
  if(interactive){
   if(b==5){cge_knob_turn(0,20);CHECK(cge.energy==75);}
   if(b==9)cge_knob_turn(1,35);
   if(b==11)cge_knob_turn(2,80);
   if(b==16)cf_perf_queue(CFPERF_DROP);
   if(b==19)cge_regenerate(0);
   if(b==23)cge_regenerate(1);
   if(b==34)cf_perf_queue(CFPERF_BACK);
   if(b==40)cf_perf_queue(CFPERF_FILL);
  }
  CHECK(cge.bar==b%16 && cge.cycle==b/16);CHECK(cpa.slot==(b%16)/2);
  if(!interactive){CHECK(sqp.dser==first+expected_serial);CHECK(!cge.arp && !cf_moment.fill);}
  if(b>=17 && b<=34 && interactive)CHECK(cf_moment.dropped);
  if(trace){const cpa_slot_t *s=&cpa.plan.slot[cpa.slot];char notes[40]={0};for(unsigned j=0;j<s->n;j++){char t[8];snprintf(t,sizeof t,"%s%u",j?" ":"",s->note[j]);strcat(notes,t);}fprintf(trace,"%u,%u,%u,%s,%s,%u,%u,%u,%u,%u,%u,%u,%u\n",cge.cycle,cge.bar+1,cpa.slot+1,cge_stage_name(),notes,cge.target[0],cge.target[1],cge.target[2],cge.arp,cge.locks,cf_moment.fill,cf_moment.dropped,cge.pending);}
  ticks(384);if(!(b&1)){}else expected_serial++;
  CHECK(cpa.on);CHECK(cs.level_chord==mix[0]&&cs.bass_level==mix[1]&&cs.level_drums==mix[2]);
 }
 CHECK(cge.cycle==4 && cge.bar==0 && cpa.slot==0);if(!interactive)CHECK(sqp.dser-first==32);
}
static void controls(void){
 start();cge_regenerate(0);CHECK(cge.pending==15);ticks(15*384);CHECK(cge.pending==15 && !cge.index[0] && !cge.index[1]);ticks(384);CHECK(!cge.pending&&cge.index[0]==1);
 cge_regenerate(1);ticks(16*384);for(unsigned i=0;i<4;i++)CHECK(!cge.index[i]);
 cge.locks=15;uint32_t generation=cge.generation;cge_regenerate(0);CHECK(cge.generation==generation&&!cge.pending);
 cge.locks=CGE_HARMONY|CGE_FX;cge_regenerate(0);CHECK(cge.request[0]==0&&cge.request[3]==0&&cge.request[1]==1&&cge.request[2]==1);ticks(768);
 cpa_home_key(1,1);CHECK(cge.locks&CGE_HARMONY);ticks(768);CHECK(cpa.plan.tonic==5 && cpa.plan.slot[0].note[0]==53);
 cpa_home_key(2,1);ticks(768);CHECK(cpa.plan.scale==CF_SC_DORIAN);for(unsigned i=0;i<8;i++)CHECK(cpa.plan.slot[i].n==3);
 cpa_home_key(2,-1);ticks(768);CHECK(cpa.plan.scale==CF_SC_MAJOR&&cpa.plan.slot[0].n==4);
 cge_knob_turn(0,-100);blocks(20);CHECK(cge.target[1]==0&&cge.target[2]==0&&cge.target[0]>0);ticks(384);CHECK(cge.drum_gate==(CGE_PROFILE[cge_phase()][3]!=0));
 cge_knob_turn(0,100);blocks(20);CHECK(cge.target[0]<=32768&&cge.target[1]<=32768&&cge.target[2]<=32768);
}
static void queued_and_ui(void){
 start();unsigned bar=cge.bar;uint32_t acc=cpa.acc;uint16_t bpm=cs.bpm;
 cf_home_bank=CF_HB_CORE;ui_knob_turn(3,3);CHECK(cs.bpm==bpm+3 && cge.bar==bar && cpa.acc==acc);
 cu.scr=SCR_GLO;cu.page[SCR_GLO]=CF_GLO_ENGINE_LOCKS;
 for(unsigned k=0;k<4;k++){ui_knob_turn(k,1);CHECK(cge.locks&(1u<<k));ui_knob_turn(k,-1);CHECK(!(cge.locks&(1u<<k)));}
 cu.scr=SCR_HOME;cf_home_bank=CF_HB_ENGINE;
 cge_regenerate(0);cge_regenerate(0);CHECK(cge.request[0]==2 && cge.request[1]==2 && !cge.request[2]);ticks(16*384);
 CHECK(cge.index[0]==2 && cge.index[1]==2 && !cge.index[2] && cge.index[3]==2 && !cge.pending);
 cge_regenerate(1);ticks(16*384);CHECK(!cge.index[0] && !cge.index[1] && !cge.index[2] && !cge.index[3]);
 /* Direct edits engage their category without changing the cycle. */
 cs.fx.v[0][0]^=1;cge_poll();CHECK(cge.locks&CGE_FX);
 trk[0].p[P_E0]^=1;cge_poll();CHECK(cge.locks&CGE_SOUND);
 cpa_ui_rhythm(1);CHECK(cge.locks&CGE_GROOVE);
 cu.pop=0;cf_intro_on=0;cf_motion=0;cf_ui_draw();FILE *f=fopen("build/engine/home.ppm","wb");CHECK(f);if(f){fprintf(f,"P6\n240 240\n255\n");for(unsigned i=0;i<FB_W*FB_H;i++){uint32_t rgb=cf_pal[fb[i]];uint8_t v[]={rgb>>16,rgb>>8,rgb};fwrite(v,1,3,f);}fclose(f);}
}
static void live_and_clock(void){
 start();ticks(9*384);cge_knob_turn(1,40);ticks(384);CHECK(cge.arp);
 cf_post(CFE_KEY,7,0,1);blocks(4);CHECK(cpa_live_chord());unsigned bar=cge.bar;blocks(FS/CTL);CHECK(cge.bar==bar || cge.bar==bar+1);
 cf_post(CFE_KEY,7,0,0);blocks(4);CHECK(!cpa_live_chord());CHECK(cfa_sq[0].n==cpa.plan.slot[cpa.slot].n);
 cpa_stop();blocks((FS/20+CTL-1)/CTL+2);for(unsigned k=0;k<3;k++)CHECK(cge.gain[k]==32768);
 start();cfp_cfg.clock_in=1;cpa_realtime(0xFC);CHECK(cpa.paused);unsigned slot=cpa.slot,phase=cge.bar;
 cpa_tick(0,24);CHECK(cpa.paused && cge.bar==phase);cpa_realtime(0xFB);CHECK(!cpa.paused && cpa.slot==slot);
 cpa_realtime(0xFA);CHECK(cge.bar==0 && cpa.slot==0);cfp_cfg.clock_in=0;
}
static void actual_trace(const char *path,int interactive){
 start();cge.feel=0;cpa_realtime(0xFA);
 usb.up=usb.config=1;cfp_cfg.midi_out=1;cfp_cfg.ch_chord=0;cfp_cfg.ch_bass=1;mo_r=mo_w;
 /* Restart with output enabled, so the first authored onset is represented. */
 cpa_realtime(0xFA);unsigned last=999,rows=0,peak=0,clips=0,maxvoices=0;double squares=0;uint64_t samples=0;
 FILE *f=fopen(path,"w");CHECK(f);if(!f)return;
 fprintf(f,"cycle,bar,slot,stage,planned_notes,emitted_chord_notes,target_chord_q15,target_bass_q15,target_drums_q15,arp,locks,fill,dropped,pending,energy,feel,space,generation,harmony_index,sound_index,groove_index,fx_index\n");
 unsigned observed[128]={0};char rows_text[64][384];unsigned events[64][128]={{0}};int32_t out[CTL*2];clock_t begin=clock();
 for(unsigned block=0;block<FS*160u/CTL && cge.cycle<4;block++){
  while(mo_r!=mo_w){uint32_t q=midi_out_q[mo_r++%MQ];unsigned st=q>>8&255,n=q>>16&127,v=q>>24&127;if((st&0xF0)==0x90 && (st&15)==0 && v){observed[n]=1;unsigned pos=cge.cycle*16+cge.bar;if(pos<64)events[pos][n]=1;}}
  unsigned position=cge.cycle*16+cge.bar;
  if(position!=last){
   last=position;rows++;
   char planned[48]={0},emitted[96]={0};const cpa_slot_t *p=&cpa.plan.slot[cpa.slot];
   for(unsigned j=0;j<p->n;j++){char t[8];snprintf(t,sizeof t,"%s%u",j?" ":"",p->note[j]);strcat(planned,t);}
   for(unsigned j=0;j<128;j++)if(observed[j]){char t[8];snprintf(t,sizeof t,"%s%u",emitted[0]?" ":"",j);strcat(emitted,t);}
   snprintf(rows_text[position],sizeof rows_text[0],"%u,%u,%u,%s,%s,%%s,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",cge.cycle,cge.bar+1,cpa.slot+1,cge_stage_name(),planned,cge.target[0],cge.target[1],cge.target[2],cge.arp,cge.locks,cf_moment.fill,cf_moment.dropped,cge.pending,cge.energy,cge.feel,cge.space,cge.generation,cge.index[0],cge.index[1],cge.index[2],cge.index[3]);
   if(!interactive){unsigned count=0;for(unsigned j=0;j<128;j++)count+=observed[j];if(!(cge.bar&1u))CHECK(count==p->n);else CHECK(!count);}
   memset(observed,0,sizeof observed);
   if(interactive){
    if(position==5)cge_knob_turn(0,20);
    if(position==9)cge_knob_turn(1,35);
    if(position==11)cge_knob_turn(2,80);
    if(position==16)cf_perf_queue(CFPERF_DROP);
    if(position==18)cge_regenerate(0);
    if(position==22)cge_regenerate(1);
    if(position==28)cge.locks^=CGE_SOUND;
    if(position==29)cge_regenerate(0);
    if(position==31)cge_regenerate(1);
    if(position==34)cf_perf_queue(CFPERF_BACK);
    if(position==40)cf_perf_queue(CFPERF_FILL);
    if(position==44)cpa_home_key(1,1);
   }
  }
  audio_block(out,CTL);if(engine_raw)fwrite(out,sizeof out,1,engine_raw);ua.cr=ua.cw;
  if(block%8==0){cf_snapshot();cpa_ui_poll();cge_poll();}
  for(unsigned j=0;j<CTL*2;j++){int32_t v=out[j]>>8;unsigned a=v<0?-v:v;if(a>peak)peak=a;clips+=a>=32767;squares+=(double)v*v;samples++;}
  unsigned voices=0;for(unsigned p=0;p<4;p++)for(unsigned j=0;j<NVOICE;j++)voices+=trk[p].v[j].active!=0;if(voices>maxvoices)maxvoices=voices;
 }
 for(unsigned i=0;i<64;i++){char notes[512]={0};for(unsigned n=0;n<128;n++)if(events[i][n]){char t[8];snprintf(t,sizeof t,"%s%u",notes[0]?" ":"",n);strcat(notes,t);}fprintf(f,rows_text[i],notes);}
 fclose(f);CHECK(rows==64 && cge.cycle==4);CHECK(fabs((double)samples/(2*FS)-64*4*60.0/104)<=2.0*CTL/FS);CHECK(peak>100 && !clips);
 cpa_stop();blocks(FS*30u/CTL);unsigned held=0;for(unsigned p=0;p<2;p++)for(unsigned j=0;j<NVOICE;j++)held+=trk[p].v[j].active!=0;CHECK(!held);
 printf("ENGINE_RENDER,%s,peak=%u,clips=%u,voices=%u,rms=%.3f,host_cpu_seconds=%.6f,audio_seconds=%.6f\n",interactive?"interactive":"baseline",peak,clips,maxvoices,sqrt(squares/samples),(double)(clock()-begin)/CLOCKS_PER_SEC,(double)samples/(2*FS));
}
#ifndef CF_ENGINE_HELPERS_ONLY
int main(int argc,char **argv){emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);if(argc==5 && !strcmp(argv[1],"--usb")){ua_reset();ua.cap_alt=atoi(argv[2]);ua.cap_hold=0;ua_rate_set(atoi(argv[3]));engine_raw=fopen(argv[4],"wb");CHECK(engine_raw);if(engine_raw){actual_trace("build/engine/usb-trace.csv",1);fclose(engine_raw);}printf("Engine USB: %u checks, %u failures\n",checks,failures);return failures!=0;}plan();controls();queued_and_ui();live_and_clock();FILE *f=argc>1?fopen(argv[1],"w"):0;cycles(f,0);if(f)fclose(f);f=argc>2?fopen(argv[2],"w"):0;cycles(f,1);if(f)fclose(f);if(argc>4){actual_trace(argv[3],0);actual_trace(argv[4],1);}
 printf("CORD Engine: %u checks, %u failures\n",checks,failures);return failures!=0;}
#endif
