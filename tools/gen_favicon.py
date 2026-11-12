#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render the browser's PNG icon variants from the RAKT rack badge."""
from pathlib import Path
import subprocess

out = Path(__file__).resolve().parents[1] / 'tools/emu/web'
for size, name in ((32, 'favicon-32.png'), (180, 'apple-touch-icon.png')):
    subprocess.run([
        'rsvg-convert', '-w', str(size), '-h', str(size), str(out / 'favicon.svg'),
        '-o', str(out / name),
    ], check=True)
