/* SPDX-License-Identifier: GPL-3.0-only
 * Only explicit star/order/appearance edits dirty preferences; SELECT/PLAY never do.
 * v1 migrates to Instrument Teal / Motion On without a write. v2 gives the two
 * reserved bytes meaning without changing record length, object ID or sectors.
 */
#define CPB_MAGIC 0x31425043u /* CPB1 */
typedef struct {
    uint32_t magic;
    uint8_t version, mode, favorites[CPB_BYTES], reserved[2];
    uint32_t check;
} cpb_rec_t;
static uint8_t cpb_dirty;
static uint32_t cpb_changed, cpb_retry;
static unsigned cpb_click_level(const cpb_rec_t *r)
{ return (r->reserved[0]>>2) | ((r->reserved[1]&2u)<<5); }
static int cpb_valid(const cpb_rec_t *r)
{
    return r->magic==CPB_MAGIC && (r->version==1 || (r->version==2 && r->reserved[0]<CF_SKIN_N && r->reserved[1]<=1) ||
        (r->version==3 && (r->reserved[0]&3u)<CF_SKIN_N && r->reserved[1]<=3u && cpb_click_level(r)>=1u && cpb_click_level(r)<=100u)) && r->mode<CPB_MODES &&
        r->check==cfs_fnv(r,__builtin_offsetof(cpb_rec_t,check));
}
static void cpb_load(void)
{
    cpb_rec_t r;
    memset(&cpb,0,sizeof cpb); cpb_dirty=0; cpb_retry=0; cf_skin=CF_SKIN_TEAL; cf_motion=1; cf_click_level=cs.metro_level?cs.metro_level:35;
    if(!cr_safe)st_check_blank(OBJ_CFPREFS);
    if(!cr_safe && st_load(OBJ_CFPREFS,&r,sizeof r)==sizeof r && cpb_valid(&r)) {
        memcpy(cpb.favorites,r.favorites,sizeof cpb.favorites); cpb.mode=r.mode;
        if(r.version==2) { cf_skin=r.reserved[0]; cf_motion=r.reserved[1]; }
        if(r.version==3) { cf_skin=r.reserved[0]&3u; cf_motion=r.reserved[1]&1u; cf_click_level=cpb_click_level(&r); }
    }
    cf_browser_order(&cpb,cpb.mode,fm1_ms);
}
static int cpb_write(uint32_t how)
{
    cpb_rec_t r={0}; r.magic=CPB_MAGIC; r.version=3; r.mode=cpb.mode;
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
