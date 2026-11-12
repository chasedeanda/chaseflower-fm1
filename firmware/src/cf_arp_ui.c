/* SPDX-License-Identifier: GPL-3.0-only
 * One large native roll; presentation reads a copied player snapshot only.
 * Both banks retain the same musical picture. No audio RNG, flash or scheduling.
 */
typedef struct {
    char chord[24],degree[12],context[40],mode[16],now[8],position[20],detail[40],owner[16];
    char label[4][12],value[4][14];
    uint16_t bpm;
    uint8_t seq[48],axis[4],axis_n,n,lo,hi,current,played,travel,run,strum,page,skin,mode_id,external,disabled[4];
    uint16_t known;
} cfarp_model_t;
static cfarp_model_t cfarp_last;
static uint8_t cfarp_valid;
static unsigned cfarp_draws;
static void cfarp_notename(unsigned note,char *dst,unsigned size)
{
    char pc[4],oct[8];cf_pc_name(note%12,&cu_spell,pc);kint(oct,(int)note/12-1,0);
    str_cpy(dst,pc,size);str_cpy(dst+str_len(dst),oct,size-str_len(dst));
}
static void cfarp_model(cfarp_model_t *m)
{
    memset(m,0,sizeof *m);m->current=255;m->skin=cf_skin;m->page=cu.page[SCR_ARP]%(cge_selected()?3:2);
    const cf_arp_view_t *a=&cf_sn.arp_view;
    uint8_t note[CF_MAX_NOTES];unsigned n=a->n,mode=a->n?a->mode:cs.arp.mode;
    unsigned oct=a->n?a->oct:cs.arp.oct,root=a->root;uint32_t mask=a->mask;
    m->run=a->on && n && a->count;m->strum=CF_ARP_STRUMMING(mode);
    if(n)memcpy(note,mode==CF_ARP_ORDER?a->order:a->note,n);
    else {
        const uint8_t *src=cf_sn.out.n?cf_sn.out.note:cf_sn.sq_dnote;
        n=cf_sn.out.n?cf_sn.out.n:cf_sn.sq_dn;
        root=cf_sn.out.n?cf_sn.out.root_pc:cf_sn.sq_droot%12;
        mask=cf_sn.out.n?cf_sn.out.mask:cf_sn.sq_dmask;
        if(n)memcpy(note,src,n);
        else {root=cs.chord.tonic;mask=cf_diatonic_mask(cs.chord.scale,root,root,3);n=cf_voice(60+root,mask,0,note);}
    }
    if(!n){n=1;note[0]=60;root=0;mask=1;}
    if(oct<1 || oct>4)oct=1;
    unsigned len=n*oct;m->mode_id=mode;m->external=cf_sn.ext_clock;m->bpm=m->external?cf_sn.ext_bpm:cs.bpm;
    cf_chord_name(root,mask,-1,&cu_spell,m->chord);
    cf_harmony_numeral(cs.chord.scale,cs.chord.tonic,root,m->degree);
    char key[4],meter[12];cf_pc_name(cs.chord.tonic,&cu_spell,key);
    str_cpy(m->context,key,sizeof m->context);str_cpy(m->context+str_len(m->context)," ",sizeof m->context-str_len(m->context));
    str_cpy(m->context+str_len(m->context),cf_progression_mode(cs.chord.scale),sizeof m->context-str_len(m->context));
    kint(meter,cs.beats,cs.beats==6?"/8":"/4");str_cpy(m->context+str_len(m->context)," / ",sizeof m->context-str_len(m->context));
    str_cpy(m->context+str_len(m->context),meter,sizeof m->context-str_len(m->context));
    str_cpy(m->mode,CF_ARP_NAME[mode%CF_ARP_NMODES],sizeof m->mode);
    m->n=m->strum?0:16;m->lo=127;
    uint8_t pitches[48];unsigned pn=0;
    for(unsigned o=0;o<oct;o++)for(unsigned i=0;i<n;i++) {
        unsigned pitch=note[i]+12*o;
        if(pitch>127){if(m->strum)continue;pitch=note[i];}
        unsigned j=pn;while(j && pitches[j-1]>pitch){pitches[j]=pitches[j-1];j--;}pitches[j]=(uint8_t)pitch;pn++;
    }
    if(m->strum && a->n && a->strum_n){pn=a->strum_n;memcpy(pitches,a->strum,pn);for(unsigned i=1;i<pn;i++){uint8_t v=pitches[i];unsigned j=i;while(j && pitches[j-1]>v){pitches[j]=pitches[j-1];j--;}pitches[j]=v;}}
    unsigned unique=0;for(unsigned i=0;i<pn;i++)if(!i || pitches[i]!=pitches[i-1])pitches[unique++]=pitches[i];
    m->axis_n=unique<4?unique:4;
    for(unsigned i=0;i<m->axis_n;i++)m->axis[i]=pitches[m->axis_n>1?i*(unique-1)/(m->axis_n-1):0];
    if(m->strum){
        for(unsigned o=0;o<oct;o++)for(unsigned i=0;i<n;i++)if(note[i]+12*o<=127)m->seq[m->n++]=(uint8_t)(note[i]+12*o);
        if(mode==CF_ARP_STRUM_DOWN)for(unsigned i=0;i<m->n/2;i++){uint8_t t=m->seq[i];m->seq[i]=m->seq[m->n-1-i];m->seq[m->n-1-i]=t;}
    }
    if(m->strum && a->n && a->strum_n){m->n=a->strum_n;memcpy(m->seq,a->strum,m->n);}
    uint32_t first=m->run?a->idx-1-(a->count-1)%16:0;
    unsigned gate=a->n?a->gate:cs.arp.gate,swing=a->n?a->swing:cs.arp.swing;
    if(m->run){m->current=m->strum?(a->strum_i?a->strum_i-1:255):(a->count-1)%16;m->known=a->seen;m->played=a->strum_i;}
    for(unsigned i=0;i<m->n;i++) {
        unsigned k=m->strum?(mode==CF_ARP_STRUM_DOWN?len-1-i:i):cf_arp_walk(mode,len,first+i);
        unsigned pitch=m->strum?m->seq[i]:note[k%n]+12*(k/n);if(pitch>127)pitch=note[k%n];
        if(!m->strum && m->run && ((m->known>>i)&1))pitch=a->trace[i];
        m->seq[i]=(uint8_t)pitch;
        if(pitch<m->lo)m->lo=pitch;if(pitch>m->hi)m->hi=pitch;
    }
    /* Stable pitch scale spans the complete selected octave range, not only
     * today's random history or the 16-step window. Include clipped notes. */
    if(!m->strum)for(unsigned o=0;o<oct;o++)for(unsigned i=0;i<n;i++){unsigned pitch=note[i]+12*o;if(pitch>127)pitch=note[i];if(pitch<m->lo)m->lo=pitch;if(pitch>m->hi)m->hi=pitch;}
    if(m->run) {
        cfarp_notename(a->trace[(a->count-1)%16],m->now,sizeof m->now);
        str_cpy(m->position,m->strum?"Note ":"Step ",sizeof m->position);
        kint(m->position+str_len(m->position),m->current+1," / ");kint(m->position+str_len(m->position),m->strum?m->n:16,0);
        if(cf_motion && !m->strum && a->interval_ms){unsigned age=cf_sn.now_ms-a->onset_ms;m->travel=(uint8_t)(age>=a->interval_ms?11:age*12/a->interval_ms);}
    } else {str_cpy(m->now,"--",sizeof m->now);str_cpy(m->position,a->on?"Waiting":"Preview / Off",sizeof m->position);}
    str_cpy(m->owner,m->run?(a->latched?"Latch holding":cf_sn.out.hold && a->source==0?"HOLD":a->source==1?"Progression":a->source==2?"Chord track":a->source==3?"Layer track":"Live keys"):"Play a chord",sizeof m->owner);
    if(cge_selected()){
        str_cpy(m->owner,cge_articulation_name(),sizeof m->owner);
        str_cpy(m->position,cge_arp_pending()?"Next chord":CGE_ARP_NAMES[cge.arp_applied],sizeof m->position);
    }
    if(m->strum){kint(m->detail,cs.arp.strum_ms," ms / sustain");}
    else {kint(m->detail,gate,"% gate / ");kint(m->detail+str_len(m->detail),swing,"% swing");}
    for(unsigned k=0;k<4;k++){cf_knob_t info;memset(&info,0,sizeof info);ui_knob_info(k,&info);str_cpy(m->label[k],info.name,sizeof m->label[k]);str_cpy(m->value[k],info.val,sizeof m->value[k]);m->disabled[k]=info.dis;}
}
static int cfarp_y(const cfarp_model_t *m,unsigned pitch)
{ return m->hi>m->lo?178-(int)(pitch-m->lo)*80/(m->hi-m->lo):138; }
static void cfarp_draw(void)
{
    cfarp_model_t m;cfarp_model(&m);
    if(cfarp_valid && !fb_full && !memcmp(&m,&cfarp_last,sizeof m))return;
    cfarp_last=m;cfarp_valid=1;cfarp_draws++;
    const char *title=cge_selected()?(m.page==2?"ARP / Live keys 3/3":m.page?"ARP / Timing 2/3":"ARP / Play It 1/3"):
        m.page?"ARP / Timing 2/2":"ARP / Pattern 1/2";
    char tempo[16];kint(tempo,m.bpm,m.external?" EXT":" BPM");
    cfn_page_header(title,tempo,C_ACC(0));
    const int rack=CF_STUDIO && !CF_SCENES;
    cfn_fit(&CFN_32,10,rack?57:50,m.chord,144,FL_INK);
    cfn_fit(&CFN_16,176,rack?57:50,m.now,54,FL_INK);
    int degree_width=cfn_width(&CFN_12,m.degree);
    cfn_text(&CFN_12,10,rack?71:66,m.degree,C_ACC(0));
    cfn_fit(&CFN_12,20+degree_width,rack?71:66,m.context,205-degree_width,FL_INK2);
    fb_hline(10,rack?76:72,220,rack?CGU_METAL+4:FL_BG3);
    cfn_text(&CFN_16,10,88,m.mode,FL_INK);
    cfn_text(&CFN_12,230-cfn_width(&CFN_12,m.position),88,m.position,FL_INK2);
    /* Native roll: 192px time axis, 80px pitch axis, four legible pitch labels. */
    for(unsigned r=0;r<m.axis_n;r++) {
        unsigned pitch=m.axis[r];
        int y=cfarp_y(&m,pitch);char name[8];cfarp_notename(pitch,name,sizeof name);
        cfn_text(&CFN_12,4,y+4,name,FL_INK2);fb_hline(37,y,192,FL_BG1);
    }
    for(unsigned i=0;i<4;i++)fb_vline(37+i*48,98,81,FL_BG1);
    for(unsigned i=0;i<m.n;i++) {
        if(!m.strum && m.mode_id==CF_ARP_RAND && m.run && !((m.known>>i)&1))continue;
        unsigned swing=cf_sn.arp_view.n?cf_sn.arp_view.swing:cs.arp.swing;
        unsigned gate=cf_sn.arp_view.n?cf_sn.arp_view.gate:cs.arp.gate;
        int x=37+(m.strum?i*192/m.n:i*12+((i&1)?12*(swing-50)/50:0));
        int interval=24*((i&1)?100-swing:swing)/100;
        int w=m.strum?230-x:interval*(int)gate/100-1;if(w<2)w=2;if(w>230-x)w=230-x;
        int y=cfarp_y(&m,m.seq[i]);
        uint8_t ink=!m.run?FL_BG3:(m.strum?i<m.played:i==m.current)?FL_INK:C_ACC(2);
        fb_rect(x,y-1,w,3,ink);
        if(i==m.current && m.run){fb_frame(x-1,y-3,w+2,7,FL_INK);if(!m.strum){int cx=x+(int)m.travel*interval/12;if(cx>229)cx=229;fb_vline(cx,98,81,C_ACC(0));fb_hline(x,184,interval>2?interval-2:2,C_ACC(0));}}
    }
    cfn_fit(&CFN_11,10,CFN_HINT_BASE,m.detail,126,FL_INK2);cfn_fit(&CFN_11,140,CFN_HINT_BASE,m.owner,90,C_ACC(0));
    const uint8_t emphasis[4]={0};cfn_controls(m.label,m.value,m.disabled,emphasis);
    fb_unclip();if(!cu.pop)cf_present();
}
