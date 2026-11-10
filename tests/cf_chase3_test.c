/* SPDX-License-Identifier: GPL-3.0-only
 * ChaseFlower integration tests: real UI, sound pools, storage, clock, DSP and USB on host HAL.
 * No physical device is opened. --render DIR writes reproducible dry audition WAVs and a CSV.
 */
#include <stdio.h>
#include <math.h>
#include "usb_mock.h"
#define FELUCCA_UAC 1
#define FELUCCA_CDC 1
#include "../tools/emu/emu_fw.c"
static void ua_service_paced(uint32_t t0, int isr) { (void)t0; (void)isr; ua_service(); }
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

static void clear_notes(void)
{
    uint32_t i;
    for (i = 0; i < NSND; i++) {
        memset(trk[i].v, 0, sizeof trk[i].v);
        trk[i].tail = trk[i].nmono = trk[i].xp_n = 0;
    }
}

/* Independent reconstruction of the chase2 lists: original factory counts, original hidden names,
 * original user-slot ordering. Compare against the prefix of the new combined lists. */
static void legacy_rows(void)
{
    uint32_t fixture, eord, part, k;
    for (fixture = 0; fixture < 5; fixture++) {
        memset(up_bank, 0, sizeof up_bank);
        for (k = 0; k < (fixture == 0 ? 0u : fixture == 1 ? 1u : 32u); k++) {
            up_rec_t *r = up_rec(k);
            r->used = UP_USED; r->ver = UP_VER; r->engine = (k & 1u) ? ENGI_VA : ENGI_FM6; r->np = P_COUNT;
            memcpy(r->name, "USER", 4);
            for (uint32_t p = 0; p < P_COUNT; p++) up_set_value(r, p, TP[p].def);
            up_set_value(r, P_VOICE, k & 2u ? V_MONO : V_POLY);
            if (fixture >= 3) {
                r->note[15] = CB_BIND_MARK;
                r->flags[15] = (uint8_t)(fixture == 4 ? 1u : (k % 20u) + 1u);
            }
        }
        up_gen++; cf_lst.ok = 0;
        for (part = 0; part < 2; part++) {
            uint32_t at = 0;
            for (eord = 0; eord < CF_NENG; eord++) {
                uint32_t e = CF_ENGINES[eord], nf = e == ENGI_FM6 ? 31u : 37u;
                CHECK(cf_lst.n[part] <= CF_LIST_MAX);
                for (uint32_t p = 0; p <= nf + UP_SLOTS; p++) {
                    uint32_t slot = UP_SLOTS, f = PF_NONE, pos = p;
                    int who;
                    if (p == 0) who = 2;
                    else if (p <= nf) {
                        f = p - 1u;
                        slot = pool_bound_slot(e, f);
                        if (slot < UP_SLOTS) who = up_value(up_rec(slot), P_VOICE) == V_MONO;
                        else {
                            const char *nm = ENGINES[e]->presets[f].name;
                            if (cf_named(CF_DROP, NELEM(CF_DROP), e, nm)) continue;
                            who = cf_named(CF_BASS, NELEM(CF_BASS), e, nm);
                        }
                    } else {
                        uint32_t count = p - nf - 1u;
                        for (k = 0; k < UP_SLOTS; k++)
                            if (cb_added(e, k) && count-- == 0) { slot = k; break; }
                        if (slot == UP_SLOTS) continue;
                        who = up_value(up_rec(slot), P_VOICE) == V_MONO;
                    }
                    if (who != 2 && who != (int)part) continue;
                    uint32_t ne, np;
                    cf_snd_at(part, at, &ne, &np);
                    CHECK(ne == e && np == pos);
                    CHECK(cf_snd_index(part, e, pos) == at);
                    pool_ent_t pe = pool_entry(e, pos);
                    CHECK(pe.kind == (p == 0 ? PK_INIT : slot < UP_SLOTS ? PK_USER : PK_FACTORY));
                    if (slot < UP_SLOTS) CHECK(pe.slot == slot);
                    if (f != PF_NONE) CHECK(pe.fk == f);
                    at++;
                }
            }
            CHECK(at < cf_snd_count(part) && cf_snd_count(part) < CF_LIST_MAX);
            /* The full original chase3 suffix must also keep its positions, including VA after FM. */
            for (eord = 0; eord < 2; eord++) {
                uint32_t e = eord ? ENGI_VA : ENGI_FM6, base = eord ? 37u : 31u;
                uint32_t end = eord ? 93u : 95u;
                for (uint32_t f = 0; f < end; f++) {
                    uint32_t pos = pool_pos_factory(e, f);
                    pool_ent_t pe = pool_entry(e, pos);
                    if (f < base && !(pe.kind == PK_FACTORY &&
                        cf_named(CF_DROP, NELEM(CF_DROP), e, ENGINES[e]->presets[f].name))) continue;
                    if (cf_ent_part(e, pos) != (int)part) continue;
                    uint32_t ne, np; cf_snd_at(part, at++, &ne, &np);
                    CHECK(ne == e && np == pos);
                }
            }
            if (part == CF_PART_CHORD) for (uint32_t f = 95; f < 97; f++) {
                uint32_t ne, np; cf_snd_at(part, at++, &ne, &np);
                CHECK(ne == ENGI_FM6 && pool_entry(ne, np).fk == f);
            }
            if (part == CF_PART_CHORD) for (uint32_t bank=1; bank<=8; bank++) {
                uint32_t e=(bank==1||bank==3||bank==7)?ENGI_FM6:ENGI_VA;
                uint32_t f=bank==1?97:bank==2?93:bank==3?98:bank==4?94:bank==5?95:bank==6?96:bank==7?99:97;
                uint32_t ne,np; cf_snd_at(part,at++,&ne,&np);
                CHECK(ne==e && pool_entry(ne,np).fk==f);
            }
            CHECK(at == cf_snd_count(part));
        }
        for (eord = 0; eord < CF_NENG; eord++) {
            uint32_t e = CF_ENGINES[eord];
            CHECK(pool_count(e) < 256u);
            for (k = 0; k < ENGINES[e]->npresets; k++) {
                pool_ent_t pe = pool_entry(e, pool_pos_factory(e, k));
                CHECK(pe.fk == k);
            }
            for (k = 0; k < UP_SLOTS; k++) if (up_used(k) && cb_rec_engine(k) == e)
                CHECK(pool_entry(e, pool_pos_slot(e, k)).slot == k);
        }
    }
    memset(up_bank, 0, sizeof up_bank); up_gen++; cf_lst.ok = 0;
}

static void banks(void)
{
    for (uint32_t part = 0; part < 3; part++) {
        for (uint32_t b = 1; b <= 8; b++) {
            uint32_t count = 0;
            cf_bank[part] = (uint8_t)b;
            for (uint32_t i = 0; i < cf_snd_count(part); i++) {
                uint32_t e, p; cf_snd_at(part, i, &e, &p);
                count += cf_bank_match(part, e, p);
            }
            CHECK(count == (part == CF_PART_BASS ? 3u : 13u));
            for (uint32_t i = 0; i < count + 2; i++) {
                CHECK(cf_snd_step(part, i & 1u ? -1 : 1));
                CHECK(cf_entry_bank(cf_snd[part].engine, cf_snd[part].pos) == b);
            }
            /* Pattern and LAYER sound picking also respects the bank; HOME remains reachable. */
            cfps_set(0, part, 0);
            for (uint32_t i = 0; i < count + 2; i++) {
                uint32_t e, p;
                sq_snd_turn(0, part, 1);
                if (cfps_where(0, part, &e, &p)) CHECK(cf_bank_match(part, e, p));
            }
        }
        cf_bank[part] = 10; /* Empty User bank must leave the patch and its generation untouched. */
        uint8_t gen = cf_snd_gen[part];
        CHECK(!cf_snd_step(part, 1) && cf_snd_gen[part] == gen);
        cf_bank[part] = 0;
    }
}

static void views(void)
{
    cs.seq_hint = 7; cu.scr = SCR_HOME; home_boot_main=1;cf_home_bank=0;
    for (uint32_t i = 0; i < 9; i++) {
        CHECK(home_view() == HV_FLOWER && cf_home_bank==i%CF_HB_N);
        ui_button(B_HOME); CHECK((cs.seq_hint & 7u) == 7u);
    }
    home_boot_main=0;cs.seq_hint = 0xC7; CHECK(home_view() == HV_DIAG);
    cu.scr = SCR_GLO; ui_button(B_HOME); CHECK(cu.scr == SCR_HOME && home_view()==HV_FLOWER && cf_home_bank==0 && cs.seq_hint==0xC7);
    /* Explicit legacy view choice remains a valid saved setting. */
    home_boot_main=0;cs.seq_hint = (HV_KEYS << HV_SHIFT) | 7u;
    CHECK(cf_settings_write(0) == 1); cs.seq_hint = 0;
    CHECK(cf_settings_load() && home_view() == HV_KEYS && (cs.seq_hint & 7u) == 7u);
    CHECK(sizeof(cf_set_t) == 188u);
    memset(&cf_sn, 0, sizeof cf_sn);
    cf_sn.out.sounding = 1; cf_sn.out.n = 4; cf_sn.out.root_pc = 2; cf_sn.out.mask = 0x489;
    uint8_t notes[] = {50, 57, 60, 65}; memcpy(cf_sn.out.note, notes, 4);
    cs.bass_on = 1; cf_sn.nb = 1; cf_sn.bass[0] = 33;
    hv_update(); CHECK(hv_n == 4 && hv_root == 2 && hv_bass[0] == 33 && !memcmp(hv_note, notes, 4));
    cf_sn.arp_on = 1; cs.arp.mode = CF_ARP_UP; cf_sn.arp_note = 62;
    CHECK(hv_arp_pc(50) && hv_arp_at(62) && !hv_arp_at(50));
    CHECK(hv_key_mark(33) == C_SEC(1) && hv_key_mark(62) == FL_INK);
    cf_sn.out.sounding = 0; cf_sn.sq_on = 1; cf_sn.sq_dn = 1; cf_sn.sq_dnote[0] = 127;
    cf_sn.sq_bn = 1; cf_sn.sq_bass[0] = 0; hv_update(); CHECK(hv_note[0] == 127 && hv_bass[0] == 0);
    cf_sn.sq_on = 0; hv_update(); CHECK(hv_note[0] == 127);
    char n[8]; hv_note_name_raw(0, n, 1); CHECK(strstr(n, "-1") != NULL);
    hv_note_name_raw(127, n, 1); CHECK(strstr(n, "9") != NULL);
    /* Extreme windows and dense labels exercised under AddressSanitizer. */
    str_cpy(cu_name, "Dm7/A", sizeof cu_name); cu_name_single = 0;
    draw_home_keys(); draw_home_chord();
}

static void save_roundtrip(int extras)
{
    int32_t out[CTL*2];
    for(uint32_t eord=extras ? 2u : 0u;eord<(extras ? 3u : 2u);eord++) for(uint32_t asnew=0;asnew<2;asnew++) {
        uint32_t e=eord==1 ? ENGI_VA : ENGI_FM6;
        uint32_t fk=eord==2 ? 96u : eord==1 ? CHASE_VA_BASE : CHASE_FM_BASE;
        cf_pool_load(0,e,pool_pos_factory(e,fk));
        for(uint32_t i=0;i<64;i++)audio_block(out,CTL);
        uint8_t before[ENG_BLOB_MAX],after[ENG_BLOB_MAX];
        ENGINES[e]->deep->blob_get(&trk[0],before); int8_t trim=trk[0].trim;
        uint32_t preview=cf_snd_save_index(0,(int)asnew);
        CHECK(cf_snd_save(0,(int)asnew,"ROUND TRIP")==0);
        CHECK(cf_snd_pos(0)==preview);
        uint32_t slot=cf_snd[0].slot;
        CHECK(up_used(slot) && up_rec(slot)->note[12]==CB_TRIM_MARK);
        if (eord==2) CHECK(up_value(up_rec(slot),P_E7)==127);
        cf_load_init(0,ENGI_FM6);
        cf_pool_load(0,e,pool_pos_slot(e,slot));
        for(uint32_t i=0;i<64;i++)audio_block(out,CTL);
        ENGINES[e]->deep->blob_get(&trk[0],after);
        CHECK(trk[0].trim==trim && !memcmp(before,after,ENGINES[e]->deep->blob_size));
        cf_stage_t st; cf_snd_build(0,e,pool_pos_slot(e,slot),&st);
        CHECK(st.trim==trim && !memcmp(st.blob,before,st.blen));
    }
    memset(up_bank,0,sizeof up_bank);up_gen++;cf_lst.ok=0;
}

static void timing(void)
{
    CHECK(dm_count(6) == 3 && dm_steps(6) == 12);
    for (uint32_t r = 0; r < DM_N; r++) {
        for (uint32_t q = 0; q < r; q++) CHECK(DM_RHY[q].id != DM_RHY[r].id);
        for (uint32_t l = 0; l < SQ_NLANE; l++) if (DM_RHY[r].lane[l])
            CHECK(strlen(DM_RHY[r].lane[l]) == dm_steps(DM_RHY[r].beats) * DM_RHY[r].bars);
    }
    for (uint32_t sig = 4; sig <= 6; sig += 2) for (uint32_t bpm = 60; bpm <= 300; bpm += 60) {
        uint32_t r = sig == 6 ? dm_nth(6, 0) : dm_nth(4, 0);
        cfm.beats = sq_beats = (uint8_t)sig; cfm.bpm = (uint16_t)bpm;
        cfx_in.active = 0; dmp_start(r, DM_RHY[r].kit, 0); cfm_restart(); cfm.running = 1;
        cfm_tick(0, 0); uint32_t g0 = cf_grid.g24;
        memset(&cfo,0,sizeof cfo); usb.config=1; mo_r=mo_w;
        cfp_cfg.clock_out=cfp_cfg.midi_out=1; cf_clock_out(); mo_r=mo_w;
        uint32_t clocks=0;
        uint32_t quarters = sig == 6 ? 3 : 4, frames = FS * 60u * quarters / bpm;
        for (uint32_t i = 0; i < frames;) {
            uint32_t n = frames-i < CTL ? frames-i : CTL;
            dmp_tick(n, 0); cfm_tick(n, 0); cf_clock_out(); i += n;
            while(mo_r!=mo_w) { clocks+=((midi_out_q[mo_r%MQ]>>8)&255u)==0xF8u; mo_r++; }
        }
        CHECK(dmp.nbar == 1 && dmp.t == 0);
        CHECK(cf_grid.g24 - g0 == quarters*24u && cfm.beats_done == sig + 1u);
        CHECK(clocks==quarters*24u);
        /* External MIDI: exactly 72 pulses for 6/8 and 96 for 4/4. */
        dmp_start(r, DM_RHY[r].kit, 0); cfm_restart(); cfm.running = 1; cfx_in.active = 1;
        cfm_tick(0, 0); g0 = cf_grid.g24;
        for (uint32_t i = 0; i < quarters*24u; i++) { dmp_tick(0, 1); cfm_tick(0, 1); }
        CHECK(dmp.nbar == 1 && dmp.t == 0 && cf_grid.g24 - g0 == quarters*24u);
        /* The same converted rhythm is playable/editable on the sequencer's clock. */
        sq_fresh(&sq_st.pat[0], sig); dm_fill(&sq_st.pat[0], r, 0);
        CHECK(sq_st.pat[0].tb[SQ_DRUMS] == DM_RHY[r].bars);
        sqp_start(0, 0);
        for (uint32_t i = 0; i < quarters*24u; i++) sqp_tick(0, 1);
        CHECK(sqp.nbar == 1 && sqp.t == 0);
        sqp_stop(); dmp_stop();
    }
    /* The second dotted-quarter pulse is accented. */
    cfm.beats = 6; cfm.beat = 3; cfm.acc = 0; cfm.beats_done = 3; cfm.running = 1; cfx_in.active = 1;
    cfm_tick(0, 12); CHECK(cfm.click == 2);
    cfx_in.active = 0; cfm.on = cfm.running = 0;
    cfp_cfg.clock_out=0;
}

static void wav_header(FILE *f, uint32_t frames)
{
    uint32_t a = 36 + frames*4, sr=FS, rate=FS*4, bytes=frames*4;
    uint16_t fmt=1, ch=2, align=4, bits=16; uint32_t sz=16;
    fwrite("RIFF",1,4,f); fwrite(&a,4,1,f); fwrite("WAVEfmt ",1,8,f); fwrite(&sz,4,1,f);
    fwrite(&fmt,2,1,f); fwrite(&ch,2,1,f); fwrite(&sr,4,1,f); fwrite(&rate,4,1,f);
    fwrite(&align,2,1,f); fwrite(&bits,2,1,f); fwrite("data",1,4,f); fwrite(&bytes,4,1,f);
}

static void library_render(const char *dir, int extras)
{
    int32_t out[CTL*2]; uint32_t i, p, k, frames = (FS*4u/CTL)*CTL;
    FILE *csv = NULL; char path[1024];
    if (dir) { snprintf(path,sizeof path,"%s/levels.csv",dir); csv=fopen(path,extras ? "a" : "w"); CHECK(csv); }
    if (csv && !extras) fprintf(csv,"id,name,peak_dbfs,rms_dbfs,clip_samples\n");
    cf_fx_t fx=CFX_DEFAULT; fx.on=0; cf_fx_apply(&fx); fx_output_set(0,1);
    cf_ready=0; /* isolate the DSP from previously queued UI/transport events */
    ua_reset(); ua.cap_alt=1; ua.cap_hold=0; usb.up=usb.config=1; usb_cdc_on=usb.suspended=0; ua_rate_set(44100);
    for (uint32_t engine=0; engine<(extras ? 1u : 2u); engine++) {
        uint32_t e=engine ? ENGI_VA : ENGI_FM6, base=engine ? CHASE_VA_BASE : CHASE_FM_BASE;
        uint32_t first=extras ? CHASE_FM_BASE+CHASE_FM_LEGACY_N : base;
        uint32_t end=engine ? CHASE_VA_BASE+CHASE_VA_N : extras ? CHASE_FM_BASE+CHASE_FM_N : CHASE_FM_BASE+CHASE_FM_LEGACY_N;
        for (k=first; k<end; k++) {
            p=engine && CHASE_VA_BASS[k-base] ? 1 : 0;
            clear_notes(); cf_pool_load(p,e,pool_pos_factory(e,k));
            for (i=0;i<64;i++) { audio_block(out,CTL); ua.cr=ua.cw; }
            trk[p].p[P_LEVEL]=92;
            if(getenv("CHASE_RAW_LEVELS")) trk[p].trim=0;
            uint8_t notes[]={48,55,59,62,64,69};
            for (i=0;i<(p ? 1u : 6u);i++) trk_note_on(&trk[p],p ? 36 : notes[i],100);
            char id[16]; snprintf(id,sizeof id,"CF3-%c%03u",engine ? 'V' : 'F',k-base+1);
            FILE *f=NULL;
            if (dir && !getenv("CHASE_LEVELS_ONLY")) { snprintf(path,sizeof path,"%s/%s.wav",dir,id); f=fopen(path,"wb"); CHECK(f); if(f)wav_header(f,frames); }
            int64_t energy=0; int32_t peak=0; uint32_t clip=0, nonzero=0;
            for (uint32_t n=0;n<frames;n+=CTL) {
                if (n==((FS*3u/CTL)*CTL)) for(i=0;i<(p ? 1u : 6u);i++) trk_note_off(&trk[p],p ? 36 : notes[i]);
                uint32_t w=ua.cw; audio_block(out,CTL);
                CHECK(ua.cw==w+CTL);
                int16_t pcm[CTL*2];
                for (i=0;i<CTL;i++) {
                    const int16_t *cap=ua.cap+((w+i)&(UA_CAP_RING-1u))*6u;
                    CHECK(cap[0]==ua_sat(out[i*2]>>8) && cap[1]==ua_sat(out[i*2+1]>>8));
                    CHECK(p ? (!cap[2] && !cap[3]) : (!cap[4] && !cap[5]));
                    for (uint32_t ch=0;ch<2;ch++) {
                        int32_t s=out[i*2+ch]>>8, a=s<0?-s:s;
                        if(a>peak)peak=a; energy+=(int64_t)s*s; clip+=a>=32767; nonzero+=a>4;
                        pcm[i*2+ch]=ua_sat(s);
                    }
                }
                ua.cr=ua.cw;
                if(f) fwrite(pcm,sizeof pcm,1,f);
            }
            if(f)fclose(f);
            CHECK(peak>100 && nonzero>FS/8 && clip==0);
            double db=20*log10(peak/32768.0+1e-12), rms=10*log10((double)energy/(frames*2.0*32768.0*32768.0)+1e-12);
            if(csv)fprintf(csv,"%s,%s,%.2f,%.2f,%u\n",id,ENGINES[e]->presets[k].name,db,rms,clip);
            printf("sound %s %-12s peak %.1f dBFS rms %.1f dBFS clips %u\n",id,ENGINES[e]->presets[k].name,db,rms,clip);
            /* The pattern's prebuilt blob and the loaded preset must describe the same voice. */
            cf_stage_t stage; cf_snd_build(p,e,pool_pos_factory(e,k),&stage);
            uint8_t blob[ENG_BLOB_MAX]; ENGINES[e]->deep->blob_get(&trk[p],blob);
            CHECK(stage.blen==ENGINES[e]->deep->blob_size && !memcmp(stage.blob,blob,stage.blen));
        }
    }
    if(csv)fclose(csv);
    cf_ready=1;
}

static void audition_drums(const char *dir)
{
    int32_t out[CTL*2]; uint32_t r=dm_nth(6,2), i, b;
    cf_fx_t fx=CFX_DEFAULT; fx.on=0; cf_fx_apply(&fx);
    CHECK(SQ_NKIT==24 && DS_NKIT==16);
    for (uint32_t a=0;a<SQ_NKIT;a++) {
        uint32_t n=0;
        for(b=0;b<SQ_NKIT;b++) n+=SQ_KIT_ORDER[b]==a;
        CHECK(n==1);
    }
    clear_notes(); cf_evq_r=cf_evq_w; cfa.on=0;
    for(uint32_t kit=14;kit<SQ_NKIT;kit++) {
        cfm.beats=sq_beats=6; cfm.bpm=90; cfm.level=0; cfm.on=0; cfx_in.active=0;
        clear_notes(); dmp_start(r,kit,0); cfm_restart(); cfm.running=1;
        ua_reset(); ua.cap_alt=1; ua.cap_hold=0; ua_rate_set(48000);
        char path[1024], id[16]; FILE *f=NULL; uint32_t frames=(FS*4u/CTL)*CTL;
        snprintf(id,sizeof id,"%s%03u",kit<22 ? "CF3-D" : "MEL-D",kit<22 ? kit-13 : kit-21);
        if(dir && !getenv("CHASE_LEVELS_ONLY")) { snprintf(path,sizeof path,"%s/%s.wav",dir,id); f=fopen(path,"wb"); CHECK(f); if(f)wav_header(f,frames); }
        int32_t peak=0; uint32_t clips=0, count=0;
        for(b=0;b<frames/CTL;b++) {
            audio_block(out,CTL);
            for(i=ua.cr;i<ua.cw;i++) {
                const int16_t *p=ua.cap+(i&(UA_CAP_RING-1u))*6u;
                CHECK(!p[2] && !p[3] && !p[4] && !p[5]); count++;
            }
            ua.cr=ua.cw;
            int16_t pcm[CTL*2];
            for(i=0;i<CTL*2;i++) {
                int32_t s=out[i]>>8,a=s<0?-s:s; if(a>peak)peak=a;
                clips+=a>=32767; pcm[i]=ua_sat(s);
            }
            if(f)fwrite(pcm,sizeof pcm,1,f);
        }
        if(f)fclose(f);
        CHECK(peak>100 && !clips && count>frames);
        CHECK(ua.rs_blocks>0);
        printf("kit %s %s peak %.1f dBFS clips %u\n",id,SQ_KITS[kit].name,20*log10(peak/32768.0+1e-12),clips);
        dmp_stop();
    }
}

static void layered_stress(void)
{
    int32_t out[CTL*2]; uint32_t i,b;
    cf_fx_t fx=CFX_DEFAULT; fx.on=(1u<<CFX_N)-1u; cf_fx_apply(&fx); fx_output_set(1,1);
    for(uint32_t bank=0;bank<8;bank++) {
        clear_notes(); dmp_stop(); sqp_stop();
        cf_pool_load(0,ENGI_FM6,pool_pos_factory(ENGI_FM6,CHASE_FM_BASE+bank*8));
        cf_pool_load(1,ENGI_VA,pool_pos_factory(ENGI_VA,CHASE_VA_BASE+bank*7+4));
        cf_pool_load(2,ENGI_VA,pool_pos_factory(ENGI_VA,CHASE_VA_BASE+bank*7));
        for(b=0;b<64;b++) { audio_block(out,CTL);ua.cr=ua.cw; }
        for(i=0;i<3;i++)trk[i].p[P_LEVEL]=127;
        for(i=0;i<6;i++) {
            static const uint8_t nn[]={48,55,59,62,64,69};
            trk_note_on(&trk[0],nn[i],127); trk_note_on(&trk[2],nn[i]+12,127);
        }
        trk_note_on(&trk[1],36,127);
        cfm.beats=sq_beats=6;cfm.bpm=120;cfm.level=0;cfm.on=0;cfx_in.active=0;
        dmp_start(dm_nth(6,bank%3),14+bank,0);
        ua_reset();ua.cap_alt=1;ua.cap_hold=0;ua_rate_set(48000);
        uint32_t nonzero=0;
        for(b=0;b<FS*2u/CTL;b++) {
            audio_block(out,CTL);ua.cr=ua.cw;
            for(i=0;i<CTL*2;i++) {
                CHECK(out[i]>-0x1000000 && out[i]<0x1000000);
                nonzero+=out[i]!=0;
            }
        }
        CHECK(nonzero>FS && ua.rs_blocks>0);
        dmp_stop();
    }
    clear_notes();
}

int main(int argc, char **argv)
{
    emu_fw_options(NULL, 1, 0, 1); emu_hal.ready=2; emu_fw_init(0);
    legacy_rows(); banks(); save_roundtrip(0); views(); timing();
    memset(up_bank,0,sizeof up_bank); up_gen++; cf_lst.ok=0;
    const char *dir=argc>2 && !strcmp(argv[1],"--render") ? argv[2] : NULL;
    /* Keep old auditions in their original order: noise/dither state must not depend on new patches. */
    library_render(dir,0); audition_drums(dir); library_render(dir,1); save_roundtrip(1); layered_stress();
    printf("chase3: %u checks, %u failures\n",checks,failures);
    return failures!=0;
}
