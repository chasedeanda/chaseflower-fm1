#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Generate the exact G1 palettes; the progression catalog stays untouched."""
import json
import sys
from pathlib import Path
root=Path(__file__).resolve().parents[1]
vibes=json.loads((root/'assets/engine/vibes-g1.json').read_text())
assert [v['name'] for v in vibes]==['Haze','Drive']
def ar(x):
    if isinstance(x,str):return json.dumps(x)
    if isinstance(x,list):return '{'+','.join(ar(v) for v in x)+'}'
    return str(x)
text='''/* SPDX-License-Identifier: GPL-3.0-only; generated G1 palettes, not authored songs. */
typedef struct {
    const char *name,*chord[3],*bass[3],*fx[3];
    uint8_t bpm,groove[4],weight[8],gain[4][3],defaults[4];
} cge_vibe_t;
static const cge_vibe_t CGE_VIBES[2]={
'''
for v in vibes:
    assert len(v['groove'])==4 and len(v['weight'])==8 and len(v['gain'])==4 and len(v['defaults'])==4
    assert all(len(v[k])==3 for k in ['chord','bass','fx'])
    text+='    {'+','.join(ar(v[k]) for k in ['name','chord','bass','fx','bpm','groove','weight','gain','defaults'])+'},\n'
text+='''};
/* Stable catalog ID plus independent role indices; a bounded audio mailbox. */
typedef struct {
    uint16_t id;
    uint8_t vibe,groove,chord,bass,fx,feel;
} cge_take_t;
'''
out=root/'firmware/src/cf_engine_data.h'
if '--check' in sys.argv:assert out.read_text()==text,'stale G1 profile data'
else:out.write_text(text)
