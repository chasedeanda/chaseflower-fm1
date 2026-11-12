/* SPDX-License-Identifier: GPL-3.0-only
 * Studio outboard gear. The plate, readout, controls and lamps share one
 * layout from the overview through the panel zoom and control close-up. */
static int cgu_unit_blend(int from,int to,int zoom)
{ return from+(to-from)*zoom/100; }

static int cgu_unit_controls(unsigned band)
{
    if(band==CGU_BAND_PROGRESSION)return 1;
    return band==CGU_BAND_GENRE?2:4;
}

static int cgu_unit_knob_x(cgu_face_t f,int i,int count)
{ return f.x+f.w*(1+2*i)/(count==2?4:8); }

static int cgu_unit_knob_y(cgu_face_t f,int zoom)
{ return f.y+cgu_unit_blend(50,f.h*64/100,zoom); }

static cgu_face_t cgu_unit_lens(cgu_face_t f,int knob,int count,int scale)
{
    int x=cgu_unit_knob_x(f,knob,count), y=cgu_unit_knob_y(f,100);
    return (cgu_face_t){f.x+(f.x-x)*scale/100,f.y+(f.y-y)*scale/100,
        f.w+f.w*scale/100,f.h+f.h*scale/100};
}

static void cgu_unit_screw(int x,int y,int large)
{
    if(large){
        cgu_metal_disc(x,y,2,7,5,2);
        fb_hline(x-1,y,3,CGU_METAL);
    } else {
        fb_pset(x,y,CGU_METAL+6);
        fb_pset(x,y+1,CGU_METAL+10);
    }
}

static void cgu_unit_vents(int x,int y,int w,int h,int pitch)
{
    fb_rbox(x,y,w,h,1,CGU_METAL+1);
    fb_hline(x+1,y+h-1,w-2,CGU_METAL+4);
    for(int i=2;i+pitch<=w;i+=pitch){
        fb_rect(x+i,y+2,pitch-2,h-4,CGU_METAL+10);
        fb_vline(x+i+pitch-2,y+3,h-5,CGU_METAL+4);
    }
}

static void cgu_unit_shell(cgu_face_t f,unsigned band,int zoom)
{
    int x=f.x,y=f.y,w=f.w,h=f.h;
    unsigned finish=band==CGU_BAND_PROGRESSION?CGU_AMP_FACE:CGU_UNIT_FACE+(band-1u)*CGU_UNIT_SHADES;
    int ear=cgu_unit_blend(7,8,zoom);
    fb_rect(x,y,w,h,CGU_METAL+10);
    fb_rect(x+1,y+1,w-2,h-3,CGU_METAL+1);
    int plate=x+ear, width=w-ear*2;
    for(int row=clampi(fb_cy0-y,2,h-3),end=clampi(fb_cy1-y,2,h-3);row<end;row++){
        int shade=5-row*3/h;
        fb_hline(plate,y+row,width,finish+shade);
        if(band==CGU_BAND_HARMONY && (row&7)==3){
            /* Short reflections along the nickel's machining direction. */
            int start=10+((row*37)%(width/2));
            fb_hline(plate+start,y+row,width/5,finish+shade+1);
        }
    }
    fb_hline(plate,y+1,width,finish+6);
    fb_vline(plate,y+2,h-5,finish+5);
    fb_vline(plate+width-1,y+2,h-5,finish+1);
    fb_hline(plate,y+h-3,width,finish+1);
    fb_hline(plate+1,y+h-2,width-2,CGU_METAL+10);
    for(int i=0;i<2;i++){
        int rail=i?x+w-1:x+1, screw=i?x+w-ear/2-2:x+ear/2+1;
        fb_vline(rail,y+2,h-5,CGU_METAL+4);
        cgu_unit_screw(screw,y+cgu_unit_blend(5,9,zoom),zoom>35);
        cgu_unit_screw(screw,y+h-cgu_unit_blend(6,10,zoom),zoom>35);
    }
    if(zoom>68){
        /* A restrained brushed highlight next to the controls and two wear marks. */
        fb_hline(x+ear+4,y+h*52/100,w-ear*2-8,finish+1);
        fb_hline(x+w-ear-22,y+h-8,8,finish+4);
        fb_hline(x+w-ear-14,y+h-7,4,finish+4);
    }
}

static void cgu_unit_lamps(const cf_stage_model_t *m,unsigned band,int x,int y,int w,int h,int zoom,unsigned ink)
{
    static const char *const labels[]={"BEAT","OUTPUT","PROGRAM","STEP","CHORD SLOT"};
    unsigned cells=8, on=0, fill=0;
    switch(band){
    case CGU_BAND_HARMONY: cells=m->rack_meter;on=m->rack_beat;break;
    case CGU_BAND_SHAPING: fill=m->rack_level;break;
    case CGU_BAND_GENRE: cells=CGS_NGENRE;on=m->genre+1u;break;
    case CGU_BAND_ARP: cells=16;on=m->rack_arp_step;break;
    case CGU_BAND_PROGRESSION:
        cells=m->count>8?16:8;on=m->slot;break;
    }
    if(cells>8 && zoom<=54){
        cells=(cells+1u)/2u;
        on=(on+1u)/2u;
    }
    if(!cells)return;
    fb_rbox(x-1,y-1,w+2,h+2,1,CGU_METAL+1);
    fb_hline(x,y+h,w,CGU_METAL+5);
    fb_rbox(x,y,w,h,1,CGU_GLASS);
    int step=(w-4)/(int)cells, gap=zoom>50?2:1;
    for(unsigned i=0;i<cells;i++){
        int left=x+(w-step*(int)cells)/2+step*(int)i;
        unsigned lit=band==CGU_BAND_SHAPING && i>=6?CGU_BAND_INK_BASE+2:ink;
        fb_rect(left,y+2,step-gap,h-4,(on==i+1u || i<fill)?lit:CGU_METAL+3);
    }
    if(zoom>68)cfn_center(&CFN_11,x+w/2,y+h+11,labels[band-1u],FL_INK2);
}

static void cgu_unit_dial(unsigned band,int x,int y,int r,unsigned pos,unsigned marker)
{
    if(band==CGU_BAND_PROGRESSION){
        fb_rbox(x-r,y-r/2,r*2+1,r+1,2,CGU_METAL+10);
        fb_hline(x-r+2,y+r/2,r*2-3,CGU_METAL+5);
        int travel=pos?r/2:-r/2;
        fb_rbox(x+travel-r/3,y-r/2+1,r*2/3+1,r-2,2,CGU_METAL+4);
        fb_hline(x+travel-r/3+2,y-r/2+2,r*2/3-3,CGU_METAL+8);
        return;
    }
    cgu_metal_disc(x,y+1,r+1,10,10,10);
    switch(band){
    case CGU_BAND_HARMONY:
        cgu_metal_disc(x,y,r,8,5,1);
        cgu_metal_disc(x,y,r-2,4,1,0);
        if(r>9){
            fb_vline(x-r+1,y-2,5,CGU_METAL+6);
            fb_vline(x+r-1,y-2,5,CGU_METAL+3);
        }
        break;
    case CGU_BAND_SHAPING:
        cgu_metal_disc(x,y,r,4,2,0);
        cgu_metal_disc(x,y,r-2,3,1,0);
        cfn_circle(x,y,r-1,CGU_METAL+2);
        break;
    case CGU_BAND_GENRE:
        cgu_metal_disc(x,y,r,9,7,3);
        cgu_metal_disc(x,y,r-3,9,8,6);
        marker=CGU_METAL+10;
        break;
    default: /* Machined cap with a polished rim and a dark pointer. */
        cgu_metal_disc(x,y,r,9,6,3);
        cgu_metal_disc(x,y,r-2,7,6,5);
        marker=CGU_METAL+10;
        break;
    }
    cgu_dial_mark(x,y,r,pos,marker);
}

static const char *cgu_unit_label(const char *name,char label[12])
{
    if(cb_streq(name,"ARP FOR"))return "For";
    str_cpy(label,name,12);
    for(char *c=label+1;*c;c++)if(*c>='A' && *c<='Z')*c=(char)(*c-'A'+'a');
    return label;
}

static void cgu_unit_readout(int left,int baseline,int width,const char *value,unsigned ink)
{
    const cfn_font_t *font=cfn_width(&CFN_14,value)<=width?&CFN_14:
        cfn_width(&CFN_12,value)<=width?&CFN_12:&CFN_11;
    cfn_fit(font,left,baseline,value,width,ink);
}

static void cgu_band_unit(const cf_stage_model_t *m,unsigned band,cgu_face_t f,int zoom,int selected)
{
    unsigned ink=cgu_module_ink(CGS_NPART+band-1u);
    int count=cgu_unit_controls(band);
    if(selected && m->rack_lens)f=cgu_unit_lens(f,cgu.turn_knob,count,m->rack_lens);
    int x=f.x,y=f.y,w=f.w,h=f.h;
    cgu_unit_shell(f,band,zoom);

    const char *title=CGU_BAND_NAMES[band-1u];
    int left=x+16, title_top=y+6;
    int title_w=cfn_width(&CFN_16,title)+11;
    int title_h=21;
    if(title_top+4>=fb_cy0)cfn_text(&CFN_16,left,title_top+16,title,FL_INK);

    char summary[20];
    const char *value=band==CGU_BAND_PROGRESSION?(m->rack_draft?"Draft":"Bank"):
        cgu_module_value(m,band,m->focus,summary);
    int readout_x=left+title_w, readout_w=x+w-16-readout_x;
    fb_rbox(readout_x-1,title_top-1,readout_w+2,title_h+2,2,CGU_METAL+1);
    fb_hline(readout_x+1,title_top+title_h,readout_w-2,CGU_METAL+5);
    fb_rbox(readout_x,title_top,readout_w,title_h,2,CGU_GLASS);
    if(title_top+4>=fb_cy0)cgu_unit_readout(readout_x+6,title_top+16,readout_w-12,value,ink);

    int meter_w=cgu_unit_blend(67,87,zoom);
    int meter_x=x+w-meter_w-18;
    int meter_y=y+cgu_unit_blend(30,42,zoom);
    int meter_h=cgu_unit_blend(8,13,zoom);
    cgu_unit_lamps(m,band,meter_x,meter_y,meter_w,meter_h,zoom,ink);
    if(zoom<=70 && band!=CGU_BAND_HARMONY && band!=CGU_BAND_ARP)
        cgu_unit_vents(left,meter_y,30,meter_h,5);

    if(zoom>70){
        if(band==CGU_BAND_ARP){
            const char *page=m->shaping?"Timing":"Pattern";
            cfn_text(&CFN_12,left,meter_y+11,page,FL_INK2);
        } else {
            int badge=left+1, vent=badge+46;
            fb_rbox(badge,meter_y,36,14,1,CGU_METAL+1);
            fb_hline(badge+1,meter_y,34,CGU_METAL+5);
            fb_hline(badge+1,meter_y+13,34,CGU_METAL+10);
            cgu_unit_screw(badge+2,meter_y+6,0);
            cgu_unit_screw(badge+33,meter_y+6,0);
            cfn_center(&CFN_11,badge+18,meter_y+11,"CORD",FL_INK);
            cgu_unit_vents(vent,meter_y,meter_x-vent-10,14,5);
        }
    }

    int detail_radius=count==1?19:15;
    int radius=cgu_unit_blend(8,detail_radius,zoom)+(selected?m->rack_lens*detail_radius/100:0);
    int cy=cgu_unit_knob_y(f,zoom);
    int clearance=meter_y+meter_h+(zoom>68?11:0)+radius+4;
    if(cy<clearance)cy=clearance;
    int value_y=cy+radius+29;
    if(value_y>fb_cy1-4)value_y=fb_cy1-4;
    if(band==CGU_BAND_SHAPING && zoom>70){
        unsigned finish=CGU_UNIT_FACE+CGU_UNIT_SHADES;
        fb_rbox(x+14,cy-radius-4,w-28,2*radius+8,2,finish+1);
        fb_hline(x+16,cy+radius+3,w-32,finish+5);
    }
    for(int i=0;i<count;i++){
        int cx=cgu_unit_knob_x(f,i,count);
        unsigned pos=selected?m->knob[i]:m->rack_knob[band-1u][i];
        int active=selected && m->emphasis[i] && !m->disabled[i];
        cgu_unit_dial(band,cx,cy,radius,pos,active?ink:selected && m->disabled[i]?CGU_METAL+6:FL_INK);
        if(zoom<74 || !selected || !m->label[i][0] || value_y>y+h-3)continue;
        if(active && band!=CGU_BAND_PROGRESSION)cfn_circle(cx,cy,radius+5,ink);
        int available=(cx-fb_cx0<fb_cx1-cx?cx-fb_cx0:fb_cx1-cx)*2;
        int cell=w/(count==2?2:4)-4;
        char label[12];
        const char *name=cgu_unit_label(m->label[i],label);
        if(cfn_width(&CFN_12,name)<=available)
            cfn_center(&CFN_12,cx,value_y-16,name,active?ink:FL_INK2);
        if(band==CGU_BAND_GENRE && i==0)continue; /* The full genre name is on the display. */
        const cfn_font_t *font=active?&CFN_16:&CFN_14;
        if(cfn_width(font,m->value[i])>cell)font=&CFN_12;
        if(cfn_width(font,m->value[i])>cell)font=&CFN_11;
        int width=cfn_width(font,m->value[i]);
        if(width<=cell && width<=available)
            cfn_center(font,cx,value_y,m->value[i],m->disabled[i]?FL_INK2:FL_INK);
        else if(cell<=available && width>cell)
            cfn_fit(font,cx-cell/2,value_y,m->value[i],cell,FL_INK);
    }
    if(zoom>74 && count==1){
        int top=cy-radius+2, start=x+w/4+12;
        int width=(x+w-start-22)/2;
        cgu_unit_vents(start,top,width-3,2*radius-2,7);
        cgu_unit_vents(start+width+3,top,width-3,2*radius-2,7);
        cgu_unit_readout(start,value_y,x+w-start-19,m->applied,FL_INK);
    }
    if(selected && zoom<70)fb_vline(x+8,y+2,h-4,ink);
}
