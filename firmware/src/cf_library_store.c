/* SPDX-License-Identifier: GPL-3.0-only
 * Saved ideas use independent A/B pairs. Only the index and one protected
 * transaction live in RAM; no flash work or library lookup runs in audio.
 * V1 uses the existing versioned settings/sound layouts inside its envelope.
 */
#define CFL_CAPACITY 6u
#define CSL_MAGIC 0x314C5343u /* CSL1 */
typedef struct {
    uint32_t magic;
    uint16_t version, size, settings_size, sound_size;
    uint8_t phrase[CF_SONG_WIRE];
    cf_set_t setup;
    cf_harmony_t harmony;
    cf_snd_t sound[2];
    cfk_rec_t patch[2];
    int32_t trim[2];
    uint32_t check;
} cfl_record_t;
_Static_assert(sizeof(cfl_record_t)<=ST_PAYLOAD_MAX,"saved Song fits one atomic object");
typedef struct { char name[CF_SONG_NAME+1]; uint8_t used, unavailable, n, beats; uint16_t bpm; } cfl_entry_t;
static cfl_entry_t cfl_index[CFL_CAPACITY];
static cfl_record_t cfl_io __attribute__((section(".pool"),aligned(4)));
static struct { uint32_t retry; uint8_t pending, error, slot, remove; } cfl_tx;
static int cfl_valid(const cfl_record_t *r)
{
    cf_song_t song;
    if(r->magic!=CSL_MAGIC || r->version!=1 || r->size!=sizeof *r ||
       r->settings_size!=sizeof r->setup || r->sound_size!=sizeof r->patch[0] ||
       r->check!=cfs_fnv(r,__builtin_offsetof(cfl_record_t,check)) ||
       !cf_song_decode(&song,r->phrase,sizeof r->phrase) || !cfd_device_valid(&song))return 0;
    cf_set_t setup=r->setup;cfs_sanitize(&setup);
    cf_harmony_t harmony=r->harmony;cf_harmony_sanitize(&harmony);
    if(memcmp(&setup,&r->setup,sizeof setup) || memcmp(&harmony,&r->harmony,sizeof harmony))return 0;
    for(unsigned p=0;p<2;p++) {
        const cfk_rec_t *v=&r->patch[p];unsigned ok=0;
        for(unsigned i=0;i<CF_NENG;i++)if(v->engine==CF_ENGINES[i])ok=1;
        if(!ok || !cfk_valid(v,p) || !v->edited || v->pat1 || r->sound[p].engine!=v->engine ||
           r->sound[p].name[12] || r->trim[p]<-128 || r->trim[p]>127)return 0;
        for(unsigned i=0;i<P_COUNT;i++)if(!param_kept(i) || i==P_PAN || i==P_MUTE || (i>=P_DIST && i<=P_REV)) {
            const param_desc_t *desc=param_desc_of(v->engine,i);
            if(v->p[i]<desc->min || v->p[i]>desc->max)return 0;
        }
        const eng_deep_t *d=ENGINES[v->engine]->deep;
        if(v->blen!=(d && d->blob_get && d->blob_set?d->blob_size:0))return 0;
    }
    return 1;
}
static void cfl_index_record(unsigned slot,const cfl_record_t *r)
{
    cf_song_t s;cf_song_decode(&s,r->phrase,sizeof r->phrase);
    cfl_entry_t *e=&cfl_index[slot];memset(e,0,sizeof *e);e->used=1;
    str_cpy(e->name,s.name,sizeof e->name);e->n=s.n;e->bpm=s.bpm;e->beats=s.beats;
}
static void cfl_load(void)
{
    memset(cfl_index,0,sizeof cfl_index);memset(&cfl_tx,0,sizeof cfl_tx);
    if(cr_safe)return;
    for(unsigned i=0;i<CFL_CAPACITY;i++) {
        st_hdr_t h;int n=st_load(OBJ_CFSONG0+i,&cfl_io,sizeof cfl_io);
        if(n==sizeof cfl_io && cfl_valid(&cfl_io))cfl_index_record(i,&cfl_io);
        else if(st_current(OBJ_CFSONG0+i,&h)>=0 && h.len)cfl_index[i].unavailable=1;
    }
}
static int cfl_read(unsigned slot)
{
    return slot<CFL_CAPACITY && !cfl_tx.pending && cfl_index[slot].used &&
        st_load(OBJ_CFSONG0+slot,&cfl_io,sizeof cfl_io)==sizeof cfl_io && cfl_valid(&cfl_io);
}
static int cfl_free(void)
{
    for(unsigned i=0;i<CFL_CAPACITY;i++)
        if(!cfl_index[i].used && !cfl_index[i].unavailable && !(cfl_tx.pending && cfl_tx.slot==i))return i;
    return -1;
}
static void cfl_stage(unsigned slot,int remove)
{ cfl_tx.slot=slot;cfl_tx.remove=remove;cfl_tx.pending=1;cfl_tx.error=0;cfl_tx.retry=0; }
static void cfl_poll(uint32_t now,int quiet)
{
    if(!cfl_tx.pending || !quiet || !cfd_audio_quiet() || cf_sn.dm_on || cfs_restore_lock ||
       (cfl_tx.retry && (int32_t)(now-cfl_tx.retry)<0))return;
    if((!cfl_tx.remove && !cfl_valid(&cfl_io)) ||
       st_save(OBJ_CFSONG0+cfl_tx.slot,&cfl_io,cfl_tx.remove?0:sizeof cfl_io)) {
        cfl_tx.error=1;cfl_tx.retry=now+1000u;return;
    }
    if(cfl_tx.remove)memset(&cfl_index[cfl_tx.slot],0,sizeof cfl_index[0]);
    else cfl_index_record(cfl_tx.slot,&cfl_io);
    if(!cfl_tx.remove && cfwd.have) {
        uint8_t current[CF_SONG_WIRE];
        if(cf_song_encode(&cfwd.song,current) && !memcmp(current,cfl_io.phrase,sizeof current))cfwd.dirty=0;
    }
    cfl_tx.pending=cfl_tx.error=0;
    cf_message(cfl_tx.remove?"Song deleted":"Song saved");
}
