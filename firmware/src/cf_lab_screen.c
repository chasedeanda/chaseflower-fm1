/* SPDX-License-Identifier: GPL-3.0-only
 * Native 240 x 240 Chord Lab. Stable labels, non-color focus, no visual timer.
 */
typedef struct {
    char title[24],target[40],key[40],chord[20],degree[12],outside[64],hint[48],message[64];
    char notes[CF_MAX_NOTES][12],interval[CF_MAX_NOTES][4],path[64];
    char candidate[3][20],numeral[3][8],reason[3][16],side[2][20],compare_notes[2][64];
    char label[4][12],value[4][14];
    uint8_t tab,skin,n,focus,selection,playing,enabled,comparison,out[CF_MAX_NOTES],disabled[4];
} cfl_model_t;
static cfl_model_t cfl_last;
static uint8_t cfl_valid;
static unsigned cfl_draws;
static void cfl_append(char *dst,unsigned size,const char *s)
{ unsigned at=str_len(dst);if(at<size)str_cpy(dst+at,s,size-at); }
static void cfl_notes_line(const cf_lab_info_t *info,char *dst,unsigned size)
{ dst[0]=0;for(unsigned i=0;i<info->n;i++){if(i)cfl_append(dst,size," ");cfl_append(dst,size,info->note[i]);} }
static void cfl_model(cfl_model_t *m)
{
    memset(m,0,sizeof *m);cfl_poll();m->tab=clu.tab;m->skin=cf_skin;m->playing=cf_sn.audition_on;
    str_cpy(m->title,CFL_TAB[clu.tab],sizeof m->title);
    const char *target=clu.target>=2?"Planned slot ":clu.target==1 || !clu.now_on?"Last":"Now";
    str_cpy(m->target,target,sizeof m->target);if(clu.target>=2)kint(m->target+str_len(m->target),clu.target-1,0);
    else if(clu.now_on && !clu.target)cfl_append(m->target,sizeof m->target,clu.now_audition?" / audition":cf_sn.out.sounding?" / live":" / loop");
    str_cpy(m->key,clu.info.key,sizeof m->key);cfl_append(m->key,sizeof m->key," ");cfl_append(m->key,sizeof m->key,CF_SCALES[clu.scale].name);
    str_cpy(m->chord,clu.info.n?clu.info.name:"--",sizeof m->chord);str_cpy(m->degree,clu.info.degree,sizeof m->degree);
    m->n=clu.info.n;m->focus=clu.note;m->selection=clu.candidate;m->comparison=clu.comparison;
    for(unsigned i=0;i<m->n;i++) {
        str_cpy(m->notes[i],clu.info.note[i],sizeof m->notes[i]);kint(m->notes[i]+str_len(m->notes[i]),clu.target_chord.note[i]/12-1,0);
        str_cpy(m->interval[i],clu.info.interval[i],sizeof m->interval[i]);m->out[i]=clu.info.outside[i];
        if(m->out[i]){if(m->outside[0])cfl_append(m->outside,sizeof m->outside," ");cfl_append(m->outside,sizeof m->outside,clu.info.note[i]);}
    }
    str_cpy(m->hint,"SELECT target / ALGO tab",sizeof m->hint);
    if(clu.tab==CFL_COMPARE) {
        m->selection=clu.side;m->enabled=cfl_comparisons();
        str_cpy(m->target,clu.compare_target>=2?"Planned slot ":"Captured chord",sizeof m->target);
        if(clu.compare_target>=2)kint(m->target+str_len(m->target),clu.compare_target-1,0);
        cf_spell_t sp;cf_key_spell(clu.compare_tonic,clu.compare_scale,&sp);cf_pc_name(clu.compare_tonic,&sp,m->key);
        cfl_append(m->key,sizeof m->key," ");cfl_append(m->key,sizeof m->key,CF_SCALES[clu.compare_scale].name);
        for(unsigned i=0;i<2 && m->enabled;i++){str_cpy(m->side[i],clu.compare_info[i].name,sizeof m->side[i]);cfl_notes_line(&clu.compare_info[i],m->compare_notes[i],sizeof m->compare_notes[i]);}
        str_cpy(m->hint,m->playing?"SEL stop / K2 switch side":"SELECT comparison / SEL hear",sizeof m->hint);
    }
    if(clu.tab==CFL_NEXT) {
        m->enabled=clu.nchoices;
        cf_spell_t sp;cf_key_spell(clu.next_history.tonic,clu.next_history.scale,&sp);
        cf_pc_name(clu.next_history.tonic,&sp,m->key);cfl_append(m->key,sizeof m->key," ");cfl_append(m->key,sizeof m->key,CF_SCALES[clu.next_history.scale].name);
        if(clu.explicit_slot){str_cpy(m->target,"Insert A / slot ",sizeof m->target);kint(m->target+str_len(m->target),clu.insert+1,0);}
        else str_cpy(m->target,"K3: choose insert slot",sizeof m->target);
        for(unsigned i=0;i<clu.next_history.n;i++){cf_lab_info_t info;cf_lab_describe(&clu.next_history.chord[i],clu.next_history.scale,clu.next_history.tonic,&info);if(i)cfl_append(m->path,sizeof m->path," > ");cfl_append(m->path,sizeof m->path,info.name);}
        if(!m->path[0])str_cpy(m->path,clu.context?"Draft path: start":"Live history: play a chord",sizeof m->path);
        for(unsigned i=0;i<clu.nchoices;i++){str_cpy(m->candidate[i],clu.choice_info[i].name,sizeof m->candidate[i]);str_cpy(m->numeral[i],clu.choice_info[i].degree,sizeof m->numeral[i]);str_cpy(m->reason[i],clu.choice[i].reason,sizeof m->reason[i]);}
        str_cpy(m->hint,!clu.nchoices?"Seven-note context required":clu.fallback?"Theory-guided fallback":"Local catalogue rules",sizeof m->hint);
    }
    for(int k=0;k<4;k++){cf_knob_t o={0};cfl_knob_info(k,&o);str_cpy(m->label[k],o.name,sizeof m->label[k]);str_cpy(m->value[k],o.val,sizeof m->value[k]);m->disabled[k]=o.dis;}
    if(cu.pop==POP_MSG)str_cpy(m->message,cu.msg,sizeof m->message);
}
static void cfl_draw(void)
{
    cfl_model_t m;cfl_model(&m);cf_stage_valid=cf_acc_valid=cfd_valid=0;
    if(cfl_valid && !fb_full && !memcmp(&m,&cfl_last,sizeof m))return;
    cfl_valid=1;cfl_last=m;cfl_draws++;
    char tab[12];kint(tab,m.tab+1,"/4");
    cfn_page_header(m.title,tab,C_ACC(0));
    cfn_fit(&CFN_12,10,45,m.target,220,C_ACC(0));
    cfn_fit(&CFN_12,10,62,m.key,220,FL_INK2);
    if(m.tab==CFL_NEXT) {
        cfn_fit(&CFN_12,10,79,m.path,220,FL_INK2);
        if(!m.enabled){cfn_text(&CFN_16,10,111,"No suggestions",FL_INK);cfn_fit(&CFN_12,10,137,"Scale unsupported / inspect notes",220,FL_INK2);}
        for(unsigned i=0;i<m.enabled;i++) {
            int y=99+i*29;
            if(m.selection==i)fb_frame(8,y-15,224,28,FL_BG3);
            cfn_text(&CFN_12,12,y,m.selection==i?">":" ",C_ACC(0));
            cfn_text(&CFN_16,26,y,m.numeral[i],C_ACC(0));cfn_fit(&CFN_16,70,y,m.candidate[i],150,FL_INK);
            cfn_text(&CFN_12,70,y+12,m.reason[i],FL_INK2);
        }
        cfn_fit(&CFN_11,10,178,m.hint,220,FL_INK2);
        cfn_hint(!m.enabled?"ALGO: inspect notes and intervals":m.playing?"AUDITION / SEL stop / EDIT insert":"SEL hear / EDIT insert",C_ACC(0));
    } else if(m.tab==CFL_COMPARE) {
        cfn_fit(&CFN_12,10,80,m.comparison?"Root / Smooth (fixed context)":"Triad / selected color",220,FL_INK2);
        if(!m.enabled)cfn_fit(&CFN_16,10,121,m.comparison?"Choose another target":m.n?"Choose a color":"Play a root or choose a slot",220,FL_INK);
        for(unsigned i=0;i<2 && m.enabled;i++) {
            int y=106+i*43;if(m.selection==i)fb_frame(8,y-18,224,40,FL_BG3);
            cfn_text(&CFN_16,14,y,i?"B":"A",C_ACC(0));cfn_fit(&CFN_16,38,y,m.side[i],182,FL_INK);cfn_fit(&CFN_12,38,y+16,m.compare_notes[i],182,FL_INK2);
        }
        cfn_fit(&CFN_11,10,178,m.playing?"AUDITION / one bar / clock continues":"Silent until SEL / one bar",220,C_ACC(0));
        cfn_hint(m.hint,FL_INK2);
    } else {
        cfn_fit(&CFN_32,10,99,m.chord,160,FL_INK);cfn_fit(&CFN_16,181,99,m.degree,49,C_ACC(0));
        if(!m.n){cfn_fit(&CFN_12,10,132,"Play a root or choose a slot",220,FL_INK2);}
        else if(m.tab==CFL_NOTES) {
            for(unsigned i=0;i<m.n;i++) {
                int x=10+(i%4)*56,y=124+(i/4)*19;char note[16];str_cpy(note,m.notes[i],sizeof note);if(m.out[i])cfl_append(note,sizeof note,"*");
                cfn_text(&CFN_12,x,y,note,FL_INK);
            }
            cfn_fit(&CFN_12,10,182,m.outside[0]?"* Outside selected scale":"All tones in selected scale",220,FL_INK2);
            cfn_hint(m.hint,FL_INK2);
        } else {
            cfn_text(&CFN_32,10,136,m.notes[m.focus],FL_INK);cfn_text(&CFN_32,135,136,m.interval[m.focus],C_ACC(0));
            cfn_text(&CFN_12,135,153,"interval",FL_INK2);
            cfn_fit(&CFN_12,10,174,m.out[m.focus]?"Outside selected scale":"In selected scale",220,FL_INK2);
            char pos[48];kint(pos,m.focus+1,"/");kint(pos+str_len(pos),m.n," voiced notes / K2 focus");cfn_hint(pos,FL_INK2);
        }
    }
    const uint8_t emphasis[4]={0};
    cfn_controls(m.label,m.value,m.disabled,emphasis);
    if(m.message[0]){cfn_dialog(8,85,224,63,FL_BG3);cfn_fit(&CFN_12,16,113,m.message,208,FL_INK);}
    cf_present();
}
