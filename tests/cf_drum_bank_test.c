/* SPDX-License-Identifier: GPL-3.0-only
 * Real firmware tests and dry WAV auditions for every built-in groove. No device access.
 */
#include <stdio.h>
#include <math.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

static void silence(void)
{
    dmp_stop(); sqp_stop();
    for (uint32_t i=0; i<NSND; i++) {
        memset(trk[i].v,0,sizeof trk[i].v);
        trk[i].tail=trk[i].nmono=trk[i].xp_n=0;
    }
}

static uint32_t by_id(uint32_t id)
{
    for (uint32_t r=0; r<DM_N; r++) if (DM_RHY[r].id==id) return r;
    return DM_N;
}

static void bank(void)
{
    const uint8_t old[]={1,2,7,30,4,18,5,13,31,14,39,32,34,35,36,38,40,41,42,29,48,49,50,47,45,46};
    const uint8_t retired[]={33,37,43,44};
    CHECK(DM_N==39 && DM_N<255);
    for (uint32_t i=0; i<sizeof old; i++) CHECK(by_id(old[i])<DM_N);
    for (uint32_t i=0; i<sizeof retired; i++) CHECK(by_id(retired[i])==DM_N);
    for (uint32_t id=51; id<=62; id++) CHECK(by_id(id)<DM_N);
    dm_defaults();
    for (uint32_t r=0; r<DM_N; r++) {
        const dm_rhythm_t *y=&DM_RHY[r];
        CHECK(strlen(y->name)<=9 && y->id<DM_NMAX && y->kit<SQ_NKIT && y->swing<=100);
        CHECK(y->bars==2 && y->bpm>=60 && y->bpm<=200);
        for (uint32_t q=0; q<r; q++) CHECK(DM_RHY[q].id!=y->id);
        int changed=0;
        for (uint32_t l=0; l<SQ_NLANE; l++) {
            CHECK(y->lev[l]<=100);
            if (y->lane[l]) {
                CHECK(strlen(y->lane[l])==dm_steps(y->beats)*y->bars && y->lev[l]>0);
                for (const char *s=y->lane[l]; *s; s++) CHECK(*s=='x' || *s=='.');
            }
            changed |= dm_mask(r,l,0)!=dm_mask(r,l,1);
            CHECK(!dm_mask(r,l,y->bars));
        }
        CHECK(changed); /* a two-bar phrase has a reason to be two bars */
        for (uint32_t b=0; b<y->bars; b++) CHECK(!(dm_mask(r,DM_H,b)&dm_mask(r,DM_O,b)));
        /* Saved selection and user overrides still resolve by stable ID after reordering. */
        cs.beats=y->beats; dm_view=0xFF; dmr.pos[y->beats]=y->id;
        dmr.kit[y->id]=DMK_909; dmr.swing[y->id]=65;
        CHECK(dm_cur()==r && dm_kit(r)==DMK_909 && dm_swing(r)==65);
        sq_beats=y->beats;
        for (uint32_t start=0; start<y->bars; start++) {
            sq_pat_t p; sq_fresh(&p,y->beats); dm_fill(&p,r,start);
            CHECK(p.kit==DMK_909 && p.swing==65 && p.tb[SQ_DRUMS]==y->bars);
            for (uint32_t l=0; l<SQ_NLANE; l++) {
                CHECK(p.dkit[l]==DMK_909 && p.dlev[l]==(y->lev[l] ? y->lev[l] : 100));
                for (uint32_t b=0; b<SQ_BARS; b++)
                    CHECK(p.hit[l][b]==(b<y->bars ? dm_mask(r,l,(start+b)%y->bars) : 0u));
            }
        }
        dmr.kit[y->id]=dmr.swing[y->id]=0xFF;
        CHECK(dm_kit(r)==y->kit && dm_swing(r)==y->swing);
    }
}

/* Observe the real player's MIDI note-ons for two complete cycles, against the text score and
 * an independent swing calculation. Check both the rhythm and its editable sequencer copy. */
static void score(void)
{
    usb.config=1; cfp_cfg.midi_out=1; cfp_cfg.ch_drums=9; cfp_cfg.clock_out=0;
    for (uint32_t r=0; r<DM_N; r++) for (uint32_t seq=0; seq<2; seq++) {
        const dm_rhythm_t *y=&DM_RHY[r];
        silence(); cfx_in.active=0; cs.beats=cfm.beats=sq_beats=y->beats;
        cfm.bpm=y->bpm; mo_r=mo_w;
        if (seq) { sq_fresh(&sq_st.pat[0],y->beats); dm_fill(&sq_st.pat[0],r,0); sqp_start(0,0); }
        else dmp_start(r,y->kit,y->swing);
        uint32_t bt=y->beats*SQ_PPQN, sd=dm_steps(y->beats);
        for (uint32_t t=0; t<bt*y->bars*2; t++) {
            if (t) { if (seq) sqp_tick1(); else dmp_tick1(); }
            uint32_t expected=0, actual=0, b=(t/bt)%y->bars, pos=t%bt;
            for (uint32_t h=0; h<sd; h++) {
                uint32_t when=h*bt/sd+(h%2 ? y->swing*(bt/sd)/200u : 0u);
                if (when==pos) for (uint32_t l=0; l<SQ_NLANE; l++)
                    if (y->lane[l] && y->lane[l][b*sd+h]=='x') expected|=1u<<l;
            }
            while (mo_r!=mo_w) {
                uint32_t ev=midi_out_q[mo_r++%MQ];
                if (((ev>>8)&0xF0)==0x90 && (ev>>24)) {
                    uint32_t found=0;
                    for (uint32_t l=0; l<SQ_NLANE; l++) if (((ev>>16)&127)==SQ_LANE_NOTE[l]) {
                        CHECK(!(actual&(1u<<l))); actual|=1u<<l; found=1;
                    }
                    CHECK(found);
                }
            }
            CHECK(actual==expected);
        }
    }
    silence(); cfp_cfg.midi_out=0;
}

static void wav_header(FILE *f,uint32_t frames)
{
    uint32_t a=36+frames*4, sz=16, sr=FS, rate=FS*4, bytes=frames*4;
    uint16_t fmt=1,ch=2,align=4,bits=16;
    fwrite("RIFF",1,4,f); fwrite(&a,4,1,f); fwrite("WAVEfmt ",1,8,f); fwrite(&sz,4,1,f);
    fwrite(&fmt,2,1,f); fwrite(&ch,2,1,f); fwrite(&sr,4,1,f); fwrite(&rate,4,1,f);
    fwrite(&align,2,1,f); fwrite(&bits,2,1,f); fwrite("data",1,4,f); fwrite(&bytes,4,1,f);
}

static void render(const char *dir)
{
    cf_fx_t fx=CFX_DEFAULT; fx.on=0; cf_fx_apply(&fx); fx_output_set(0,1);
    cf_ready=0; cf_evq_r=cf_evq_w; cfa.on=0;
    cfm.on=cfm.level=0; cfx_in.active=0;
    ua_reset(); ua.cap_alt=0;
    char path[1024]; FILE *csv=NULL;
    if (dir) { snprintf(path,sizeof path,"%s/loops.csv",dir); csv=fopen(path,"w"); CHECK(csv); }
    if (csv) fprintf(csv,"id,name,beats,bars,bpm,swing,kit,peak_dbfs,rms_dbfs,clipped_samples,file\n");
    for (uint32_t r=0; r<DM_N; r++) {
        const dm_rhythm_t *y=&DM_RHY[r];
        silence(); cs.beats=cfm.beats=sq_beats=y->beats; cfm.bpm=y->bpm;
        /* Two complete phrases, in quarter-note BPM even for compound 6/8. */
        uint32_t quarters=y->beats==6 ? 3u : y->beats;
        uint32_t frames=(uint32_t)((uint64_t)FS*60u*quarters*y->bars*2u/y->bpm);
        FILE *f=NULL;
        if (dir) { snprintf(path,sizeof path,"%s/%02u-%s.wav",dir,y->id,y->name); f=fopen(path,"wb"); CHECK(f); if(f)wav_header(f,frames); }
        dmp_start(r,y->kit,y->swing);
        int32_t peak=0; double energy=0; uint32_t clipped=0; int32_t mix[CTL*2];
        for (uint32_t n=0; n<frames;) {
            uint32_t count=frames-n<CTL ? frames-n : CTL;
            audio_block(mix,count);
            int16_t pcm[CTL*2];
            for (uint32_t i=0; i<count*2; i++) {
                int32_t s=mix[i]>>8, a=s<0 ? -s : s;
                if (a>peak) peak=a;
                energy+=(double)s*s; clipped+=a>=32767;
                pcm[i]=ua_sat(s);
            }
            if (f) fwrite(pcm,sizeof(int16_t),count*2,f);
            n+=count;
        }
        if(f)fclose(f);
        double db=20*log10(peak/32768.0+1e-12), rms=10*log10(energy/(frames*2.0*32768.0*32768.0)+1e-24);
        printf("loop %02u %-9s %3u BPM %-7s peak %5.1f dBFS clips %u\n",y->id,y->name,y->bpm,SQ_KITS[y->kit].name,db,clipped);
        CHECK(peak>100 && clipped==0 && isfinite(rms));
        if(csv)fprintf(csv,"%u,%s,%u,%u,%u,%u,%s,%.2f,%.2f,%u,%02u-%s.wav\n",y->id,y->name,y->beats,y->bars,y->bpm,y->swing,SQ_KITS[y->kit].name,db,rms,clipped,y->id,y->name);
    }
    if(csv)fclose(csv);
    silence(); cf_ready=1;
}

int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0);
    bank(); score(); render(argc==3 && !strcmp(argv[1],"--render") ? argv[2] : NULL);
    printf("drum bank: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
