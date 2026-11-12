/* SPDX-License-Identifier: GPL-3.0-only
 * Shared native page chrome and control readouts. Studio uses the same
 * materials as the rack, with character colors available for content.
 */
enum { CFN_FOOTER_Y=198, CFN_LABEL_BASE=214, CFN_VALUE_BASE=233, CFN_HINT_BASE=193 };
static void cgu_grille(int x,int y,int w,int h)
{
    int left=clampi(fb_cx0-x,0,w), right=clampi(fb_cx1-x,0,w);
    if(left>=right)return;
    for(int row=clampi(fb_cy0-y,0,h),end=clampi(fb_cy1-y,0,h);row<end;row++){
        unsigned shade=row<2?1:row<h/2?3:2;
        uint8_t *line=fb+(y+row)*FB_W+x+left;
        for(int col=left;col<right;col++)
            *line++=(uint8_t)(CGU_GRILLE+shade-(((col+1+2*(row&1))&3)<2));
    }
}

static void cfn_page_header(const char *title,const char *status,uint8_t ink)
{
    fb_unclip();
    fb_fill(CF_STUDIO && !CF_SCENES?CGU_METAL+10:FL_BG);
    int right=230, left=10;
    if(CF_STUDIO && !CF_SCENES){
        cf_rack_plate(3,2,234,26,7,FL_BG2);
        cgu_grille(11,4,218,22);
        fb_hline(8,27,224,CGU_METAL+4);
        fb_hline(9,29,20,CGU_PROGRESSION_INK);
        left=15;
        right=226;
    } else fb_hline(10,28,220,FL_BG3);

    int status_width=clampi(cfn_width(&CFN_12,status),0,108);
    int status_x=right-status_width;
    int title_width=(status_width?status_x-13:right)-left;
    const cfn_font_t *font=cfn_width(&CFN_16,title)<=title_width?&CFN_16:&CFN_12;
    if(CF_STUDIO && !CF_SCENES){
        int badge=clampi(cfn_width(font,title),0,title_width);
        fb_rbox(left-5,6,badge+10,18,2,CGU_METAL+10);
        if(status_width){
            cf_rack_window(status_x-6,6,status_width+12,18,CGU_GLASS);
            if(ink==C_ACC(0))ink=RAKT_GREEN;
        }
    }
    cfn_fit(font,left,20,title,title_width,FL_INK);
    if(status_width)cfn_fit(&CFN_12,status_x,20,status,status_width,ink);
}

static void cfn_dialog(int x,int y,int w,int h,uint8_t border)
{
    if(CF_STUDIO && !CF_SCENES){
        fb_rbox(x,y,w,h,3,CGU_METAL+4);
        fb_rbox(x+1,y+1,w-2,h-2,2,CGU_METAL);
        fb_hline(x+6,y+1,w-12,border==FL_BG3?CGU_PROGRESSION_INK:border);
    } else {
        fb_rect(x,y,w,h,FL_BG);
        fb_frame(x,y,w,h,border);
    }
}

static void cfn_controls(const char label[4][12],const char value[4][14],const uint8_t disabled[4],const uint8_t emphasis[4])
{
    const int rack=CF_STUDIO && !CF_SCENES;
    fb_unclip();
    if(rack){
        fb_rbox(2,CFN_FOOTER_Y,236,FB_H-CFN_FOOTER_Y,2,FL_BG2);
        fb_hline(8,CFN_FOOTER_Y,224,CGU_METAL+4);
    } else {
        fb_rect(0,CFN_FOOTER_Y,FB_W,FB_H-CFN_FOOTER_Y,FL_BG);
        fb_hline(10,CFN_FOOTER_Y,220,FL_BG3);
        for(int k=1;k<4;k++)fb_vline(k*60,203,34,FL_BG3);
    }
    for(int k=0;k<4;k++) {
        int x=k*60;char name[12],text[14];
        if(rack && label[k][0])fb_rbox(x+2,203,56,34,2,CGU_GLASS);
        str_cpy(name,label[k],sizeof name);str_cpy(text,value[k],sizeof text);
        for(unsigned j=1;name[0] && j<sizeof name && name[j];j++)if(name[j]>='A' && name[j]<='Z')name[j]+='a'-'A';
        /* The Tempo label carries the unit; retain EXT and WAIT clock states. */
        unsigned n=str_len(text);if(n>=4 && !memcmp(text+n-4," BPM",4))text[n-4]=0;
        fb_clip(x+3,201,x+57,240);
        int width=cfn_width(&CFN_11,name);
        cfn_fit(&CFN_11,width>54?x+3:x+30-width/2,CFN_LABEL_BASE,name,54,FL_INK2);
        const cfn_font_t *face=cfn_width(&CFN_14,text)<=54?&CFN_14:&CFN_12;
        width=cfn_width(face,text);
        cfn_fit(face,width>54?x+3:x+30-width/2,CFN_VALUE_BASE,text,54,
            disabled[k]?FL_INK2:k==0?(rack?CGU_PROGRESSION_INK:C_ACC(0)):FL_INK);
        if(emphasis[k])fb_hline(x+30-emphasis[k],239,2*emphasis[k],rack?CGU_PROGRESSION_INK:C_ACC(0));
        fb_unclip();
    }
}
static void cfn_hint(const char *text,uint8_t ink)
{ cfn_fit(&CFN_11,10,CFN_HINT_BASE,text,220,ink); }
