#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Real-render four-cycle G1 matrix, deterministic MIDI and USB PCM comparisons."""
import array
import csv
import hashlib
import json
import math
from pathlib import Path
import subprocess
import sys
import wave
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build/g1'
SEED=0x10203040
FS=44100

def capture(vibe,direction,surprise=100,active=0,rate=48000,interaction=0,suffix=''):
    name=f'{vibe}-{direction}-{surprise}-{active}-{rate}-{interaction}'+suffix
    prefix=OUT/name
    r=subprocess.run([str(ROOT/'build/host/cf_g1_test'),'--render',str(vibe),str(direction),str(surprise),str(SEED),str(prefix),str(active),str(rate),str(interaction)],cwd=ROOT,text=True,capture_output=True)
    prefix.with_suffix('.log').write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError(r.stdout+r.stderr)
    raw=prefix.with_suffix('.raw')
    digest=hashlib.sha256(raw.read_bytes()).hexdigest()
    rows=list(csv.DictReader(prefix.with_suffix('.csv').open()))
    assert all(None not in row for row in rows),'Trace field count mismatch'
    markers=[row for row in rows if int(row['status'])==0]
    assert [(int(row['cycle']),int(row['bar'])) for row in markers]==[(c,b) for c in range(4) for b in range(1,17)]
    data=array.array('i');data.frombytes(raw.read_bytes())
    if sys.byteorder!='little':data.byteswap()
    # Two cycles include one or more bounded Jam changes in a short demo.
    count=round(FS*32*4*60/(104 if vibe==0 else 122))*2
    pcm=array.array('h',(max(-32768,min(32767,x>>8)) for x in data[:count]))
    stats={'pcm_sha256':digest,'event_sha256':hashlib.sha256(prefix.with_suffix('.csv').read_bytes()).hexdigest(),
           'peak':max(map(abs,pcm)),'rms':math.sqrt(sum(x*x for x in pcm)/len(pcm)),'frames':len(data)//2}
    raw.unlink()
    return rows,stats,pcm

def attacks(rows,cycle):
    return [(int(r['bar']),int(r['tick']),int(r['status']),int(r['note']),int(r['velocity'])) for r in rows
            if int(r['cycle'])==cycle and int(r['status'])&0xf0==0x90 and int(r['velocity'])]

def durations(rows):
    held={};out=[[] for _ in range(4)]
    for r in rows:
        status=int(r['status']);ch=status&15;note=int(r['note']);key=(ch,note)
        if ch>=2 or status&0xf0 not in (0x80,0x90):continue
        if status&0xf0==0x90 and int(r['velocity']):
            held[key]=r
        elif key in held:
            on=held.pop(key);cycle=int(on['cycle'])
            if cycle<4:out[cycle].append(((int(on['bar']),int(on['tick']),ch,note),int(r['sample'])-int(on['sample'])))
    return [sorted(v) for v in out]

def verify(rows,vibe,direction,surprise):
    markers=[r for r in rows if int(r['status'])==0]
    assert len({r['progression_id'] for r in markers})==1,'autonomous harmony change'
    for bar in range(1,17):
        reference=markers[bar-1]
        for cycle in range(1,4):
            now=markers[cycle*16+bar-1]
            assert now['chord_notes']==reference['chord_notes'] and now['bass_notes']==reference['bass_notes']
    combinations=[]
    for cycle in range(4):
        first=markers[cycle*16]
        combo=(first['groove_id'],first['feel'],first['fx_hash'])
        combinations.append(combo)
        for r in markers[cycle*16:cycle*16+16]:
            assert (r['groove_id'],r['feel'],r['fx_hash'])==combo,'mid-cycle generative change'
    if direction==0 or surprise==0:
        assert all(attacks(rows,c)==attacks(rows,0) for c in range(1,4)),'Repeat MIDI event schedule differs'
        gates=durations(rows)
        for cycle in gates[1:]:
            assert len(cycle)==len(gates[0])
            assert all(a[0]==b[0] and abs(a[1]-b[1])<=32 for a,b in zip(gates[0],cycle)),'Repeat note release differs beyond one control block'
        assert len(set(combinations))==1
    else:
        assert len(set(combinations))>=2
        assert len({combo[:2] for combo in combinations})>=2
        for a,b in zip(combinations,combinations[1:]):assert sum(x!=y for x,y in zip(a,b))<=1
    return combinations

def save_wav(name,pcm):
    with wave.open(str(OUT/(name+'.wav')),'wb') as w:
        w.setnchannels(2);w.setsampwidth(2);w.setframerate(FS)
        if sys.byteorder!='little':pcm.byteswap()
        w.writeframes(pcm.tobytes())

def main():
    OUT.mkdir(parents=True,exist_ok=True);metrics={}
    for vibe in range(2):
        for direction in range(2):
            base,stats,pcm=capture(vibe,direction)
            stats['cycle_combinations']=verify(base,vibe,direction,100)
            key=f'{vibe}-{direction}';metrics[key]=stats
            if direction:save_wav(('haze' if vibe==0 else 'drive')+'-jam-demo',pcm)
            repeat,repeated,_=capture(vibe,direction,suffix='-repeat')
            assert repeated=={k:v for k,v in stats.items() if k!='cycle_combinations'},'Fixed-seed render differs'
            for rate in (44100,48000):
                idle,idle_stats,_=capture(vibe,direction,active=0,rate=rate,suffix='-usb-idle')
                active,active_stats,_=capture(vibe,direction,active=1,rate=rate,suffix='-usb-active')
                assert idle_stats==active_stats,'USB capture changes physical PCM or musical events'
            print('PASS: '+('Haze' if vibe==0 else 'Drive')+' '+('Repeat' if direction==0 else 'Jam')+' deterministic cycles and USB PCM at both rates.',flush=True)
        rows,stats,_=capture(vibe,1,surprise=0)
        verify(rows,vibe,1,0);metrics[f'{vibe}-jam-zero']=stats
        rows,stats,_=capture(vibe,0,surprise=0,interaction=2)
        verify(rows,vibe,0,0);metrics[f'{vibe}-repeat-manual-random']=stats
        rows,stats,_=capture(vibe,0,interaction=1)
        marks=[r for r in rows if int(r['status'])==0]
        assert all(int(r['vibe']=='Drive')==vibe for r in marks[:16])
        assert int(marks[16]['vibe']=='Drive')!=vibe
        assert all(r['progression_id']==marks[0]['progression_id'] for r in marks[:16])
        assert marks[16]['progression_id']!=marks[0]['progression_id']
        metrics[f'{vibe}-interactive']=stats
    _,drawing,_=capture(0,0,interaction=3)
    assert drawing['pcm_sha256']==metrics['0-0']['pcm_sha256'],'UI changes PCM'
    (OUT/'metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    print('PASS: G1 catalog-backed Repeat/Jam, bounded changes, Vibe queue, manual ARP random repeat and drawing isolation.')
if __name__=='__main__':main()
