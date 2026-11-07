/* SPDX-License-Identifier: GPL-3.0-only
 * Protected working draft. A checkpoint is not a saved Song or a recording.
 * Included after cfs_sounding; all storage happens in the foreground at rest.
 */
_Static_assert(CF_SONG_WIRE<=ST_LMAX(OBJ_CFDRAFT),"draft fits logged entry");
enum { CFD_RAM,CFD_RECOVERED,CFD_CHECKPOINTED };
static cf_song_draft_t cfwd;
static struct { uint32_t changed,retry;uint8_t pending,error,unavailable,checkpoint; } cfds;
static cf_progression_t cfd_view;
static cf_source_chord_t cfd_source[CF_SONG_MAX];
static void cfd_view_update(void)
{
    if(!cfwd.have){cf_progression_draft_register(0,0);return;}
    memset(&cfd_view,0,sizeof cfd_view);memset(cfd_source,0,sizeof cfd_source);
    cfd_view.name=cfwd.song.name;cfd_view.context="Working draft";cfd_view.id=0;
    int active=cf_sn.ac_on && cf_sn.ac_idea==CF_P_DRAFT && cf_sn.ac_n && cf_sn.ac_n<=CF_SONG_MAX;
    cfd_view.n=active?cf_sn.ac_n:cfwd.song.n;cfd_view.beats=active?cf_sn.ac_beats:cfwd.song.beats;
    cfd_view.tonic=active?cf_sn.ac_tonic:cfwd.song.tonic;cfd_view.scale=active?cf_sn.ac_scale:cfwd.song.scale;
    for(unsigned i=0;i<cfd_view.n;i++) {
        cfd_view.degree[i]=active?cf_sn.ac_degree[i]:cfwd.song.slot[i].degree;
        cf_source_chord_t *v=&cfd_source[i];v->degree=cfd_view.degree[i];
        v->mask=active?cf_sn.ac_mask[i]:cf_song_mask(&cfwd.song,i,cfwd.song.scale);
        if(active) {
            int alter=((int)cf_sn.ac_root[i]-cfd_view.tonic-cf_scale_step(cfd_view.scale,v->degree))%12;
            if(alter<0)alter+=12;v->alter=(int8_t)(alter>6?alter-12:alter);
            v->bass=(int8_t)((cf_sn.ac_bass_pc[i]-cf_sn.ac_root[i]+132)%12);
        } else {v->alter=cfwd.song.slot[i].alter;v->bass=cfwd.song.slot[i].bass;}
    }
    cf_progression_draft_register(&cfd_view,cfd_source);
}
static int cfd_device_valid(const cf_song_t *s)
{
    if(!cf_song_valid(s) || s->kit>=SQ_NKIT)return 0;
    if(s->rhythm==65535 || s->rhythm==65534)return 1;
    for(unsigned i=0;i<DM_N;i++)if(DM_RHY[i].id==s->rhythm)return DM_RHY[i].beats==s->beats;
    return 0;
}
static void cfd_load(void)
{
    memset(&cfwd,0,sizeof cfwd);memset(&cfds,0,sizeof cfds);
    if(!cr_safe) {
        uint8_t bytes[CF_SONG_WIRE];st_hdr_t h;
        if(st_current(OBJ_CFDRAFT,&h)>=0 && h.len) {
            if(st_load(OBJ_CFDRAFT,bytes,sizeof bytes)==sizeof bytes && cf_song_decode(&cfwd.song,bytes,sizeof bytes) && cfd_device_valid(&cfwd.song))
                {cfwd.have=cfwd.dirty=1;cfds.checkpoint=CFD_RECOVERED;}
            else cfds.unavailable=1; /* preserve unknown/corrupt checkpoint until explicit recovery */
        }
    }
    cfd_view_update();
}
static void cfd_changed(void)
{ cfds.pending=1;cfds.changed=cu.now;cfds.error=cfds.checkpoint=CFD_RAM;cfd_view_update(); }
static int cfd_audio_quiet(void)
{
    return !cr_safe && !cpa.on && !cpa_commanded && !cf_sn.ac_on &&
        !cf_sn.sq_on && sqp.rec==255 && !sqp.on && cf_evq_r==cf_evq_w && !cu.keys && !cfs_sounding();
}
static int cfd_quiet(void)
{ return !cfs_restore_lock && !cfds.unavailable && cfd_audio_quiet(); }
static int cfd_checkpoint(void)
{
    if(!cfwd.have || !cfds.pending)return 1;
    if(!cfd_quiet())return 0;
    uint8_t bytes[CF_SONG_WIRE];
    if(!cfd_device_valid(&cfwd.song) || !cf_song_encode(&cfwd.song,bytes) || st_put(OBJ_CFDRAFT,bytes,sizeof bytes,0)) {
        cfds.error=1;cfds.retry=cu.now+1000u;return 0;
    }
    cfds.pending=cfds.error=0;cfds.checkpoint=CFD_CHECKPOINTED;return 1;
}
static void cfd_poll(uint32_t now,int quiet)
{
    if(!quiet || !cfds.pending || now-cfds.changed<CF_SET_QUIET_MS || (cfds.retry && (int32_t)(now-cfds.retry)<0))return;
    (void)cfd_checkpoint();
}
