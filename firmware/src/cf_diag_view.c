/* SPDX-License-Identifier: GPL-3.0-only */
/* Compact engineering pages, central HOME area only. Background scene still renders underneath. */
static void diag_num(char *b,uint32_t n)
{
    char t[12];uint32_t k=0,i=0;char suffix=0;
    if(n>=10000000u) {n/=1000000u;suffix='M';}
    else if(n>=10000u) {n/=1000u;suffix='k';}
    do {t[k++]=(char)('0'+n%10u);n/=10u;}while(n);
    while(k)b[i++]=t[--k];if(suffix)b[i++]=suffix;b[i]=0;
}
static uint32_t diag_pct(uint32_t n,uint32_t budget)
{ return budget ? n/budget*100u+(n%budget)*100u/budget : 0; }
static void diag_line(int row,const char *label,uint32_t a,const char *mid,uint32_t b,const char *unit,int warn)
{
    char s[48],n[12];
    str_cpy(s,label,sizeof s);diag_num(n,a);str_cpy(s+str_len(s),n,sizeof s-str_len(s));
    if(mid) {str_cpy(s+str_len(s),mid,sizeof s-str_len(s));diag_num(n,b);str_cpy(s+str_len(s),n,sizeof s-str_len(s));}
    if(unit)str_cpy(s+str_len(s),unit,sizeof s-str_len(s));
    fb_text(&CF_FONT_UI,6,Y_VIEW+4+row*15,s,warn?C_ACC(1):FL_INK,1);
}
static void diag_label(int row,const char *s)
{fb_text(&CF_FONT_UI,6,Y_VIEW+4+row*15,s,FL_INK2,1);}
static void draw_home_diag(void)
{
    const uint32_t *v=cfd.v;uint32_t hw=v[D_HARDWARE],peak=diag_pct(v[D_PEAK],v[D_BUDGET]);
    fb_rect(0,Y_VIEW,FB_W,H_VIEW,FL_BG);
    fb_text(&CF_FONT_UI,6,Y_VIEW+4,cfd.hold?"DIAG HOLD":hw?"DIAG LIVE":"DIAG EMU",C_ACC(0),1);
    fb_text(&CF_FONT_UI,128,Y_VIEW+4,CF_DIAG_PAGES[cfd.page],FL_INK2,1);
    switch(cfd.page) {
    case DP_OVERVIEW:
        if(hw && v[D_HALVES]) {
            diag_line(1,"AUDIO ",diag_pct(v[D_CPU_Q8],256),"% PK ",peak,"%",peak>=85);
            diag_line(2,"WORK ",v[D_PEAK],"/",v[D_BUDGET],"us",peak>=85);
        } else {diag_label(1,"AUDIO --  PEAK --");diag_label(2,"TIMING: DEVICE REQUIRED");}
        diag_line(3,"VOX C",v[D_CHORD]," B",v[D_BASS],0,0);
        diag_line(4,"LAYER ",v[D_LAYER]," DRUM ",v[D_DRUMS],0,0);
        diag_line(5,"SHED ",v[D_SHED]," LATE ",v[D_LATE],0,v[D_SHED]||v[D_LATE]);
        diag_line(6,"USB U",v[D_UNDER]," O",v[D_OVER],0,v[D_UNDER]||v[D_OVER]);
        if(hw) {diag_line(7,"RAM ",v[D_RAM]/1024,"/",v[D_RAM_CAP]/1024,"K FIX",0);
            diag_line(8,"POOL ",v[D_POOL]/1024,"/",v[D_POOL_CAP]/1024,"K FIX",0);}
        else {diag_label(7,"RAM/POOL: DEVICE BUILD");diag_line(8,"SESSION ",v[D_SESSION_MS]/1000,0,0,"s",0);}
        break;
    case DP_TIMING:
        diag_line(1,"SESSION ",v[D_SESSION_MS]/1000,0,0,"s",0);
        if(hw) {
            diag_line(2,"AVG ",v[D_AVG]," PK ",v[D_PEAK],"us",0);
            diag_line(3,"WALL ",v[D_WALL]," LAG ",v[D_LAG],"us",0);
            diag_line(4,"LAG+WALL ",v[D_PRESSURE],0,0,"us",v[D_PRESSURE]>=v[D_BUDGET]);
        } else {diag_label(2,"CPU: NOT MEASURED HERE");diag_label(3,"HOST IS NOT THE DEVICE");}
        diag_line(5,"LATE ",v[D_LATE]," STALE~",v[D_STALE],0,v[D_STALE]!=0);
        diag_line(6,"SHED ",v[D_SHED]," STEAL ",v[D_STEAL],0,v[D_SHED]!=0);
        diag_line(7,"UI ",v[D_UI_HZ],"Hz GAP ",v[D_UI_GAP],"ms",0);
        { static const char *const core[]={" C1 OFF"," C1 OK"," C1 FAILED"," C1 FALLBACK"};
          diag_line(8,"TAIL ",v[D_TAIL],0,0,core[v[D_CORE1]<4?v[D_CORE1]:0],0); } break;
    case DP_MEMORY:
        if(hw) {
            diag_line(1,"RAM ",v[D_RAM],"/",v[D_RAM_CAP],"B",0);
            diag_line(2,"RAM LEFT ",v[D_RAM_CAP]-v[D_RAM],0,0,"B",0);
            diag_line(3,"POOL LEFT ",v[D_POOL_CAP]-v[D_POOL],0,0,"B",0);
            diag_line(4,"XIP ",v[D_XIP]/1024,"/",v[D_XIP_CAP]/1024,"K",0);
            diag_line(5,"RAM CODE ",v[D_RAMTEXT],0,0,"B",0);
        } else {diag_label(1,"MEMORY FROM DEVICE LINKER");diag_label(2,"HOST SIZES NOT SHOWN");}
        diag_label(6,"STACK0: NOT INSTRUMENTED");
        if(v[D_CORE1]==C1_OK)diag_line(7,"C1 STACK ",v[D_C1_STACK],"/",2816,"B",0);
        else diag_label(7,"CORE1: OFF/UNAVAILABLE");
        diag_label(8,"STATIC RAM IS NOT HEAP");break;
    default:
        diag_label(1,!(v[D_USB_STATE]&1)?"USB OFF":!(v[D_USB_STATE]&2)?"USB CLOSED":
            v[D_USB_STATE]& (4|8|16)?"USB HOLD/SUSPEND":"USB CAPTURE");
        diag_line(2,"RATE ",v[D_USB_RATE],0,0,"Hz",0);
        diag_line(3,"RING ",v[D_RING],"/",v[D_RING_CAP],"fr",0);
        diag_line(4,"UNDERRUN ",v[D_UNDER],0,0,0,v[D_UNDER]!=0);
        diag_line(5,"OVERRUN ",v[D_OVER],0,0,0,v[D_OVER]!=0);
        diag_line(6,"MISSED ",v[D_MISS],0,0,0,v[D_MISS]!=0);
        if(hw)diag_line(7,"RESAMPLE PK ",v[D_RS_PEAK],0,0,"us",0);
        else diag_label(7,"TIMING: DEVICE REQUIRED");
        diag_line(8,"STALE EST ",v[D_STALE],0,0,0,v[D_STALE]!=0);break;
    }
}
