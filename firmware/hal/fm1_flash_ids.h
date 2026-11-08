/* SPDX-License-Identifier: GPL-3.0-only */
/* Supported 1 MiB FM-1 SPI NOR parts. Shared by the app and RAM loader. */
#pragma once
#include <stdint.h>
static inline int fm1_flash_jedec_ok(uint32_t id)
{
    return id == 0x856014u || id == 0xB36014u; /* Puya P25Q80 / UC25HQ80 */
}
#define FL_JEDEC_OK(id) fm1_flash_jedec_ok(id)
