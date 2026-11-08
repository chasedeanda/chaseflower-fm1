#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Reuse CoralFlower's installer, package validation, backup and recovery UI."""
from pathlib import Path
import hashlib
import re
import shutil

root = Path(__file__).resolve().parents[1]
out = root / 'build/play-site'
source = (root / 'web/index.html').read_text()
head = source[:source.index('<body>')]
head = head.replace('<head>', '<head>\n<base href="../">')
head = head.replace('CoralFlower', 'CORD').replace('Copyright (C) 2026 CORD contributors', 'Copyright (C) 2026 CoralFlower contributors')
head = head.replace('and a pixel flower grows while you play', 'with live chord guides, drums and editable progressions').replace('A pixel flower grows while you play.', 'Explore chords, drums and your own phrases.').replace('images/home.png', 'images/cord-playing.gif')
hero = source[source.index('  <section class="hero"'):source.index('  <!-- 2 facts')]
hero = hero.replace('<span class="brand-a">Coral</span><span class="brand-b">Flower</span>', 'Install CORD')
hero = hero.replace('Your M-VAVE&nbsp;FM-1, made for chords.', 'Install the latest published CORD release on your M-VAVE FM-1.')
hero = hero.replace('images/grow.gif', 'images/cord-playing.gif').replace('width="480" height="280"', 'width="496" height="270"').replace('A pink pixel flower grows on the FM-1\'s screen as chords are played', 'CORD Devil and Cat during progression playback')
hero = hero.replace('CoralFlower', 'CORD')
tools = source[source.index('        <details class="fold" id="backup"'):source.index('  <!-- 7 FAQ')]
tools = '<section class="section"><div class="container container--narrow"><h2 class="display-2">Files, backups and recovery</h2><div class="tools">' + tools
# Preserve the updater's complete module, including all validation and backup guards.
module = re.search(r'<script type="module">[\s\S]*?</script>', source)[0].replace('CoralFlower', 'CORD')
module = module.replace('installV: "Install on your FM-1"', 'installV: "Install {name} now"').replace('installNone: "Install on your FM-1"', 'installNone: "Install latest release"')
page_script = source[source.index('<script>\n/* The page:'):source.index('</body>', source.index('<script>\n/* The page:'))]
header = '<body class="cord-player"><header class="site-header"><div class="container site-header__row"><a class="brand" href="play/">CORD · FM-1</a><nav class="site-nav"><a href="play/">Play in browser</a><a href="https://github.com/chasedeanda/cord-fm1/releases">Releases</a></nav></div></header><main>'
footer = '</main><footer class="site-footer"><div class="container"><p>Installer by CoralFlower / ChoralRoot / Felucca contributors. <a href="LICENSE">GPL-3.0</a> · <a href="LICENSING.md">Credits and licences</a></p></div></footer>'
text = head + header + hero + tools + footer + module + page_script + '</body></html>'
for filename in ('fm1pkg.js', 'fm1ota.js', 'fm1backup.js'):
    shutil.copyfile(root / 'web' / filename, out / filename)
    digest = hashlib.sha256((out / filename).read_bytes()).hexdigest()[:10]
    text = text.replace('from "./' + filename + '"', 'from "./' + filename + '?v=' + digest + '"')
ids = re.findall(r'\bid="([^"]+)"', text)
assert len(ids) == len(set(ids)), 'Duplicate installer IDs'
assert set(re.findall(r'\$\("([^"]+)"\)', module + page_script)) <= set(ids), 'Missing installer controls'
(out / 'install').mkdir(exist_ok=True)
(out / 'install/index.html').write_text(text)
(out / 'images').mkdir(exist_ok=True)
shutil.copyfile(root / 'docs/images/house-style/mascots-playing.gif', out / 'images/cord-playing.gif')
print('Packaged CORD installer with all updater, file, backup and recovery controls')
