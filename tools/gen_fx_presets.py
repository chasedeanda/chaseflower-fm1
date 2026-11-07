#!/usr/bin/env python3
"""Generate main's existing factory bank type from the approved audition recipes."""
import json
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
def generate():
    bank = json.loads((ROOT/'previews/fx-candidates/candidates.json').read_text())
    assert bank['order'] == ['Filter','Drive','Chorus','Phaser','Delay','Reverb']
    assert len(bank['presets']) == 13
    lines=['/* SPDX-License-Identifier: GPL-3.0-only',
           ' * Approved, auditioned factory recipes. Generated from previews/fx-candidates/candidates.json. */',
           'static const fxp_preset_t FXP_FACTORY[] = {']
    for p in bank['presets']:
        assert 0 <= p['on'] < 64 and len(p['values']) == 6
        for e,v in enumerate(p['values']):
            assert len(v)==3 and all(isinstance(x,int) and 0<=x<=100 for x in v)
            if e in (0,1): assert v[0]<=2
            if e==0: assert v[1]<=99
        values=', '.join('{'+', '.join(map(str,v))+'}' for v in p['values'])
        lines.append('    {'+json.dumps(p['name'])+', {{CFX_FILTER, CFX_DIST, CFX_CHORUS, CFX_PHASER, CFX_DELAY, CFX_REVERB}, '+str(p['on'])+', {'+values+'}}},')
    lines+=['};','#define FXP_FACTORY_N ((unsigned)(sizeof FXP_FACTORY / sizeof FXP_FACTORY[0]))',
            '#define FXP_GROUP_SIZE 13u','static const char *const FXP_GROUP[] = {"Factory"};',
            '_Static_assert(FXP_FACTORY_N == 13u, "approved factory bank");',
            '_Static_assert(FXP_FACTORY_N + FXP_SLOTS <= 256u, "FX browser index fits one byte");','']
    return '\n'.join(lines)
if __name__=='__main__': (ROOT/'firmware/src/cf_fx_factory.c').write_text(generate())
