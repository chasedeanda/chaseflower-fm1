#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Package the existing FM-1 WASM player without the inherited installer/release list."""
from pathlib import Path
import hashlib, json, re, shutil, subprocess
root=Path(__file__).resolve().parents[1]
out=root/'build/play-site'; out.mkdir(parents=True,exist_ok=True)
play=out/'play';play.mkdir(exist_ok=True)
for name in ('index.html','emu.js','keymap.js','worklet.js','coralflower.wasm'):
    shutil.copyfile(root/'build/emu-web'/name,play/name)
shutil.copyfile(root/'web/site.css',out/'site.css')
shutil.copytree(root/'web/fonts',out/'fonts',dirs_exist_ok=True)
for name in ('favicon.svg','favicon-32.png','apple-touch-icon.png'):
    shutil.copyfile(root/'tools/emu/web'/name,out/name)
for name in ('LICENSE','LICENSING.md'):
    shutil.copyfile(root/name,out/name)
shutil.copytree(root/'LICENSES',out/'LICENSES',dirs_exist_ok=True)
sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip()
source=f'https://github.com/chasedeanda/cord-fm1/tree/{sha}'
text=(play/'index.html').read_text()
head=text[:text.index('<body>')].replace('Play CoralFlower in your browser','Play CORD in your browser').replace('CoralFlower','CORD').replace('../images/home.png','../images/cord-playing.gif')
head=head.replace('<title>Play CORD in your browser</title>', '<title>CORD · Your songwriting companion</title>')
head=re.sub(r'(<meta (?:name="description"|property="og:description") content=")[^"]*(">)', r'\1A songwriting companion for your browser and FM-1. Pick a chord progression, add drums and bass, edit the chords and save your ideas.\2', head)
head=head.replace('content="Play CORD in your browser"', 'content="CORD · Your songwriting companion"')
main=text[text.index('<main id="main">'):text.index('  <section class="section scope-accent final"')]
main=main.replace('Play it in your browser','CORD, your songwriting companion').replace('This is the real CoralFlower firmware, running in your browser with sound.','Your custom FM-1 firmware, running in your browser with sound.')
main=main.replace('<p class="lead">Your custom FM-1 firmware, running in your browser with sound. Tap or click the keys, or use your computer keyboard.</p>', '<p class="lead">Pick a chord progression, add drums and bass, and play along. Change a chord, try a different groove, or save the idea for later.</p><div class="actions"><a class="btn btn--secondary btn--sm" href="#quick-start">Quick start ↓</a><a class="btn btn--secondary btn--sm" href="#keys-title">Computer controls</a><a class="btn btn--secondary btn--sm" href="../install/">Install on your FM-1</a></div>')
assert '  <section class="section" aria-labelledby="keys-title">' in main, 'Player controls section missing'
main=main.replace('  <section class="section" aria-labelledby="keys-title">', (root/'web/cord-play-guide.html').read_text() + '\n  <section class="section" aria-labelledby="keys-title">')

header=f'''<body class="cord-player"><header class="site-header"><div class="container site-header__row"><a class="brand" href="#play-title">CORD · FM-1</a><nav class="site-nav" aria-label="Play page"><a href="#quick-start">Quick start</a><a href="#explore">Features</a><a href="#keys-title">Controls</a><a href="https://github.com/chasedeanda/cord-fm1#readme">Full guide ↗</a><a href="{source}">Source</a></nav></div></header>'''
footer=f'''</main><footer class="site-footer"><div class="container"><p>Development preview · <a href="{source}">{sha[:7]}</a>. Browser performance does not measure device CPU headroom.</p><p>Built on CoralFlower, ChoralRoot FM-1, Felucca and Melodee. <a href="../LICENSE">GPL-3.0</a> · <a href="../LICENSING.md">Credits and licences</a>.</p></div></footer><script src="keymap.js"></script><script src="emu.js"></script></body></html>'''
# Keep the build script's cache-busting query strings.
for filename in ('keymap.js','emu.js'):
    match=re.search(r'src="('+re.escape(filename)+r'\?[^\"]+)"',text)
    if match:footer=footer.replace('src="'+filename+'"','src="'+match.group(1)+'"')
# Refresh styles when this guide changes, including through browser/CDN caches.
css_hash=hashlib.sha256((out/'site.css').read_bytes()).hexdigest()[:10]
head=head.replace('href="../site.css"', f'href="../site.css?v={css_hash}"')
(play/'index.html').write_text(head+header+main+footer)
(out/'index.html').write_text('<!doctype html><meta charset="utf-8"><meta http-equiv="refresh" content="0;url=play/"><title>Play CORD</title><a href="play/">Play CORD</a>')
(out/'.nojekyll').touch()
(out/'build.json').write_text(json.dumps({'revision':sha,'wasm_sha256':hashlib.sha256((play/'coralflower.wasm').read_bytes()).hexdigest()},indent=2)+'\n')
print(f'Browser site: {out} · source {sha[:7]}')
subprocess.run(['python3', str(root/'tools/make_cord_installer.py')], check=True)
