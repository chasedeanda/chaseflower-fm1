#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Mirror the latest published main-line CORD package for same-origin installation."""
import hashlib
import json
import os
from pathlib import Path
import re
from urllib.request import Request, urlopen

REPO = 'chasedeanda/cord-fm1'

def api(path):
    headers = {'Accept': 'application/vnd.github+json', 'User-Agent': 'CORD-Pages'}
    if os.environ.get('GITHUB_TOKEN'):
        headers['Authorization'] = 'Bearer ' + os.environ['GITHUB_TOKEN']
    with urlopen(Request('https://api.github.com/repos/' + REPO + path, headers=headers), timeout=45) as response:
        return json.load(response)

def download(url):
    if not url.startswith('https://github.com/' + REPO + '/releases/download/'):
        raise ValueError('Release asset is outside the CORD repository')
    with urlopen(Request(url, headers={'User-Agent': 'CORD-Pages'}), timeout=60) as response:
        return response.read()

def main():
    release = api('/releases/latest')
    tag = release['tag_name']
    if release['draft'] or release['prerelease'] or not re.fullmatch(r'v[0-9]+\.[0-9]+(?:\.[0-9]+)?(?:-[A-Za-z0-9.]+)?', tag):
        raise ValueError('Expected a published firmware release')
    if api('/compare/' + tag + '...main')['status'] not in ('ahead', 'identical'):
        raise ValueError('The release is not on main')
    version = tag[1:]
    filename = 'coralflower-' + version + '.fwsc'
    assets = {a['name']: a for a in release['assets']}
    sums = download(assets['SHA256SUMS']['browser_download_url']).decode()
    expected = next(line.split()[0] for line in sums.splitlines() if line.split()[-1].lstrip('*') == filename)
    package = download(assets[filename]['browser_download_url'])
    actual = hashlib.sha256(package).hexdigest()
    if actual != expected or len(package) != assets[filename]['size']:
        raise ValueError('Firmware checksum or size mismatch')
    digest = assets[filename].get('digest')
    if digest and digest != 'sha256:' + actual:
        raise ValueError('GitHub asset digest mismatch')
    out = Path(__file__).resolve().parents[1] / 'build/play-site/firmware'
    out.mkdir(parents=True, exist_ok=True)
    (out / filename).write_bytes(package)
    (out / 'SHA256SUMS').write_text(actual + '  ' + filename + '\n')
    (out / 'releases.json').write_text(json.dumps({'releases': [{
        'version': version, 'tag': tag, 'sha256': actual, 'pkg': filename,
        'date': release['published_at'][:10], 'title': 'CORD ' + version,
        'notes': ['Published firmware from main. The browser preview may contain newer changes.'],
    }]}, indent=2) + '\n')
    print(f'Installer: {tag}, {len(package)} bytes, SHA-256 {actual}')

if __name__ == '__main__':
    main()
