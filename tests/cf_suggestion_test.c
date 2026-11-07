/* SPDX-License-Identifier: GPL-3.0-only
 * Real UI/ISR integration: browsing must not disturb performance or user data. */
#include <stdio.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; if (failures <= 10) printf("FAIL %d: %s\n", __LINE__, #x); } } while(0)

static void controls(void)
{
    cf_knob_t knob = {0};
    cf_set_t settings = cs;
    cf_harmony_t harmony = ch;
    uint32_t patterns = cfs_fnv(&sq_st, sizeof sq_st), queue = cf_evq_w;
    CHECK(sizeof(cf_set_t) == 188 && sizeof(sq_ev_t) == 22 && sizeof cpg == 2);
    cu.scr = SCR_GLO; cu.page[SCR_GLO] = CF_GLO_HARMONY; ui_select(1);
    CHECK(cu.page[SCR_GLO] == CF_GLO_PROGRESSIONS && screen_pages(SCR_GLO) == CF_GLO_NPAGES);
    CHECK(cf_progression_current() == cf_progression_get(0));
    ui_knob_turn(1,1); CHECK(cpg.idea == 1);
    ui_knob_turn(0,CF_P_EMOTIONAL); CHECK(cpg.idea == 1);
    ui_knob_turn(1,1); CHECK(cpg.idea == 2);
    ui_knob_info(1,&knob); CHECK(!strcmp(knob.val,"2/3") && !knob.dis);
    ui_button(B_HOME); ui_button(B_GLO);
    CHECK(cu.scr == SCR_GLO && cu.page[SCR_GLO] == CF_GLO_PROGRESSIONS && cpg.idea == 2);
    CHECK(!memcmp(&settings,&cs,sizeof cs) && !memcmp(&harmony,&ch,sizeof ch));
    CHECK(cf_evq_w == queue && cfs_fnv(&sq_st,sizeof sq_st) == patterns && !sq_dirty);
    ui_knob_turn(2,7); CHECK(cs.chord.tonic == 7 && cpg.idea == 2);
    ui_knob_turn(3,CF_SC_MINOR); CHECK(cs.chord.scale == CF_SC_MINOR && cpg.idea == 9);
    ui_knob_turn(0,CF_P_JAZZY-CF_P_EMOTIONAL); CHECK(cpg.idea == CF_P_NONE);
    ui_knob_info(1,&knob); CHECK(!strcmp(knob.val,"0/0") && knob.dis);
    ui_knob_turn(1,1000); CHECK(cpg.idea == CF_P_NONE);
    ui_knob_turn(0,-CF_P_JAZZY); CHECK(cpg.idea == 8);
    ui_select(-1); ui_knob_turn(1,2); ui_select(1);
    CHECK(ch.style == CF_H_BASIC && cpg.idea == 8);
    cs.chord.scale = CF_SC_LYDIAN;
    CHECK(!cf_progression_current() && cf_harmony_supported(cs.chord.scale,cs.chord.quantize));
    cs.chord.scale = CF_SC_MAJOR; cs.chord.quantize = CF_Q_OFF;
    unsigned tonic=cs.chord.tonic; ui_knob_turn(2,1); ui_knob_turn(3,1);
    CHECK(cs.chord.tonic == tonic && cs.chord.scale == CF_SC_MAJOR);
    CHECK(cf_progression_key(&cs.chord,0) == -1);
    /* Every rendered idea/tag and legacy style, plus Off, Quantize Off and AUTO?. */
    cf_intro_on=0; cu.pop=POP_NONE; cv_turn_ms=0;
    for (unsigned i=0;i<cf_progression_count();i++) {
        const cf_progression_t *p=cf_progression_get(i);
        for(int scale=0;scale<CF_NSCALES;scale++) if((p->scales>>scale)&1u) {
            cs.chord.scale=(uint8_t)scale; cpg.tag=CF_P_ALL; cpg.idea=(uint8_t)i;
            for(int style=0;style<CF_H_NSTYLES;style++) for(int mode=0;mode<2;mode++) {
                ch.style=(uint8_t)style; ch.mode=(uint8_t)mode; cs.chord.quantize=CF_Q_FULL;
                for(int tonic=0;tonic<12;tonic++) { cs.chord.tonic=(uint8_t)tonic; cf_ui_draw(); }
            }
        }
    }
    for(int scale=0;scale<CF_NSCALES;scale++) for(int tag=0;tag<CF_P_NTAGS;tag++) {
        cs.chord.scale=(uint8_t)scale; cpg.tag=(uint8_t)tag;
        for(int quant=CF_Q_OFF;quant<=CF_Q_FULL;quant++) { cs.chord.quantize=(uint8_t)quant; cf_ui_draw(); }
    }
    CHECK(cfs_fnv(&sq_st,sizeof sq_st) == patterns);
    cs=settings; ch=harmony; cf_send_config();
}

static void render_audio(int browse, unsigned rate, const char *path)
{
    FILE *out=fopen(path,"wb"); CHECK(out); if(!out) return;
    int32_t mix[CTL*2];
    cf_intro_on=0; cf_boot_go=0; cu.pop=POP_NONE;
    cf_harmony_defaults(&ch); ch.mode=CF_H_AUTO; ch.slot[0]=CF_MOD_13;
    cu.scr=SCR_GLO; cu.page[SCR_GLO]=CF_GLO_PROGRESSIONS;
    cf_send_config();
    for(unsigned b=0;b<64;b++) audio_block(mix,CTL);
    cf_post(CFE_KEY,1,0,1); cf_post(CFE_KEY,1,0,0); /* select 13 explicitly */
    cf_post(CFE_KEY,25,0,1); cf_post(CFE_KEY,25,0,0); /* HOLD */
    cf_post(CFE_KEY,7,0,1); cf_post(CFE_KEY,7,0,0);
    sq_fresh(&sq_st.pat[0],4); dm_fill(&sq_st.pat[0],dm_nth(4,0),0);
    sqp_start(0,0); cfm_restart(); cfm.running=1;
    ua_reset(); ua.cap_alt=1; ua.cap_hold=0; ua_rate_set(rate);
    uint32_t patterns=cfs_fnv(&sq_st,sizeof sq_st);
    for(unsigned b=0;b<FS*3u/CTL;b++) {
        if(b%13u==0) {
            uint32_t queue=cf_evq_w; cf_chord_t held=cfc;
            if(browse) {
                ui_knob_turn(0,b%2?1:-1); ui_knob_turn(1,b%3?1:-1);
                cf_snapshot(); cf_ui_draw();
                if (cfc.out.sounding) CHECK(cu_name_live && !strcmp(cu_name,"Cmaj13"));
            }
            CHECK(queue==cf_evq_w && !memcmp(&held,&cfc,sizeof held));
        }
        uint32_t w=ua.cw; audio_block(mix,CTL);
        CHECK(cfc.out.hold && cfc.out.n==6 && sqp.on);
        fwrite(mix,sizeof mix,1,out);
        for(unsigned i=w;i<ua.cw;i++) fwrite(ua.cap+(i&(UA_CAP_RING-1u))*6u,6u*sizeof(int16_t),1,out);
        ua.cr=ua.cw;
    }
    CHECK(patterns==cfs_fnv(&sq_st,sizeof sq_st));
    fclose(out);
}
int main(int argc,char **argv)
{
    emu_fw_options(NULL,1,0,1); emu_hal.ready=2; emu_fw_init(0);
    if(argc==5 && !strcmp(argv[1],"--audio")) render_audio(atoi(argv[2]),(unsigned)atoi(argv[3]),argv[4]);
    else controls();
    printf("suggestions: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
