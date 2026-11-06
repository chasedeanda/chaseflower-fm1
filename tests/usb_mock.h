/* SPDX-License-Identifier: GPL-3.0-only */
/* Register/DMA stand-in, not a USB host or timing simulator. Load the real header once, then redirect only its
 * callers. Production sources and the loader keep using the unchanged hardware HAL. */
#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../firmware/hal/fm1_usb.h"

static uint8_t um_regs[5][32], um_index, um_read;
static uint32_t um_enabled;
static struct { uint8_t bytes[1024]; uint32_t n, sends; } um_ep[5];
static void um_reset(void) { memset(um_regs, 0, sizeof um_regs); um_index = 0; um_enabled = 0; }
static void um_attach(void *p) { (void)p; }
static void um_off(void) {}
static uint32_t um_on(void) { return 1; }
static void um_wr(uint32_t r, uint32_t v)
{
    if (r == 14u) { um_index = (uint8_t)v; return; }
    /* The endpoint flush/clear writes used by the driver leave the TX slot free. */
    if (r == 17u && um_index && v == 0x48u) v = 0;
    um_regs[r < 16u ? 0u : um_index][r] = (uint8_t)v;
}
static void um_rd(uint32_t r) { um_read = um_regs[r < 16u ? 0u : um_index][r]; }
static uint32_t um_done(void) { return 1; }
static uint32_t um_data(void) { return um_read; }
static void um_buf(uint32_t ep, void *p) { (void)ep; (void)p; }
static void um_buf0(void *p) { (void)p; }
static void um_send(uint32_t ep, const void *p, uint32_t n)
{
    if (ep >= 5u || n > sizeof um_ep[ep].bytes) abort();
    memcpy(um_ep[ep].bytes, p, n); um_ep[ep].n = n; um_ep[ep].sends++;
}
static void um_send0(void *p, uint32_t n) { um_send(0, p, n); }
static void um_sync(void) {}
static void um_enable(uint32_t mask) { um_enabled |= mask; }
static uint32_t um_sof(void) { return 1; }

#define fm1_usb_reset um_reset
#define fm1_usb_attach um_attach
#define fm1_usb_off um_off
#define fm1_usb_sie_on um_on
#define fm1_usb_sie_wr_start um_wr
#define fm1_usb_sie_rd_start um_rd
#define fm1_usb_sie_done um_done
#define fm1_usb_sie_data um_data
#define fm1_usb_ep_txbuf um_buf
#define fm1_usb_ep_rxbuf um_buf
#define fm1_usb_ep0_buf um_buf0
#define fm1_usb_ep_send um_send
#define fm1_usb_ep0_send um_send0
#define fm1_usb_rx_sync um_sync
#define fm1_usb_ep_enable um_enable
#define fm1_usb_sof_take um_sof
