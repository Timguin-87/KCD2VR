import importlib.util
from pathlib import Path
import unittest
spec=importlib.util.spec_from_file_location('reload_report',Path(__file__).resolve().parents[1]/'tools/report_reload178r36c2.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)

class ReloadReport(unittest.TestCase):
    def fixture(self,**changes):
        fields=dict(session='1',call='9',owner='HENRY',actor='A',henry='7',action='B',handRead='1',handId='0')
        fields.update(changes)
        return 'V66.178R36C2 RELOAD_NATIVE '+' '.join(k+'='+v for k,v in fields.items())
    def test_transfer_requires_same_call_owner_and_order(self):
        lines=[self.fixture(stage='HAND_BEGIN',create='0',handId='42'),self.fixture(stage='HAND_END'),
            self.fixture(stage='TRANSFER'),self.fixture(stage='AMMO_BIND'),self.fixture(stage='NOCK_BEGIN'),
            self.fixture(stage='NOCK_END',nockRead='1',nockId='99')]
        report=module.summarize('\n'.join(lines))
        self.assertEqual(report['visual_transfers'],1)
        self.assertTrue(report['henry_calls'][0]['distinct_visual_ids'])
        self.assertEqual(report['inventory_delta'],'UNOBSERVED')
        for replacement in ('call=10','owner=UNQUALIFIED','actor=OTHER'):
            broken=lines[:-1]+[lines[-1].replace({'call=10':'call=9','owner=UNQUALIFIED':'owner=HENRY','actor=OTHER':'actor=A'}[replacement],replacement)]
            self.assertEqual(module.summarize('\n'.join(broken))['visual_transfers'],0)
        self.assertEqual(module.summarize('\n'.join(reversed(lines)))['visual_transfers'],0)
    def test_coverage_loss_and_no_records_are_not_success(self):
        self.assertFalse(module.summarize('')['recording_complete'])
        begin='V66.178R36C2 RELOAD_NATIVE_COVERAGE session=1 ready=1'
        end='V66.178R36C2 RELOAD_NATIVE_END session=1 dropped=0 budgetRejected=0 scopeLost=0'
        self.assertTrue(module.summarize(begin+'\n'+end)['recording_complete'])
        self.assertFalse(module.summarize(begin+'\n'+end.replace('dropped=0','dropped=1'))['recording_complete'])
        self.assertFalse(module.summarize(begin.replace('ready=1','ready=0')+'\n'+end)['recording_complete'])
        self.assertFalse(module.summarize(begin+'\n'+end.replace('session=1','session=2'))['recording_complete'])
    def test_unqualified_dispatch_never_becomes_henry(self):
        s=self.fixture(stage='DISPATCH',owner='UNQUALIFIED',event='LoadReachedWeapon')
        report=module.summarize(s)
        self.assertEqual(report['henry_calls'],[])
        self.assertEqual(report['unqualified_candidates'],{'DISPATCH:LoadReachedWeapon':1})
    def test_missing_read_not_empty_visual(self):
        s='\n'.join([self.fixture(stage='HAND_BEGIN',create='0',handId='42'),self.fixture(stage='HAND_END',handRead='0')])
        self.assertFalse(module.summarize(s)['henry_calls'][0]['temporary_visual_cleared'])
