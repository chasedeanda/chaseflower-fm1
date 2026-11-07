#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render draft FX recipes through real firmware; produce level-matched audition WAVs."""
import array
import csv
import json
import math
from pathlib import Path
import subprocess
import sys
import wave

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'previews/fx-candidates'

def main():
    bank=json.loads((OUT/'candidates.json').read_text())
    presets=bank['presets']
    assert len({p['id'] for p in presets})==len(presets)
    for p in presets:
        assert len(p['values'])==6 and 0<=p['on']<64
        for e,v in enumerate(p['values']):
            assert len(v)==3 and all(isinstance(x,int) and 0<=x<=100 for x in v)
            if e in (0,1):assert v[0]<=2
            if e==0:assert v[1]<=99
    build=ROOT/'build/fx-candidates';build.mkdir(parents=True,exist_ok=True)
    binary=build/'render'
    subprocess.run(['cc','-std=gnu11','-O2','-w','-Itools/emu/web/compat','-Ibuild/gen','-Ifirmware/src','-Itests',str(OUT/'render.c'),'-o',str(binary),'-lm'],cwd=ROOT,check=True)
    reports=[];audios=[]
    for p in presets:
        raw=build/(p['id']+'-raw.wav')
        cmd=[str(binary),str(raw),str(p['on'])]+[str(x) for v in p['values'] for x in v]
        result=subprocess.run(cmd,cwd=ROOT,capture_output=True,text=True,check=True)
        (build/(p['id']+'.log')).write_text(result.stdout+result.stderr)
        with wave.open(str(raw),'rb') as w:
            params=w.getparams();x=array.array('h',w.readframes(w.getnframes()))
        if sys.byteorder!='little':x.byteswap()
        # Match active phrase RMS, excluding the long tail, so extra ambience is not penalized.
        active=x[:int(4.4*44100)*2]
        rms=math.sqrt(sum(s*s for s in active)/len(active))
        peak=max(abs(s) for s in x)
        gain=min((32768*10**(-21/20))/rms,(32767*10**(-2/20))/peak)
        y=array.array('h',(round(s*gain) for s in x))
        out=OUT/(p['id']+'.wav')
        if sys.byteorder!='little':y.byteswap()
        with wave.open(str(out),'wb') as w:w.setparams(params);w.writeframes(y.tobytes())
        if sys.byteorder!='little':y.byteswap()
        matched=math.sqrt(sum(s*s for s in y[:len(active)])/len(active))
        assert peak>100 and max(abs(s) for s in y)<32767
        assert all(y!=z for z in audios),p['id']+' duplicates a previous render'
        audios.append(y)
        reports.append({'name':p['name'],'file':out.name,'start_seconds':round((len(reports))*9.25,2),'raw_peak_dbfs':round(20*math.log10(peak/32768),2),'audition_gain_db':round(20*math.log10(gain),2),'phrase_rms_dbfs':round(20*math.log10(matched/32768),2),'clipped_samples':0})
        print(p['name'],reports[-1],flush=True)
    with (OUT/'levels.csv').open('w',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=reports[0]);writer.writeheader();writer.writerows(reports)
    with wave.open(str(OUT/'sampler.wav'),'wb') as w:
        w.setparams(params)
        for i,y in enumerate(audios):
            if i:w.writeframes(bytes(round(.25*44100)*4))
            if sys.byteorder!='little':y.byteswap()
            w.writeframes(y.tobytes())
    print(f'Wrote {len(presets)} candidates and sampler to',OUT)

if __name__=='__main__':main()
