/* SPDX-License-Identifier: GPL-3.0-only
 * Native 240x240 Songs list. Rendering uses the foreground index only.
 */
typedef struct {
    char name[4][28],meta[4][40],title[24],status[24],hint[44],detail[32],message[64];
    uint8_t row,first,n,mode,choice,skin,pending,cursor;
} cfl_model_t;
static cfl_model_t cfl_last;
static uint8_t cfl_screen_valid;
static unsigned cfl_draws;
static void cfl_model(cfl_model_t *m)
{
    memset(m,0,sizeof *m);m->skin=cf_skin;m->mode=cflu.mode;m->choice=cflu.choice;m->cursor=cflu.cursor;
    uint8_t rows[1+CFL_CAPACITY+SQ_NPAT];unsigned n=cfl_rows(rows);m->row=cflu.row%n;m->first=m->row/4*4;m->n=n;
    str_cpy(m->title,cflu.mode==CFL_CONFIRM?"Delete 1 Song?":cflu.mode==CFL_RENAME?"Rename Song":cflu.mode==CFL_MANAGE?"Manage Song":"Songs",sizeof m->title);
    str_cpy(m->status,cfl_tx.error?"WRITE ERROR":cfl_tx.pending?"PENDING":"",sizeof m->status);m->pending=cfl_tx.pending;
    for(unsigned i=0;i<4 && m->first+i<n;i++) {
        unsigned row=rows[m->first+i];char *name=m->name[i],*meta=m->meta[i];
        if(!row){str_cpy(name,"+ Save current idea",28);str_cpy(meta,cfl_free()<0?"Full / EDIT a saved row":"Progression + musical setup",40);continue;}
        if(row<=CFL_CAPACITY) {
            unsigned slot=row-1;const cfl_entry_t *e=&cfl_index[slot];
            int pending=cfl_tx.pending && cfl_tx.slot==slot;
            kint(name,slot+1," ");
            char pending_name[CF_SONG_NAME+1];memcpy(pending_name,cfl_io.phrase+8,CF_SONG_NAME);pending_name[CF_SONG_NAME]=0;
            str_cpy(name+str_len(name),pending&&!cfl_tx.remove?pending_name:e->unavailable?"Unavailable":e->name,28-str_len(name));
            str_cpy(meta,pending?(cfl_tx.remove?"P / Delete pending":"P / Pending: stop + release"):e->unavailable?"P / Restore backup":"P / Saved / ",40);
            if(!pending && e->used){kint(meta+str_len(meta),e->n," slots / ");kint(meta+str_len(meta),e->bpm," bpm");}
        } else {
            unsigned pat=row-1-CFL_CAPACITY;kint(name,pat+1," Recording");
            const sq_pat_t *p=&sq_st.pat[pat];
            str_cpy(meta,sq_unavailable&(1u<<pat)?"R / Unavailable":sqk.on&&sqk.pat==pat?"R / Recording":sq_take_image(pat)->check!=sq_saved[pat]?"R / Pending":"R / Saved",40);
            if(p->bars){str_cpy(meta+str_len(meta)," / ",40-str_len(meta));kint(meta+str_len(meta),p->bars," bars");}
        }
    }
    str_cpy(m->hint,cflu.mode==CFL_LIST?"SELECT row / SEL load / EDIT manage":cflu.mode==CFL_MANAGE?"K1 action / SEL do / EDIT back":cflu.mode==CFL_CONFIRM?"K1 choice / SEL confirm":"K1 position / K2 letter / SEL save",sizeof m->hint);
    if(cflu.mode==CFL_LIST && !rows[m->row])str_cpy(m->hint,"SEL saves current / HOME returns",sizeof m->hint);
    if(cflu.mode==CFL_MANAGE)str_cpy(m->detail,CFL_ACTION[cflu.action],sizeof m->detail);
    if(cflu.mode==CFL_CONFIRM)str_cpy(m->detail,cflu.choice?"Delete selected Song":"Cancel",sizeof m->detail);
    if(cflu.mode==CFL_RENAME)str_cpy(m->detail,cflu.name,sizeof m->detail);
    if(cu.pop==POP_MSG)str_cpy(m->message,cu.msg,sizeof m->message);
}
static void cfl_draw(void)
{
    cfl_model_t m;cfl_model(&m);cf_stage_valid=cf_acc_valid=cfd_valid=0;
    if(cfl_screen_valid && !fb_full && !memcmp(&m,&cfl_last,sizeof m))return;
    cfl_last=m;cfl_screen_valid=1;cfl_draws++;fb_unclip();fb_fill(FL_BG);
    cfn_text(&CFN_16,10,20,m.title,FL_INK);cfn_fit(&CFN_12,145,20,m.status,85,C_ACC(0));fb_hline(10,29,220,FL_BG3);
    for(unsigned i=0;i<4 && m.first+i<m.n;i++) {
        int y=36+i*41;if(m.row==m.first+i)fb_rect(7,y,226,40,FL_BG2);
        cfn_fit(&CFN_16,12,y+17,m.name[i],216,FL_INK);cfn_fit(&CFN_12,12,y+34,m.meta[i],216,C_ACC(0));
    }
    fb_hline(10,204,220,FL_BG3);cfn_fit(&CFN_12,10,221,m.hint,220,FL_INK2);
    cfn_text(&CFN_12,10,238,"P Progression    R Recording",FL_INK2);
    if(m.mode!=CFL_LIST || m.message[0]) {
        fb_rect(8,70,224,100,FL_BG);fb_frame(8,70,224,100,FL_BG3);
        cfn_fit(&CFN_12,16,93,m.message[0]?"Songs":(m.mode==CFL_CONFIRM || m.mode==CFL_MANAGE)?m.name[m.row-m.first]:m.title,208,FL_INK2);
        cfn_fit(&CFN_16,16,120,m.message[0]?m.message:m.detail,208,C_ACC(0));
        if(m.mode==CFL_RENAME){char pos[20];kint(pos,m.cursor+1," / 24");cfn_text(&CFN_12,16,147,pos,FL_INK2);}
        else cfn_fit(&CFN_12,16,147,m.message[0]?"":m.hint,208,FL_INK2);
    }
    cf_present();
}
