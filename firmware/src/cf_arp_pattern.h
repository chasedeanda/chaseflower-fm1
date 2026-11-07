/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef CF_ARP_PATTERN_H
#define CF_ARP_PATTERN_H
#include <stdint.h>
/* Stable saved IDs: append modes; Strum is specifically 7/8, not a range. */
enum { CF_ARP_UP, CF_ARP_DOWN, CF_ARP_INC, CF_ARP_EXC, CF_ARP_RAND, CF_ARP_ORDER,
       CF_ARP_REPEAT, CF_ARP_STRUM_UP, CF_ARP_STRUM_DOWN, CF_ARP_CONVERGE,
       CF_ARP_DIVERGE, CF_ARP_LOW_PEDAL, CF_ARP_HIGH_PEDAL, CF_ARP_NMODES };
#define CF_ARP_STRUMMING(m) ((m) == CF_ARP_STRUM_UP || (m) == CF_ARP_STRUM_DOWN)
/* Shared deterministic walk, used by both player and foreground preview.
 * Random is intentionally absent: its display consumes actual played history. */
static uint32_t cf_arp_walk(unsigned mode, uint32_t len, uint32_t i)
{
    uint32_t k;
    if (len < 2) return 0;
    switch (mode) {
    case CF_ARP_DOWN: return len - 1 - i % len;
    case CF_ARP_INC: k=i%(2*len); return k<len?k:2*len-1-k;
    case CF_ARP_EXC:
        if (len<3) return i%len;
        k=i%(2*len-2); return k<len?k:2*len-2-k;
    case CF_ARP_REPEAT: return (i/2)%len;
    case CF_ARP_CONVERGE: k=i%len; return k&1?len-1-k/2:k/2;
    case CF_ARP_DIVERGE: k=i%len; return k&1?(len+1)/2+k/2:(len-1)/2-k/2;
    case CF_ARP_LOW_PEDAL: k=i%(2*(len-1)); return k&1?1+k/2:0;
    case CF_ARP_HIGH_PEDAL: k=i%(2*(len-1)); return k&1?k/2:len-1;
    default: return i%len;
    }
}
/* One bounded IRQ snapshot. Arrays contain event history, never audio samples. */
typedef struct {
    uint32_t idx,count,onset_ms,interval_ms,mask;
    uint16_t seen;
    uint8_t trace[16],note[12],order[12],strum[48];
    uint8_t n,on,mode,oct,rate,gate,swing,latched,strum_n,strum_i,source,root;
} cf_arp_view_t;
#endif
