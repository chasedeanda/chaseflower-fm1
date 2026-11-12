/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#define CF_STUDIO 0
#include "../tools/emu/emu_fw.c"
static unsigned checks,failures;
#define CHECK(x) do{checks++;if(!(x)){failures++;if(failures<30)printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static void blocks(unsigned n){int32_t out[CTL*2];while(n--)audio_block(out,CTL);cf_snapshot();cpa_ui_poll();}
static void rest(void)
{
    cpa_stop();sqp_stop();dmp_stop();cfp_panic();cu.keys=0;cs.clock_in=0;blocks(FS/CTL*4);
    cf_sn.metro_on=cf_sn.running=0;cfx_quiet=FS/CTL*2;cu.pop=POP_NONE;
}
static void save_current(void)
{ui_button(B_SAVE);CHECK(cu.scr==SCR_SONGS);cslu.row=0;ui_button(B_SCL);CHECK(csl_tx.pending);}
static void library(void)
{
    rest();cf_progression_choose(0);ui_button(B_EDIT);CHECK(cfwd.have);
    ui_knob_turn(1,2);cf_song_t original=cfwd.song;
    cs.bpm=137;cs.level_chord=87;cs.level_drums=71;cs.bass_level=79;cs.fx.v[CFX_REVERB][1]=31;
    trk[0].p[P_E0]=31;cf_edited(0);cf_send_config();blocks(2);cfd_ui_poll();original=cfwd.song;
    unsigned writes=emu_flash_writes;save_current();CHECK(emu_flash_writes==writes);
    csl_record_t saved=csl_io;CHECK(csl_valid(&saved));CHECK(!csl_read(0));
    csl_model_t model;csl_model(&model);CHECK(model.pending && strstr(model.meta[1],"Pending"));
    /* Held notes and FX tails cannot be cut to persist a Song. */
    cf_post(CFE_KEY,7,0,1);blocks(2);csl_poll(cu.now,1);CHECK(csl_tx.pending && emu_flash_writes==writes);
    cf_post(CFE_KEY,7,0,0);rest();csl_poll(cu.now,0);CHECK(csl_tx.pending);
    csl_io.check^=1;csl_poll(cu.now,1);CHECK(csl_tx.error && csl_tx.pending && emu_flash_writes==writes);
    csl_io.check^=1;csl_poll(cu.now,1);CHECK(csl_tx.pending);cu.now+=1001;
    cfs_sound_ms=cu.now-CF_SET_SILENT_MS;settings_poll();CHECK(!csl_tx.pending && csl_index[0].used && !cfwd.dirty && emu_flash_writes>writes);
    csl_load();CHECK(csl_index[0].used && csl_index[0].bpm==137);
    cs.bpm=90;cs.level_chord=12;cs.level_drums=13;cs.bass_level=14;cs.fx.v[CFX_REVERB][1]=1;
    unsigned skin=cs.accent;cs.clock_in=2;CHECK(csl_recall(0));blocks(3);
    CHECK(cs.bpm==137 && cs.level_chord==87 && cs.level_drums==71 && cs.bass_level==79);
    CHECK(cs.fx.v[CFX_REVERB][1]==31 && cs.accent==skin && cs.clock_in==2);
    CHECK(trk[0].p[P_E0]==31 && cfk_edited(0) && cfwd.song.slot[0].quality==CF_SONG_MINOR);
    CHECK(!memcmp(&original,&cfwd.song,sizeof original) && cpg.idea==CF_P_DRAFT && cu.scr==SCR_HOME && cf_home_bank==CF_HB_CORE);
    /* Recall selects editable semantic playback, never the fixed-note recorder. */
    cs.clock_in=0;ui_button(B_PLAY);blocks(2);CHECK(cpa.on && cpa.plan.idea==CF_P_DRAFT);rest();
    /* Dirty working draft and live HOLD survive failed recall unchanged. */
    ui_button(B_EDIT);ui_knob_turn(0,1);cf_song_t dirty=cfwd.song;
    CHECK(!csl_recall(0) && !memcmp(&dirty,&cfwd.song,sizeof dirty));
    cfwd.dirty=0;cf_post(CFE_KEY,7,0,1);blocks(2);CHECK(!csl_recall(0));cf_post(CFE_KEY,7,0,0);rest();
    /* Frozen pending image cannot be overwritten by another save. */
    cpg.idea=CF_P_DRAFT;save_current();saved=csl_io;csl_save_current();CHECK(!memcmp(&saved,&csl_io,sizeof saved));
    cfwd.song.slot[0].degree=(cfwd.song.slot[0].degree+1)%7;cfwd.dirty=1;
    csl_poll(cu.now,1);CHECK(!csl_tx.pending && csl_index[1].used && cfwd.dirty);
    cfwd.dirty=0;
    /* Backup restores validate semantic payloads and wait for Pending/notes. */
    CHECK(csl_read(0));saved=csl_io;
    for(unsigned i=0;i<CSL_CAPACITY;i++)CHECK(crb_obj(CFB_SONG+i)==OBJ_CFSONG0+i && crb_size_ok(CFB_SONG+i,sizeof saved));
    memcpy(cfb_raw,&saved,sizeof saved);crb.id=CFB_SONG+5;crb.pos=crb.len=sizeof saved;crb.crc=st_crc32(cfb_raw,sizeof saved);
    CHECK(crb_commit()==0 && cfs_restore_lock);cfs_restore_lock=0;CHECK(!st_save(OBJ_CFSONG5,0,0));
    ((csl_record_t *)cfb_raw)->version++;crb.crc=st_crc32(cfb_raw,sizeof saved);writes=emu_flash_writes;
    CHECK(crb_commit()==2 && emu_flash_writes==writes);
    memcpy(cfb_raw,&saved,sizeof saved);crb.crc=st_crc32(cfb_raw,sizeof saved);csl_stage(3,0);
    CHECK(crb_commit()==3);crb_list();CHECK(!crb.listed);csl_tx.pending=0;
    /* Duplicate, rename, and Cancel-default deletion address the selected row. */
    csl_open();cslu.row=1;csl_edit();cslu.action=CSL_DUPLICATE;csl_execute();CHECK(csl_tx.pending && csl_tx.slot==2);
    csl_poll(cu.now,1);CHECK(csl_index[2].used);
    cslu.row=3;csl_edit();cslu.action=CSL_RENAME_ACTION;csl_execute();CHECK(cslu.mode==CSL_RENAME);
    memset(cslu.name,' ',CF_SONG_NAME);memcpy(cslu.name,"Evening",7);cslu.name[CF_SONG_NAME]=0;csl_execute();csl_poll(cu.now,1);
    CHECK(!strcmp(csl_index[2].name,"Evening") && strcmp(csl_index[0].name,"Evening"));
    csl_edit();cslu.action=CSL_DELETE;csl_execute();CHECK(cslu.mode==CSL_CONFIRM && !cslu.choice);
    writes=emu_flash_writes;csl_execute();CHECK(!csl_tx.pending && csl_index[2].used && writes==emu_flash_writes);
    csl_edit();cslu.action=CSL_DELETE;csl_execute();csl_turn(0,1);csl_execute();CHECK(csl_tx.pending && csl_tx.remove);
    csl_poll(cu.now,1);csl_load();CHECK(!csl_index[2].used && !csl_index[2].unavailable);
    /* Unknown versions remain visible and cannot be allocated over. */
    saved.version++;saved.check=cfs_fnv(&saved,__builtin_offsetof(csl_record_t,check));
    CHECK(!st_save(OBJ_CFSONG0+2,&saved,sizeof saved));csl_load();CHECK(csl_index[2].unavailable && csl_free()==3);
    /* Bounds and corruption never decode into active playback. */
    CHECK(csl_read(0));saved=csl_io;
    for(unsigned i=0;i<sizeof saved;i++){((uint8_t *)&saved)[i]^=1;CHECK(!csl_valid(&saved));((uint8_t *)&saved)[i]^=1;}
    for(unsigned i=0;i<CSL_CAPACITY;i++) {
        CHECK(st_sector(OBJ_CFSONG0+i,0)==0xF0000+i*8192 && st_sector(OBJ_CFSONG0+i,1)==0xF1000+i*8192);
        for(unsigned j=0;j<OBJ_COUNT;j++)if(j!=OBJ_CFSONG0+i)for(unsigned copy=0;copy<2;copy++)
            CHECK(st_sector(OBJ_CFSONG0+i,copy)!=st_sector(j,0) && st_sector(OBJ_CFSONG0+i,copy)!=st_sector(j,1));
    }
    /* Full does not overwrite the last item or protected unknown records. */
    for(unsigned i=3;i<CSL_CAPACITY;i++){CHECK(!st_save(OBJ_CFSONG0+i,&saved,sizeof saved));}csl_load();CHECK(csl_free()<0);
    writes=emu_flash_writes;csl_save_current();CHECK(!csl_tx.pending && writes==emu_flash_writes);
    csl_open();cu.pop=POP_NONE;cf_ui_draw();unsigned hash=cfs_fnv(fb,FB_W*FB_H),draws=csl_draws;
    cu.now+=5000;cf_ui_draw();CHECK(draws==csl_draws && hash==cfs_fnv(fb,FB_W*FB_H));
    /* Recording rows retain fixed notes and load the existing recorder route. */
    sq_defaults();sq_st.pat[0].bars=1;sq_st.pat[0].nev=1;sq_st.pat[0].ev[0].n=1;sq_st.pat[0].ev[0].note[0]=61;
    csl_open();uint8_t rows[1+CSL_CAPACITY+SQ_NPAT];unsigned n=csl_rows(rows);cslu.row=n-1;
    CHECK(csl_selected()==1+CSL_CAPACITY);csl_execute();CHECK(cu.scr==SCR_SQ && sqk.last==0 && sq_st.pat[0].ev[0].note[0]==61);
    printf("saved Song record: %zu bytes; index: %zu bytes; pending transaction: %zu bytes\n",sizeof(csl_record_t),sizeof csl_index,sizeof csl_tx);
}
static void factory_patches(void)
{
    rest();CHECK(csl_read(0));csl_record_t base=csl_io;
    for(unsigned part=0;part<2;part++)for(unsigned i=0;i<cf_snd_count(part);i++) {
        cf_snd_load_index(part,i);blocks(2);csl_io=base;csl_capture_patch(part);csl_seal();
        CHECK(csl_valid(&csl_io));
    }
}
static void lab_save_route(void)
{
    rest();cfl_open();CHECK(cu.scr==SCR_LAB);
    ui_button(B_SAVE);CHECK(cu.scr==SCR_SONGS && cla_mail.request==2);
    blocks(2);CHECK(!cla.on);
}
int main(void){emu_fw_options(0,1,0,1);emu_hal.ready=2;emu_fw_init(0);library();factory_patches();lab_save_route();printf("Songs library: %u checks, %u failures\n",checks,failures);return failures!=0;}
