/* SPDX-License-Identifier: GPL-3.0-only
 * Explicit browser/appearance/Click-level edits dirty preferences; PLAY never does.
 * v1/v2 use the 28-byte record. v3 expanded favorites to 255 IDs (44 bytes).
 * v4 packs remembered Click level into the appearance bytes, preserving v3 size.
 * Old records load without a write; object ID, sectors and cf_set_t stay put.
 */
#define CPB_MAGIC 0x31425043u /* CPB1 */
typedef struct {
    uint32_t magic;
    uint8_t version, mode, favorites[16], reserved[2];
    uint32_t check;
} cpb_legacy_rec_t;
typedef struct {
    uint32_t magic;
    uint8_t version, mode, favorites[CPB_BYTES], reserved[2];
    uint32_t check;
} cpb_rec_t;
_Static_assert(sizeof(cpb_legacy_rec_t)==28 && sizeof(cpb_rec_t)==44, "browser record layouts");
static uint8_t cpb_dirty;
static uint32_t cpb_changed, cpb_retry;
static unsigned cpb_click_level(const cpb_rec_t *r)
{ return (r->reserved[0]>>2) | ((r->reserved[1]&2u)<<5); }
static int cpb_legacy_valid(const cpb_legacy_rec_t *r)
{
    return r->magic==CPB_MAGIC && (r->version==1 || (r->version==2 && r->reserved[0]<CF_SKIN_N && r->reserved[1]<=1)) && r->mode<CPB_MODES &&
        r->check==cfs_fnv(r,__builtin_offsetof(cpb_legacy_rec_t,check));
}
static int cpb_valid(const cpb_rec_t *r)
{
    return r->magic==CPB_MAGIC && r->mode<CPB_MODES &&
        ((r->version==3 && r->reserved[0]<CF_SKIN_N && r->reserved[1]<=1) ||
         (r->version==4 && (r->reserved[0]&3u)<CF_SKIN_N && r->reserved[1]<=3u && cpb_click_level(r)>=1u && cpb_click_level(r)<=100u)) &&
        r->check==cfs_fnv(r,__builtin_offsetof(cpb_rec_t,check));
}
static int cpb_record_valid(const void *raw, unsigned len)
{
    if(len==sizeof(cpb_legacy_rec_t)) return cpb_legacy_valid(raw);
    return len==sizeof(cpb_rec_t) && cpb_valid(raw);
}
static void cpb_load(void)
{
    union { cpb_rec_t current; cpb_legacy_rec_t legacy; } r;
    memset(&cpb,0,sizeof cpb); cpb_dirty=0; cpb_retry=0; cf_skin=CF_SKIN_TEAL; cf_motion=1; cf_click_level=cs.metro_level?cs.metro_level:35;
    if(!cr_safe)st_check_blank(OBJ_CFPREFS);
    unsigned len=cr_safe?0:st_load(OBJ_CFPREFS,&r,sizeof r);
    if(cpb_record_valid(&r,len)) {
        if(len==sizeof(cpb_rec_t)) {
            memcpy(cpb.favorites,r.current.favorites,sizeof cpb.favorites); cpb.mode=r.current.mode;
            if(r.current.version==3) { cf_skin=r.current.reserved[0]; cf_motion=r.current.reserved[1]; }
            else { cf_skin=r.current.reserved[0]&3u; cf_motion=r.current.reserved[1]&1u; cf_click_level=cpb_click_level(&r.current); }
        } else {
            memcpy(cpb.favorites,r.legacy.favorites,sizeof r.legacy.favorites); cpb.mode=r.legacy.mode;
            if(r.legacy.version==2) { cf_skin=r.legacy.reserved[0]; cf_motion=r.legacy.reserved[1]; }
        }
    }
    cf_browser_order(&cpb,cpb.mode,fm1_ms);
}
static int cpb_write(uint32_t how)
{
    cpb_rec_t r={0}; r.magic=CPB_MAGIC; r.version=4; r.mode=cpb.mode;
    r.reserved[0]=(uint8_t)(cf_skin | ((cf_click_level&63u)<<2));
    r.reserved[1]=(uint8_t)(cf_motion | ((cf_click_level>>6)<<1));
    memcpy(r.favorites,cpb.favorites,sizeof r.favorites);
    r.check=cfs_fnv(&r,__builtin_offsetof(cpb_rec_t,check));
    int rc=st_put(OBJ_CFPREFS,&r,sizeof r,how);
    if(!rc)cpb_dirty=0;
    return rc;
}
static void cpb_poll(uint32_t now, int quiet)
{
    if(!cpb_dirty || cr_safe || cfs_restore_lock || (cpb_retry && (int32_t)(now-cpb_retry)<0) ||
       now-cpb_changed<CF_SET_QUICK_MS)return;
    if(!quiet && felucca_dbg.last_us*20u>HALF_FRAMES*1000000u/FS*17u)return;
    if(cpb_write(quiet?0u:ST_NOERASE))cpb_retry=now+1000u;
}
static void cpb_flush(void)
{
    if(cpb_dirty && !cr_safe && !cfs_restore_lock)(void)cpb_write(0);
}
