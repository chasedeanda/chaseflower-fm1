/* SPDX-License-Identifier: GPL-3.0-only
 * FX -> editor, FX again -> browser, FX again -> HOME. SELECT browses without
 * changing sound. SAVE opens management; a further one-second hold commits. */
#include "cf_fx_factory.c"
enum { FXP_EDIT, FXP_BROWSE, FXP_MANAGE };
static struct {
    uint8_t mode, pick, slot, cursor, action, previous, prev_bypass;
    char name[FXP_NAME];
    cf_fx_t prev;
} fxpu;
static const fxp_preset_t *fxp_pick(void)
{
    unsigned i = fxpu.pick;
    if (i < FXP_FACTORY_N) return &FXP_FACTORY[i];
    i -= FXP_FACTORY_N;
    return i < FXP_SLOTS && (fxp_bank.used & (1u << i)) ? &fxp_bank.slot[i] : 0;
}
static const char *fxp_pick_name(void)
{
    const fxp_preset_t *p = fxp_pick();
    return p ? p->name : "Empty user slot";
}
static void fxp_recall_selected(void)
{
    const fxp_preset_t *p = fxp_pick();
    if (!p || !fxp_preset_valid(p)) { cf_message("Empty FX slot"); return; }
    fxpu.prev = cs.fx; fxpu.previous = 1; fxpu.prev_bypass = cfx_bypass;
    cs.fx = p->fx; cf_fx_recall(&cs.fx);
    cf_message(p->name);
}
static void fxp_previous(void)
{
    cf_fx_t tmp;
    uint8_t bypass;
    if (!fxpu.previous) { cf_message("No previous FX chain"); return; }
    tmp = cs.fx; bypass = cfx_bypass; cs.fx = fxpu.prev;
    cf_fx_recall(&cs.fx); cf_fx_bypass(fxpu.prev_bypass);
    fxpu.prev = tmp; fxpu.prev_bypass = bypass;
    cf_message("Previous FX chain");
}
static void fxp_manage_open(void)
{
    const fxp_preset_t *p = fxp_pick();
    fxpu.mode = FXP_MANAGE; fxpu.action = 0; fxpu.cursor = 0;
    fxpu.slot = 0;
    while (fxpu.slot + 1 < FXP_SLOTS && (fxp_bank.used & (1u << fxpu.slot))) fxpu.slot++;
    if (fxpu.pick >= FXP_FACTORY_N) fxpu.slot = (uint8_t)(fxpu.pick - FXP_FACTORY_N);
    memset(fxpu.name, 0, sizeof fxpu.name);
    str_cpy(fxpu.name, p ? p->name : "MY FX", sizeof fxpu.name);
    cu.save_hold = 0;
}
static void fxp_manage_do(void)
{
    int ok;
    char name[FXP_NAME];
    if (fxp_protected || cfs_restore_lock || cr_safe) { cf_message("FX bank is protected"); return; }
    if (!fxpu.action && (fxp_bank.used & (1u << fxpu.slot))) { cf_message("New: choose an empty slot"); return; }
    if (fxpu.action && !(fxp_bank.used & (1u << fxpu.slot))) { cf_message("Choose a saved user slot"); return; }
    memset(name, 0, sizeof name);
    memcpy(name, fxpu.name, FXP_NAME - 1);
    for (int i = FXP_NAME - 2; i >= 0 && (name[i] == ' ' || !name[i]); i--) name[i] = 0;
    ok = fxpu.action == 2 ? fxp_delete_slot(fxpu.slot) : fxp_save_slot(fxpu.slot, name);
    if (!ok) { cf_message("FX name must contain text"); return; }
    fxpu.pick = FXP_FACTORY_N + fxpu.slot; fxpu.mode = FXP_BROWSE;
    cf_message("FX queued for storage");
}
static void fxp_knob_info(int k, cf_knob_t *o)
{
    if (fxpu.mode == FXP_BROWSE) {
        static const char *const N[4] = {"RECALL", "BYPASS", "UNDO FX", "EDIT FX"};
        kfmt(o->name, 12, N[k]);
        kfmt(o->val, 14, k == 1 ? (cfx_bypass ? "On" : "Off") : "Turn +");
    } else {
        static const char *const N[4] = {"SLOT", "LETTER", "CHAR", "ACTION"};
        static const char *const A[3] = {"New", "Replace", "Delete"};
        kfmt(o->name, 12, N[k]);
        if (k == 0) kint(o->val, fxpu.slot + 1, "");
        else if (k == 1) kint(o->val, fxpu.cursor + 1, "");
        else if (k == 2) { o->val[0] = fxpu.name[fxpu.cursor] ? fxpu.name[fxpu.cursor] : ' '; o->val[1] = 0; }
        else kfmt(o->val, 14, A[fxpu.action]);
    }
}
static void fxp_knob_turn(int k, int d)
{
    if (!d) return;
    cu.save_hold = 0; /* changing a target or action cancels an in-progress hold */
    if (fxpu.mode == FXP_BROWSE) {
        if (k == 0 && d > 0) fxp_recall_selected();
        if (k == 1) cf_fx_bypass(d > 0);
        if (k == 2 && d > 0) fxp_previous();
        if (k == 3 && d > 0) fxpu.mode = FXP_EDIT;
    } else {
        if (k == 0) fxpu.slot = (uint8_t)clampi(fxpu.slot + d, 0, FXP_SLOTS - 1);
        if (k == 1) fxpu.cursor = (uint8_t)clampi(fxpu.cursor + d, 0, FXP_NAME - 2);
        if (k == 2) {
            unsigned i = 0;
            char c = fxpu.name[fxpu.cursor];
            while (CF_NAME_CHARS[i] && CF_NAME_CHARS[i] != c) i++;
            if (!CF_NAME_CHARS[i]) i = 0;
            for (unsigned j = 0; j < fxpu.cursor; j++) if (!fxpu.name[j]) fxpu.name[j] = ' ';
            fxpu.name[fxpu.cursor] = CF_NAME_CHARS[wrapi((int)i + d, sizeof CF_NAME_CHARS - 1)];
        }
        if (k == 3) fxpu.action = (uint8_t)clampi(fxpu.action + d, 0, 2);
    }
}
