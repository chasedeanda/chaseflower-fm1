/* SPDX-License-Identifier: GPL-3.0-only
 * Shared range policy; kept independent of target register access for host tests. */
#pragma once
#include <stdint.h>

#define FL_DATA_LO      0x00097000u                /* Felucca main store */
#define FL_DATA_HI      0x000E0000u
#define FL_GLOB_LO      0x000FC000u                /* Felucca superblock / globals */
#define FL_GLOB_HI      0x000FF000u
#define FL_OTA_LO       0x000E0000u                /* M-UPGRADE loader staging, ota.c */
#define FL_OTA_HI       0x000E5000u
/* Three recording A/B pairs and their policy, beyond the updater staging area and below globals.
 * Legacy stores and the loader's five sectors retain their existing addresses. */
#define FL_REC_LO       0x000E6000u
#define FL_REC_HI       0x000EE000u
#define FL_DRAFT_LO     0x000EE000u
#define FL_DRAFT_HI     0x000F0000u
/* [off, off + n) inside [lo, hi), without wrapping: off + n can overflow, and
 * the 1 MiB part ignores the high address bits, so a wrapped range lands low. */
#define FL_IN(off, n, lo, hi) ((uint32_t)(off) >= (lo) && (uint32_t)(off) <= (hi) && \
                               (uint32_t)(n) <= (hi) - (uint32_t)(off))
/* Felucca's own store (projects, user samples; settings) */
#define FL_STORE_OK(off, n) (FL_IN(off, n, FL_DATA_LO, FL_DATA_HI) || FL_IN(off, n, FL_GLOB_LO, FL_GLOB_HI) || \
                            FL_IN(off, n, FL_REC_LO, FL_REC_HI) || FL_IN(off, n, FL_DRAFT_LO, FL_DRAFT_HI))
