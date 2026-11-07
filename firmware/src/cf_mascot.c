/* SPDX-License-Identifier: GPL-3.0-only
 * Foreground-only presentation of the approved Devil and Cat atlases.
 * Integer affine sampling directly from const row spans into the existing
 * indexed framebuffer. No decode buffer, heap, flash writes or audio work.
 */
enum { CFM_IDLE, CFM_BLINK, CFM_REACT, CFM_RELAX };
typedef struct {
    int16_t x,y;                 /* Q8 translation, including subpixel sampling */
    uint16_t sx,sy;              /* Q8 squash/stretch */
    int8_t angle;                /* signed 1/1024-turn angle */
    uint8_t character,face,ring,ring_ink,burst;
} cfm_pose_t;

static unsigned cfm_ease(unsigned t)
{ return t*t*(768u-2u*t)/65536u; }
static int cfm_lerp(int a,int b,unsigned t)
{ return a+(b-a)*(int)cfm_ease(t)/256; }

static void cfm_model(cfm_pose_t *p,uint32_t attack_ms)
{
    memset(p,0,sizeof *p);p->character=cf_skin%CF_SKIN_N;p->sx=p->sy=256;
    if(!cf_motion)return;
    unsigned clock=cfn_motion.fresh && (cf_sn.running || cf_sn.metro_on) && cf_sn.beats_done &&
        !cf_sn.ac_waiting && !cf_sn.ac_lost && !cf_sn.ac_paused;
    unsigned active=cfn_motion.active && !cf_sn.visual_muted;
    unsigned phase=cf_sn.beat_acc/(FS*60u/256u);if(phase>255)phase=255;
    unsigned bpm=cs.clock_in==1u && cf_sn.ext_bpm?cf_sn.ext_bpm:cs.bpm;if(!bpm)bpm=1;
    unsigned steps=(60000u/bpm/40u)&~3u;if(steps<4)steps=4;if(steps>32)steps=32;
    phase=(phase*steps/256u)*256u/steps; /* bounded poses from real clock phase */
    unsigned age=attack_ms && (int32_t)(attack_ms-cf_motion_epoch)>0?cu.now-attack_ms:1000u;
    if(clock && active) {
        static const uint16_t at[]={0,26,102,195,256};
        static const int16_t yy[]={205,346,-806,-102,205};
        static const uint16_t xx[]={261,264,252,257,261},ys[]={252,248,261,255,252};
        unsigned i=1;while(i<4 && phase>at[i])i++;
        unsigned t=(phase-at[i-1])*256u/(at[i]-at[i-1]);
        unsigned gain=96+cfn_motion.energy*160u/255u;
        if(p->character==CF_SKIN_PLUM)gain=gain*220u/256u;
        if(cf_sn.perf_dropped)gain=gain*35u/100u;
        else if(cf_sn.perf_fill)gain=gain*125u/100u;
        p->y=(int16_t)(cfm_lerp(yy[i-1],yy[i],t)*(int)gain/256);
        p->sx=(uint16_t)(256+(cfm_lerp(xx[i-1],xx[i],t)-256)*(int)gain/256);
        p->sy=(uint16_t)(256+(cfm_lerp(ys[i-1],ys[i],t)-256)*(int)gain/256);
        unsigned slow=((cf_sn.beats_done-1u)&3u)*256u+phase;
        p->x=(int16_t)(fl_fsin(slow)*(int)gain/(p->character?22000:26000));
        p->angle=(int8_t)(fl_fsin(slow-40u)*(int)gain/(16384*90));
        if((cf_sn.beats_done&7u)==0 && phase<56)p->face=CFM_BLINK;
        p->ring=(uint8_t)(43+phase*8u/256u);
        unsigned strength=1+cfn_motion.energy/86;
        if(cf_sn.perf_fill)strength=4;
        if(cf_sn.perf_dropped)strength=1;
        unsigned ink=(strength*(256-phase)+255)/256;
        p->ring_ink=(uint8_t)(CFN_HALO_0+(ink?ink-1:0));
    }
    if(active && age<400u) {
        unsigned t=age*256u/400u;
        /* Small anticipation then lift, with a calm zero-velocity landing. */
        int bounce=t<64?cfm_lerp(0,256,t*4):cfm_lerp(256,0,(t-64)*256u/192u);
        p->y-=(int16_t)(bounce*2);
        p->face=CFM_REACT;p->burst=1;
        if(!clock){p->ring=43+t*8u/256u;p->ring_ink=CFN_HALO_0+(255-t)/86u;}
    }
    if(clock && cf_sn.perf_fill){p->face=CFM_REACT;p->burst=(uint8_t)active;}
    if(clock && cf_sn.perf_dropped)p->face=CFM_RELAX;
    unsigned return_age=cu.now-cfn_moment_ms;
    if(clock && active && cfn_moment_ms && return_age<650u) {
        unsigned t=return_age*256u/650u;
        p->face=CFM_REACT;p->burst=1;
        p->ring=(uint8_t)(43+t*8u/256u);
        p->ring_ink=(uint8_t)(CFN_HALO_0+3-t*4u/256u);
        p->y-=(int16_t)(cfm_lerp(512,0,t));
    }
}

static void cfm_draw(const cfm_pose_t *p)
{
    const cfm_art_t *art=&CFM_ART[p->character%CF_SKIN_N];
    int angle=p->angle;
    int cosine=fl_fcos((unsigned)angle),sine=fl_fsin((unsigned)angle);
    int a=cosine*256/(int)p->sx,b=sine*256/(int)p->sx;
    int c=-sine*256/(int)p->sy,d=cosine*256/(int)p->sy;
    int cx=59*256+p->x,cy=105*256+p->y;
    /* Includes the transformed silhouette, but excludes all chord text and
     * the planned rail. Sampling advances affinely; no per-pixel division. */
    for(int y=49;y<156;y++) {
        int qx=CFM_ART_W/2*16384+((1*256-cx)*a+(y*256-cy)*b)/256;
        int qy=CFM_ART_H/2*16384+((1*256-cx)*c+(y*256-cy)*d)/256;
        for(int x=1;x<116;x++,qx+=a,qy+=c) {
            int sx=qx>>14,sy=qy>>14;
            if((unsigned)sx>=CFM_ART_W || (unsigned)sy>=CFM_ART_H)continue;
            const uint8_t *row=art->data+art->rows[p->face*CFM_ART_H+(unsigned)sy];
            unsigned index=(unsigned)sx-row[0];
            if(index<row[1]){unsigned ink=row[2+index];if(ink)fb_pset(x,y,(uint8_t)(CFM_PAL_BASE+ink-1));}
        }
        if((y&7)==7)CF_PUMP();
    }
    if(p->burst) {
        fb_line(100,63,103,57,C_ACC(0));fb_line(104,67,110,63,C_ACC(0));fb_line(106,72,113,71,C_ACC(0));
    }
}
