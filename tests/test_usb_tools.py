"""Validate the actual firmware diagnostic wire reply against the Python hardware-check tools. No device access."""
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import fm1_install as fi
import uac_check


class UsbDiagnostics(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.wire = Path("build/usb-tests/diagnostics.syx").read_bytes()
        assert cls.wire[:5] == bytes((0xF0, 0x7D, 0x46, 0x4C, 79)) and cls.wire[-1] == 0xF7
        cls.reply = list(cls.wire[5:-1])

    def decode(self, reply):
        with patch.object(fi, "BackupLink", return_value=SimpleNamespace(request=lambda *a: reply)):
            return fi.usb_values(object())

    def test_wire_contract(self):
        d = self.decode(self.reply)
        self.assertEqual(len(d), len(fi.DEBUG_USB))
        self.assertEqual((d["ua_rate"], d["ms"], d["ua_cap_alt"], d["ua_opens"]), (48000, 12345, 1, 7))
        self.assertEqual(d["ua_packet_max"], 588)
        self.assertEqual(d["ua_48k_offered"], 1)
        self.assertIn("audio_stale", d)

    def test_legacy_reply_still_parses(self):
        d = self.decode([1, 14] + self.reply[2:2 + 5 * 14])
        self.assertEqual(len(d), 14)
        self.assertNotIn("ua_opens", d)

    def test_incomplete_reply_rejected(self):
        self.assertEqual(self.decode(self.reply[:-1]), {})

    def test_chaseflower_does_not_request_unsupported_legacy_debug(self):
        d = self.decode(self.reply)
        closed = []
        dev = SimpleNamespace(id=SimpleNamespace(loader=False), link=SimpleNamespace(close=lambda: closed.append(True)))
        up = SimpleNamespace(find=lambda: dev)
        with patch.object(fi, "usb_values", return_value=d), patch.object(fi, "debug_values", side_effect=AssertionError("73 is unsupported")):
            got, error = uac_check.counters(up)
        self.assertIsNone(error)
        self.assertEqual(got["ua_opens"], 7)
        self.assertEqual(closed, [True])


if __name__ == "__main__":
    unittest.main()
