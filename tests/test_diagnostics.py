# SPDX-License-Identifier: GPL-3.0-only
import copy
import json
from pathlib import Path
import sys
import unittest
from unittest.mock import patch
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"tools"))
import diagnostics as d

class Diagnostics(unittest.TestCase):
    def test_real_firmware_wire(self):
        w=Path("build/diagnostics/metrics.syx").read_bytes()
        self.assertEqual(w[:5],bytes((240,125,70,76,80)))
        self.assertEqual(w[-1],247)
        v=d.decode(list(w[5:-1]))
        self.assertEqual(len(v),63)
        self.assertEqual((v["peak_work_us"],v["peak_pressure_us"],v["voices_chord"]),(2100,2230,1))
        self.assertEqual(v["hardware"],0)
        Path("build/diagnostics/host-fixture.json").write_text(json.dumps(dict(format="chaseflower-performance",version=1,
            firmware="host fixture",revision="test",workload="synthetic",samples=[v]),indent=2)+"\n")

    def test_bad_frames(self):
        for r in ([],[1,63],[2,63]+[0]*315,[1,63]+[128]*315,[1,63]+[0]*314,[1,63]+[0,0,0,0,16]+[0]*310):
            with self.assertRaises(ValueError):d.decode(r)
        self.assertEqual(len(d.decode([1,64]+[0]*320)),63)

    def test_capture_disconnect_closes_link(self):
        import fm1_install as fi
        from types import SimpleNamespace
        class FakeLink:
            closed=False
            def close(self):self.closed=True
            def request(self,*args):raise fi.InstallError("lost","disconnected")
        link=FakeLink();link.link=link
        with patch.object(fi,"open_backup",return_value=(link,("fixture","coralflower"))):
            with self.assertRaisesRegex(ValueError,"disconnected"):
                d.capture(SimpleNamespace(port=None,seconds=0))
        self.assertTrue(link.closed)

    def test_comparison(self):
        s=dict.fromkeys(d.FIELDS,0);s.update(hardware=1,session_ms=30000,uptime_ms=40000,peak_work_us=1000,ram_bytes=94000)
        a=dict(workload="pad-six-notes",samples=[s]);b=copy.deepcopy(a)
        b["samples"][0].update(peak_work_us=900,ram_bytes=94100)
        notes,rows=d.comparison(a,b);self.assertFalse(notes)
        self.assertEqual(next(r for r in rows if r["metric"]=="peak_work_us")["delta"],-100)
        self.assertEqual(next(r for r in rows if r["metric"]=="ram_bytes")["delta"],100)
        b["workload"]="different";self.assertTrue(d.comparison(a,b)[0])
        b["samples"][0]["hardware"]=0
        with self.assertRaises(ValueError):d.comparison(a,b)

    def test_reset_during_capture(self):
        s=dict.fromkeys(d.FIELDS,0);s.update(hardware=1,session_ms=100,uptime_ms=100)
        a=dict(workload="x",samples=[s,dict(s,session_ms=1,uptime_ms=101)])
        with self.assertRaises(ValueError):d.comparison(a,a)

if __name__=="__main__":unittest.main()
