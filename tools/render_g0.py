#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Reproducible real-firmware G0 MIDI/audio checks and dry/wet patch audition.

Run tools/test_g0.sh after tools/build.py --gen-only. Output: build/g0.
--ab-only renders only the same-arrangement Ghost Choir/Breath Pad comparison.
WAVs are the first 16 bars. Patch pairs share the lower measured RMS; no patch
parameters are edited. All other metrics are unnormalized renderer output.
"""
import argparse
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
OUT=ROOT/'build/g0'
FS=44100
CTL=32
FIRST_CYCLE=int(FS*16*4*60/104)*2

def run(kind,value):
    name=f'{kind}-{value}'
    raw=OUT/f'{name}.raw';events=OUT/f'{name}.csv'
    result=subprocess.run([str(ROOT/'build/host/cf_g0_test'),'--capture',kind,str(value),str(raw),str(events)],
                          cwd=ROOT,text=True,capture_output=True)
    (OUT/f'{name}.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError(result.stdout+result.stderr)
    data=array.array('i');data.frombytes(raw.read_bytes())
    if sys.byteorder!='little':data.byteswap()
    data=array.array('h',(max(-32768,min(32767,x>>8)) for x in data[:FIRST_CYCLE]))
    rows=list(csv.DictReader(events.open()))
    stats={'peak':max(map(abs,data)), 'rms':math.sqrt(sum(x*x for x in data)/len(data)),
           'raw_sha256':hashlib.sha256(raw.read_bytes()).hexdigest(), 'samples_per_channel':len(data)//2,
           'event_sha256':hashlib.sha256(events.read_bytes()).hexdigest()}
    raw.unlink()
    return data,rows,stats

def wav(name,data):
    with wave.open(str(OUT/f'{name}.wav'),'wb') as w:
        w.setnchannels(2);w.setsampwidth(2);w.setframerate(FS)
        copy=array.array('h',data)
        if sys.byteorder!='little':copy.byteswap()
        w.writeframes(copy.tobytes())

def chord_ons(rows):return [r for r in rows if int(r['status'])==144 and int(r['velocity'])]
def signature(rows):return [(r['sample'],r['status'],r['note'],r['velocity']) for r in rows]

def check_arp(rows,beats,gate=70,mode='up',octaves=1,swing=50):
    ons=chord_ons(rows)
    assert all(any(int(r['bar'])==b for r in ons) for b in (1,5,9,13))
    # Check actual attacks on the 96-PPQN grid for all four cycles, with one
    # control block of renderer quantization at onsets and 1/24-beat swing rounding.
    unit=beats*24;pair=2*unit;second=int(pair*swing/100+0.5)
    interval=FS*60/104*beats
    for a,b in zip(ons,ons[1:]):
        same_slot=(int(a['cycle']), (int(a['bar'])-1)//2)==(int(b['cycle']), (int(b['bar'])-1)//2)
        if same_slot and swing==50:assert abs(int(b['sample'])-int(a['sample'])-interval)<=2*CTL
    first=[r for r in ons if int(r['cycle'])==0 and int(r['bar'])<=2]
    pitches=[int(r['note']) for r in first]
    assert pitches[0]==(66 if mode=='down' else 52)
    assert (max(pitches)>66)==(octaves==2)
    if swing!=50:
        intervals=[int(b['sample'])-int(a['sample']) for a,b in zip(first,first[1:])]
        assert max(intervals)-min(intervals)>FS*0.1
    # Every note is actually released; inspect the first non-boundary gate.
    for a in first[1:5]:
        off=next(r for r in rows if int(r['sample'])>int(a['sample']) and int(r['status'])==128 and r['note']==a['note'])
        duration=int(off['sample'])-int(a['sample'])
        assert duration>0
        if swing==50:assert abs(duration-interval*gate/100)<=2*CTL
    return len(ons)

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--ab-only',action='store_true');args=ap.parse_args()
    OUT.mkdir(parents=True,exist_ok=True);metrics={};patch={}
    for kind in ('patchdry','patch'):
        for i,label in enumerate(('ghost-choir','breath-pad')):
            data,rows,stats=run(kind,i);metrics[f'{kind}-{i}']=stats;patch[kind,i]=(data,rows)
        assert signature(patch[kind,0][1])==signature(patch[kind,1][1]),'A/B musical event schedules differ'
        target=min(metrics[f'{kind}-{i}']['rms'] for i in range(2))
        for i,label in enumerate(('ghost-choir','breath-pad')):
            scale=target/metrics[f'{kind}-{i}']['rms']
            matched=array.array('h',(round(x*scale) for x in patch[kind,i][0]))
            rms=math.sqrt(sum(x*x for x in matched)/len(matched))
            metrics[f'{kind}-{i}'].update(level_match_gain=scale,matched_rms=rms,matched_peak=max(map(abs,matched)))
            wav(f'{label}-'+('dry' if kind=='patchdry' else 'wet'),matched)
        assert abs(20*math.log10(metrics[f'{kind}-0']['matched_rms']/metrics[f'{kind}-1']['matched_rms']))<0.01
    if not args.ab_only:
        controls={}
        for kind,values in [('energy',(20,55,100)),('space',(0,50,100)),('feel',(0,24,25,64,65,100)),
                            ('arp',(2,4)),('manual',(0,1,2,4,8)),('live',(0,))]:
            for value in values:
                data,rows,stats=run(kind,value);key=f'{kind}-{value}';metrics[key]=stats;controls[kind,value]=(data,rows)
                wav(key,data)
        # Attack density across the exact Feel regions, all eligible stages.
        counts={f:len({r['sample'] for r in chord_ons(controls['feel',f][1])}) for f in (0,24,25,64,65,100)}
        assert counts[0]==counts[24] and counts[25]==counts[64] and counts[65]==counts[100]
        assert counts[25]>counts[0] and counts[65]>counts[25]
        assert signature(controls['feel',65][1])==signature(controls['arp',4][1])
        for rate,beats in ((2,1),(4,0.5)):check_arp(controls['arp',rate][1],beats)
        assert len(chord_ons(controls['arp',4][1]))==2*len(chord_ons(controls['arp',2][1]))
        for value in (0,1,2,4,8):
            check_arp(controls['manual',value][1],0.5,30 if value&2 else 80,
                      'down' if value&1 else 'up',2 if value&4 else 1,65 if value&8 else 50)
        for kind in ('energy','space'):
            values=(20,55,100) if kind=='energy' else (0,50,100)
            assert len({metrics[f'{kind}-{v}']['raw_sha256'] for v in values})==3
            assert all(signature(controls[kind,v][1])==signature(controls[kind,values[0]][1]) for v in values)
        # A repeat process must emit identical PCM and MIDI, including the opening.
        _,_,repeat=run('energy',55);assert repeat==metrics['energy-55']
        metrics['feel_attack_counts_64_bars']=counts
    (OUT/'metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
    print('PASS: G0 real MIDI timing/sequence/gates, control audio, deterministic repeat, level-matched dry/wet A/B.')

if __name__=='__main__':main()
