/* SPDX-License-Identifier: GPL-3.0-only
 * Pointer-free semantic phrase, independent of UI, flash and live Harmony state.
 */
#ifndef CF_SONG_H
#define CF_SONG_H
#include "cf_progression.h"
#define CF_SONG_MAX 16u
#define CF_SONG_NAME 24u
#define CF_SONG_WIRE 344u
enum { CF_SONG_FOLLOW, CF_SONG_MAJOR, CF_SONG_MINOR, CF_SONG_DIM, CF_SONG_AUG, CF_SONG_POWER, CF_SONG_NQUALITY };
enum { CF_SONG_BARS, CF_SONG_QUARTERS };
typedef struct {
    uint32_t mask;                 /* 0 follows mode; nonzero preserves an exact source chord */
    uint8_t degree;
    int8_t alter, inversion, reg;  /* chromatic root; -1 smooth inversion; chord octaves */
    uint8_t bars;
    int8_t bass, bass_oct;         /* -1 root, otherwise explicit root-relative pitch class */
    uint8_t bass_off, quality, color, gate, tie, unit, quarters;
} cf_song_slot_t;
typedef struct {
    char name[CF_SONG_NAME+1];
    uint16_t source_id, bpm, rhythm; /* stable source/rhythm IDs; rhythm 65535=Off, 65534=Click */
    uint8_t tonic, scale, beats, n, kit, swing, drums_only, repeat_off;
    cf_song_slot_t slot[CF_SONG_MAX];
} cf_song_t;
typedef struct {
    uint32_t mask;
    uint16_t ticks;
    uint8_t root, n, note[6], gate, tie, bass_off;
    int8_t bass_pc, bass_oct;
} cf_song_chord_t;
typedef struct {
    cf_song_t song, previous;
    uint8_t have, dirty, can_undo;
} cf_song_draft_t;
int cf_song_valid(const cf_song_t *s);
int cf_song_clone(cf_song_t *s,const cf_progression_t *p);
uint32_t cf_song_mask(const cf_song_t *s,unsigned slot,int scale);
int cf_song_resolve(const cf_song_t *s,unsigned slot,const cf_chord_cfg_t *cfg,
                    const uint8_t *previous,unsigned nprevious,cf_song_chord_t *out);
int cf_song_insert(cf_song_t *s,unsigned at,int duplicate);
int cf_song_remove(cf_song_t *s,unsigned at);
/* Dirty drafts cannot be silently replaced. Commit is one complete undo step. */
int cf_song_draft_begin(cf_song_draft_t *d,const cf_song_t *s);
int cf_song_draft_commit(cf_song_draft_t *d,const cf_song_t *s);
int cf_song_draft_undo(cf_song_draft_t *d);
/* Fixed little-endian v1 record. Decode validates into a temporary, then commits. */
int cf_song_encode(const cf_song_t *s,uint8_t out[CF_SONG_WIRE]);
int cf_song_decode(cf_song_t *s,const uint8_t *bytes,unsigned size);
#endif
