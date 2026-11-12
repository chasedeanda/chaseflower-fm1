/* SPDX-License-Identifier: GPL-3.0-only
 * Compact Cord devil boot: existing mascot/wordmark assets, generated glow.
 * No baked GIF frames, heap or additional drawing buffer.
 */
#define CF_CORD_BOOT_MS 1800u
static uint8_t cf_intro_on;
static uint32_t cf_intro_t,cf_cord_started;
static struct { uint16_t t;uint8_t valid; } cf_cord_last;

static void cf_cord_begin(void)
{cf_cord_started=fm1_ms;cf_intro_t=0;cf_cord_last.valid=0;}

static void cf_cord_palette(void)
{
    for(unsigned i=0;i<32;i++)cf_pal[8+i]=cfn_blend(0x0C0707,0x823012,i*255/31);
    cf_pal[FL_BG]=0x0C0707;cf_pal[FL_INK]=0xFFFFFF;
    cf_pal[48]=0xFFA55B;cf_pal[FL_INK2]=0xFFFFFF;cf_pal[FL_INK3]=0x999999;
    for(unsigned i=0;i<CFM_ART_COLORS;i++)cf_pal[CFM_PAL_BASE+i]=CFM_BOOT_ART.rgb[i];
    cf_pal[CFN_AA_BASE]=cfn_blend(cf_pal[FL_BG],0xFFFFFF,85);
    cf_pal[CFN_AA_BASE+1]=cfn_blend(cf_pal[FL_BG],0xFFFFFF,170);
    fb_palette(cf_pal,CFN_PAL_N);
}

static void cf_cord_devil(int top,int w,int h)
{
    const cfm_art_t *art=&CFM_BOOT_ART;
    for(int y=0;y<h;y++) {
        int yy=top+y;if((unsigned)yy>=FB_H)continue;
        const uint8_t *row=art->data+art->rows[(unsigned)y*CFM_ART_H/(unsigned)h];
        for(int x=0;x<w;x++) {
            unsigned sx=(unsigned)x*CFM_ART_W/(unsigned)w,index=sx-row[0];
            if(index<row[1]){unsigned ink=row[2+index];if(ink)fb_pset(120-w/2+x,yy,CFM_PAL_BASE+ink-1);}
        }
        if((y&7)==7)CF_PUMP();
    }
}

static void cf_cord_draw(unsigned elapsed)
{
    unsigned t=cf_motion?(elapsed>CF_CORD_BOOT_MS?CF_CORD_BOOT_MS:elapsed):CF_CORD_BOOT_MS;
    t=t/40*40;
    if(cf_cord_last.valid && !fb_full && cf_cord_last.t==t)return;
    cf_cord_last.t=t;cf_cord_last.valid=1;cf_cord_palette();fb_unclip();
    for(int y=0;y<FB_H;y++) {
        for(int x=0;x<FB_W;x++) {
            int dx=x-120,dy=y-105,d=dx*dx+dy*dy-4225;if(d<0)d=-d;
            int shade=d<1100?(1100-d)*14/1100:0;
            dy=y-166;d=dx*dx+dy*dy*16;
            if(d<2200)shade+=(2200-d)*13/2200;
            fb[y*FB_W+x]=shade?(uint8_t)(8+shade):FL_BG;
        }
        if((y&15)==15)CF_PUMP();
    }
    /* Reuse the approved idle face. One fall, squash and settle, then rest. */
    if(t>=160) {
        int top,w=92,h=92;
        if(t<640)top=-92+(int)(t-160)*(int)(t-160)*190/(480*480);
        else if(t<880){unsigned a=t-640;top=98-(int)a*24/240;w=104-(int)a*12/240;h=68+(int)a*24/240;}
        else top=74;
        cf_cord_devil(top,w,h);
        if(t>=440){fb_line(157,79,160,71,48);fb_line(162,82,168,77,48);}
    }
    /* Existing tracked wordmark, sampled down to the supplied layout. White
     * throughout, including the subtitle; no theme tint or orange fade. */
    for(unsigned y=0;y<20;y++)for(unsigned x=0;x<96;x++) {
        unsigned bit=(y*CFN_CORD_H/20)*CFN_CORD_W+x*CFN_CORD_W/96;
        if(CFN_CORD_BITS[bit/8]&(128u>>(bit&7)))fb_pset(72+x,178+y,FL_INK);
    }
    cfn_center(&CFN_11,120,215,"CHORD-FIRST FIRMWARE",FL_INK);
    cf_present();
}
static void cf_cord_finish(void)
{cf_intro_on=0;cf_cord_last.valid=0;cf_palette_build();fb_full=1;}
static int cf_intro_draw(uint32_t dt)
{
    if(!cf_intro_on || cr_safe)return 0;
    if(!cf_motion || cu.idle_ms<=dt){cf_cord_finish();return 0;}
    uint32_t actual=fm1_ms-cf_cord_started;
    uint32_t next=dt>=CF_CORD_BOOT_MS-cf_intro_t?CF_CORD_BOOT_MS:cf_intro_t+dt;
    cf_intro_t=actual>next?actual:next;
    if(cf_intro_t>=CF_CORD_BOOT_MS){cf_cord_finish();return 0;}
    cf_cord_draw(cf_intro_t);return 1;
}
