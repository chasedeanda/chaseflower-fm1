/* SPDX-License-Identifier: GPL-3.0-only
 * Planned-slot identity is separate from accepted sounding notes and position.
 */
typedef struct {
    char title[20],status[20],name[28],chord[24],degree[12],notes[48],position[40],hint[48],message[64];
    char label[4][12],value[4][14],rail[4][24],numeral[4][12];
    uint8_t disabled[4],focus,slot,first,n,skin,confirm,conflict,choice,rec;
    uint8_t emphasis[4],cursor_width,focus_weight;
    int8_t dx,chord_dy;
} cfd_model_t;
static cfd_model_t cfd_last;
static uint8_t cfd_valid;
static unsigned cfd_draws;
static char cfd_symbol[24];
static uint32_t cfd_symbol_ms;
static void cfd_model(cfd_model_t *m)
{
    memset(m,0,sizeof *m);m->skin=cf_skin;
    m->dx=cfn_page_dx();
    if(!cfwd.have){str_cpy(m->title,"No working draft",sizeof m->title);return;}
    m->n=cfwd.song.n;m->focus=cfdu.focus%cfwd.song.n;m->first=m->focus/4*4;
    m->slot=cf_sn.ac_on && cf_sn.ac_idea==CF_P_DRAFT && !(cf_sn.ac_waiting&&!cf_sn.ac_paused)?cf_sn.ac_slot:255;
    m->confirm=cfdu.confirm;m->conflict=cfdu.conflict;m->choice=cfdu.choice;
    str_cpy(m->title,m->conflict?"Draft exists":m->confirm?"Remove slot?":CFD_TAB[cu.page[SCR_PEDIT]],sizeof m->title);
    str_cpy(m->status,cfds.error?"WRITE ERROR":cfds.pending?"RAM PENDING":cfds.checkpoint==CFD_RECOVERED?"RECOVERED":cfds.checkpoint==CFD_CHECKPOINTED?"CHECKPOINT":"FACTORY COPY",sizeof m->status);
    if(cf_sn.ac_on && (cf_sn.ac_waiting || cf_sn.ac_paused || cf_sn.ac_lost))str_cpy(m->status,cf_acc_transport(),sizeof m->status);
    char rec[32];unsigned plaque=dm_plaque(rec,sizeof rec);
    if(cf_sn.sq_on)str_cpy(m->status,"REPLAY",sizeof m->status);
    if(plaque==DMP_REC || plaque==DMP_RECIN){m->rec=1;str_cpy(m->status,rec,sizeof m->status);}
    str_cpy(m->name,cfwd.song.name,sizeof m->name);kint(m->position,m->focus+1,"/");kint(m->position+str_len(m->position),m->n," focus");
    if(m->slot!=255){str_cpy(m->position+str_len(m->position)," / ",sizeof m->position-str_len(m->position));kint(m->position+str_len(m->position),m->slot+1,cf_sn.ac_paused?" paused":" playing");}
    cf_spell_t spell;cf_key_spell(cfwd.song.tonic,cfwd.song.scale,&spell);cf_chord_cfg_t cfg=cs.chord;cfg.tonic=cfwd.song.tonic;cfg.scale=cfwd.song.scale;
    cpa_plan_t plan;
    if(cfd_prepare(&plan,cfg,CPA_OFF,CPA_BOTH,0,0)) {
        const cpa_slot_t *s=&plan.slot[m->focus];cf_chord_name(s->root%12,s->mask,cfwd.song.slot[m->focus].bass>=0?s->bass_pc:-1,&spell,m->chord);
        cf_harmony_numeral(cfg.scale,cfg.tonic,s->root%12,m->degree);
        for(unsigned i=0;i<s->n;i++){char note[4];cf_pc_name(s->note[i]%12,&spell,note);if(i)str_cpy(m->notes+str_len(m->notes)," ",sizeof m->notes-str_len(m->notes));str_cpy(m->notes+str_len(m->notes),note,sizeof m->notes-str_len(m->notes));}
        for(unsigned i=0;i<4 && m->first+i<m->n;i++){unsigned at=m->first+i;const cpa_slot_t *v=&plan.slot[at];cf_chord_name(v->root%12,v->mask,cfwd.song.slot[at].bass>=0?v->bass_pc:-1,&spell,m->rail[i]);cf_harmony_numeral(cfg.scale,cfg.tonic,v->root%12,m->numeral[i]);}
    }
    for(int k=0;k<4;k++){cf_knob_t o;memset(&o,0,sizeof o);cfd_knob_info(k,&o);str_cpy(m->label[k],o.name,sizeof m->label[k]);str_cpy(m->value[k],o.val,sizeof m->value[k]);m->disabled[k]=o.dis;}
    str_cpy(m->hint,m->conflict?"SEL confirms / old source stays intact":m->confirm?"SEL confirms / default Cancel":cu.page[SCR_PEDIT]==CFD_PHRASE?"SEL executes action / SELECT slot":"SELECT slot / PLAY hears full draft",sizeof m->hint);
    if(cu.pop==POP_MSG)str_cpy(m->message,cu.msg,sizeof m->message);
    for(unsigned k=0;k<4;k++)m->emphasis[k]=cfn_knob(k);
    m->cursor_width=cfn_cursor(0x30000u|m->slot);m->focus_weight=cfn_focus(0x30000u|m->focus);
    if(str_len(cfd_symbol)!=str_len(m->chord) || memcmp(cfd_symbol,m->chord,str_len(m->chord))){str_cpy(cfd_symbol,m->chord,sizeof cfd_symbol);cfd_symbol_ms=cf_motion?(cu.now?cu.now:1):0;}
    m->chord_dy=cf_tween_offset(cfd_symbol_ms,120,5);
}
static void cfd_draw(void)
{
    cfd_model_t m;cfd_model(&m);cf_stage_valid=cf_acc_valid=0;
    if(cfd_valid && !fb_full && !memcmp(&m,&cfd_last,sizeof m))return;
    cfd_last=m;cfd_valid=1;cfd_draws++;fb_unclip();fb_fill(FL_BG);
    cfn_fit(&CFN_16,10,20,m.title,130,FL_INK);cfn_text(&CFN_12,230-cfn_width(&CFN_12,m.status),20,m.status,m.rec?FL_C(FL_R_ROSE,0):C_ACC(0));fb_hline(10,28,220,FL_BG3);
    cfn_fit(&CFN_12,10,45,m.name,220,FL_INK2);
    cfn_text(&CFN_12,10,65,"PLANNED",FL_INK2);cfn_text(&CFN_16,230-cfn_width(&CFN_16,m.degree),65,m.degree,C_ACC(0));
    fb_clip(0,70,240,107);cfn_fit(&CFN_32,10,101+m.chord_dy,m.chord,220,FL_INK);fb_unclip();cfn_fit(&CFN_12,10,119,m.notes,220,FL_INK2);
    for(unsigned i=0;i<4 && m.first+i<m.n;i++){int cx=30+i*60;fb_clip(i*60,125,(i+1)*60,170);if(m.focus==m.first+i){fb_frame(cx-27,127,54,39,FL_BG3);if(m.focus_weight)fb_hline(cx-26,128,52,C_ACC(0));}cfn_center(&CFN_16,cx,144,m.numeral[i],C_ACC(0));cfn_center(&CFN_12,cx,160,m.rail[i],FL_INK);if(m.slot==m.first+i)fb_hline(cx-22,168,44*m.cursor_width/100,C_ACC(0));}
    fb_unclip();cfn_text(&CFN_11,10,180,m.position,FL_INK2);cfn_shift_panel(m.dx,31,182);cfn_hint(m.hint,FL_INK2);
    cfn_controls(m.label,m.value,m.disabled,m.emphasis);
    if(m.conflict || m.confirm || m.message[0]) {
        fb_rect(8,70,224,95,FL_BG);fb_frame(8,70,224,95,FL_BG3);
        const char *a=m.message[0]?m.message:m.conflict?"Keep the protected draft?":"Remove this numbered slot?";
        const char *b=m.message[0]?"":m.conflict?(m.choice?"Discard draft and edit new":"Return to draft"):(m.choice?"Remove":"Cancel");
        if(m.message[0]) {
            char text[64];str_cpy(text,a,sizeof text);unsigned at=0;while(text[at] && text[at]!='\n')at++;
            char *next=text[at]?text+at+1:0;text[at]=0;cfn_fit(&CFN_12,16,104,text,208,FL_INK);
            if(next)cfn_fit(&CFN_12,16,130,next,208,FL_INK2);
        } else {cfn_fit(&CFN_12,16,104,a,208,FL_INK);cfn_fit(&CFN_16,16,137,b,208,C_ACC(0));}
    }
    if(!cu.pop)cf_present();
}
