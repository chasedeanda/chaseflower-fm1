/* SPDX-License-Identifier: GPL-3.0-only
 * Byte-only FX payload; no synth, harmony, master volume or transport fields.
 * FM1 and the host HAL are little endian; fixed sizes/offsets lock the v1 wire layout. */
#include "cf_fx_preset.h"
#include <stddef.h>
#if !defined(CF_UNIT) && !defined(CF_EMU)
#include <string.h>
#endif
_Static_assert(sizeof(cf_fx_t) == 25, "FX v1 chain layout");
_Static_assert(sizeof(fxp_preset_t) == 46, "FX v1 preset layout");
_Static_assert(offsetof(fxp_bank_t, check) == 748 && sizeof(fxp_bank_t) == 752, "FX v1 bank layout");
static uint32_t fxp_hash(const void *p, unsigned n)
{
    const uint8_t *s = p; uint32_t h = 2166136261u;
    while (n--) h = (h ^ *s++) * 16777619u;
    return h;
}
int fxp_chain_valid(const cf_fx_t *f)
{
    unsigned seen = 0;
    if (f->on & ~((1u << CFX_N) - 1u)) return 0;
    for (unsigned e = 0; e < CFX_N; e++) {
        if (f->order[e] >= CFX_N || (seen & (1u << f->order[e]))) return 0;
        seen |= 1u << f->order[e];
        for (unsigned k = 0; k < 3; k++) {
            unsigned max = !k && (e == CFX_FILTER || e == CFX_REVERB) ? 2u :
                           e == CFX_FILTER && k == 1 ? 99u : 100u;
            if (f->v[e][k] > max) return 0;
        }
    }
    return 1;
}
int fxp_preset_valid(const fxp_preset_t *p)
{
    unsigned i, text = 0;
    for (i = 0; i < FXP_NAME && p->name[i]; i++) {
        unsigned c = (uint8_t)p->name[i];
        if (c < 32 || c > 126) return 0;
        text |= c != ' ';
    }
    if (i == FXP_NAME || !text) return 0;
    for (; i < FXP_NAME; i++) if (p->name[i]) return 0;
    return fxp_chain_valid(&p->fx);
}
void fxp_bank_seal(fxp_bank_t *b)
{
    b->magic = FXP_MAGIC; b->version = FXP_VERSION; b->size = sizeof *b; b->reserved = 0;
    b->check = fxp_hash(b, offsetof(fxp_bank_t, check));
}
void fxp_bank_init(fxp_bank_t *b) { memset(b, 0, sizeof *b); fxp_bank_seal(b); }
int fxp_bank_valid(const fxp_bank_t *b)
{
    if (b->magic != FXP_MAGIC || b->version != FXP_VERSION || b->size != sizeof *b || b->reserved ||
        b->check != fxp_hash(b, offsetof(fxp_bank_t, check))) return 0;
    for (unsigned i = 0; i < FXP_SLOTS; i++) {
        if (b->used & (1u << i)) { if (!fxp_preset_valid(&b->slot[i])) return 0; }
        else {
            const uint8_t *p = (const uint8_t *)&b->slot[i];
            for (unsigned j = 0; j < sizeof b->slot[i]; j++) if (p[j]) return 0;
        }
    }
    return 1;
}
int fxp_bank_save(fxp_bank_t *b, unsigned slot, const fxp_preset_t *p)
{
    if (slot >= FXP_SLOTS || !fxp_preset_valid(p)) return 0;
    b->slot[slot] = *p; b->used |= (uint16_t)(1u << slot); fxp_bank_seal(b); return 1;
}
int fxp_bank_delete(fxp_bank_t *b, unsigned slot)
{
    if (slot >= FXP_SLOTS || !(b->used & (1u << slot))) return 0;
    memset(&b->slot[slot], 0, sizeof b->slot[slot]);
    b->used &= (uint16_t)~(1u << slot); fxp_bank_seal(b); return 1;
}
