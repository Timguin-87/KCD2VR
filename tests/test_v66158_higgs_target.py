import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parents[1]
MOD=read_source(ROOT/'src/mod.cpp')
FRIK=read_source(ROOT/'src/frik_pose_probe.cpp')
BODY=read_source(ROOT/'src/vrik/interaction_body122.inl')
HDR=read_source(ROOT/'src/frik_pose_probe.h')
class HiggsTarget158(unittest.TestCase):
    def test_marker(self):
        assert_current_identity(ROOT)
        self.assertIn('V66.158 HIGGS_TARGET', BODY)
    def test_hand_context_published(self):
        self.assertIn('SetInteractionHandContext158',HDR)
        self.assertIn('SetInteractionHandContext158(active158?side:-1',MOD)
        self.assertIn('g_interaction_hand_context158',FRIK)
    def test_probe_runs_across_f5_diagnostic_modes(self):
        self.assertIn('const auto f5=diagnostics::lab::Get(diagnostics::lab::Key::F5);',BODY)
        # Same legacy modes remain eligible; plain F5 capture adds observation without changing those modes.
        self.assertIn('if((f5>=1||cc::interactionOnly168r1(GetTickCount64()))&&g_interaction_body_contract122',BODY)
        self.assertIn('ArmInteractionBody122();',FRIK)
        self.assertIn('current==id',BODY)
        self.assertIn('w.entity!=target||w.id!=id||w.sequence!=sequence',BODY)
        self.assertIn('diagnostics::lab::Get(diagnostics::lab::Key::F5)<1',BODY)
    def test_classification_stays_conservative(self):
        self.assertIn('UNCLASSIFIED_RETAIL_BODY',BODY)
        self.assertIn('NONE_OR_UNREADABLE',BODY)
        self.assertIn('grabPoint=UNPROVEN constraint=NONE mutation=NONE',BODY)
        self.assertIn('rva==0x3A578C0',BODY)
        self.assertIn('CPhysicalEntity_BASE',BODY)
        self.assertNotIn('LOOSE_BODY',BODY)
    def test_action_and_hand_are_correlated(self):
        self.assertIn('WatchInteractionBody122(target,targetId,seq,action)',FRIK)
        self.assertIn("action='%s' source=%s hand=%d passive=%d",BODY)
if __name__=='__main__': unittest.main()
