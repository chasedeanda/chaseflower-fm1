/* SPDX-License-Identifier: GPL-3.0-only
 * HOME 6: observe the same live key-cap/assignment owners as normal playing.
 * No audition, parameter edits, event posting, clocks or storage in this view.
 */
typedef struct {
    cf_keycap_t cap[CF_NKEYS];
    char title[40],help[64],detail[64];
    uint8_t focus,automatic;
} cf_learn_model_t;

static const char *cf_learn_modifier_help(unsigned mod)
{
    static const char *const help[CF_MOD_N]={
        "No modifier assigned to this key.",
        "Adds the sixth to the chord.",
        "Adds a major or minor seventh.",
        "Adds the seventh and ninth.",
        "Adds a ninth; keeps the third.",
        "Adds seventh, ninth and eleventh.",
        "Adds seventh, ninth and thirteenth.",
        "Replaces the third with a second.",
        "Replaces the third with a fourth.",
        "Adds a second; keeps the third.",
        "Adds a fourth; keeps the third.",
        "Chooses a major third and fifth.",
        "Chooses a minor third and fifth.",
        "Minor third and lowered fifth.",
        "Major third and raised fifth.",
        "Root and fifth; leaves out the third."
    };
    return help[mod<CF_MOD_N?mod:CF_MOD_NONE];
}
static void cf_learn_model(cf_learn_model_t *m)
{
    memset(m,0,sizeof *m);m->focus=cf_learn_focus;m->automatic=ch.mode==CF_H_AUTO;
    for(int key=0;key<CF_NKEYS;key++)ui_keycap(key,&m->cap[key]);
    if(m->focus>=CF_NKEYS) {
        str_cpy(m->title,"Play a key to learn",sizeof m->title);
        str_cpy(m->help,"Upper: chord choices and HOLD",sizeof m->help);
        str_cpy(m->detail,"Lower: root notes / numbers: degrees",sizeof m->detail);
        return;
    }
    int idx,key=m->focus;
    if(cf_key_role(cs.chord.algo,key,&idx)==CF_ROLE_ROOT) {
        int n=cf_key_root_note(&cs.chord,key);char note[4];
        cf_pc_name(n%12,&cu_spell,note);str_cpy(m->title,note,sizeof m->title);
        kint(m->title+str_len(m->title),n/12-1," - Root");
        str_cpy(m->help,"Root for notes and chosen chords.",sizeof m->help);
        str_cpy(m->detail,"Key, scale and OCT change these notes.",sizeof m->detail);
    } else if(cf_is_off(&cs.chord,idx)) {
        str_cpy(m->title,"HOLD",sizeof m->title);
        str_cpy(m->help,"Keeps roots playing after release.",sizeof m->help);
        str_cpy(m->detail,"Tap again to release held roots.",sizeof m->detail);
    } else if(m->automatic) {
        unsigned mod=ch.slot[idx];
        static const char *const name[CF_MOD_N]={"Unassigned","Sixth","Seventh","Ninth","Added ninth",
            "Eleventh","Thirteenth","Suspended second","Suspended fourth","Added second","Added fourth",
            "Major","Minor","Diminished","Augmented","Power chord"};
        str_cpy(m->title,name[mod<CF_MOD_N?mod:0],sizeof m->title);
        str_cpy(m->help,cf_learn_modifier_help(mod),sizeof m->help);
        str_cpy(m->detail,mod==CF_MOD_7?"Major chord: maj7 / minor chord: m7.":
            "Tap to latch; play a lower root key.",sizeof m->detail);
        if(!cf_harmony_supported(cs.chord.scale,cs.chord.quantize)) {
            str_cpy(m->help,"Unavailable with this scale/quantize.",sizeof m->help);
            str_cpy(m->detail,"Use a 7-note scale with Quantize on.",sizeof m->detail);
        } else if((cf_sn.out.conflict>>idx)&1u)
            str_cpy(m->detail,"Ignored: a later chord choice wins.",sizeof m->detail);
    } else {
        const cf_form_t *f=cf_button_form(&cs.chord,idx);
        str_cpy(m->title,cf_type_name(f->type),sizeof m->title);
        if(f->add) {
            int add=cf_adds_index(f->add);
            str_cpy(m->title+str_len(m->title)," + ",sizeof m->title-str_len(m->title));
            str_cpy(m->title+str_len(m->title),add>=0?cf_adds_name(add):"Custom tones",sizeof m->title-str_len(m->title));
        }
        static const char *const type_help[CF_NTYPES]={"Root plus any assigned added tones.",
            "Major third and fifth above the root.","Minor third and fifth above the root.",
            "Minor third and lowered fifth.","Major third and raised fifth.",
            "Replaces the third with a second.","Replaces the third with a fourth.","Root and fifth; no third."};
        str_cpy(m->help,type_help[f->type<CF_NTYPES?f->type:0],sizeof m->help);
        str_cpy(m->detail,f->add==CF_A_6?"Adds a sixth to that chord.":
            f->add==CF_A_7?"Adds a major seventh to that chord.":
            f->add==CF_A_b7?"Adds a minor seventh to that chord.":
            f->add==CF_A_9?"Adds a ninth; keeps the third.":
            "Tap to latch; play a lower root key.",sizeof m->detail);
    }
}

/* Original 16-lower / 11-staggered-upper physical layout. Full-width key
 * labels use the normal face when they fit, otherwise the existing micro face.
 * Selected key has an outline; held/latched keys retain normal fill semantics. */
static void cf_learn_keyboard(const cf_learn_model_t *m,int left,int width)
{
    for(int key=0;key<CF_NKEYS;key++) {
        const cf_keycap_t *cap=&m->cap[key];
        int x,y,w,h;key_box(key,&x,&y,&w,&h);
        int right=left+(x+w)*width/FB_W;
        x=left+x*width/FB_W;w=right-x;
        int upper=cf_key_is_upper(key);y=upper?99:134;h=upper?31:37;
        uint8_t fill=cap->style==KC_DOWN?FL_INK:cap->style==KC_ON?C_ACC(0):cap->style==KC_SOUNDING?FL_BG2:FL_BG1;
        uint8_t ink=cap->style==KC_DOWN || cap->style==KC_ON?FL_BG:FL_INK;
        fb_rect(x,y,w,h,fill);
        char label[8];str_cpy(label,cap->top,sizeof label);
        for(unsigned i=0;label[i];i++)if((uint8_t)label[i]>=0x10u && (uint8_t)label[i]<=0x19u)
            label[i]=(char)('0'+label[i]-0x10);
        if(cfn_width(&CFN_12,label)<=w-1)cfn_center(&CFN_12,x+w/2,y+16,label,ink);
        else fb_text_c(&CF_FONT_MICRO,x+w/2,y+11,label,ink,1);
        if(!upper && cap->bot[0])fb_text_c(&CF_FONT_MICRO,x+w/2,y+26,cap->bot,ink,1);
        if(key==m->focus){fb_frame(x,y,w,h,C_ACC(0));fb_hline(x+3,y+h-3,w-6,ink);}
    }
}
