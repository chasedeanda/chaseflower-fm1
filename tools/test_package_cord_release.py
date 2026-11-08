#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Check modern/legacy release names and refuse bad/unmerged packages."""
import contextlib
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import tempfile
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('cord_release', Path(__file__).with_name('package_cord_release.py'))
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)
package = b'fixture firmware package'
digest = hashlib.sha256(package).hexdigest()
for prefix, version in [('cord', '1.0'), ('coralflower', '0.6-chase14')]:
    name = f'{prefix}-{version}.fwsc'
    base = f'https://github.com/chasedeanda/cord-fm1/releases/download/v{version}/'
    release = {'tag_name': 'v'+version, 'draft': False, 'prerelease': False,
               'published_at': '2026-11-08T00:00:00Z',
               'assets': [{'name': name, 'size': len(package), 'digest': 'sha256:'+digest, 'browser_download_url': base+name},
                          {'name': 'SHA256SUMS', 'browser_download_url': base+'SHA256SUMS'}]}
    for valid, status in [(True, 'ahead'), (False, 'ahead'), (True, 'diverged')]:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            def api(path):
                return release if path == '/releases/latest' else {'status': status}
            def download(url):
                return (digest+'  '+name+'\n').encode() if url.endswith('SHA256SUMS') else package if valid else b'damaged'
            with patch.object(m, '__file__', str(root/'tools/package_cord_release.py')), patch.object(m, 'api', api), patch.object(m, 'download', download), contextlib.redirect_stdout(io.StringIO()):
                try:
                    m.main()
                    assert valid and status == 'ahead', 'Bad release accepted'
                    data = json.loads((root/'build/play-site/firmware/releases.json').read_text())
                    assert data['releases'][0]['pkg'] == name
                    assert (root/'build/play-site/firmware'/name).read_bytes() == package
                except ValueError:
                    assert not valid or status != 'ahead', 'Valid release refused'
print('PASS: CORD and legacy packages; corrupt checksums and releases outside main rejected')
