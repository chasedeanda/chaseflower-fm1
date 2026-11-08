/* SPDX-License-Identifier: GPL-3.0-only
 * Real allocator regression; host timing is not hardware CPU evidence. */
#include <stdio.h>
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void blocks(unsigned n){int32_t out[CTL*2];while(n--)audio_block(out,CTL);}
static unsigned active(unsigned p){unsigned n=0;for(unsigned i=0;i<NVOICE;i++)n+=trk[p].v[i].active&&trk[p].v[i].stage!=4;return n;}
static void exercise(unsigned engine)
{
    cfp_panic();dmp_stop();sqp_stop();cf_pool_load(0,engine,pool_pos_factory(engine,0));blocks(300);
    for(unsigned p=0;p<NSND;p++){memset(trk[p].v,0,sizeof trk[p].v);trk[p].xp_n=trk[p].nmono=0;}
    memset(vsq,0,sizeof vsq);
    unsigned kills=voice_kills;
    const unsigned chord[]={48,52,55,59,62,65};
    for(unsigned i=0;i<6;i++)trk_note_on(&trk[0],chord[i],100);
    blocks(4);CHECK(active(0)==6);
    const unsigned drums[]={36,38,42,45,49,51};
    for(unsigned i=0;i<6;i++) {trk_note_on(&trk[3],drums[i],100);blocks(1);CHECK(active(0)==6);}
    CHECK(active(3)>=4 && voice_kills==kills);
    CHECK(!VB_SAME(0,3) && !VB_SAME(3,0));
    /* A chord retrigger cannot choose a percussion voice as its victim. */
    track_t *victim=0;unsigned v=voice_victim(&trk[0],0,&victim);
    CHECK(v==NVOICE || victim!=&trk[3]);
    victim=0;v=voice_victim(&trk[3],0,&victim);CHECK(v==NVOICE || victim==&trk[3]);
    /* CPU pressure must still be able to shed a voice. */
    cf_boot_pending=cf_boot_go=0;cfb.on=0;
    unsigned before=voice_kills;shed_voice();CHECK(voice_kills>before);
}
int main(void)
{
    emu_fw_options(0,1,0,1);emu_hal.ready=2;emu_fw_init(0);
    cf_boot_pending=cf_boot_go=0;cfb.on=0;
    exercise(ENGI_FM6);exercise(ENGI_VA);
    printf("chord/drum budget: %u checks, %u failures\n",checks,failures);return failures!=0;
}
