#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Rebuild native README screenshots and the two-character playback GIF.

Run tools/build.py --gen-only first. Requires Pillow and a host C compiler.
"""
from pathlib import Path
import json
import subprocess
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / 'build/house-style-readme'
OUT = ROOT / 'docs/images/house-style'
SCREENS = ('home', 'groove', 'effects', 'lab', 'editor', 'arp', 'display', 'songs')


def main():
    BUILD.mkdir(parents=True, exist_ok=True)
    OUT.mkdir(parents=True, exist_ok=True)
    binary = BUILD / 'capture'
    subprocess.run(['cc', '-std=gnu11', '-O1', '-w', '-Itools/emu/web/compat',
                    '-Ibuild/gen', '-Ifirmware/src', '-Itests',
                    'tools/capture_house_style.c', '-o', str(binary), '-lm'], cwd=ROOT, check=True)
    themes = []
    for skin, name in enumerate(('devil', 'cat')):
        frames = BUILD / name
        frames.mkdir(exist_ok=True)
        subprocess.run([str(binary), str(skin), str(frames)], cwd=ROOT, check=True)
        for screen in SCREENS:
            Image.open(frames / f'{screen}.ppm').save(OUT / f'{name}-{screen}.png', optimize=True)
        images = [Image.open(p).convert('RGB') for p in sorted(frames.glob('motion-*.ppm'))]
        assert len(images) == 160
        assert len({im.crop((1, 49, 116, 156)).tobytes() for im in images}) > 20
        themes.append(images)

    paired = []
    for devil, cat in zip(*themes):
        im = Image.new('RGB', (496, 270), '#17171a')
        draw = ImageDraw.Draw(im)
        draw.text((8, 7), 'Devil / Ember', fill='#ffa55b')
        draw.text((256, 7), 'Cat / Plum', fill='#f29bcd')
        im.paste(devil, (4, 26));im.paste(cat, (252, 26))
        paired.append(im)
    # One palette across every frame prevents frame-to-frame color shimmer.
    atlas = Image.new('RGB', (496, 270 * 16))
    for i in range(16):atlas.paste(paired[i * 10], (0, i * 270))
    palette = atlas.quantize(colors=256)
    indexed = [im.quantize(palette=palette, dither=Image.Dither.NONE) for im in paired]
    indexed[0].save(OUT / 'mascots-playing.gif', save_all=True, append_images=indexed[1:],
                    duration=50, loop=0, optimize=True, disposal=1)
    with Image.open(OUT / 'mascots-playing.gif') as gif:
        assert gif.size == (496, 270) and gif.n_frames == 160
        assert sum(gif.seek(i) or gif.info['duration'] for i in range(gif.n_frames)) == 8000
    (OUT / 'manifest.json').write_text(json.dumps({
        'renderer': 'tools/capture_house_style.c', 'themes': ['Devil / Ember', 'Cat / Plum'],
        'screens': list(SCREENS), 'native_size': [240, 240],
        'animation': {'file': 'mascots-playing.gif', 'frames': 160, 'fps': 20,
                      'duration_ms': 8000, 'source': 'native framebuffer during actual progression playback'},
    }, indent=2) + '\n')
    print(f'Saved 16 native screenshots and an eight-second mascot preview to {OUT}')


if __name__ == '__main__':
    main()
