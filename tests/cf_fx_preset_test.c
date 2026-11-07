/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include <string.h>
#include "cf_fx_preset.h"
static unsigned checks, failures;
#define CHECK(x) do { checks++; if (!(x)) { failures++; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)
static cf_fx_t chain = {{0,3,4,5,2,1}, 6, {{0,99,10},{1,40,5},{40,35,8},{40,50,70},{30,45,50},{30,60,60}}};
int main(void)
{
    fxp_bank_t b, before; fxp_preset_t p = {"Original FX", {0}};
    p.fx = chain; fxp_bank_init(&b); CHECK(fxp_bank_valid(&b));
    CHECK(sizeof b == 752 && sizeof p == 46);
    for (unsigned i = 0; i < FXP_SLOTS; i++) {
        p.fx.v[CFX_DELAY][0] = i;
        CHECK(fxp_bank_save(&b, i, &p)); CHECK(fxp_bank_valid(&b));
    }
    CHECK(b.used == 65535);
    before = b;
    for (unsigned i = 0; i < sizeof b; i++) {
        ((uint8_t *)&b)[i] ^= 1;
        CHECK(!fxp_bank_valid(&b)); b = before;
    }
    CHECK(!fxp_bank_save(&b, FXP_SLOTS, &p) && !memcmp(&b, &before, sizeof b));
    p.fx.order[0] = p.fx.order[1]; CHECK(!fxp_bank_save(&b,0,&p)); p.fx = chain;
    p.fx.on = 128; CHECK(!fxp_preset_valid(&p)); p.fx = chain;
    for (unsigned e = 0; e < CFX_N; e++) for (unsigned k = 0; k < 3; k++) {
        p.fx = chain; p.fx.v[e][k] = (!k && (e == CFX_FILTER || e == CFX_REVERB)) ? 3 :
            e == CFX_FILTER && k == 1 ? 100 : 101;
        CHECK(!fxp_preset_valid(&p));
    }
    p.fx = chain; memset(p.name,'A',FXP_NAME); CHECK(!fxp_preset_valid(&p));
    memset(p.name,0,FXP_NAME); CHECK(!fxp_preset_valid(&p));
    memset(p.name,' ',FXP_NAME-1); CHECK(!fxp_preset_valid(&p));
    memset(p.name,0,FXP_NAME); memcpy(p.name,"Bad\nname",8); CHECK(!fxp_preset_valid(&p));
    memset(p.name,0,FXP_NAME); memcpy(p.name,"FX",2); p.name[5]='X'; CHECK(!fxp_preset_valid(&p));
    b = before; b.slot[0].fx.order[1] = b.slot[0].fx.order[0]; fxp_bank_seal(&b); CHECK(!fxp_bank_valid(&b));
    b = before; b.version++; CHECK(!fxp_bank_valid(&b));
    b = before;
    for (unsigned i = 0; i < FXP_SLOTS; i++) { CHECK(fxp_bank_delete(&b,i)); CHECK(fxp_bank_valid(&b)); }
    CHECK(!b.used && !fxp_bank_delete(&b,0));
    b.slot[0].name[0] = 'X'; fxp_bank_seal(&b); CHECK(!fxp_bank_valid(&b));
    printf("FX preset format: %u checks, %u failures\n", checks, failures);
    return failures != 0;
}
