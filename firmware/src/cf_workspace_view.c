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
    cf_stage_append(m->theory,sizeof m->theory,CF_SCALES[scale%CF_NSCALES].name);
    cf_stage_append(m->theory,sizeof m->theory,"   ");
    cf_stage_append(m->theory,sizeof m->theory,sig);
    cf_stage_append(m->theory,sizeof m->theory,"   ");
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

static void cgu_model(cf_stage_model_t *m)
{
    int active=cgs_active();
    unsigned idea=active?cf_sn.ac_idea:cpa_setup.plan.idea;
    const cf_progression_t *p=(active || cgs.ready)?cf_progression_get(idea):0;
    m->engine_page=3;
    m->workspace=cgu.tab;
    m->zoom=(uint8_t)cgu_zoom();
    m->mixer=cgu.mixer;
    m->bank=cgu.tab==CGU_STUDIO?cgu.band_panel:0;
    m->shaping=cgu.part_harmony;
    m->sound_page=cgu.sound_page;
    m->genre=cgs.genre;
    m->solo=cgs.solo|cgs.held_solo;
    m->muted=cgs.muted|cgs.held_mute;
    m->focus=cgu.tab==CGU_HELP && !m->mixer?cgu.topic:cgs.focus;
    m->live=cgs.zoom;
    m->pin_mask=(uint8_t)cgs_mute_mask();
    memcpy(m->part_activity,cgs.activity,sizeof m->part_activity);
    memcpy(m->part_level,cgs.level,sizeof m->part_level);
    m->activity=cf_sn.visual_parts & ~m->pin_mask;
    str_cpy(m->status,sqk.on || cf_sn.sq_rec!=255?"Recording":
        active && (cf_sn.ac_waiting || cf_sn.ac_paused)?"MIDI wait":
        active || cf_sn.sq_on || cf_sn.dm_on?"Playing":
        cgu.tab==CGU_STUDIO?"PLAY to start":"Ready",sizeof m->status);
    static const char *const feel[]={"Low Tide","Glass Chorus","Night Drive"};
    str_cpy(m->context,p?p->name:idea>=CF_P_COMPOSED && idea<CF_P_COMPOSED+NELEM(feel)?
        feel[idea-CF_P_COMPOSED]:"Press PLAY",sizeof m->context);
    cgu_theory(m,active);
    if(cgu.tab==CGU_STUDIO){
        if(active && cpa_setup.flags)str_cpy(m->pending,
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
        if(cgu.sound_page==CGU_PRESETS){
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
    if(cgu.tab==CGU_HELP && m->focus==CGU_HARMONY && m->zoom>65)cf_learn_model(&m->learn);
    if(m->mixer || (cgu.tab==CGU_STUDIO && cgs.focus==CGS_DRUMS)){
        m->moment=cf_sn.perf_pending;
        m->build=cf_sn.perf_dropped;
        m->bloom=cf_sn.perf_fill;
    }
    if(cgu.tab==CGU_STUDIO){
        unsigned bpm=cs.clock_in==1u?cf_sn.ext_bpm:cs.bpm;
        unsigned tempo=(unsigned)clampi((int)bpm-30,0,270)*100u/270u;
        if(cgu_on_harmony()){
            unsigned notes=active && (cpa_edit.flags&CPA_NOTES);
            m->knob[0]=(uint8_t)((notes?cpa_edit.plan.tonic:cs.chord.tonic)*100u/11u);
            m->knob[1]=(uint8_t)tempo;
            m->knob[2]=(uint8_t)((notes?cpa_edit.plan.scale:cs.chord.scale)*100u/(CF_NSCALES-1u));
            m->knob[3]=(uint8_t)(cgs.meter==3?0:cgs.meter==6?100:50);
        } else if(m->bank==CGU_BAND_GENRE){
            m->knob[0]=(uint8_t)(cgs.genre*100u/(CGS_NGENRE-1u));
        } else if(m->bank==CGU_BAND_ARP){
            m->shaping=cgu.arp_timing;
        } else if(cgs.zoom && !m->bank){
            m->knob[0]=(uint8_t)(cgs.style[cgs.focus]*100);
        } else {
            m->knob[0]=cgs.space;
            m->knob[1]=(uint8_t)tempo;
            m->knob[2]=cgs.energy;m->knob[3]=cgs.evolution;
        }
    } else if(cgu.tab==CGU_SOUND){
        unsigned p=cgs.focus;
        m->knob[0]=cgu.all_sounds?100:0;
        m->knob[1]=(uint8_t)(p==CGS_DRUMS?cgu_kit()*100/SQ_NKIT:cf_snd_pos(p)*100/cf_snd_count(p));
        if(cgs.zoom)m->knob[0]=m->knob[1];
        if(!m->mixer)m->build=cgu.all_sounds;
    } else if(cgu.tab==CGU_RECORD){
        m->slot=cgu.take_slot;
        for(unsigned i=0;i<SQ_NPAT;i++)if(sq_has(i))m->count|=(uint8_t)(1u<<i);
        m->total=sq_unavailable;
        if(cgu.clear_down)m->pulse=(uint8_t)clampi((int)(cu.now-cgu.clear_down)/30,0,100);
    }
    if(cgu.turn_ms && cu.now-cgu.turn_ms<180u)m->emphasis[cgu.turn_knob]=1;
    for(unsigned i=0;i<4;i++){
        cf_knob_t knob={.bar=-1};
        cgu_knob_info(i,&knob);
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
    unsigned body=CGU_PEDAL_BODY+(band-1u)*CGU_PEDAL_SHADES;
    fb_rbox(x+3,y+2,w-6,h-2,2,CGU_METAL+10);
    fb_rect(x+1,y+h/2-3,5,6,CGU_METAL+5);
    fb_rect(x+w-6,y+h/2-3,5,6,CGU_METAL+5);
    fb_hline(x+1,y+h/2-3,5,CGU_METAL+7);
    fb_hline(x+w-6,y+h/2-3,5,CGU_METAL+7);
    cgu_brushed_face(x+4,y+1,w-8,h-3,body);
    fb_hline(x+8,y+2,w-16,body+6);
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

static void cgu_module(const cf_stage_model_t *m,unsigned module,int x,int y,int w,int h,int zoom)
{
    unsigned band=module<CGS_NPART?0:module-CGS_NPART+1u;
    unsigned part=band?m->focus:module, sound=m->workspace==CGU_SOUND;
    unsigned accent=band?CGU_BAND_INK_BASE+band-1u:CGU_INK_BASE+part;
    unsigned ink=!band && (m->pin_mask&(1u<<part))?FL_INK2:accent;
    const char *title=band?CGU_BAND_NAMES[band-1u]:CGS_PART_NAME[part];
    const char *value;
    unsigned level;
    char summary[20];
    switch(band){
    case CGU_BAND_HARMONY:
        kpc(summary,cs.chord.tonic);
        cf_stage_append(summary,sizeof summary," ");
        cf_stage_append(summary,sizeof summary,CF_SCALES[cs.chord.scale].shrt);
        value=summary;level=cs.chord.tonic*100u/11u;break;
    case CGU_BAND_SHAPING:
        str_cpy(summary,"Energy ",sizeof summary);
        kint(summary+7,cgs.energy,"%");
        value=summary;level=cgs.energy;break;
    case CGU_BAND_GENRE:
        value=CGS_PROFILES[m->genre].name;level=m->genre*100u/(CGS_NGENRE-1u);break;
    case CGU_BAND_ARP:
        value=part==CGS_CHORDS || part==CGS_LAYER?CGU_ARP_NAMES[cgu_arp_mode()]:"Chords / Layer";
        level=cgu_arp_mode()*100u/CGU_ARP_BAND;break;
    default:
        value=sound?m->rail[part]:CGS_PROFILES[m->genre].style[part][cgs.style[part]];
        level=sound?m->part_level[part]:m->part_activity[part];
    }
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
    if(band && zoom<=57)cgu_footswitch(x+w-14,y+h-12,5);
    else if(zoom<100 || m->label[1][0]){
        int knob_x=x+w-14+(14-w*5/8)*zoom/100;
        cgu_dial(knob_x,knob_y,5+zoom/10,zoom>45?m->knob[1]:level,m->emphasis[1]?accent:FL_INK);
    }
    if(zoom>57){
        if(band!=CGU_BAND_GENRE){
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

static void cgu_display(const cf_stage_model_t *m)
{
    fb_rect(10,22,220,27,FL_BG2);
    fb_rect(13,24,214,23,FL_BG);
    cfn_fit(&CFN_11,17,33,m->theory,204,C_ACC(0));
    cfn_fit(&CFN_12,17,45,m->degree,49,FL_INK);
    const char *context=m->workspace==CGU_SOUND?CGS_PROFILES[m->genre].name:
        m->workspace==CGU_STUDIO && m->live && !m->bank && m->focus==CGS_DRUMS?cgu_drum_status(m):m->context;
    if(context[0])cfn_fit(&CFN_11,64,45,context,157,FL_INK2);
}

static void cgu_keys_footer(void)
{
    unsigned part=cgs.focus, ink=CGU_INK_BASE+part;
    fb_rect(151,228,79,12,FL_BG2);
    fb_rect(151,229,2,10,ink);
    cfn_text(&CFN_11,156,237,"KEYS",FL_INK2);
    cfn_fit(&CFN_11,188,237,CGS_PART_NAME[part],39,ink);
}

static void cgu_overview_controls(const cf_stage_model_t *m)
{
    for(int i=0;i<4;i++){
        if(!m->label[i][0])continue;
        int x=10+i*56;
        cfn_fit(&CFN_11,x,79,m->label[i],53,FL_INK2);
        cfn_fit(&CFN_11,x,92,m->value[i],53,FL_INK);
    }
}

static void cgu_record_controls(const cf_stage_model_t *m)
{
    fb_rect(10,55,220,44,FL_BG2);
    fb_frame(13,58,214,38,FL_BG3);
    cfn_text(&CFN_11,18,69,"Saved takes",FL_INK);
    cfn_text(&CFN_11,96,69,"K1 Select   Hold SAVE 3s",FL_INK2);
    for(unsigned slot=0;slot<SQ_NPAT;slot++){
        unsigned mask=1u<<slot;
        int x=17+(int)slot*70;
        char text[14];
        kint(text,slot+1u,(m->total&mask)?" Unread":(m->count&mask)?" Saved":" Empty");
        fb_rect(x,76,66,15,slot==m->slot?FL_BG3:FL_BG);
        cfn_fit(&CFN_11,x+3,87,text,60,slot==m->slot?FL_INK:FL_INK2);
        if(slot==m->slot)fb_hline(x,90,66,C_ACC(0));
    }
    if(m->pulse)fb_hline(17+(int)m->slot*70,93,66*m->pulse/100,CGU_INK_BASE+CGS_DRUMS);
}

static void cgu_studio_modules(const cf_stage_model_t *m)
{
    int zoom=m->zoom;
    unsigned selected=cgu_studio_tile(m->focus,m->bank);
    int left=10+(int)(selected&1u)*112, top=53+(int)(selected>>1)*42;
    int x0=left+(10-left)*zoom/100, y0=top+(63-top)*zoom/100;
    int w=108+112*zoom/100, h=39+(m->bank?94:76)*zoom/100;
    for(unsigned i=0;i<CGS_NPART+CGU_BAND_PANELS-1;i++){
        if(zoom==100 && i!=selected)continue;
        int x=x0+((int)(i&1u)-(int)(selected&1u))*(w+4);
        int y=y0+((int)(i>>1)-(int)(selected>>1))*(h+3);
        if(x>=240 || x+w<=0 || y>=218 || y+h<=51)continue;
        unsigned module=i<CGS_NPART?cgu_part(i):i;
        cgu_module(m,module,x,y,w,h,i==selected?zoom:0);
        if(i==selected)fb_rframe(x,y,w,h,i<CGS_NPART?CGU_INK_BASE+module:CGU_BAND_INK_BASE+i-CGS_NPART);
    }
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
    int studio=tab==CGU_STUDIO, zoom=m->zoom;
    unsigned focus=m->focus;
    if(studio){
        fb_clip(0,51,240,218);
        cgu_studio_modules(m);
    } else {
        unsigned tile=tab==CGU_HELP?focus:cgu_part(focus);
        int left=10+(tile&1u)*112, top=107+(tile>>1)*44;
        int sends=tab==CGU_SOUND && m->sound_page==CGU_SENDS;
        int x0=left+(10-left)*zoom/100, y0=top+((sends?54:75)-top)*zoom/100;
        int w=108+112*zoom/100, h=39+(sends?97:76)*zoom/100;
        if(zoom<30){
            if(tab==CGU_SOUND)cgu_overview_controls(m);
            else if(tab==CGU_RECORD)cgu_record_controls(m);
            else cfn_fit(&CFN_14,10,95,"On-device guide",220,FL_INK);
        }
        fb_clip(0,52,240,196);
        for(unsigned i=0;i<4;i++){
            unsigned part=tab==CGU_HELP?i:cgu_part(i);
            int selected=i==tile;
            if(zoom==100 && !selected)continue;
            int x=x0+((int)(i&1u)-(int)(tile&1u))*(w+4);
            int y=y0+((int)(i>>1)-(int)(tile>>1))*(h+5);
            if(x>=240 || y>=196 || x+w<=0 || y+h<=52)continue;
            if(tab==CGU_SOUND)cgu_module(m,part,x,y,w,h,selected?zoom:0);
            else fb_rect(x,y,w,h,selected?FL_BG2:FL_BG1);
            if(selected){
                if(tab==CGU_HELP && part==CGU_HARMONY && zoom>65)fb_hline(x,y,w,CGU_INK_BASE+part);
                else if(tab==CGU_SOUND)fb_rframe(x,y,w,h,CGU_INK_BASE+part);
                else fb_frame(x,y,w,h,CGU_INK_BASE+part);
            }
            if(tab==CGU_SOUND)continue;
            const char *name=tab==CGU_HELP?CGU_TOPICS[part]:CGS_PART_NAME[part];
            cfn_fit(selected && zoom>60?&CFN_16:&CFN_12,x+8,y+(selected && zoom>60?25:17),name,w-18,FL_INK);
            if(tab==CGU_RECORD)
                cgu_grid(m,part,x+8,y+(selected && zoom>60?58:30),selected && zoom>60?12:5,selected && zoom>60?2:1);
        }
        if(tab==CGU_HELP && zoom>65)cgu_help_draw(m);
    }
    fb_unclip();
    int band_overview=studio && !m->live;
    int fx_presets=tab==CGU_SOUND && m->live && m->sound_page==CGU_PRESETS;
    if(!band_overview)fb_hline(10,197,220,FL_BG3);
    const char *hint=m->pending[0]?m->pending:
        studio?(m->live && !m->bank?"EDIT New part":"EDIT New take"):
        tab==CGU_SOUND?(!m->live?"EDIT New sounds":m->sound_page==CGU_TONE?"EDIT New sound":""):
        tab==CGU_RECORD?(m->rec?"REC Finish take":"REC Manual take"):"";
#if CHASE_LIBRARY && FELUCCA_VA
    if(tab==CGU_SOUND && !m->live && !m->build && !m->pending[0])hint="EDIT Load palette";
#endif
    if(fx_presets)hint="K1 Browse FX";
    int footer=band_overview?227:207;
    cfn_fit(&CFN_11,10,footer,hint,106,FL_INK2);
    cfn_text(&CFN_11,119,footer,fx_presets?"SEL Apply":m->live?"SEL Back":"SEL Zoom",fx_presets?FL_INK:FL_INK2);
    cfn_text(&CFN_11,173,footer,m->live && (studio || tab==CGU_SOUND)?"GLO Mix":"SELECT Tabs",FL_INK2);
    if(band_overview){
        cfn_text(&CFN_11,10,238,"SEQ Panels",FL_INK);
        cfn_fit(&CFN_11,68,238,"PRESETS Genre",79,FL_INK2);
    } else if(tab==CGU_SOUND && !m->live){
        cgu_pages(10,210,220,tab,m->focus,CGU_SOUND_PAGES);
    } else if(tab==CGU_SOUND && m->live){
        const char *sound_hint=m->sound_page==CGU_PRESETS?"SAVE Manage FX   FX EQ":
            m->sound_page==CGU_SENDS?"FX Presets":"FX Sends";
        cfn_text(&CFN_11,10,221,sound_hint,FL_INK2);
        if(m->focus!=CGS_DRUMS)cfn_text(&CFN_11,158,221,"LFO Voice",FL_INK2);
        char page[12];
        kint(page,1u+cgu_sound_page_rank(m->focus,m->sound_page),"/");
        kint(page+str_len(page),cgu_sound_page_count(m->focus),0);
        cfn_text(&CFN_11,10,237,"SELECT Pages",FL_INK);
        cfn_text(&CFN_11,91,237,page,CGU_INK_BASE+m->focus);
    } else if(studio && m->live){
        if(!m->bank || m->bank==CGU_BAND_ARP){
            cfn_text(&CFN_11,10,221,"SELECT Pages",FL_INK);
            cfn_text(&CFN_11,91,221,m->shaping?"2/2":"1/2",CGU_INK_BASE+m->focus);
        }
        if(m->bank==CGU_BAND_GENRE)cfn_text(&CFN_11,10,221,"K1 or PRESETS changes genre",FL_INK);
        if(!m->bank && m->focus==CGS_DRUMS)cfn_fit(&CFN_11,10,237,"PRESETS Fill / Drop / Return",139,FL_INK2);
    } else if(tab==CGU_HELP && m->live && m->focus==CGU_HARMONY){
        cfn_text(&CFN_11,10,222,m->value[0],FL_INK);
        cfn_text(&CFN_11,100,222,"EDIT Chord Lab",FL_INK2);
    }
    cgu_keys_footer();
}
