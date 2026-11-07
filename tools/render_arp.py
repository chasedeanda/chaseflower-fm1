#!/usr/bin/env python3
"""Render actual native ARP screens and animation; run tools/test_arp.sh first."""
from pathlib import Path
import subprocess
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
def main():
    frames=ROOT/'build/arp-tests'
    out=ROOT/'previews/arp-roll'
    out.mkdir(parents=True,exist_ok=True)
    binary=ROOT/'build/host/cf_arp_test'
    if not binary.exists():raise SystemExit('Run sh tools/test_arp.sh first.')
    subprocess.run([str(binary),'--captures',str(frames)],cwd=ROOT,check=True)
    for name in ['hero','pattern','timing','latch','waiting','four-octaves','pattern-9','pattern-10','pattern-11','pattern-12']:
        Image.open(frames/(name+'.ppm')).save(out/(name+'.png'))
    images=[Image.open(p).resize((480,480),Image.Resampling.NEAREST) for p in sorted(frames.glob('motion-*.ppm'))]
    assert len(images)==60
    assert len({im.crop((74,196,460,370)).tobytes() for im in images})>10
    images[0].save(out/'native-roll.gif',save_all=True,append_images=images[1:],duration=50,loop=0,optimize=False)
    print('Saved actual native ARP captures and three seconds of player-driven motion to',out)
if __name__=='__main__':main()
