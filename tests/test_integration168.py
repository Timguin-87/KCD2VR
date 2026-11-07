"""Carry-forward contracts supplement (not replace) executable bridge tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib
import json
import unittest
ROOT=Path(__file__).parents[1]
def source(name):return read_source(ROOT/name)

class Integration168Tests(unittest.TestCase):
    def test_accepted_boundaries_unchanged_from_user_baseline(self):
        contract=json.loads((ROOT/'tests/protected_baseline171.json').read_text())
        self.assertGreaterEqual(len(contract['files']),5)
        for name,expected in contract['files'].items():
            data=(ROOT/name).read_bytes().replace(b'\r\n',b'\n')
            self.assertEqual(hashlib.sha256(data).hexdigest(),expected,name)

    def test_native_request_is_not_windows_key_fallback(self):
        s=source('src/mod.cpp')
        self.assertIn('SetNativeSpecialRoute168(true)',s)
        self.assertIn('RequestNativeSpecial168()',s)
        b=source('src/vrik/native_bridges168.inl')
        self.assertIn('PhysCallAction6698(owner.player,4,1)',b)
        self.assertIn('pressed&&PhysCallAction6698(owner.player,4,2)',b)
        for forbidden in ('keybd_event','SendInput(','BuildAndSubmitMeleeHit(','SetHealth('):self.assertNotIn(forbidden,b)

    def test_logging_is_not_guard_gameplay_switch(self):
        s=source('src/vrik/native_combat104.inl')
        start=s.index('static void NativeGuardTick112(')
        end=s.index('\n}',start)+2
        body=s[start:end]
        self.assertNotIn('capture168::active',body)
        self.assertIn('fresh168=f3==3',body)
        self.assertIn('!manual',body)
        self.assertIn('g.motion168.update',body)
        self.assertIn('PhysCallAction6698(owner.player,3,2)',body)
        self.assertIn('PhysCallAction6698(owner.player,3,1)',body)

    def test_master_rejects_only_at_optin_qualified_factory(self):
        s=source('src/vrik/native_bridges168.inl')
        self.assertIn('FocusedTestMode167()!=2',s)
        self.assertIn('fallbackType==master',s)
        self.assertIn('if(nativeResult&&!permit)gpr(ctx,Gpr::Rax)&=~std::uintptr_t(255)',s)
        self.assertIn('fail_if_already_hooked=true',s)
        self.assertIn('retail_build112::Verify(b)',s)
        self.assertEqual(s.count('{0x1483A'),10)
        self.assertIn('!tracking.head_held&&!tracking.right_held',s)

    def test_contact_mode_not_diagnostic_domain_owns_body_proxy(self):
        s=source('src/vrik/coherent_pose.inl')
        self.assertIn('selfContact167',s)
        # Body proxy activation no longer relies on enabling verbose F5 logs.
        line=next(x for x in s.splitlines() if 'const bool selfContact167=' in x)
        self.assertIn('collisionMode',line)
        self.assertNotIn('F5',line.split('//')[0])

if __name__=='__main__':unittest.main()
