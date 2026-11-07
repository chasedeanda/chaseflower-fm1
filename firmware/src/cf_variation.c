/* SPDX-License-Identifier: GPL-3.0-only
 * One-page contextual variation. Main-loop only; the existing player, FX fade,
 * preset save workflows and settings persistence retain ownership of their data.
 * No new flash records, playback scheduler or audio-callback generation. */
enum { CV_HARMONY, CV_ARP, CV_DRUMS, CV_FX, CV_SOUND, CV_SCALE, CV_N };
static const char *const CV_NAMES[CV_N] = {"Harmony", "Arp", "Drums", "FX", "Sound", "Scale"};
static const char *const CV_LEVELS[3] = {"Subtle", "Musical", "Chaos"};
typedef struct {
    cf_fx_t fx;
    cf_arp_t arp;
    cf_snd_t sound;
    uint16_t bpm;
    uint8_t tonic, scale, quantize, tag, idea, beats, rhythm, kit, swing, auto_on;
    uint8_t part, env_kind, carrier, edited, env[30]; /* kind 0 ADSR, 1 FM, 2 VA */
} cv_value_t;
static struct {
    cv_value_t previous;
    dm_rec_t previous_drums;
    uint32_t seed;
    uint8_t context, level, part, have_previous, previous_context, previous_view, previous_beats;
} cvr;
static const uint8_t CV_FM_BYTE[5] = {FP_R1, FP_R1 + 1, FP_R1 + 2, FP_L1 + 2, FP_R1 + 3};
static const uint8_t CV_ENV_PARAM[4] = {P_ATK, P_DEC, P_SUS, P_REL};
static void cvr_init(void) { memset(&cvr, 0, sizeof cvr); cvr.seed = 1; }
static int cvr_value_valid(const cv_value_t *v)
{
    if (!fxp_chain_valid(&v->fx) || v->tonic > 11 || v->scale >= CF_NSCALES || v->quantize > CF_Q_FULL ||
        v->tag >= CF_P_NTAGS || (v->idea != CF_P_NONE && v->idea >= cf_progression_count()) ||
        v->beats < 2 || v->beats > 7 || v->rhythm >= DM_N || DM_RHY[v->rhythm].beats != v->beats ||
        v->kit >= SQ_NKIT || v->swing > 100 || v->bpm < 30 || v->bpm > 300 ||
        v->auto_on > 1 || v->part > 2 || v->env_kind > 2 || v->carrier > 63 || v->edited > 1 ||
        v->sound.engine >= NENGINES || v->sound.name[12]) return 0;
    if (v->arp.mode >= CF_ARP_NMODES || v->arp.oct < 1 || v->arp.oct > 4 || v->arp.rate >= CF_ARP_NRATES ||
        v->arp.gate < 20 || v->arp.gate > 100 || v->arp.swing < 50 || v->arp.swing > 75 ||
        v->arp.latch > 1 || v->arp.strum_ms < 5 || v->arp.strum_ms > 250) return 0;
    for (unsigned i = 0; i < 30; i++) if (v->env[i] > (v->env_kind == 1 ? 99 : 127)) return 0;
    return 1;
}
static void cvr_capture(cv_value_t *v, unsigned part)
{
    unsigned r = dm_cur();
    memset(v, 0, sizeof *v);
    v->fx = cs.fx; v->arp = cs.arp; v->bpm = cs.bpm;
    v->tonic = cs.chord.tonic; v->scale = cs.chord.scale; v->quantize = cs.chord.quantize;
    v->tag = cpg.tag; v->idea = cpg.idea; v->auto_on = ch.mode != 0;
    v->beats = DM_RHY[r].beats; v->rhythm = (uint8_t)r; v->kit = (uint8_t)dm_kit(r); v->swing = (uint8_t)dm_swing(r);
    v->part = (uint8_t)part; v->sound = cf_snd[part]; v->edited = (uint8_t)cfk_edited(part);
    if (cfe_fm6(part)) {
        const uint8_t *p = fm6_patch[part]; v->env_kind = 1;
        for (unsigned op = 0; op < 6; op++) {
            if (fm6_carrier(p[FP_ALG] & 31u, 6u - op)) v->carrier |= 1u << op;
            for (unsigned k = 0; k < 5; k++) v->env[op * 5 + k] = p[op * FP_OP + CV_FM_BYTE[k]];
        }
    } else {
        v->env_kind = cfe_va(part) && cfe_deep(part) ? 2 : 0;
        for (unsigned k = 0; k < 4; k++)
            v->env[k] = (uint8_t)(v->env_kind == 2 ? cfe_deep(part)->get(&trk[part], 10, k) : trk[part].p[CV_ENV_PARAM[k]]);
    }
}
/* Local PRNG: does not consume the arp/player random stream. Same input,
 * context, level and seed always produce identical reusable settings. */
static uint32_t cvr_random(uint32_t *s) { *s = *s * 1664525u + 1013904223u; return *s >> 8; }
static uint8_t cvr_vary(uint32_t *s, int x, int radius, int lo, int hi)
{
    return (uint8_t)clampi(x + (int)(cvr_random(s) % (unsigned)(radius * 2 + 1)) - radius, lo, hi);
}
static int cvr_generate(cv_value_t *v, unsigned context, unsigned level, uint32_t seed)
{
    uint32_t rng = seed;
    if (context >= CV_N || level > 2) return 0;
    switch (context) {
    case CV_HARMONY: {
        if (level == 2) v->tag = CF_P_ALL;
        unsigned choices[64], n = 0;
        for (unsigned i = 0; i < cf_progression_count() && n < 64; i++)
            if (cf_progression_matches(i, v->scale, v->tag)) choices[n++] = i;
        if (!n) return 0;
        if (!level) v->idea = cf_progression_pick(v->scale, v->tag, v->idea, cvr_random(&rng) & 1u ? 1 : -1);
        else v->idea = (uint8_t)choices[cvr_random(&rng) % n];
        break;
    }
    case CV_ARP:
        if (!level) {
            v->arp.gate = cvr_vary(&rng, v->arp.gate, 8, 20, 100);
            v->arp.swing = cvr_vary(&rng, v->arp.swing, 3, 50, 75);
        } else {
            static const uint8_t modes[] = {CF_ARP_UP, CF_ARP_DOWN, CF_ARP_INC, CF_ARP_EXC, CF_ARP_ORDER, CF_ARP_REPEAT};
            v->arp.mode = level == 1 ? modes[cvr_random(&rng) % sizeof modes] : (uint8_t)(cvr_random(&rng) % CF_ARP_NMODES);
            v->arp.rate = (uint8_t)((level == 1 ? 2 : 0) + cvr_random(&rng) % (level == 1 ? 5 : CF_ARP_NRATES));
            v->arp.oct = (uint8_t)(1 + cvr_random(&rng) % (level == 1 ? 2 : 4));
            v->arp.gate = (uint8_t)(40 + cvr_random(&rng) % 61);
            v->arp.swing = (uint8_t)(50 + cvr_random(&rng) % (level == 1 ? 13 : 26));
            v->arp.strum_ms = (uint8_t)(5 + cvr_random(&rng) % (level == 1 ? 76 : 246));
        } /* never toggle ON or LATCH: no surprise self-sustaining playback */
        break;
    case CV_DRUMS:
        if (level) {
            unsigned n = dm_count(v->beats);
            if (!n) return 0;
            v->rhythm = (uint8_t)dm_nth(v->beats, cvr_random(&rng) % n);
            v->kit = level == 1 ? DM_RHY[v->rhythm].kit : (uint8_t)(cvr_random(&rng) % SQ_NKIT);
            v->swing = DM_RHY[v->rhythm].swing;
        }
        v->swing = cvr_vary(&rng, v->swing, level == 2 ? 20 : 5, 0, 100);
        break;
    case CV_FX:
        for (unsigned e = 0; e < CFX_N; e++) {
            for (unsigned k = 0; k < 3; k++) {
                unsigned max = !k && (e == CFX_FILTER || e == CFX_REVERB) ? 2 : e == CFX_FILTER && k == 1 ? 99 : 100;
                if (!level && max == 2) continue;
                v->fx.v[e][k] = level == 2 ? (uint8_t)(cvr_random(&rng) % (max + 1)) : cvr_vary(&rng, v->fx.v[e][k], level ? 18 : 5, 0, (int)max);
                /* Bound energy/feedback even in Chaos. */
                if (e == CFX_DELAY && k == 1 && v->fx.v[e][k] > 75) v->fx.v[e][k] = 75;
                if (e == CFX_REVERB && k == 1 && v->fx.v[e][k] > 80) v->fx.v[e][k] = 80;
                if (e == CFX_DIST && k == 0 && v->fx.v[e][k] > 65) v->fx.v[e][k] = 65;
            }
            if (level && cvr_random(&rng) % 3 == 0) v->fx.on ^= 1u << e;
        }
        if (level == 2) {
            for (unsigned i = CFX_N - 1; i; i--) {
                unsigned j = cvr_random(&rng) % (i + 1), a = v->fx.order[i], b = v->fx.order[j];
                v->fx.order[i] = (uint8_t)b; v->fx.order[j] = (uint8_t)a;
            }
        }
        break;
    case CV_SOUND:
        for (unsigned i = 0; i < (v->env_kind == 1 ? 30u : 4u); i++) {
            if (v->env_kind == 1 && !(v->carrier & (1u << (i / 5)))) continue;
            v->env[i] = cvr_vary(&rng, v->env[i], level == 0 ? 4 : level == 1 ? 15 : 40, 0, v->env_kind == 1 ? 99 : 127);
        }
        v->edited = 1;
        break;
    case CV_SCALE: {
        unsigned choices[CF_NSCALES], n = 0;
        if (v->quantize == CF_Q_OFF) return 0;
        for (unsigned i = 0; i < CF_NSCALES; i++) {
            if ((!level || v->auto_on) && !cf_harmony_supported(i, v->quantize)) continue;
            choices[n++] = i;
        }
        if (!n) return 0;
        v->scale = (uint8_t)choices[cvr_random(&rng) % n];
        v->idea = cf_progression_pick(v->scale, v->tag, v->idea, 0);
        break;
    }
    }
    return cvr_value_valid(v);
}
static int cvr_ready(unsigned context, const cv_value_t *v)
{
    if (cr_safe || cfs_restore_lock) { cf_message("Discovery is protected"); return 0; }
    if (context == CV_SOUND && (v->sound.engine != cf_snd[v->part].engine ||
        v->sound.kind != cf_snd[v->part].kind || v->sound.fk != cf_snd[v->part].fk ||
        v->sound.slot != cf_snd[v->part].slot || memcmp(v->sound.name, cf_snd[v->part].name, 13))) {
        cf_message("Recall the original sound\nbefore its variation"); return 0;
    }
    if (context == CV_SOUND && v->env_kind != (cfe_fm6(v->part) ? 1 : cfe_va(v->part) && cfe_deep(v->part) ? 2 : 0)) {
        cf_message("Sound envelope is incompatible"); return 0;
    }
    if (context == CV_HARMONY || context == CV_FX) return 1;
    if (context == CV_DRUMS && cf_sn.dm_on && !sqk.on && cf_sn.sq_rec == 0xFFu && v->bpm == cs.bpm) return 1;
    if (cf_sn.running || cf_sn.sq_on || sqk.on || cf_sn.sq_rec != 0xFFu || cf_sn.n || cf_sn.nb || cu.keys || cfs_sounding()) {
        cf_message("Stop playback and release\nnotes; let tails finish"); return 0;
    }
    return 1;
}
static void cvr_apply(const cv_value_t *v, unsigned context)
{
    switch (context) {
    case CV_HARMONY: cpg.tag = v->tag; cpg.idea = cf_progression_pick(cs.chord.scale, v->tag, v->idea, 0); break;
    case CV_ARP: cs.arp = v->arp; cf_send_config(); break;
    case CV_FX: cs.fx = v->fx; cf_fx_recall(&cs.fx); break;
    case CV_SCALE:
        cs.chord.tonic = v->tonic; cs.chord.scale = v->scale;
        cpg.tag = v->tag; cpg.idea = v->idea; cu_spell_update(); cf_send_config(); break;
    case CV_DRUMS:
        dmr.pos[v->beats] = DM_RHY[v->rhythm].id; dmr.last = 1; dm_view = v->rhythm;
        dmr.kit[dm_uid(v->rhythm)] = v->kit; dmr.swing[dm_uid(v->rhythm)] = v->swing;
        if (cf_sn.dm_on) dm_play(v->rhythm); /* existing next-bar handoff, no new scheduler */
        else { cs.beats = v->beats; cs.bpm = v->bpm; dm_view = 0xFF; cf_send_metro(); cf_send_config(); }
        break;
    case CV_SOUND: {
        unsigned p = v->part;
        if (v->env_kind == 1) {
            uint8_t patch[FP_SIZE + 1u]; memcpy(patch, fm6_patch[p], sizeof patch);
            for (unsigned op = 0; op < 6; op++) for (unsigned k = 0; k < 5; k++) patch[op * FP_OP + CV_FM_BYTE[k]] = v->env[op * 5 + k];
            fm6_put_patch(p, patch, 0);
        } else for (unsigned k = 0; k < 4; k++) {
            if (v->env_kind == 2) cfe_deep(p)->set(&trk[p], 10, k, v->env[k]);
            else { fm1_irq_off(); trk[p].p[CV_ENV_PARAM[k]] = v->env[k]; fm1_irq_on(); }
        }
        if (v->edited) cf_edited(p);
        else { cfk_drop(cfk_own[p], p); cfk_on[p] = 0; cu.edited[p] = 0; }
        break;
    }
    }
}
static void cvr_remember(unsigned context, unsigned part)
{
    cvr_capture(&cvr.previous, part); cvr.previous_context = (uint8_t)context; cvr.have_previous = 1;
    cvr.previous_drums = dmr; cvr.previous_view = dm_view; cvr.previous_beats = cs.beats;
}
static void cvr_new(void)
{
    cv_value_t value;
    cvr_capture(&value, cvr.part);
    if (!cvr_ready(cvr.context, &value)) return;
    if (!cvr_generate(&value, cvr.context, cvr.level, cvr.seed)) {
        cf_message("No compatible material\nin this context"); return;
    }
    cvr_remember(cvr.context, value.part);
    cvr.seed++;
    cvr_apply(&value, cvr.context);
    cf_message(cvr.context == CV_DRUMS && cf_sn.dm_on ? "Drums queued: next bar" : "Variation ready; K4 undo");
}
static void cvr_undo(void)
{
    cv_value_t current;
    dm_rec_t drums = dmr; uint8_t view = dm_view, beats = cs.beats;
    if (!cvr.have_previous) { cf_message("No previous variation"); return; }
    if (!cvr_ready(cvr.previous_context, &cvr.previous)) return;
    cvr_capture(&current, cvr.previous.part);
    cvr_apply(&cvr.previous, cvr.previous_context); cvr.previous = current;
    if (cvr.previous_context == CV_DRUMS) {
        dmr = cvr.previous_drums; dm_view = cvr.previous_view;
        if (!cf_sn.dm_on) { cs.beats = cvr.previous_beats; cf_send_metro(); }
        cvr.previous_drums = drums; cvr.previous_view = view; cvr.previous_beats = beats;
    }
    cf_message("Previous / current swapped");
}
static void cvr_context_from_screen(void)
{
    if (cu.scr == SCR_ARP) cvr.context = CV_ARP;
    else if (cu.scr == SCR_DRUM || cu.scr == SCR_SEQ) cvr.context = CV_DRUMS;
    else if (cu.scr == SCR_FX) cvr.context = CV_FX;
    else if (cu.scr == SCR_EDIT || cu.scr == SCR_ENV || cu.scr == SCR_BASS) {
        cvr.context = CV_SOUND;
        cvr.part = cu.scr == SCR_EDIT ? cu.edit_part : cu.scr == SCR_BASS ? 1 : cu.env_tgt == 3 ? 2 : cu.env_tgt == 2 ? 1 : 0;
    } else if (cu.scr == SCR_HOME) cvr.context = CV_SCALE;
    else if (cu.scr == SCR_SEL) cvr.context = CV_HARMONY;
}
static void cvr_knob_info(int k, cf_knob_t *o)
{
    static const char *const KN[4] = {"CONTEXT", "AMOUNT", "GENERATE", "UNDO"};
    kfmt(o->name, 12, KN[k]);
    kfmt(o->val, 14, k == 0 ? CV_NAMES[cvr.context] : k == 1 ? CV_LEVELS[cvr.level] : "Turn +");
    if (k == 3) o->dis = !cvr.have_previous;
}
static void cvr_knob_turn(int k, int d)
{
    if (!d) return;
    if (k == 0) cvr.context = (uint8_t)clampi(cvr.context + d, 0, CV_N - 1);
    if (k == 1) cvr.level = (uint8_t)clampi(cvr.level + d, 0, 2);
    if (k == 2 && d > 0) cvr_new();
    if (k == 3 && d > 0) cvr_undo();
}
static void cvr_save(void)
{
    if (cvr.context == CV_FX) {
        ui_open(SCR_FX); fxp_manage_open(); /* existing empty-slot-first FX workflow */
    } else if (cvr.context == CV_SOUND) {
        cf_save_open(); cu.page[SCR_SAVE] = cvr.part;
        cu.save_new = 1; cf_save_name_init(); /* preserve the original saved sound */
    } else if (cvr.context == CV_HARMONY) cf_message("Suggestions stay this session");
    else cf_message("Settings kept automatically\nK4 can still undo");
}
