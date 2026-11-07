/* SPDX-License-Identifier: GPL-3.0-only
 * Offline candidate auditions through the actual synth, effects and output path.
 * Each invocation boots a fresh RAM-only emulator; never opens physical hardware.
 */
#include <stdio.h>
#include <math.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }

static void wav_header(FILE *f, uint32_t frames)
{
    uint32_t a=36+frames*4,sz=16,sr=FS,rate=FS*4,bytes=frames*4;
    uint16_t fmt=1,ch=2,align=4,bits=16;
    fwrite("RIFF",1,4,f);fwrite(&a,4,1,f);fwrite("WAVEfmt ",1,8,f);fwrite(&sz,4,1,f);
    fwrite(&fmt,2,1,f);fwrite(&ch,2,1,f);fwrite(&sr,4,1,f);fwrite(&rate,4,1,f);
    fwrite(&align,2,1,f);fwrite(&bits,2,1,f);fwrite("data",1,4,f);fwrite(&bytes,4,1,f);
}

int main(int argc,char **argv)
{
    if(argc!=21)return 2; /* WAV, on bitmask, six parameter triplets */
    cf_fx_t candidate=CFX_DEFAULT;
    candidate.on=(uint8_t)atoi(argv[2]);
    for(unsigned e=0;e<CFX_N;e++)for(unsigned k=0;k<3;k++)candidate.v[e][k]=(uint8_t)atoi(argv[3+e*3+k]);
    emu_fw_options(NULL,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    cf_ready=0;cf_boot_note();
    cf_pool_load(0,ENGI_FM6,pool_pos_factory(ENGI_FM6,0));
    trk[0].p[P_LEVEL]=78;
    cf_fx_t dry=CFX_DEFAULT;dry.on=0;cf_fx_apply(&dry);fx_output_set(1,1);
    int32_t out[CTL*2];
    for(unsigned b=0;b<FS*2/CTL;b++)audio_block(out,CTL);
    cf_fx_apply(&candidate);
    for(unsigned b=0;b<FS/2/CTL;b++)audio_block(out,CTL);
    FILE *f=fopen(argv[1],"wb");if(!f)return 3;
    const unsigned frames=(FS*9/CTL)*CTL;
    wav_header(f,frames);
    const unsigned times[]={0,260,700,960,1400,1660,2400,4400};
    const uint8_t chords[4][4]={{60,64,67,71},{57,60,64,67},{53,57,60,64},{55,59,62,65}};
    unsigned ev=0,clips=0;int peak=0;
    for(unsigned n=0;n<frames;n+=CTL){
        if(ev<8 && n>=(times[ev]*FS/1000/CTL)*CTL){
            for(unsigned j=0;j<4;j++){
                if(!(ev&1))trk_note_on(&trk[0],chords[ev/2][j],100);
                else trk_note_off(&trk[0],chords[ev/2][j]);
            }
            ev++;
        }
        audio_block(out,CTL);
        int16_t pcm[CTL*2];
        for(unsigned i=0;i<CTL*2;i++){
            int s=out[i]>>8,a=s<0?-s:s;if(a>peak)peak=a;
            clips+=a>=32767;pcm[i]=ua_sat(s);
        }
        fwrite(pcm,sizeof pcm,1,f);
    }
    fclose(f);
    printf("candidate peak=%d clipped=%u events=%u\n",peak,clips,ev);
    return clips || peak<100 || ev!=8;
}
