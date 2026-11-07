/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include "../firmware/hal/fm1_flash_ranges.h"
int main(void)
{
    int fail = 0;
#define CHECK(x) do { if (!(x)) { printf("FAIL: %s\n", #x); fail++; } } while (0)
    CHECK(FL_REC_HI - FL_REC_LO == 4u * 2u * 4096u);
    CHECK(FL_REC_LO >= FL_OTA_HI && FL_REC_HI <= FL_GLOB_LO);
    CHECK(FL_STORE_OK(FL_REC_LO, FL_REC_HI - FL_REC_LO));
    CHECK(FL_STORE_OK(FL_REC_HI - 256, 256));
    CHECK(!FL_STORE_OK(FL_REC_HI - 256, 257));
    CHECK(!FL_STORE_OK(FL_REC_LO - 1, 1));
    CHECK(!FL_STORE_OK(FL_REC_HI, 1));
    CHECK(!FL_STORE_OK(FL_REC_LO, 0xFFFF0000u));
    CHECK(!FL_STORE_OK(0xFFFFF000u, 0x2000u));
    CHECK(!FL_STORE_OK(FL_OTA_LO, FL_OTA_HI - FL_OTA_LO));
    CHECK(!FL_STORE_OK(0xFF000u, 4096u));
    CHECK(FL_STORE_OK(FL_DATA_LO, FL_DATA_HI - FL_DATA_LO));
    CHECK(FL_STORE_OK(FL_GLOB_LO, FL_GLOB_HI - FL_GLOB_LO));
    printf("recording flash ranges: %s\n", fail ? "FAIL" : "PASS");
    return fail != 0;
}
