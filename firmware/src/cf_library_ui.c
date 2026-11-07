/* SPDX-License-Identifier: GPL-3.0-only
 * One foreground list: saved semantic ideas and existing fixed-note takes.
 */
enum { CFL_LIST, CFL_MANAGE, CFL_CONFIRM, CFL_RENAME };
enum { CFL_LOAD, CFL_DUPLICATE, CFL_RENAME_ACTION, CFL_DELETE, CFL_NACTION };
static const char *const CFL_ACTION[]={"Load","Duplicate","Rename","Delete"};
static struct { uint8_t row, mode, action, choice, cursor;char name[CF_SONG_NAME+1]; } cflu;
/* 0 = Save current; 1..6 = persistent ideas; 7..9 = recorder slots. */
static unsigned cfl_rows(uint8_t rows[1+CFL_CAPACITY+SQ_NPAT])
{
    unsigned n=0;rows[n++]=0;
    for(unsigned i=0;i<CFL_CAPACITY;i++)if(cfl_index[i].used || cfl_index[i].unavailable || (cfl_tx.pending && cfl_tx.slot==i))rows[n++]=1+i;
    for(unsigned i=0;i<SQ_NPAT;i++)if(sq_has(i) || (sq_unavailable&(1u<<i)))rows[n++]=1+CFL_CAPACITY+i;
    return n;
}
static unsigned cfl_selected(void)
{ uint8_t rows[1+CFL_CAPACITY+SQ_NPAT];unsigned n=cfl_rows(rows);return rows[cflu.row%n]; }
static void cfl_open(void)
{ cflu.row=0;cflu.mode=CFL_LIST;cflu.action=0;cu.scr=SCR_SONGS;cu.pop=POP_NONE; }
static void cfl_select(int d)
{
    if(cflu.mode!=CFL_LIST)return;
    uint8_t rows[1+CFL_CAPACITY+SQ_NPAT];int n=cfl_rows(rows);
    cflu.row=(uint8_t)((cflu.row+d%n+n)%n);
}
static int cfl_available(void)
{
    if(cr_safe || cfs_restore_lock){cf_message("Songs unavailable: restart");return 0;}
    if(cfl_tx.pending){cf_message(cfl_tx.error?"Write failed: retry pending":"Song pending: stop/release to save");return 0;}
    if(sqk.on || cf_sn.sq_rec!=255){cf_message("Finish recording first");return 0;}
    return 1;
}
static void cfl_seal(void)
{ cfl_io.check=cfs_fnv(&cfl_io,__builtin_offsetof(cfl_record_t,check)); }
static void cfl_capture_patch(unsigned part)
{
    cfk_rec_t *r=&cfl_io.patch[part];const track_t *t=&trk[part];const eng_deep_t *d=cfk_deep(t);
    memset(r,0,sizeof *r);r->magic=CFK_MAGIC;r->part=part;r->edited=1;r->np=P_COUNT;
    r->engine=cf_snd[part].engine;r->kind=cf_snd[part].kind;r->fk=cf_snd[part].fk;r->slot=cf_snd[part].slot;
    memcpy(r->name,cf_snd[part].name,sizeof r->name);
    for(unsigned i=0;i<P_COUNT;i++)r->p[i]=t->p[i];
    if(d){r->blen=d->blob_size;d->blob_get(t,r->blob);}
    r->check=cfs_fnv(r,__builtin_offsetof(cfk_rec_t,check));
    cfl_io.sound[part]=cf_snd[part];cfl_io.trim[part]=t->trim;
}
static void cfl_save_current(void)
{
    if(!cfl_available())return;
    int slot=cfl_free();if(slot<0){cf_message("Songs full: delete a saved idea");return;}
    if(cpa_setup.flags || cpa_edit.flags){cf_message("Wait for queued setup to play");return;}
    for(unsigned p=0;p<2;p++)if(trk[p].xf_on || trk[p].eng_req!=trk[p].engine){cf_message("Wait for sound change to finish");return;}
    cfd_ui_poll();cf_song_t song;
    if(cpg.idea==CF_P_DRAFT && cfwd.have)song=cfwd.song;
    else if(!cf_song_clone(&song,cf_progression_current())){cf_message("No progression to save");return;}
    song.tonic=cs.chord.tonic;song.scale=cs.chord.scale;song.bpm=cs.bpm;
    song.rhythm=cpg.rhythm<DM_N?DM_RHY[cpg.rhythm].id:cpg.rhythm==CPA_CLICK?65534:65535;
    if(cpg.rhythm<DM_N){song.kit=dm_kit(cpg.rhythm);song.swing=dm_swing(cpg.rhythm);}
    song.drums_only=cpg.mode;song.repeat_off=cpg.repeat_off;
    memset(&cfl_io,0,sizeof cfl_io);cfl_io.magic=CSL_MAGIC;cfl_io.version=1;cfl_io.size=sizeof cfl_io;
    cfl_io.settings_size=sizeof cfl_io.setup;cfl_io.sound_size=sizeof cfl_io.patch[0];
    if(!cf_song_encode(&song,cfl_io.phrase)){cf_message("Song setup cannot be saved");return;}
    cfl_io.setup=cs;cfl_io.setup.arp_keep=cu.arp_on;cfl_io.harmony=ch;for(unsigned p=0;p<2;p++)cfl_capture_patch(p);cfl_seal();
    if(!cfl_valid(&cfl_io)){cf_message("Song setup cannot be saved");return;}
    cfl_stage(slot,0);cflu.mode=CFL_LIST;
    uint8_t rows[1+CFL_CAPACITY+SQ_NPAT];unsigned n=cfl_rows(rows);for(unsigned i=0;i<n;i++)if(rows[i]==1u+(unsigned)slot)cflu.row=i;
    cf_message("Pending: stop/release to save");
}
static int cfl_recall(unsigned slot)
{
    if(!cfl_available())return 0;
    if(!cfd_audio_quiet() || cf_sn.dm_on){cf_message("Stop/release notes to load");return 0;}
    if(cfwd.have && cfwd.dirty){cf_message("Save working draft before loading");return 0;}
    if(!cfl_read(slot)){cf_message("Song unavailable: restore backup");return 0;}
    cf_song_t song;cf_song_decode(&song,cfl_io.phrase,sizeof cfl_io.phrase);
    /* Restore musical settings explicitly; system, USB and appearance stay local. */
    const cf_set_t *s=&cfl_io.setup;
    cs.chord=s->chord;cs.glide_ms=s->glide_ms;cs.glide_always=s->glide_always;cs.vel=s->vel;
    cs.bass_on=s->bass_on;cs.octaver=s->octaver;cs.bass_oct=s->bass_oct;cs.bass_level=s->bass_level;
    cs.level_chord=s->level_chord;cs.level_layer=s->level_layer;cs.level_drums=s->level_drums;
    cs.metro_level=s->metro_level;cs.click=s->click;cs.env_link=s->env_link;cs.retrig=s->retrig;cs.arp=s->arp;cs.fx=s->fx;
    ch=cfl_io.harmony;cu.arp_on=s->arp_keep;
    for(unsigned p=0;p<2;p++) {
        const cfk_rec_t *r=&cfl_io.patch[p];cf_pool_load(p,r->engine,0);cfk_own[p]=0;
        /* A recalled patch is detached from mutable user preset slots. Saving
         * this sound later must create a sound, not overwrite its old source. */
        fm1_irq_off();for(unsigned i=0;i<P_COUNT;i++)if(!param_kept(i) || i==P_PAN || i==P_MUTE || (i>=P_DIST && i<=P_REV))trk[p].p[i]=r->p[i];trk[p].trim=cfl_io.trim[p];fm1_irq_on();
        const eng_deep_t *d=cfk_deep(&trk[p]);if(d)d->blob_set(&trk[p],r->blob);
        cf_edited(p);cs.snd[p]=cf_snd_pos(p);
    }
    cf_song_draft_begin(&cfwd,&song);memset(&cfds,0,sizeof cfds);cfd_changed();cfdu.focus=0;
    cfd_view_update();cpg.idea=CF_P_DRAFT;cs.chord.tonic=song.tonic;cs.chord.scale=song.scale;
    cfd_select_source();cf_send_config();cu_spell_update();cu.scr=SCR_HOME;home_boot_main=1;cu.pop=POP_NONE;return 1;
}
static void cfl_execute(void)
{
    unsigned row=cfl_selected();
    if(!row){cfl_save_current();return;}
    if(row>CFL_CAPACITY) {
        unsigned pat=row-1-CFL_CAPACITY;
        if(cflu.mode==CFL_CONFIRM) {
            if(cflu.choice && (!cfl_available() || !cfd_audio_quiet())){cf_message("Stop/release notes to delete");return;}
            if(cflu.choice)sq_clear_take(pat);
            cflu.mode=CFL_LIST;return;
        }
        if(cflu.mode==CFL_MANAGE && cflu.action==CFL_DELETE){cflu.mode=CFL_CONFIRM;cflu.choice=0;return;}
        if(!cfl_available() || !cfd_audio_quiet()){cf_message("Stop/release notes to load");return;}
        if(!sq_has(pat)){cf_message("Recording unavailable");return;}
        sqk.last=pat;sqk.song=0;squ.pat=pat;sq_activate(pat);cu.scr=SCR_SQ;cu.pop=POP_NONE;return;
    }
    unsigned slot=row-1;
    if(cflu.mode==CFL_LIST){(void)cfl_recall(slot);return;}
    if(!cfl_available())return;
    if(cflu.mode==CFL_CONFIRM) {
        if(cflu.choice)cfl_stage(slot,1);cflu.mode=CFL_LIST;return;
    }
    if(cflu.mode==CFL_RENAME) {
        if(!cfl_read(slot))return;
        cf_song_t song;cf_song_decode(&song,cfl_io.phrase,sizeof cfl_io.phrase);
        unsigned end=CF_SONG_NAME;while(end && cflu.name[end-1]==' ')end--;cflu.name[end]=0;
        if(!end){cf_message("Enter a Song name");return;}
        memset(song.name,0,sizeof song.name);memcpy(song.name,cflu.name,end);
        cf_song_encode(&song,cfl_io.phrase);cfl_seal();cfl_stage(slot,0);cflu.mode=CFL_LIST;return;
    }
    if(cflu.action==CFL_LOAD){(void)cfl_recall(slot);return;}
    if(cflu.action==CFL_DELETE){cflu.mode=CFL_CONFIRM;cflu.choice=0;return;}
    if(cflu.action==CFL_DUPLICATE) {
        int to=cfl_free();if(to<0){cf_message("Songs full: delete a saved idea");return;}
        if(cfl_read(slot)){cfl_stage(to,0);cflu.mode=CFL_LIST;}return;
    }
    if(cflu.action==CFL_RENAME_ACTION && cfl_index[slot].used) {
        memset(cflu.name,' ',CF_SONG_NAME);cflu.name[CF_SONG_NAME]=0;
        memcpy(cflu.name,cfl_index[slot].name,str_len(cfl_index[slot].name));cflu.cursor=0;cflu.mode=CFL_RENAME;
    }
}
static void cfl_turn(int k,int d)
{
    if(cflu.mode==CFL_LIST){if(k==0)cfl_select(d);return;}
    if(cflu.mode==CFL_CONFIRM){if(!k)cflu.choice=d>0;return;}
    if(cflu.mode==CFL_RENAME){if(!k)cflu.cursor=wrapi(cflu.cursor+d%CF_SONG_NAME,CF_SONG_NAME);if(k==1)cflu.name[cflu.cursor]=(char)(32+wrapi(cflu.name[cflu.cursor]-32+d%95,95));return;}
    if(!k){if(cfl_selected()>CFL_CAPACITY)cflu.action=cflu.action==CFL_DELETE?CFL_LOAD:CFL_DELETE;else cflu.action=wrapi(cflu.action+d%CFL_NACTION,CFL_NACTION);}
}
static void cfl_edit(void)
{ if(!cfl_selected())return;if(cflu.mode!=CFL_LIST){cflu.mode=CFL_LIST;return;}cflu.mode=CFL_MANAGE;cflu.action=CFL_LOAD; }
