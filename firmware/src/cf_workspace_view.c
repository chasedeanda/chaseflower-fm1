/* SPDX-License-Identifier: GPL-3.0-only
 * Draw the active workspace in the shared indexed framebuffer. Only the body
 * pans and scales. Tabs and controls are drawn at their fixed coordinates. */
static void cgu_take_model(cf_stage_model_t *m)
{
    const sq_pat_t *p=&sq_st.pat[sqk.pat];
    unsigned bar=sqk.nbar<SQ_BARS?sqk.nbar:SQ_BARS-1;
    unsigned first=bar?bar-1:0;
    unsigned unit=sq_beats*SQ_UB;
    if(!unit)return;
    for(unsigned i=0;i<p->nev;i++){
        const sq_ev_t *e=&p->ev[i];
        unsigned step=e->pos*CGS_STEPS/unit;
        if(e->trk<CGS_DRUMS && e->bar>=first && e->bar<first+2 && step<CGS_STEPS)
            m->steps[e->trk][e->bar-first]|=(uint16_t)(1u<<step);
    }
    for(unsigned b=0;b<2;b++)for(unsigned lane=0;lane<SQ_NLANE;lane++)
        m->steps[CGS_DRUMS][b]|=(uint16_t)p->hit[lane][first+b];
    m->rec=1;
    if(sqk.rolled && sqk.bt){
        unsigned step=sqk.t*CGS_STEPS/sqk.bt;
        if(step>=CGS_STEPS)step=CGS_STEPS-1;
        m->beat=(uint8_t)(1+(bar-first)*CGS_STEPS+step);
    }
}

static void cgu_theory(cf_stage_model_t *m,int active)
{
    unsigned scale=active?cf_sn.ac_scale:cs.chord.scale;
    unsigned tonic=active?cf_sn.ac_tonic:cs.chord.tonic;
    unsigned meter=active?cf_sn.ac_beats:cgs.meter;
    char key[8],sig[8],tempo[12];
    kpc(key,(int)tonic);
    kint(sig,meter,meter==6u?"/8":"/4");
    kint(tempo,cs.clock_in==1u?cf_sn.ext_bpm:cs.bpm,cs.clock_in==1u?" EXT":" BPM");
    str_cpy(m->theory,key,sizeof m->theory);
    cf_stage_append(m->theory,sizeof m->theory," ");
    const cf_scale_t *mode=&CF_SCALES[scale%CF_NSCALES];
    const char *scale_name=mode->name;
    if(cfn_width(&CFN_11,key)+cfn_width(&CFN_11,scale_name)+
        cfn_width(&CFN_11,sig)+cfn_width(&CFN_11,tempo)+5*cfn_width(&CFN_11," ")>134)scale_name=mode->shrt;
    cf_stage_append(m->theory,sizeof m->theory,scale_name);
    cf_stage_append(m->theory,sizeof m->theory,"  ");
    cf_stage_append(m->theory,sizeof m->theory,sig);
    cf_stage_append(m->theory,sizeof m->theory,"  ");
    cf_stage_append(m->theory,sizeof m->theory,tempo);

    if(cf_sn.out.sounding && cf_sn.out.mask){
        cf_lab_numeral(scale,tonic,cf_sn.out.root_pc,cf_sn.out.mask,m->degree);
    } else if(active && cf_sn.ac_slot<cf_sn.ac_n){
        unsigned slot=cf_sn.ac_slot;
        cf_lab_numeral(scale,tonic,cf_sn.ac_root[slot]%12,cf_sn.ac_mask[slot],m->degree);
    } else if(cgs.ready && cpa_setup.plan.studio){
        const cpa_slot_t *s=&cpa_setup.plan.slot[0];
        cf_lab_numeral(scale,tonic,s->root%12,s->mask,m->degree);
    }
}

static unsigned cgu_workspace_tiles(unsigned tab)
{
    if(tab==CGU_STUDIO)return CGU_RACK_COUNT;
    return CGS_NPART+(tab==CGU_SOUND || tab==CGU_RECORD);
}

static unsigned cgu_workspace_tile(const cf_stage_model_t *m)
{
    if(m->workspace==CGU_STUDIO)return cgu_studio_tile(m->focus,m->bank);
    if(m->workspace==CGU_HELP)return m->focus;
    return m->bank?CGS_NPART:cgu_part(m->focus);
}

static void cgu_model(cf_stage_model_t *m)
{
    int active=cgs_active();
    unsigned idea=active?cf_sn.ac_idea:cpa_setup.plan.idea;
    const cf_progression_t *p=(active || cgs.ready)?cf_progression_get(idea):0;
    m->engine_page=3;
    m->workspace=cgu.tab;
    m->zoom=(uint8_t)cgu_zoom();
    int detail=cgs.zoom;
    m->signal_peak=(uint8_t)cfn_motion.energy;
    m->mixer=cgu.mixer;
    m->bank=cgu.tab==CGU_STUDIO?cgu.band_panel:cgu.rack_controls;
    m->bank_dx=(int8_t)(cf_tween_offset(cgu.tab_ms,185,96)*cgu.tab_dir);
    m->shaping=cgu.band_panel==CGU_BAND_ARP?cgu_arp_page(detail):detail && cgu.part_harmony;
    m->sound_page=cgu_tone_page(detail);
    m->genre=cgs.genre;
    m->solo=cgs.solo|cgs.held_solo;
    m->muted=cgs.muted|cgs.held_mute;
    m->focus=cgu.tab==CGU_HELP && !m->mixer?cgu.topic:cgs.focus;
    m->live=cgs.zoom;
    m->pin_mask=(uint8_t)cgs_mute_mask();
    memcpy(m->part_level,cgs.level,sizeof m->part_level);
    m->activity=cf_sn.visual_parts & ~m->pin_mask;
    str_cpy(m->status,sqk.on || cf_sn.sq_rec!=255?"Recording":
        active && (cf_sn.ac_waiting || cf_sn.ac_paused)?"MIDI wait":
        active || cf_sn.sq_on || cf_sn.dm_on?"Playing":
        cgu.tab==CGU_STUDIO?"PLAY to start":"Ready",sizeof m->status);
    static const char *const feel[]={"Low Tide","Glass Chorus","Night Drive"};
    str_cpy(m->context,p?p->name:idea>=CF_P_COMPOSED && idea<CF_P_COMPOSED+NELEM(feel)?
        feel[idea-CF_P_COMPOSED]:"Press PLAY",sizeof m->context);
    if(idea==CF_P_DRAFT)str_cpy(m->context,cgs_custom_harmony.name,sizeof m->context);
    cgu_theory(m,active);
    if(cgu.tab==CGU_STUDIO){
        if(m->bank==CGU_BAND_PROGRESSION)
            str_cpy(m->applied,(active?idea==CF_P_DRAFT:cgs.custom)?cgs_custom_harmony.name:"Genre bank",sizeof m->applied);
        if(active && cpa_setup.flags)str_cpy(m->pending,
            cpa_setup.plan.idea==CF_P_DRAFT?"Draft next bar":cpa.plan.idea==CF_P_DRAFT?"Bank next bar":
            cgs.genre!=cgs.playing_genre?"Genre next bar":"Take next bar",sizeof m->pending);
        else if(cgs.pending)str_cpy(m->pending,"Part queued",sizeof m->pending);
        else if(cgu.action && cu.now-cgu.action_ms<1300)
            str_cpy(m->pending,cgu.action==1?"Part regenerated":"New take ready",sizeof m->pending);
    }
    if(cgu.tab==CGU_STUDIO || cgu.tab==CGU_SOUND){
        for(unsigned i=0;i<3;i++)str_cpy(m->rail[i],cf_snd[i].name,sizeof m->rail[i]);
        str_cpy(m->rail[CGS_DRUMS],SQ_KITS[cgu_kit()%SQ_NKIT].name,sizeof m->rail[0]);
    }
    if(cgu.tab==CGU_SOUND){
        if(!m->bank && m->sound_page==CGU_PRESETS){
            cfx_sync();
            str_cpy(m->applied,cfx_loaded_name(),sizeof m->applied);
        }
        if(active && (cpa_edit.flags&CPA_GROOVE))str_cpy(m->pending,"Kit queued",sizeof m->pending);
    }
    if(cgu.tab==CGU_RECORD && sqk.on && sqk.pat<SQ_NPAT)cgu_take_model(m);
    else if(cgu.tab==CGU_RECORD){
        uint8_t cells[CGS_NPART][2][CGS_STEPS];
        fm1_irq_off();
        uint32_t tick=cpa.t, bar=cgs.bar-cgs.origin_bar;
        unsigned tick_step=cpa.plan.beats==6u?CGS_TICKS*2u:CGS_TICKS;
        for(unsigned i=0;i<CGS_NPART;i++)memcpy(cells[i],cgs.active[i].event,sizeof cells[i]);
        fm1_irq_on();
        m->beat=active?(uint8_t)(1+(bar&1u)*CGS_STEPS+tick/tick_step):0;
        for(unsigned i=0;i<CGS_NPART;i++)for(unsigned b=0;b<2;b++){
            uint16_t events=i==CGS_CHORDS?1u:0u;
            for(unsigned j=0;j<CGS_STEPS;j++)if(cells[i][b][j])events|=(uint16_t)(1u<<j);
            m->steps[i][b]=events;
        }
    }
    if(cgu.tab==CGU_HELP && m->focus==CGU_HARMONY && m->zoom>82)cf_learn_model(&m->learn);
    if(m->mixer || (cgu.tab==CGU_STUDIO && cgs.focus==CGS_DRUMS)){
        m->moment=cf_sn.perf_pending;
        m->build=cf_sn.perf_dropped;
        m->bloom=cf_sn.perf_fill;
    }
    if(cgu.tab==CGU_SOUND && !m->mixer)m->build=cgu.all_sounds;
    if(cgu.tab==CGU_RECORD){
        m->slot=cgu.take_slot;
        for(unsigned i=0;i<SQ_NPAT;i++)if(sq_has(i))m->count|=(uint8_t)(1u<<i);
        m->total=sq_unavailable;
        if(cgu.clear_down)m->pulse=(uint8_t)clampi((int)(cu.now-cgu.clear_down)/30,0,100);
    }
    if(cgu.turn_ms && cu.now-cgu.turn_ms<180u)m->emphasis[cgu.turn_knob]=1;
    for(unsigned i=0;i<4;i++){
        cf_knob_t knob={.bar=-1};
        cgu_knob_view(i,&knob,detail);
        if(knob.bar>=0)m->knob[i]=(uint8_t)(knob.bar*100/255);
        m->disabled[i]=knob.dis;
        str_cpy(m->label[i],knob.name,sizeof m->label[i]);
        str_cpy(m->value[i],knob.val,sizeof m->value[i]);
    }
}

static void cgu_grid(const cf_stage_model_t *m,unsigned part,int x,int y,int cell,int rows)
{
    for(int b=0;b<rows;b++){
        int bar=rows==1?m->beat>CGS_STEPS:b;
        for(int i=0;i<CGS_STEPS;i++){
            int playing=m->beat==1+bar*CGS_STEPS+i;
            int on=(m->steps[part][bar]>>i)&1u;
            fb_rect(x+i*cell,y+b*17,cell-1,rows==1?4:11,playing?FL_INK:on?(CGU_INK_BASE+part):FL_BG3);
        }
    }
}

static void cgu_help_draw(const cf_stage_model_t *m)
{
    static const char *const lines[4][3]={
        {"PLAY starts or stops the band.","SELECT switches the top tab.","ALGORITHM picks a musician."},
        {0,0,0},
        {"SEL zooms in or out.","SELECT turns tabs or zoomed pages.","GLO opens the live mixer."},
        {"The lanes show generated phrases.","REC starts a manual take.","USB audio captures the mix."}
    };
    if(m->focus==CGU_HARMONY){
        cf_learn_keyboard(&m->learn);
        cfn_fit(&CFN_14,12,183,m->learn.title,216,FL_INK);
        cfn_fit(&CFN_11,12,194,m->learn.help,216,FL_INK2);
    } else {
        for(unsigned i=0;i<3;i++)cfn_fit(&CFN_12,19,120+i*20,lines[m->focus][i],203,FL_INK2);
    }
}

static void cgu_metal_disc(int x,int y,int r,int top,int mid,int bottom)
{
    int span=r;
    for(int row=0;row<=r;row++){
        while(span*span+row*row>r*r)span--;
        fb_hline(x-span,y-row,2*span+1,CGU_METAL+mid+(top-mid)*row/r);
        if(row)fb_hline(x-span,y+row,2*span+1,CGU_METAL+mid+(bottom-mid)*row/r);
    }
}

static void cgu_dial(int x,int y,int r,unsigned value,unsigned ink)
{
    static const int8_t xy[][2]={{-7,7},{-10,2},{-9,-5},{-5,-9},{0,-10},{5,-9},{9,-5},{10,2},{7,7},{7,7}};
    unsigned phase=value*8u, step=phase/100u, mix=phase%100u;
    int dx=xy[step][0]*(int)(100u-mix)+xy[step+1u][0]*(int)mix;
    int dy=xy[step][1]*(int)(100u-mix)+xy[step+1u][1]*(int)mix;
    if(r>8){
        for(unsigned i=0;i<9;i++)fb_pset(x+xy[i][0]*(r+3)/10,y+xy[i][1]*(r+3)/10,CGU_METAL+5);
    }
    cgu_metal_disc(x,y+1,r+1,10,10,10);
    cgu_metal_disc(x,y,r,7,4,1);
    cgu_metal_disc(x,y,r-2,4,2,1);
    fb_line(x+dx/350,y+dy/350,x+dx*(r-2)/1000,y+dy*(r-2)/1000,ink);
}

static void cgu_pages(int x,int y,int w,unsigned tab,unsigned part,unsigned selected)
{
    static const char *const studio[]={"Part","Harmony"};
    static const uint8_t pitched[]={17,27,21,35}, drums[]={25,0,25,50};
    const uint8_t *width=part==CGS_DRUMS?drums:pitched;
    unsigned accent=CGU_INK_BASE+part;
    unsigned count=tab==CGU_SOUND?CGU_SOUND_PAGES:NELEM(studio);
    for(unsigned i=0;i<count;i++){
        int cell=tab==CGU_SOUND?w*width[i]/100:w/2;
        if(!cell)continue;
        const char *name=tab==CGU_SOUND?CGU_SOUND_NAMES[i]:studio[i];
        fb_rect(x,y,cell-2,16,i==selected?FL_BG3:FL_BG);
        if(i==selected)fb_hline(x,y+15,cell-2,accent);
        cfn_fit(&CFN_11,x+4,y+12,name,cell-6,i==selected?FL_INK:FL_INK2);
        x+=cell;
    }
}

static void cgu_send_graph(unsigned effect,unsigned amount,int x,int y,int w,int h,unsigned ink)
{
    fb_rect(x,y,w,h,FL_BG);
    int mid=y+h/2, base=y+h-2;
    fb_hline(x+2,effect<2?mid:base,w-4,FL_BG3);
    if(!amount)return;
    if(effect<2){
        int previous=mid;
        for(int i=0;i<w-4;i++){
            int v;
            if(effect==0){
                int in=(i*2-(w-5))*4096/(w-5);
                unsigned drive=amount*127u/100u;
                v=softclip(in*(4096+(int)drive*(int)drive*2)/4096);
            } else {
                uint32_t phase=(uint32_t)i*(15000000u+(uint32_t)song.g[G_CRATE]*1200000u);
                v=osc_sine(phase)*song.g[G_CDEPTH]*(int)amount/(127*100);
            }
            int now=mid-clampi(v*(h/2-2)/32768,2-h/2,h/2-2);
            if(i)fb_line(x+i+1,previous,x+i+2,now,ink);
            previous=now;
        }
        return;
    }
    int height=(h-3)*(int)amount, step=effect==2?5+song.g[G_DTIME]:3;
    int feedback=effect==2?clampi(song.g[G_DFDBK]*2,0,246):175+song.g[G_RSIZE]/2;
    for(int i=3;i<w-2 && height>=100;i+=step){
        int bar=height/100;
        if(effect==3 && i&1)bar=bar*3/4;
        fb_rect(x+i,base-bar,2,bar,ink);
        height=height*feedback/256;
    }
}

/* The grain follows the enclosure during zoom. Each row is a solid span with
 * a short reflected streak, so the texture needs no image or scratch buffer. */
static void cgu_brushed_face(int x,int y,int w,int h,unsigned ramp)
{
    static const uint8_t edge[]={3,1,0};
    static const uint8_t sheen[]={4,3,3,3,3,4,4,3,3,2,2,2,2,1,1,0};
    for(int row=clampi(fb_cy0-y,0,h),end=clampi(fb_cy1-y,0,h);row<end;row++){
        int distance=row<h-1-row?row:h-1-row;
        int inset=distance<3?edge[distance]:0;
        unsigned shade=sheen[row*15/(h-1)];
        fb_hline(x+inset,y+row,w-2*inset,ramp+shade);
        uint32_t grain=(uint32_t)row*2654435761u;
        if(h>52 && distance>=3 && (grain>>29)<3){
            int start=4+(int)(grain>>16)%(w/2);
            int length=w/7+(int)(grain>>24)%(w/6);
            fb_hline(x+start,y+row,length,ramp+shade+1u);
        }
    }
}

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

static void cgu_amp_shell(int x,int y,int w,int h,int zoom,unsigned ink)
{
    fb_rbox(x+1,y+1,w-2,h-2,2,CGU_METAL+10);
    fb_rbox(x+2,y+1,w-4,h-4,2,CGU_METAL+2);
    fb_hline(x+5,y+2,w-10,CGU_METAL+5);
    fb_hline(x+6,y+5,w-12,ink);
    fb_hline(x+w/2-10,y+2,20,CGU_METAL+7);
    fb_hline(x+w/2-8,y+3,16,CGU_METAL);
    int grille_top=zoom>72?43:23;
    cgu_grille(x+5,y+grille_top,w-10,zoom>72?12:h-grille_top-4);
    if(zoom>72)cgu_brushed_face(x+5,y+55,w-10,h-59,CGU_AMP_FACE);
    fb_hline(x+6,y+h-4,w-12,CGU_METAL);
}

static void cgu_pedal_shell(int x,int y,int w,int h,unsigned band)
{
    unsigned body=band==CGU_BAND_PROGRESSION?CGU_AMP_FACE:CGU_PEDAL_BODY+(band-1u)*CGU_PEDAL_SHADES;
    fb_rbox(x+3,y+2,w-6,h-2,2,CGU_METAL+10);
    fb_rect(x+1,y+h/2-3,5,6,CGU_METAL+5);
    fb_rect(x+w-6,y+h/2-3,5,6,CGU_METAL+5);
    fb_hline(x+1,y+h/2-3,5,CGU_METAL+7);
    fb_hline(x+w-6,y+h/2-3,5,CGU_METAL+7);
    cgu_brushed_face(x+4,y+1,w-8,h-3,body);
    fb_hline(x+8,y+2,w-16,band==CGU_BAND_PROGRESSION?CGU_PROGRESSION_INK:body+6);
    fb_hline(x+8,y+h-3,w-16,body);
    fb_pset(x+7,y+5,CGU_METAL+7);
    fb_pset(x+w-8,y+5,CGU_METAL+7);
    fb_pset(x+7,y+h-6,CGU_METAL+7);
    fb_pset(x+w-8,y+h-6,CGU_METAL+7);
}

static void cgu_footswitch(int x,int y,int r)
{
    cgu_metal_disc(x,y+1,r+1,10,10,10);
    cgu_metal_disc(x,y,r,9,5,2);
    cgu_metal_disc(x,y,r-2,7,4,3);
}

static void cgu_lamp(int x,int y,int on,unsigned ink)
{
    cgu_metal_disc(x,y,3,6,3,0);
    cgu_metal_disc(x,y,2,0,0,0);
    if(on){
        fb_rect(x-1,y-1,3,3,ink);
        fb_pset(x-1,y-1,CGU_METAL+9);
    }
}

static unsigned cgu_module_ink(unsigned module)
{
    if(module<CGS_NPART)return CGU_INK_BASE+module;
    return module==CGS_NPART+CGU_BAND_PROGRESSION-1u?CGU_PROGRESSION_INK:CGU_BAND_INK_BASE+module-CGS_NPART;
}

static const char *cgu_module_value(const cf_stage_model_t *m,unsigned band,unsigned part,char summary[20])
{
    switch(band){
    case CGU_BAND_HARMONY:
        kpc(summary,cs.chord.tonic);
        cf_stage_append(summary,20," ");
        cf_stage_append(summary,20,CF_SCALES[cs.chord.scale].shrt);
        return summary;
    case CGU_BAND_SHAPING:
        str_cpy(summary,"Energy ",20);
        kint(summary+7,cgs.energy,"%");
        return summary;
    case CGU_BAND_GENRE:
        return CGS_PROFILES[m->genre].name;
    case CGU_BAND_ARP:
        return part==CGS_CHORDS || part==CGS_LAYER?CGU_ARP_NAMES[cgu_arp_mode()]:"Chords / Layer";
    case CGU_BAND_PROGRESSION:
        return cgs.custom?cgs_custom_harmony.name:"Genre bank";
    default:
        return m->workspace==CGU_SOUND?m->rail[part]:CGS_PROFILES[m->genre].style[part][cgs.style[part]];
    }
}

static void cgu_module(const cf_stage_model_t *m,unsigned module,int x,int y,int w,int h,int zoom)
{
    unsigned band=module<CGS_NPART?0:module-CGS_NPART+1u;
    unsigned part=band?m->focus:module, sound=m->workspace==CGU_SOUND;
    unsigned accent=cgu_module_ink(module);
    unsigned ink=!band && (m->pin_mask&(1u<<part))?FL_INK2:accent;
    const char *title=band?CGU_BAND_NAMES[band-1u]:CGS_PART_NAME[part];
    char summary[20];
    const char *value=cgu_module_value(m,band,part,summary);
    if(band)cgu_pedal_shell(x,y,w,h,band);
    else cgu_amp_shell(x,y,w,h,zoom,ink);
    fb_rbox(x+7,y+7,w-14,16+zoom*18/100,2,CGU_METAL);
    cfn_fit(zoom>62?&CFN_16:&CFN_12,x+10,y+18+zoom*6/100,title,w-34,FL_INK);
    cgu_lamp(x+w-13,y+13,band?m->bank==band:(m->activity&(1u<<part))!=0,ink);

    if(zoom>82 && band==CGU_BAND_ARP){
        static const char *const names[]={"Pattern","Timing"};
        for(unsigned i=0;i<2;i++){
            int left=x+10+(int)i*(w-20)/2, width=(w-20)/2-2;
            fb_rect(left,y+28,width,16,i==m->shaping?FL_BG3:FL_BG);
            if(i==m->shaping)fb_hline(left,y+43,width,accent);
            cfn_text(&CFN_11,left+4,y+40,names[i],i==m->shaping?FL_INK:FL_INK2);
        }
    } else if(zoom>82 && !band)
        cgu_pages(x+10,y+28,w-20,m->workspace,part,sound?m->sound_page:m->shaping);
    else if(zoom<=82){
        int baseline=y+30+zoom*9/100, width=zoom>60?w-22:w-32;
        fb_rbox(x+8,baseline-8,width+3,11,1,CGU_METAL);
        cfn_fit(&CFN_11,x+10,baseline,value,width,ink);
    }
    int knob_y=y+h-12-zoom*(band?53:35)/100;
    if(m->label[1][0]){
        int knob_x=x+w-14+(14-w*5/8)*zoom/100;
        cgu_dial(knob_x,knob_y,5+zoom/10,m->knob[1],m->emphasis[1]?accent:FL_INK);
    }
    if(zoom>57){
        if(band!=CGU_BAND_GENRE && band!=CGU_BAND_PROGRESSION){
            const char *mode=sound && m->sound_page==CGU_PRESETS?"Band FX":value;
            cfn_fit(&CFN_11,x+w-99,y+24,mode,76,ink);
        }
        if(sound && m->sound_page==CGU_PRESETS && zoom>82){
            cfn_fit(&CFN_14,x+64,knob_y,m->value[0],w-76,FL_INK);
            cfn_fit(&CFN_11,x+64,knob_y+18,"Loaded",w-76,FL_INK2);
            cfn_fit(&CFN_11,x+109,knob_y+18,m->applied,w-121,FL_INK2);
        }
        for(int i=0;i<4;i++){
            int col=x+w*(1+2*i)/8;
            if(!m->label[i][0])continue;
            if(sound && m->sound_page==CGU_SENDS && zoom>89)
                cgu_send_graph((unsigned)i,m->knob[i],col-21,knob_y-39,42,20,accent);
            if(i!=1)cgu_dial(col,knob_y,zoom*15/100,m->knob[i],m->disabled[i]?FL_INK2:m->emphasis[i]?accent:FL_INK);
            if(zoom<82)continue;
            cfn_fit(&CFN_11,col-w/8+4,knob_y+25,m->label[i],w/4-6,FL_INK2);
            if(band!=CGU_BAND_GENRE){
                int width=w/4-6;
                const cfn_font_t *face=cfn_width(&CFN_12,m->value[i])<=width?&CFN_12:&CFN_11;
                cfn_fit(face,col-w/8+4,knob_y+37,m->value[i],width,m->disabled[i]?FL_INK2:FL_INK);
            }
        }
        if(band==CGU_BAND_GENRE && zoom>82)cfn_fit(&CFN_16,x+60,knob_y+6,value,w-70,FL_INK);
        if(band==CGU_BAND_PROGRESSION && zoom>82){
            cfn_text(&CFN_11,x+65,knob_y-3,"Active progression",FL_INK2);
            cfn_fit(&CFN_16,x+65,knob_y+17,m->applied,w-77,FL_INK);
        }
        if(band && zoom>82){
            fb_rbox(x+w/2-24,y+h-21,48,15,3,FL_BG);
            cgu_footswitch(x+w/2-14,y+h-14,5);
            cfn_text(&CFN_11,x+w/2-3,y+h-10,"SEL",FL_INK);
        }
    }
}

static const char *cgu_drum_status(const cf_stage_model_t *m)
{
    static const char *const queued[]={"","Fill next bar","Drop next bar","Return next bar"};
    return m->moment?queued[m->moment]:m->build?"Drums dropped":m->bloom?"Fill active":m->status;
}

static void cgu_mixer_draw(const cf_stage_model_t *m)
{
    cfn_text(&CFN_14,12,92,"Mixer",FL_INK);
    cfn_fit(&CFN_12,103,92,cgu_drum_status(m),121,C_ACC(0));
    for(unsigned i=0;i<4;i++){
        unsigned p=cgu_part(i), bit=1u<<p, ink=(CGU_INK_BASE+p);
        int x=10+(int)i*56;
        fb_rect(x,101,52,89,FL_BG2);
        if(p==m->focus)fb_frame(x,101,52,89,ink);
        fb_rect(x+44,106,3,3,m->activity&bit?C_ACC(0):FL_BG3);
        cfn_center(&CFN_12,x+26,118,CGS_PART_NAME[p],FL_INK);
        cgu_dial(x+26,141,10,m->part_level[p],m->emphasis[i]?C_ACC(0):FL_INK);
        cfn_center(&CFN_11,x+26,167,m->muted&bit?"Mute":m->solo&bit?"Solo":m->pin_mask&bit?"Cut":"On",ink);
        cfn_center(&CFN_11,x+26,183,m->value[i],FL_INK2);
    }
    cfn_text(&CFN_11,10,205,"Hold upper 1-4 Mute / 5-8 Solo",FL_INK);
    cfn_text(&CFN_11,10,219,"EDIT Mute   SEL Solo   GLO Back",FL_INK2);
    cfn_text(&CFN_11,10,237,"PRESETS Fill / Drop",FL_INK2);
    cfn_text(&CFN_11,157,205,"SAVE Settings",FL_INK2);
}

static void cgu_output_meter(int x,int y,unsigned peak,unsigned tab)
{
    static const uint8_t marks[]={0,3,7,13,22,35,52,76,112,168,255};
    static const int8_t px[]={-27,-23,-18,-12,-6,0,6,12,18,23,27};
    static const int8_t py[]={-9,-12,-14,-16,-17,-17,-17,-16,-14,-12,-9};
    static const uint8_t accent[]={CGU_INK_BASE+CGS_CHORDS,CGU_INK_BASE+CGS_BASS,CGU_INK_BASE+CGS_DRUMS,CGU_INK_BASE+CGS_LAYER};
    fb_rbox(x,y,71,23,2,CGU_METAL+5);
    fb_rbox(x+2,y+2,67,19,2,CGU_METAL+8);
    int cx=x+35, base=y+20;
    for(unsigned i=0;i<NELEM(marks);i++){
        unsigned ink=i>=9?CGU_INK_BASE+CGS_DRUMS:CGU_METAL+2;
        fb_line(cx+px[i],base+py[i],cx+px[i]*9/10,base+py[i]*9/10,ink);
    }
    unsigned i=1;
    while(i+1<NELEM(marks) && peak>marks[i])i++;
    int n=(int)peak-marks[i-1], d=marks[i]-marks[i-1];
    int nx=px[i-1]+(px[i]-px[i-1])*n/d, ny=py[i-1]+(py[i]-py[i-1])*n/d;
    fb_line(cx,base,cx+nx,base+ny,CGU_METAL);
    fb_hline(cx-2,base,5,CGU_METAL+2);
    fb_rect(x+60,y+17,5,2,accent[tab]);
}

static void cgu_display(const cf_stage_model_t *m)
{
    fb_rect(10,22,220,27,FL_BG2);
    fb_rect(13,24,214,23,FL_BG);
    cfn_fit(&CFN_11,17,33,m->theory,134,C_ACC(0));
    cfn_fit(&CFN_12,17,45,m->degree,49,FL_INK);
    const char *context=m->workspace==CGU_SOUND?CGS_PROFILES[m->genre].name:
        m->workspace==CGU_STUDIO && m->live && !m->bank && m->focus==CGS_DRUMS?cgu_drum_status(m):m->context;
    if(context[0])cfn_fit(&CFN_11,53,45,context,99,FL_INK2);
    cgu_output_meter(156,24,m->signal_peak,m->workspace);
}

static void cgu_keys_footer(void)
{
    unsigned part=cgs.focus, ink=CGU_INK_BASE+part;
    fb_rect(151,228,79,12,FL_BG2);
    fb_rect(151,229,2,10,ink);
    cfn_text(&CFN_11,156,237,"KEYS",FL_INK2);
    cfn_fit(&CFN_11,188,237,CGS_PART_NAME[part],39,ink);
}

static void cgu_library_rack(const cf_stage_model_t *m,int x,int y,int w,int h,int zoom)
{
    cgu_amp_shell(x,y,w,h,zoom,C_ACC(0));
    cfn_fit(zoom>62?&CFN_16:&CFN_12,x+10,y+(zoom>62?24:18),"Sound Library",w-20,FL_INK);
    if(zoom<=62){
        cfn_text(&CFN_11,x+10,y+30,m->value[0],C_ACC(0));
        return;
    }
    int center=x+w/2, knob=y+h-45;
    cgu_dial(center,knob,15,m->knob[0],m->emphasis[0]?C_ACC(0):FL_INK);
    cfn_center(&CFN_11,center,knob+24,m->label[0],FL_INK2);
    cfn_center(&CFN_12,center,knob+36,m->value[0],FL_INK);
}

static void cgu_record_controls(const cf_stage_model_t *m,int x,int y,int w,int h,int zoom)
{
    cgu_amp_shell(x,y,w,h,zoom,C_ACC(0));
    cfn_fit(zoom>62?&CFN_16:&CFN_12,x+10,y+(zoom>62?24:18),"Saved Takes",w-20,FL_INK);
    int panel_y=zoom>62?y+61:y+22;
    if(zoom>62)cfn_text(&CFN_11,x+10,y+40,"K1 Select   Hold SAVE 3s to clear",FL_INK2);
    int slot_w=(w-20)/SQ_NPAT;
    for(unsigned slot=0;slot<SQ_NPAT;slot++){
        unsigned mask=1u<<slot;
        int left=x+10+(int)slot*slot_w;
        char text[14];
        kint(text,slot+1u,(m->total&mask)?" Unread":(m->count&mask)?" Saved":" Empty");
        fb_rect(left,panel_y,slot_w-3,13,slot==m->slot?FL_BG3:FL_BG);
        cfn_fit(&CFN_11,left+3,panel_y+10,text,slot_w-8,slot==m->slot?FL_INK:FL_INK2);
        if(slot==m->slot)fb_hline(left,panel_y+12,slot_w-3,C_ACC(0));
    }
    if(m->pulse)fb_hline(x+10+(int)m->slot*slot_w,panel_y+14,(slot_w-3)*m->pulse/100,CGU_INK_BASE+CGS_DRUMS);
}

typedef struct { int x,y,w,h; } cgu_face_t;
enum { CGU_FACE_CONTROLS_H=24 };

static cgu_face_t cgu_rack_face(unsigned tab,unsigned tile,int control_row)
{
    unsigned count=cgu_workspace_tiles(tab);
    int full=(count&1u) && tile==count-1u;
    int row=(int)tile/2;
    int x=10+(full?0:(tile&1u)*112);
    int h=tab==CGU_STUDIO?(row<2?28:24):tab==CGU_HELP?62:43;
    int y=tab==CGU_STUDIO && row>=2?114+(row-2)*27:52+row*(h+3);
    if(control_row>=0 && row>control_row)y+=CGU_FACE_CONTROLS_H;
    return (cgu_face_t){x,y,full?220:108,h};
}

static void cgu_rack_overview_face(const cf_stage_model_t *m,unsigned tile,cgu_face_t face,int selected)
{
    int x=face.x,y=face.y,w=face.w,h=face.h;
    unsigned tab=m->workspace;
    unsigned part=tile<CGS_NPART && tab!=CGU_HELP?cgu_part(tile):m->focus;
    unsigned ink=C_ACC(0);
    if(tab==CGU_STUDIO && tile>=CGS_NPART){
        unsigned band=tile-CGS_NPART+1u;
        ink=cgu_module_ink(tile);
        char summary[20];
        cgu_pedal_shell(x,y,w,h,band);
        cfn_fit(&CFN_12,x+11,y+12,CGU_BAND_NAMES[band-1u],w-37,FL_INK);
        cfn_fit(&CFN_11,x+11,y+22,cgu_module_value(m,band,part,summary),w-37,ink);
        cgu_footswitch(x+w-16,y+h/2+2,5);
    } else if(tile<CGS_NPART && tab!=CGU_HELP){
        ink=CGU_INK_BASE+part;
        int muted=m->pin_mask&(1u<<part);
        int grille=h<=34?11:20;
        fb_rbox(x,y,w,h,3,CGU_METAL+10);
        fb_rbox(x+2,y+2,w-4,h-4,2,CGU_METAL+4);
        cgu_grille(x+4,y+4,w-8,grille);
        cgu_brushed_face(x+4,y+grille+5,w-8,h-grille-8,CGU_AMP_FACE);
        fb_hline(x+4,y+grille+4,w-8,muted?CGU_METAL+5:ink);
        cfn_text(&CFN_12,x+10,y+(h<=34?13:18),CGS_PART_NAME[part],FL_INK);
        cgu_lamp(x+w-13,y+(h<=34?9:11),(m->activity&(1u<<part))!=0,ink);
        if(tab==CGU_RECORD)cgu_grid(m,part,x+7,y+33,5,1);
        else {
            char summary[20];
            cfn_fit(&CFN_11,x+8,y+grille+14,cgu_module_value(m,0,part,summary),w-32,FL_INK2);
            cgu_dial(x+w-14,y+grille+10,5,m->part_level[part],ink);
        }
    } else {
        static const char *const help_sub[]={"Transport & tabs","Key & scale","Live controls","Record & USB"};
        const char *title=tab==CGU_HELP?CGU_TOPICS[tile]:tab==CGU_SOUND?"Sound Library":"Saved Takes";
        const char *value=tab==CGU_HELP?help_sub[tile]:tab==CGU_SOUND?(m->build?"All sounds":"Genre"):"Select or clear a take";
        fb_rbox(x,y,w,h,3,CGU_METAL+2);
        cgu_brushed_face(x+3,y+3,w-6,h-6,CGU_AMP_FACE);
        fb_hline(x+7,y+4,w-14,CGU_METAL+5);
        cfn_fit(&CFN_12,x+11,y+h/2-2,title,w-24,FL_INK);
        cfn_fit(&CFN_11,x+11,y+h/2+12,value,w-24,FL_INK2);
        cgu_lamp(x+w-12,y+12,selected,ink);
        if(tab==CGU_SOUND)cgu_dial(x+w-25,y+h/2+3,8,m->build?100:0,ink);
        if(tab==CGU_RECORD){
            for(unsigned slot=0;slot<SQ_NPAT;slot++)
                fb_rect(x+w-50+13*(int)slot,y+h/2,9,7,
                    (m->count&(1u<<slot))?ink:CGU_METAL+1);
        }
    }
    if(selected)fb_rframe(x,y,w,h,ink);
}

static void cgu_face_controls(const cf_stage_model_t *m,unsigned selected,cgu_face_t face)
{
    static const uint8_t part[]={66,57,37,52}, tone[]={78,43,49,42},
        harmony[]={39,58,68,47}, shaping[]={47,60,50,55}, arp[]={59,54,50,49};
    const uint8_t *width=m->workspace==CGU_SOUND?tone:m->bank==CGU_BAND_HARMONY?harmony:
        m->bank==CGU_BAND_SHAPING?shaping:m->bank==CGU_BAND_ARP?arp:part;
    unsigned ink=m->workspace==CGU_STUDIO && selected>=CGS_NPART?cgu_module_ink(selected):
        selected<CGS_NPART && m->workspace!=CGU_HELP?CGU_INK_BASE+cgu_part(selected):C_ACC(0);
    int y=face.y+face.h;
    fb_rbox(10,y+1,220,CGU_FACE_CONTROLS_H-1,2,CGU_METAL+2);
    fb_hline(face.x+3,y,face.w-6,ink);
    fb_rect(face.x+3,y,2,4,ink);
    fb_rect(face.x+face.w-5,y,2,4,ink);
    int left=14;
    int single=!m->label[1][0];
    for(unsigned k=0;k<4;k++){
        int w=single?212:width[k];
        if(m->label[k][0]){
            unsigned active=m->emphasis[k] && !m->disabled[k];
            const char *value=m->workspace==CGU_STUDIO && m->bank==CGU_BAND_GENRE?
                CGS_PROFILES[m->genre].name:m->value[k];
            if(active)fb_rbox(left,y+2,w-2,20,2,CGU_METAL);
            cfn_fit(&CFN_11,left+3,y+10,m->label[k],w-6,active?ink:FL_INK2);
            cfn_fit(&CFN_11,left+3,y+21,value,w-6,m->disabled[k]?FL_INK2:active?ink:FL_INK);
        }
        if(single)break;
        left+=w;
    }
}

static void cgu_rack_modules(const cf_stage_model_t *m)
{
    unsigned tab=m->workspace, selected=cgu_workspace_tile(m), count=cgu_workspace_tiles(tab);
    int zoom=m->zoom;
    int control_row=m->label[0][0]?(int)selected/2:-1;
    cgu_face_t from=cgu_rack_face(tab,selected,control_row);
    /* Leave neighboring faceplates anchored while the chosen one fills the view. */
    int inset=zoom*120/100;
    if(zoom<100){
        fb_clip(inset,51,240-inset,218-clampi(zoom*21/60,0,21));
        fb_rbox(4,51,232,167,3,CGU_METAL+10);
        fb_vline(6,54,161,CGU_METAL+3);
        fb_vline(233,54,161,CGU_METAL+3);
        for(unsigned i=0;i<count;i++){
            if(i==selected)continue;
            cgu_rack_overview_face(m,i,cgu_rack_face(tab,i,control_row),0);
        }
        if(zoom<=60 && control_row>=0)cgu_face_controls(m,selected,from);
    }
    fb_clip(0,51,240,218);
    int sends=tab==CGU_SOUND && !m->bank && m->sound_page==CGU_SENDS;
    int top=tab==CGU_STUDIO?63:sends?54:72;
    int height=tab==CGU_STUDIO?(m->bank?133:125):sends?136:122;
    cgu_face_t face={from.x+(10-from.x)*zoom/100,from.y+(top-from.y)*zoom/100,
        from.w+(220-from.w)*zoom/100,from.h+(height-from.h)*zoom/100};
    if(zoom<=60){cgu_rack_overview_face(m,selected,face,1);return;}
    int x=face.x,y=face.y,w=face.w,h=face.h;
    unsigned module=selected<CGS_NPART && tab!=CGU_HELP?cgu_part(selected):selected;
    unsigned ink=selected<CGS_NPART && tab!=CGU_HELP?CGU_INK_BASE+module:C_ACC(0);
    if(tab==CGU_STUDIO || (tab==CGU_SOUND && selected<CGS_NPART)){
        ink=cgu_module_ink(module);
        cgu_module(m,module,x,y,w,h,zoom);
    } else if(tab==CGU_SOUND)cgu_library_rack(m,x,y,w,h,zoom);
    else if(tab==CGU_RECORD && selected==CGS_NPART)cgu_record_controls(m,x,y,w,h,zoom);
    else {
        cgu_amp_shell(x,y,w,h,zoom,ink);
        const char *name=tab==CGU_HELP?CGU_TOPICS[module]:CGS_PART_NAME[module];
        cfn_fit(&CFN_16,x+10,y+25,name,w-20,FL_INK);
        if(tab==CGU_RECORD)cgu_grid(m,module,x+11,y+57,(w-20)/CGS_STEPS,2);
    }
    fb_rframe(x,y,w,h,ink);
}

static void cgu_workspace_draw(const cf_stage_model_t *m)
{
    unsigned tab=m->workspace;
    for(unsigned i=0;i<CGU_NTAB;i++){
        int x=10+(int)i*55;
        fb_rect(x,3,53,16,i==tab?FL_BG3:FL_BG1);
        cfn_center(&CFN_11,x+26,15,CGU_TABS[i],i==tab?FL_INK:FL_INK2);
        if(i==tab)fb_hline(x,18,53,C_ACC(0));
    }
    cgu_display(m);
    if(m->mixer){cgu_mixer_draw(m);cgu_keys_footer();return;}
    int studio=tab==CGU_STUDIO;
    fb_clip(0,51,240,218);
    cgu_rack_modules(m);
    if(tab==CGU_HELP && m->zoom>82)cgu_help_draw(m);
    fb_unclip();
    cfn_shift_panel(m->bank_dx,51,218);
    int overview=m->zoom<=60;
    int fx_presets=tab==CGU_SOUND && m->live && !m->bank && m->sound_page==CGU_PRESETS;
    int palette=tab==CGU_SOUND && m->live && m->bank && !m->build;
    int progression=studio && m->live && m->bank==CGU_BAND_PROGRESSION;
    int apply=fx_presets || palette || progression;
    if(!overview)fb_hline(10,197,220,FL_BG3);
    const char *hint=m->pending[0]?m->pending:
        studio?(m->live && !m->bank?"EDIT New part":"EDIT New take"):
        tab==CGU_SOUND?(!m->live || m->bank?"EDIT New sounds":m->sound_page==CGU_TONE?"EDIT New sound":""):
        tab==CGU_RECORD?(m->rec?"REC Finish take":"REC Manual take"):"";
    if(fx_presets)hint="EDIT Loaded FX";
    if(progression && !m->pending[0])hint="EDIT Draft";
    int footer=overview?227:207;
    cfn_fit(&CFN_11,10,footer,hint,106,FL_INK2);
    cfn_text(&CFN_11,119,footer,apply?"SEL Apply":m->live?"SEL Back":"SEL Zoom",apply?FL_INK:FL_INK2);
    cfn_text(&CFN_11,173,footer,m->live && (studio || tab==CGU_SOUND)?"GLO Mix":"SELECT Tabs",FL_INK2);
    if(overview){
        cfn_text(&CFN_11,10,238,"ALGO Select",FL_INK);
        if(studio)cfn_fit(&CFN_11,77,238,"SEQ Pedals",70,FL_INK2);
    } else if(tab==CGU_SOUND && !m->bank){
        const char *sound_hint=m->sound_page==CGU_PRESETS?"K1 Browse   SAVE Manage":
            m->sound_page==CGU_SENDS?"FX Presets":"FX Sends";
        cfn_text(&CFN_11,10,221,sound_hint,FL_INK2);
        if(m->focus!=CGS_DRUMS)cfn_text(&CFN_11,158,221,"LFO Voice",FL_INK2);
        char page[12];
        kint(page,1u+cgu_sound_page_rank(m->focus,m->sound_page),"/");
        kint(page+str_len(page),cgu_sound_page_count(m->focus),0);
        cfn_text(&CFN_11,10,237,"SELECT Pages",FL_INK);
        cfn_text(&CFN_11,91,237,page,CGU_INK_BASE+m->focus);
    } else if(tab==CGU_SOUND && m->bank){
        cfn_text(&CFN_11,10,221,"K1 Library   HOME Back",FL_INK2);
    } else if(studio && m->live){
        if(!m->bank || m->bank==CGU_BAND_ARP){
            cfn_text(&CFN_11,10,221,"SELECT Pages",FL_INK);
            cfn_text(&CFN_11,91,221,m->shaping?"2/2":"1/2",CGU_INK_BASE+m->focus);
        }
        if(m->bank==CGU_BAND_GENRE)cfn_text(&CFN_11,10,221,"K1 or PRESETS changes genre",FL_INK);
        if(progression)cfn_text(&CFN_11,10,221,"K1 Source   HOME Back",FL_INK2);
        if(!m->bank && m->focus==CGS_DRUMS)cfn_fit(&CFN_11,10,237,"PRESETS Fill / Drop / Return",139,FL_INK2);
    } else if(tab==CGU_HELP && m->live && m->focus==CGU_HARMONY){
        cfn_text(&CFN_11,10,222,m->value[0],FL_INK);
        cfn_text(&CFN_11,100,222,"EDIT Chord Lab",FL_INK2);
    }
    cgu_keys_footer();
}
