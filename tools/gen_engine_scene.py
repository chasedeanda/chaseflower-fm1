#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Emit only the authored Build 01 pilot; other scene data remains unexposed."""
import json,sys
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=json.loads((root/'assets/engine/scenes-v1.json').read_text())
s=next(x for x in p['scene_definitions'] if x['id']==2)
assert p['length_bars']==16 and p['ppqn']==96 and s['slug']=='HEAVENLY_GAZE'
assert s['bars_chord_index']==[i//2%4 for i in range(16)]
r=s['regenerate_candidates'];rows=[]
for chords in r['harmonies']:
 assert len(chords)==4
 rows.append('{'+','.join('{%d,{%s}}'%(len(c['notes_midi']),','.join(map(str,c['notes_midi']))) for c in chords)+'}')
text='/* SPDX-License-Identifier: GPL-3.0-only; generated from supplied scene JSON. */\n'
text+='static const struct { uint8_t n,note[6]; } CGE_NOTES[3][4] = {'+','.join(rows)+'};\n'
for key,values in [('CHORD_SOUND',r['chord_sounds']),('BASS_SOUND',r['bass_sounds']),('FX_NAMES',r['fx_recipes'])]:
 text+='static const char *const CGE_'+key+'[] = {'+','.join(json.dumps(x) for x in values)+'};\n'
text+='static const uint8_t CGE_GROOVES[] = {'+','.join(map(str,r['grooves']))+'};\n'
text+='static const uint8_t CGE_PROFILE[5][4] = {'+','.join('{%d,%d,%d,%d}'%(x['bars'][1],x['chord'],x['bass'],x['drums']) for x in p['phase_profiles']['dreampop'])+'};\n'
text+='static const uint8_t CGE_DEFAULTS[] = {%d,%d,%d};\n'%(s['defaults']['energy'],s['defaults']['movement'],s['defaults']['variation'])
out=root/'firmware/src/cf_engine_data.h'
if '--check' in sys.argv: assert out.read_text()==text,'stale Engine scene data'
else: out.write_text(text)
