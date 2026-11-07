#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Known-size ELF fixtures and provenance failures; no device tools required."""
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import audit_resources as audit
import build


def fixture():
    names = ['.text', '.data', '.bss', '.pool', '.ram_text', '.c1_text', '.noinit', '.shstrtab', '.strtab', '.symtab']
    shstr = b'\0' + b'\0'.join(x.encode() for x in names) + b'\0'
    strtab = b'\0fl_px\0some_function\0'
    symtab = bytes(16) + struct.pack('<IIIBBH', 1, 0x01C20000, 8, 1, 0, 4) + struct.pack('<IIIBBH', 7, 0x02001000, 16, 2, 0, 1)
    sizes = [16, 4, 8, 64, 4, 4, 4, len(shstr), len(strtab), len(symtab)]
    types = [1, 1, 8, 8, 1, 1, 8, 3, 3, 2]
    chunks = [bytes(16), bytes(4), b'', b'', bytes(4), bytes(4), b'', shstr, strtab, symtab]
    data = bytearray(52); headers = [bytes(40)]
    for i, name in enumerate(names):
        offset = len(data); data.extend(chunks[i])
        headers.append(struct.pack('<10I', shstr.index(name.encode()), types[i], 2 if i < 7 else 0,
                                   0x01C20000 if i == 3 else 0x02001000, offset, sizes[i], 9 if i == 9 else 0,
                                   0, 4, 16 if i == 9 else 0))
    shoff = len(data); data.extend(b''.join(headers))
    ident = b'\x7fELF\x01\x01\x01' + bytes(9)
    struct.pack_into('<16sHHIIIIIHHHHHH', data, 0, ident, 2, 0, 1, 0, 0, shoff, 0, 52, 0, 0, 40, len(headers), 8)
    return data


class Resources(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(); self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name); self.elf = self.root / 'test.elf'; self.elf.write_bytes(fixture())
        self.patch = patch.object(audit, 'ROOT', self.root); self.patch.start(); self.addCleanup(self.patch.stop)
        dep = self.root / 'firmware/a file.c'; dep.parent.mkdir(); dep.write_text('example')
        (self.root / 'dependencies.d').write_text('unit.o: firmware/a\\ file.c\n')
        (self.root / 'macros.txt').write_text('#define CR_CPU1 1\n')
        self.identity = {'revision': 'fixture', 'elf_sha256': audit.sha(self.elf),
                         'memory_regions': {x: {'capacity': 1024, 'origin': 0} for x in ['RAM','POOL','XIP','RAMTEXT','NOINIT']},
                         'dependency_sha256': {'firmware/a file.c': audit.sha(dep)}}
        (self.root / 'identity.json').write_text(json.dumps(self.identity))

    def test_known_allocations_and_escaped_dependency(self):
        report = audit.make_report(self.elf, self.root)
        self.assertEqual(report['regions']['ordinary_ram']['bytes'], 12)
        self.assertEqual(report['regions']['pool']['bytes'], 64)
        self.assertEqual(report['regions']['xip']['bytes'], 28)
        self.assertEqual(report['regions']['ram_code']['bytes'], 8)
        self.assertEqual(report['regions']['ordinary_ram']['headroom'], 1012)
        self.assertEqual(report['allocation_groups']['scene_allocations_gross_not_net_savings']['bytes_by_section'], {'.pool': 8})
        self.assertEqual(report['compilation_dependencies'], ['firmware/a file.c'])
        self.assertIn('| ordinary_ram | 12 | 1,024 | 1,012 |', audit.render(report))

    def test_refuse_wrong_elf_or_changed_dependency(self):
        self.elf.write_bytes(self.elf.read_bytes() + b'changed')
        with self.assertRaisesRegex(ValueError, 'ELF does not match'): audit.make_report(self.elf, self.root)
        self.elf.write_bytes(fixture()); (self.root / 'firmware/a file.c').write_text('changed')
        with self.assertRaisesRegex(ValueError, 'Dependency contents changed'): audit.make_report(self.elf, self.root)

    def test_refuse_invalid_elf_headers(self):
        cases = [b'not ELF', fixture()[:50], fixture()[:-1]]
        wrong = fixture(); wrong[4] = 2; cases.append(wrong)  # host ELF64 is not a device image
        wrong = fixture(); wrong[5] = 2; cases.append(wrong)
        for data in cases:
            with self.subTest(length=len(data)):
                self.elf.write_bytes(data)
                with self.assertRaises(ValueError): audit.read_elf(self.elf)

    def test_refuse_stripped_symbols(self):
        data = fixture(); header = struct.unpack_from('<16sHHIIIIIHHHHHH', data)
        struct.pack_into('<I', data, header[6] + 10 * 40 + 4, 1)  # remove SHT_SYMTAB
        self.elf.write_bytes(data)
        with self.assertRaisesRegex(ValueError, 'Unstripped ELF'): audit.read_elf(self.elf)

    def test_target_linker_units(self):
        path = Path(__file__).resolve().parents[1] / 'firmware/app.ld'
        regions = audit.linker_regions(path)
        self.assertEqual(regions['RAM'], {'origin': 0x01C08000, 'capacity': 98304})
        self.assertEqual(regions['POOL']['capacity'], 344064)
        self.assertEqual(regions['XIP']['capacity'], 573440)

    def test_canonical_flags_are_used_by_builder(self):
        class Captured(Exception): pass
        def capture(*commands):
            self.assertEqual(commands[-1][:len(build.app_flags()) + 1], ('cc', *build.app_flags()))
            raise Captured
        with patch.dict('os.environ', {'FELUCCA_UAC': '0', 'CR_FX_REVERB_DS': '1'}):
            flags = build.app_flags()
            self.assertIn('-DFELUCCA_UAC=0', flags)
            self.assertIn('-DCR_FX_REVERB_DS=1', flags)
            with patch.object(build, 'tc_all', capture):
                with self.assertRaises(Captured): build.build_app(b'')


if __name__ == '__main__': unittest.main()
