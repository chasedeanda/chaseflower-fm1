#!/usr/bin/env python3
"""Export actual native FX screens and a real-DSP playback animation. Requires Pillow."""
from pathlib import Path
import subprocess
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'previews/fx-screen'
FRAMES = ROOT / 'build/fx-screen-frames'

def main():
    OUT.mkdir(parents=True, exist_ok=True)
    FRAMES.mkdir(parents=True, exist_ok=True)
    binary = ROOT / 'build/host/cf_fx_test'
    if not binary.exists():
        raise SystemExit('Run sh tools/test_fx.sh first to build the native runner.')
    subprocess.run([str(binary), '--captures', str(FRAMES)], cwd=ROOT, check=True)
    for p in FRAMES.glob('*.ppm'):
        if not p.name.startswith('motion-'):
            Image.open(p).save(OUT / (p.stem + '.png'))
    frames = [Image.open(p).resize((480, 480), Image.Resampling.NEAREST)
              for p in sorted(FRAMES.glob('motion-*.ppm'))]
    assert len(frames) == 60
    # Test the response drawing itself, excluding the charging battery or text.
    assert len({im.crop((0, 240, 480, 336)).tobytes() for im in frames}) > 10
    frames[0].save(OUT / 'orbit-motion.gif', save_all=True, append_images=frames[1:],
                   duration=50, loop=0, optimize=False)
    board = Image.new('RGB', (960, 1440))
    for i, name in enumerate(['preset-10', 'preset-11', 'preset-12', 'preset-13',
                              'individual', 'browsing']):
        im = Image.open(OUT / (name + '.png')).resize((480, 480), Image.Resampling.NEAREST)
        board.paste(im, ((i % 2) * 480, (i // 2) * 480))
    board.save(OUT / 'screen-overview.png')
    print('Saved native screen captures and three-second real-DSP Orbit animation to', OUT)

if __name__ == '__main__':
    main()
