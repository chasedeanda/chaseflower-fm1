/* SPDX-License-Identifier: GPL-3.0-only
 * Saved ideas use independent A/B pairs. Only the index and one protected
 * transaction live in RAM; no flash work or library lookup runs in audio.
 * V1 uses the existing versioned settings/sound layouts inside its envelope.
 */
#define CSL_CAPACITY 6u
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
} csl_record_t;
_Static_assert(sizeof(csl_record_t)<=ST_PAYLOAD_MAX,"saved Song fits one atomic object");
typedef struct { char name[CF_SONG_NAME+1]; uint8_t used, unavailable, n, beats; uint16_t bpm; } csl_entry_t;
static csl_entry_t csl_index[CSL_CAPACITY];
static csl_record_t csl_io __attribute__((section(".pool"),aligned(4)));
static struct { uint32_t retry; uint8_t pending, error, slot, remove; } csl_tx;
static int csl_valid(const csl_record_t *r)
{
    cf_song_t song;
    if(r->magic!=CSL_MAGIC || r->version!=1 || r->size!=sizeof *r ||
       r->settings_size!=sizeof r->setup || r->sound_size!=sizeof r->patch[0] ||
       r->check!=cfs_fnv(r,__builtin_offsetof(csl_record_t,check)) ||
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
static void csl_index_record(unsigned slot,const csl_record_t *r)
{
    cf_song_t s;cf_song_decode(&s,r->phrase,sizeof r->phrase);
    csl_entry_t *e=&csl_index[slot];memset(e,0,sizeof *e);e->used=1;
    str_cpy(e->name,s.name,sizeof e->name);e->n=s.n;e->bpm=s.bpm;e->beats=s.beats;
}
static void csl_load(void)
{
    memset(csl_index,0,sizeof csl_index);memset(&csl_tx,0,sizeof csl_tx);
    if(cr_safe)return;
    for(unsigned i=0;i<CSL_CAPACITY;i++) {
        st_hdr_t h;int n=st_load(OBJ_CFSONG0+i,&csl_io,sizeof csl_io);
        if(n==sizeof csl_io && csl_valid(&csl_io))csl_index_record(i,&csl_io);
        else if(st_current(OBJ_CFSONG0+i,&h)>=0 && h.len)csl_index[i].unavailable=1;
    }
}
static int csl_read(unsigned slot)
{
    return slot<CSL_CAPACITY && !csl_tx.pending && csl_index[slot].used &&
        st_load(OBJ_CFSONG0+slot,&csl_io,sizeof csl_io)==sizeof csl_io && csl_valid(&csl_io);
}
static int csl_free(void)
{
    for(unsigned i=0;i<CSL_CAPACITY;i++)
        if(!csl_index[i].used && !csl_index[i].unavailable && !(csl_tx.pending && csl_tx.slot==i))return i;
    return -1;
}
static void csl_stage(unsigned slot,int remove)
{ csl_tx.slot=slot;csl_tx.remove=remove;csl_tx.pending=1;csl_tx.error=0;csl_tx.retry=0; }
static void csl_poll(uint32_t now,int quiet)
{
    if(!csl_tx.pending || !quiet || !cfd_audio_quiet() || cf_sn.dm_on || cfs_restore_lock ||
       (csl_tx.retry && (int32_t)(now-csl_tx.retry)<0))return;
    if((!csl_tx.remove && !csl_valid(&csl_io)) ||
       st_save(OBJ_CFSONG0+csl_tx.slot,&csl_io,csl_tx.remove?0:sizeof csl_io)) {
        csl_tx.error=1;csl_tx.retry=now+1000u;return;
    }
    if(csl_tx.remove)memset(&csl_index[csl_tx.slot],0,sizeof csl_index[0]);
    else csl_index_record(csl_tx.slot,&csl_io);
    if(!csl_tx.remove && cfwd.have) {
        uint8_t current[CF_SONG_WIRE];
        if(cf_song_encode(&cfwd.song,current) && !memcmp(current,csl_io.phrase,sizeof current))cfwd.dirty=0;
    }
    csl_tx.pending=csl_tx.error=0;
    cf_message(csl_tx.remove?"Song deleted":"Song saved");
}
