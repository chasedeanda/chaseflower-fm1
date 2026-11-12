#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Mirror the latest published main-line RAKT package for same-origin installation."""
import hashlib
import json
import os
from pathlib import Path
import re
from urllib.error import HTTPError
from urllib.request import Request, urlopen

REPO = 'chasedeanda/rakt-fm1'

def api(path):
    headers = {'Accept': 'application/vnd.github+json', 'User-Agent': 'RAKT-Pages'}
    if os.environ.get('GITHUB_TOKEN'):
        headers['Authorization'] = 'Bearer ' + os.environ['GITHUB_TOKEN']
    with urlopen(Request('https://api.github.com/repos/' + REPO + path, headers=headers), timeout=45) as response:
        return json.load(response)

def download(url):
    if not url.startswith('https://github.com/' + REPO + '/releases/download/'):
        raise ValueError('Release asset is outside the RAKT repository')
    with urlopen(Request(url, headers={'User-Agent': 'RAKT-Pages'}), timeout=60) as response:
        return response.read()

def main():
    out = Path(__file__).resolve().parents[1] / 'build/play-site/firmware'
    try:
        release = api('/releases/latest')
    except HTTPError as error:
        if error.code != 404:
            raise
        out.mkdir(parents=True, exist_ok=True)
        (out / 'releases.json').write_text('{"releases": []}\n')
        print('No RAKT release yet. The installer can use a local package.')
        return
    tag = release['tag_name']
    if release['draft'] or release['prerelease'] or not re.fullmatch(r'v[0-9]+\.[0-9]+(?:\.[0-9]+)?(?:-[A-Za-z0-9.]+)?', tag):
        raise ValueError('Expected a published firmware release')
    if api('/compare/' + tag + '...main')['status'] not in ('ahead', 'identical'):
        raise ValueError('The release is not on main')
    version = tag[1:]
    assets = {a['name']: a for a in release['assets']}
    filename = 'rakt-' + version + '.fwsc'
    if filename not in assets:
        raise ValueError('The release is missing its RAKT firmware package')
    sums = download(assets['SHA256SUMS']['browser_download_url']).decode()
    expected = next(line.split()[0] for line in sums.splitlines() if line.split()[-1].lstrip('*') == filename)
    package = download(assets[filename]['browser_download_url'])
    actual = hashlib.sha256(package).hexdigest()
    if actual != expected or len(package) != assets[filename]['size']:
        raise ValueError('Firmware checksum or size mismatch')
    digest = assets[filename].get('digest')
    if digest and digest != 'sha256:' + actual:
        raise ValueError('GitHub asset digest mismatch')
    out.mkdir(parents=True, exist_ok=True)
    (out / filename).write_bytes(package)
    (out / 'SHA256SUMS').write_text(actual + '  ' + filename + '\n')
    (out / 'releases.json').write_text(json.dumps({'releases': [{
        'version': version, 'tag': tag, 'sha256': actual, 'pkg': filename,
        'date': release['published_at'][:10], 'title': 'RAKT ' + version,
        'notes': ['Published firmware from main. The browser preview may contain newer changes.'],
    }]}, indent=2) + '\n')
    print(f'Installer: {tag}, {len(package)} bytes, SHA-256 {actual}')

if __name__ == '__main__':
    main()
