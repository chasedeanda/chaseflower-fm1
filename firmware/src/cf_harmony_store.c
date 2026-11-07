/* SPDX-License-Identifier: GPL-3.0-only
 * Separate CHH1 record in the unused CoralFlower session pair. The legacy
 * 188-byte settings and all manual forms retain their exact wire layout.
 * Included by cf_settings.c after cfs_fnv; no audio-side flash access. */
#define CFH_MAGIC 0x31484843u /* CHH1 */
typedef struct {
    uint32_t magic;
    uint16_t version, size;
    cf_harmony_t h;
    uint16_t reserved;
    uint32_t check;
} cfh_rec_t;
_Static_assert(sizeof(cfh_rec_t) <= ST_LOG_MAX, "harmony fits a logged record");
static cfh_rec_t cfh_rec;
static uint32_t cfh_saved, cfh_seen, cfh_changed, cfh_retry;
static uint8_t cfh_tidy;
static int cfh_valid(const cfh_rec_t *r)
{
    return r->magic == CFH_MAGIC && r->version == 1 && r->size == sizeof r->h &&
        r->check == cfs_fnv(r, __builtin_offsetof(cfh_rec_t, check));
}
static void cfh_load(void)
{
    cf_harmony_defaults(&ch);
    if (!cr_safe) {
        st_check_blank(OBJ_CFHARM);
        for (int i = 0; i < 3 && st_tidy(OBJ_CFHARM) > 0; i++) fm1_wdt_feed();
        if (st_load(OBJ_CFHARM, &cfh_rec, sizeof cfh_rec) == sizeof cfh_rec && cfh_valid(&cfh_rec)) {
            ch = cfh_rec.h;
            cf_harmony_sanitize(&ch);
        }
    }
    cfh_saved = cfh_seen = cfs_fnv(&ch, sizeof ch);
    cfh_changed = cfh_retry = 0;
    cfh_tidy = 0;
}
static int cfh_write(uint32_t how)
{
    int rc;
    memset(&cfh_rec, 0, sizeof cfh_rec);
    cfh_rec.magic = CFH_MAGIC; cfh_rec.version = 1; cfh_rec.size = sizeof ch;
    cfh_rec.h = ch;
    cfh_rec.check = cfs_fnv(&cfh_rec, __builtin_offsetof(cfh_rec_t, check));
    rc = st_put(OBJ_CFHARM, &cfh_rec, sizeof cfh_rec, how);
    if (!rc) { cfh_saved = cfs_fnv(&ch, sizeof ch); cfh_tidy = 0; }
    return rc;
}
static void cfh_poll(uint32_t now, int quiet)
{
    uint32_t h = cfs_fnv(&ch, sizeof ch);
    if (h != cfh_seen) { cfh_seen = h; cfh_changed = now; }
    if (cfh_retry && (int32_t)(now - cfh_retry) < 0) return;
    cfh_retry = 0;
    quiet = quiet && now - cfh_changed >= CF_SET_QUIET_MS;
    if (h == cfh_saved) {
        if (!cfh_tidy && quiet) cfh_tidy = st_tidy(OBJ_CFHARM) == 0;
        return;
    }
    if (now - cfh_changed < CF_SET_QUICK_MS) return;
    if (!quiet && felucca_dbg.last_us * 20u > HALF_FRAMES * 1000000u / FS * 17u) return;
    if (cfh_write(quiet ? 0u : ST_NOERASE)) cfh_retry = now + 1000u;
}
static void cfh_flush(void)
{
    if (!cr_safe && !cfs_restore_lock && cfs_fnv(&ch, sizeof ch) != cfh_saved) (void)cfh_write(0);
}
