/* SPDX-License-Identifier: GPL-3.0-only
 * House style shared by HOME and every native control surface. The character
 * palette is global (cf_palette_build); these metrics keep the footer global too.
 */
enum { CFN_FOOTER_Y=198, CFN_LABEL_BASE=214, CFN_VALUE_BASE=233, CFN_HINT_BASE=193 };
static void cfn_controls(const char label[4][12],const char value[4][14],const uint8_t disabled[4],const uint8_t emphasis[4])
{
    fb_unclip();fb_rect(0,CFN_FOOTER_Y,FB_W,FB_H-CFN_FOOTER_Y,FL_BG);
    fb_hline(10,CFN_FOOTER_Y,220,FL_BG3);
    for(int k=1;k<4;k++)fb_vline(k*60,203,34,FL_BG3);
    for(int k=0;k<4;k++) {
        int x=k*60;char name[12],text[14];
        str_cpy(name,label[k],sizeof name);str_cpy(text,value[k],sizeof text);
        for(unsigned j=1;name[j];j++)if(name[j]>='A' && name[j]<='Z')name[j]+='a'-'A';
        /* The Tempo label carries the unit; retain EXT and WAIT clock states. */
        unsigned n=str_len(text);if(n>=4 && !memcmp(text+n-4," BPM",4))text[n-4]=0;
        fb_clip(x+3,201,x+57,240);
        int width=cfn_width(&CFN_11,name);
        cfn_fit(&CFN_11,width>54?x+3:x+30-width/2,CFN_LABEL_BASE,name,54,FL_INK2);
        const cfn_font_t *face=cfn_width(&CFN_14,text)<=54?&CFN_14:&CFN_12;
        width=cfn_width(face,text);
        cfn_fit(face,width>54?x+3:x+30-width/2,CFN_VALUE_BASE,text,54,disabled[k]?FL_INK2:k==0?C_ACC(0):FL_INK);
        if(emphasis[k])fb_hline(x+30-emphasis[k],239,2*emphasis[k],C_ACC(0));
    }
    fb_unclip();
}
static void cfn_hint(const char *text,uint8_t ink)
{ cfn_fit(&CFN_11,10,CFN_HINT_BASE,text,220,ink); }
