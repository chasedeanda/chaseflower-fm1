#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Export the native Devil row-span drawing as the browser favicon.

Run tools/build.py --gen-only first. PNG fallbacks are rendered from this SVG.
"""
from pathlib import Path
import re
import subprocess

root = Path(__file__).resolve().parents[1]
header = (root / 'build/gen/cf_mascot_art.h').read_text()
out = root / 'tools/emu/web'

def values(name):
    return [int(v) for v in re.search(r'\b' + name + r'\[\] = \{(.*?)\};', header, re.S)[1].replace('\n', '').split(',') if v.strip()]

colors = values('CFM_DEVIL_RGB')
rows = values('CFM_DEVIL_ROWS')
data = values('CFM_DEVIL_DATA')
paths = [[] for _ in colors]
points = []
for y in range(112):
    pos = rows[y]
    x, length = data[pos:pos + 2]
    pixels = data[pos + 2:pos + 2 + length]
    start = 0
    while start < length:
        color = pixels[start]
        end = start + 1
        while end < length and pixels[end] == color:
            end += 1
        if color:
            paths[color - 1].append(f'M{x+start} {y}h{end-start}v1h-{end-start}z')
            points.extend(((x + start, y), (x + end, y + 1)))
        start = end
left = min(x for x, y in points) - 3
top = min(y for x, y in points) - 3
size = max(max(x for x, y in points) - left, max(y for x, y in points) - top) + 3
svg = f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="{left} {top} {size} {size}"><title>CORD Devil</title>'
svg += ''.join(f'<path fill="#{color:06x}" d="{"".join(path)}"/>' for color, path in zip(colors, paths) if path)
svg += '</svg>\n'
(out / 'favicon.svg').write_text(svg)
for size, name in ((32, 'favicon-32.png'), (180, 'apple-touch-icon.png')):
    subprocess.run(['rsvg-convert', '-w', str(size), '-h', str(size), str(out / 'favicon.svg'), '-o', str(out / name)], check=True)
print('Exported native Devil SVG, 32px favicon and 180px touch icon')
