#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Repeatable CORD release checks: branding, compatibility, SDK provenance, secrets."""
import ast
import argparse
import hashlib
import json
from html.parser import HTMLParser
from pathlib import Path
import re
import subprocess
from build import release_brand

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--site', type=Path, help='Also audit the packaged player and installer')
args = parser.parse_args()
errors = []
def check(ok, message):
    print(('PASS: ' if ok else 'FAIL: ') + message)
    if not ok:
        errors.append(message)
def read(path):
    return (root / path).read_text()

usb = read('firmware/src/usb.c')
check('"CORD", "CORD FM-1", "CORD In"' in usb, 'USB product, maker and audio names are CORD')
check('"Chase FM-1"' not in read('firmware/src/cf_boot.c'), 'Optional legacy boot does not show the old product name')
check('"coralflower-fm1"' in read('tools/emu/web/emu.js'), 'Existing browser save namespace is retained')
check('file.format !== "felucca-backup"' in read('web/fm1backup.js'), 'Legacy backup envelope is retained')
check('PRODUCT = "FM-1_26730001"' in read('tools/build.py'), 'CORD has its own update package identity')
check('#define FELUCCA_VERSION "CORD ' in read('firmware/src/coralflower.c') and
      release_brand('1.0.3-dev.studio') == 'CORD' and release_brand('1.0.3') == 'CoralFlower',
      'Development INFO uses CORD and release INFO retains the legacy family')
for path in ['web/fm1backup.js', 'tools/fm1_install.py']:
    check('cord' in read(path) and '-backup-' in read(path), path + ': CORD backup filename is present')
    check(not re.search(r'(?:return |return f)["\`](?:ChaseFlower|CoralFlower) (?:settings|saved Song|working draft|recording|favorites|FX|Auto|loop|patterns|unsaved|pattern)', read(path)), path + ': owned backup labels use the public product name')
check('CoralFlower contributors' in read('firmware/src/coralflower.c'), 'Upstream copyright notice retained')
check('CORD is a fork of **CoralFlower**' in read('LICENSING.md'), 'Fork and upstream origin disclosed')
for path in ['LICENSE', 'LICENSES/GPL-3.0-only.txt', 'LICENSES/Apache-2.0.txt', 'assets/fonts/OFL.txt', 'web/fonts/OFL-InterTight.txt', 'web/fonts/OFL-DepartureMono.txt', 'assets/sounds/candidates/aquanode/LICENSE-CC0.txt']:
    # The canonical GPL text is LICENSE; some trees do not duplicate it in LICENSES.
    if path == 'LICENSES/GPL-3.0-only.txt' and not (root/path).exists():
        continue
    check((root/path).is_file(), 'Licence text included: ' + path)
tree = ast.parse(read('tools/build.py'))
expected = next(ast.literal_eval(n.value) for n in tree.body if isinstance(n, ast.Assign) and any(getattr(t, 'id', '') == 'SDK_SHA256' for t in n.targets))
for name, digest in expected.items():
    path = root/'tools/sdk/cpu/wl82/tools'/name
    check(path.is_file() and hashlib.sha256(path.read_bytes()).hexdigest() == digest, 'Pinned JieLi SDK file: ' + name)
# Deliberately a limited scan; never print matched secret values.
pattern = re.compile(rb'BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY|github_pat_[A-Za-z0-9_]{20}|ghp_[A-Za-z0-9]{25}|sk-proj-[A-Za-z0-9_-]{25}')
found = []
for filename in subprocess.check_output(['git', 'ls-files', '-z'], cwd=root).decode().split('\0'):
    if not filename or filename == 'tools/audit_public_release.py':
        continue
    path = root/filename
    if path.is_file() and path.stat().st_size < 2_000_000:
        data = path.read_bytes()
        if b'\0' not in data and pattern.search(data):
            found.append(filename)
check(not found, 'Limited tracked-file credential signature scan (file names only): ' + (', '.join(found) if found else 'no matches'))
if args.site:
    class Visible(HTMLParser):
        def __init__(self):
            super().__init__();self.hidden=0;self.parts=[]
        def handle_starttag(self, tag, attrs):
            if tag in ('script','style'):self.hidden+=1
        def handle_endtag(self, tag):
            if tag in ('script','style'):self.hidden-=1
        def handle_data(self, text):
            if not self.hidden:self.parts.append(text)
    for name in ['play/index.html', 'install/index.html']:
        html=(args.site/name).read_text()
        visible=Visible();visible.feed(html);text=' '.join(visible.parts)
        check(not re.search(r'ChaseFlower|Chase FM-1', text), name+': no retired product labels')
        check('CORD' in text, name+': public product is CORD')
        check(not re.search(r'wallets|donateTitle|iambemoli@gmail.com',html), name+': no inherited donation UI or upstream contact presented as CORD')
        check('CoralFlower' in text, name+': upstream attribution remains visible')
    manifest=json.loads((args.site/'firmware/releases.json').read_text())
    for release in manifest['releases']:
        pkg=args.site/'firmware'/release['pkg']
        check(hashlib.sha256(pkg.read_bytes()).hexdigest()==release['sha256'], 'Published installer package matches manifest: '+release['pkg'])
if errors:
    raise SystemExit(f'{len(errors)} release audit checks failed')
print('Public-release source checks passed. Physical-device acceptance and legal review are not certified by this script.')
