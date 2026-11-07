/* SPDX-License-Identifier: GPL-3.0-only
 * Native adaptation of the supplied CORD piano-key/ring boot reference.
 * A finite, input-skippable presentation; never a loading/transport timer.
 */
#define CF_CORD_BOOT_MS 1300u
static uint8_t cf_intro_on;
static uint32_t cf_intro_t,cf_cord_started;
static struct { uint16_t t;uint8_t skin,motion,valid; } cf_cord_last;
static void cf_cord_palette(void)
{
    /* bg / surface / text / muted / warm accent / ring accent, supplied variants. */
    static const uint32_t rgb[CF_SKIN_N][6]={
        {0x150F0C,0x241813,0xFFF4E5,0xD0BFB1,0xFFA55B,0xFFA55B},
        {0x190F19,0x291929,0xFFF2FB,0xD0B8CD,0xF29BCD,0xF29BCD}
    };
    const uint32_t *p=rgb[cf_skin%CF_SKIN_N];
    cf_pal[FL_BG]=p[0];cf_pal[FL_BG1]=p[1];cf_pal[FL_BG2]=cfn_blend(p[0],0,72);
    cf_pal[FL_BG3]=cfn_blend(p[0],p[2],12);cf_pal[FL_INK]=p[2];cf_pal[FL_INK2]=p[3];
    cf_pal[FL_INK3]=cfn_blend(p[0],p[2],92);
    cf_pal[C_ACC(0)]=p[4];cf_pal[C_ACC(1)]=p[5];cf_pal[C_ACC(2)]=cfn_blend(p[0],p[5],68);
    cf_pal[C_ACC(3)]=cfn_blend(0xFFFFFF,p[4],72);
    for(unsigned i=0;i<4;i++)cf_pal[CFN_HALO_0+i]=cfn_blend(p[0],p[5],12+12*i);
    fb_palette(cf_pal,CFN_PAL_N);
}
static int cf_cord_offset(unsigned age,unsigned duration,unsigned distance)
{
    if(!cf_motion || age>=duration)return 0;
    unsigned left=duration-age;return distance*left*left*left/(duration*duration*duration);
}
static void cf_cord_track(int baseline,const char *s,int gap,uint8_t ink)
{
    int x=120-(cfn_width(&CFN_12,s)+(int)(str_len(s)-1)*gap)/2;
    for(;*s;s++){char c[2]={*s,0};cfn_text(&CFN_12,x,baseline,c,ink);x+=cfn_width(&CFN_12,c)+gap;}
}
static void cf_cord_draw(unsigned elapsed)
{
    unsigned t=cf_motion?(elapsed>CF_CORD_BOOT_MS?CF_CORD_BOOT_MS:elapsed):CF_CORD_BOOT_MS;
    /* Quantize only presentation work; no repeated raster/hash/LCD at rest. */
    t=t/40*40;
    if(cf_cord_last.valid && !fb_full && cf_cord_last.t==t && cf_cord_last.skin==cf_skin && cf_cord_last.motion==cf_motion)return;
    cf_cord_last.t=t;cf_cord_last.skin=cf_skin;cf_cord_last.motion=cf_motion;cf_cord_last.valid=1;
    cf_cord_palette();fb_unclip();fb_fill(FL_BG);
    if(t>=760) {
        int r=68+(t<1080?(int)(t-760)*9/320:9);
        cfn_circle(120,82,r,C_ACC(2));cfn_circle(120,82,r-1,C_ACC(2));
        if(cf_motion && t>=1080){int pulse=77+(t-1080)/100;if(pulse>79)pulse=79;cfn_circle(120,82,pulse,CFN_HALO_0);}
    }
    static const uint16_t reveal[7]={160,360,1080,560,1080,760,1080};
    static const uint8_t lit[7]={1,1,0,1,0,1,0};
    for(unsigned i=0;i<7;i++)if(t>=reveal[i]) {
        int dy=cf_cord_offset(t-reveal[i],320,8);
        fb_rbox(62+i*17,57+dy,14,46,2,t>=760 && lit[i]?C_ACC(3):FL_INK3);
    }
    if(t>=1080){fb_rbox(83,57,10,28,2,FL_BG2);fb_rbox(101,57,10,28,2,FL_BG2);fb_rbox(137,57,10,28,2,FL_BG2);cfn_center(&CFN_12,120,129,"Dm7",C_ACC(0));}
    if(t>=850) {
        unsigned age=t-850;int dy=cf_cord_offset(age,450,10),x0=(240-CFN_CORD_W)/2;
        uint8_t ink=age<120?FL_BG3:age<280?FL_INK2:FL_INK;
        for(unsigned y=0;y<CFN_CORD_H;y++) {
            for(unsigned x=0;x<CFN_CORD_W;x++){unsigned bit=y*CFN_CORD_W+x;if(CFN_CORD_BITS[bit/8]&(128u>>(bit&7)))fb_pset(x0+x,165+dy+y,ink);}
            if((y&7)==7)CF_PUMP();
        }
    }
    if(t>=1080){cf_cord_track(216+cf_cord_offset(t-1080,220,4),"CHORD-FIRST FIRMWARE",1,FL_INK2);if(cu.booted)cf_cord_track(236,"FM-1 READY",1,FL_INK2);}
    cf_present();
}
static void cf_cord_finish(void)
{cf_intro_on=0;cf_cord_last.valid=0;cf_palette_build();fb_full=1;}
static int cf_intro_draw(uint32_t dt)
{
    if(!cf_intro_on || cr_safe)return 0;
    if(!cf_motion || cu.idle_ms<=dt){cf_cord_finish();return 0;}
    unsigned actual=fm1_ms-cf_cord_started,next=cf_intro_t+dt;
    cf_intro_t=actual>next?actual:next;
    if(cf_intro_t>=CF_CORD_BOOT_MS){cf_cord_finish();return 0;}
    cf_cord_draw(cf_intro_t);return 1;
}
