/* SPDX-License-Identifier: GPL-3.0-only
 * Only an explicit star/order edit dirties preferences; SELECT/PLAY never do.
 */
#define CPB_MAGIC 0x31425043u /* CPB1 */
typedef struct {
    uint32_t magic;
    uint8_t version, mode, favorites[CPB_BYTES], reserved[2];
    uint32_t check;
} cpb_rec_t;
static uint8_t cpb_dirty;
static uint32_t cpb_changed, cpb_retry;
static int cpb_valid(const cpb_rec_t *r)
{
    return r->magic==CPB_MAGIC && r->version==1 && r->mode<CPB_MODES &&
        r->check==cfs_fnv(r,__builtin_offsetof(cpb_rec_t,check));
}
static void cpb_load(void)
{
    cpb_rec_t r;
    memset(&cpb,0,sizeof cpb); cpb_dirty=0; cpb_retry=0;
    if(!cr_safe)st_check_blank(OBJ_CFPREFS);
    if(!cr_safe && st_load(OBJ_CFPREFS,&r,sizeof r)==sizeof r && cpb_valid(&r)) {
        memcpy(cpb.favorites,r.favorites,sizeof cpb.favorites); cpb.mode=r.mode;
    }
    cf_browser_order(&cpb,cpb.mode,fm1_ms);
}
static int cpb_write(uint32_t how)
{
    cpb_rec_t r={0}; r.magic=CPB_MAGIC; r.version=1; r.mode=cpb.mode;
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
