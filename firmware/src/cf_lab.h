/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef CF_LAB_H
#define CF_LAB_H
#include "cf_chord.h"
#define CF_LAB_HISTORY 3
#define CF_LAB_CHOICES 3
typedef struct { uint8_t root,n,note[CF_MAX_NOTES]; int8_t bass; uint32_t mask; } cf_lab_chord_t;
typedef struct {
    char name[16],degree[8],key[4],note[CF_MAX_NOTES][4];
    uint8_t n,outside[CF_MAX_NOTES];
    const char *interval[CF_MAX_NOTES];
} cf_lab_info_t;
typedef struct { uint8_t n,tonic,scale; cf_lab_chord_t chord[CF_LAB_HISTORY]; } cf_lab_history_t;
typedef struct { uint8_t degree; uint32_t mask; const char *reason; } cf_lab_choice_t;
void cf_lab_numeral(int scale,int tonic,int root,uint32_t mask,char out[8]);
void cf_lab_describe(const cf_lab_chord_t *c,int scale,int tonic,cf_lab_info_t *out);
/* Only caller-identified committed harmonies belong here, never arp/audition events. */
int cf_lab_remember(cf_lab_history_t *h,int scale,int tonic,const cf_lab_chord_t *c);
/* Bounded foreground scan of the shipped catalogue; no probability claims. */
unsigned cf_lab_suggest(const cf_lab_history_t *h,cf_lab_choice_t out[CF_LAB_CHOICES],int *fallback);
#endif
